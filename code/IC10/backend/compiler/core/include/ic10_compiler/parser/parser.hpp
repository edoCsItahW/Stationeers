// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser.hpp
 * @author edocsitahw
 * @version 1.3
 * @date 2026/09/24
 * @if zh
 * @brief IC10 语法分析器：按 FIRST 集做有界前瞻（类 LL(k)）的节点预测与分派
 * @details 把词法标记序列解析为抽象语法树（AST）。自 v3 起，语句分派不再是手写的递归下降链，
 *          而是一台**以节点自带 FIRST 集为准的有界前瞻预测器**。
 *
 *          @par 心智模型：三条规则
 *          1. **每个节点自带 FIRST 集**。AST 节点以 `static constexpr auto FIRST` 声明首部序列
 *             （`std::tuple<std::array<TokenType, N>...>`），例如 `AliasDirective::FIRST` 即
 *             `{ { TokenType::KEYWORD_ALIAS } }`。它与文法中的 FIRST 集等价，序列长度就是该节点的
 *             前瞻宽度 k —— 因此 k 逐节点而定，并非全局固定的 LL(1)。
 *          2. **前瞻命中才真正解析**。`match<Ts...>()` 依次取候选节点的每条 FIRST 序列，
 *             先用 `matchFirst` / `matchArray` 比对「接下来 k 个 token 的类型」；命中后若该节点还
 *             提供了语义谓词 `NodeParser<T>::is(Parser&)`（FIRST 集重叠时的消歧手段，相当于把冲突
 *             推迟成一次只读检查），则再行确认，最后才调用 `NodeParser<T>::parse` 构造节点。
 *          3. **游标只在成功时前进**。上述比对与谓词全部只读，故调用方无需自行保存/恢复 `idx_`；
 *             当所有候选都落空时，`match` 消费一个 token、报 IEP34_1 并产出 `ErrorNode`（不抛异常），
 *             使语法分析能在错误之后继续。
 *
 *          @par 两条语句通道（见 @ref Parser::parseStatement）
 *          - **指令通道**：首个 token 是 `KEYWORD` 时交给 @ref dispatch（instruction_dispatcher.hpp）。
 *            它把全部 `InstructionKeyword` 取值在编译期展开，运行时只进入命中的那一支；各操作数经
 *            @ref Parser::matchOperand 按 `OperandType` → 语义变体 → FIRST 前瞻逐个解析。
 *          - **非指令通道**：其余语句（标签定义、`alias` / `define` 预处理指令、`#>` 类型注解）
 *            统一交给 `matchVariant<FirstStatement>()`，同样走 FIRST 前瞻。
 *
 *          @par 解析流程:
 *          ```mermaid
 *          graph LR
 *              A[Token 序列] --> B{首个 token 是 KEYWORD ?}
 *              B -->|是| C[dispatch 编译期展开关键字分支]
 *              C --> D[matchOperand 逐操作数前瞻]
 *              D --> G[ExecutableInstruction]
 *              B -->|否| E[matchVariant FirstStatement]
 *              E --> F[LabelDef / alias / define / 类型注解]
 *              F --> H[Program.statements]
 *              G --> H
 *          ```
 *
 *          @par 集成关系
 *          | 协作方 | 关系 |
 *          |:-----|:-----|
 *          | `lexer/lexer.hpp` | 上游：产出 `std::vector<std::shared_ptr<Token>>`，并以 `END` 收尾 |
 *          | `parser/node_parser.hpp` | 下游且互为友元：`Parser` 提供游标与诊断，`NodeParser<T>` 提供 `is` 谓词与 `parse` 实现 |
 *          | `parser/ast/node.hpp` | 节点契约：`HasFirst` 概念与 `AST<T>`（位置、`toString`、`toJSON`） |
 *          | `parser/instruction_dispatcher.hpp` | 指令通道的编译期分派 |
 *          | `parser/ast/semantic_operand.hpp` | `OperandType` 到语义变体的映射 |
 *          | `semantic/analyser.hpp` | 下游：消费 AST 建立符号表与类型；本文件不涉及 |
 *          | `incremental/inc_parser.hpp` | 复用本解析器，做按语句粒度的增量重解析 |
 *
 *          @par 错误恢复（两级，互不替代）
 *          - **语句级**：`match` 落空 → 消费一个 token 并产出 `ErrorNode`；`expect` 失败 → 抛 @ref Error，
 *            由 `matchArray` 捕获后转成 `ErrorNode`。
 *          - **行级**：语句之间缺少换行分隔时报 IEP26，并用 @ref Parser::gotoNextLine 跳到下一行，
 *            以 `NEWLINE` 作为同步点 —— 「一行出错不拖累下一行」正源于此。
 *
 *          @attention 本解析器不做语义分析：符号表、类型推导与操作数合法性判定都在语义阶段
 *                     （analyser.hpp / operand_check.hpp）。
 *          @warning `debug_` 目前只被赋值、**没有任何读取点**，因此构造时的 `debug` 参数当前不会产生
 *                   任何调试输出；历史注释「设置 `debug_ = true` 可输出详细解析过程」已不成立。
 *          @note @ref detail::FirstLookaheadMatcher 仅有前置声明，既无定义也无使用点，属历史残留。
 *
 *          @see node_parser.hpp 各节点解析器与 `NodeParserDispatcher`
 *          @see instruction_dispatcher.hpp 指令通道的编译期分派
 *          @see semantic_operand.hpp 操作数语义变体
 *
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 parser: bounded-lookahead (LL(k)-like) node prediction driven by FIRST sets
 * @details Turns a token sequence into an abstract syntax tree. Since v3, statement dispatch is no
 *          longer a hand-written recursive-descent chain but a **bounded-lookahead predictor driven
 *          by the FIRST set each node declares**.
 *
 *          @par Mental model: three rules
 *          1. **Every node carries its own FIRST set.** AST nodes declare head sequences via
 *             `static constexpr auto FIRST` (`std::tuple<std::array<TokenType, N>...>`), e.g.
 *             `AliasDirective::FIRST` is `{ { TokenType::KEYWORD_ALIAS } }`. It is the grammar's FIRST
 *             set, and a sequence's length is that node's lookahead width k — so k varies per node
 *             rather than being a fixed LL(1).
 *          2. **A parse only happens after a lookahead hit.** `match<Ts...>()` walks each candidate's
 *             FIRST sequences and compares "the types of the next k tokens" via `matchFirst` /
 *             `matchArray`; on a hit, an optional semantic predicate `NodeParser<T>::is(Parser&)`
 *             (the disambiguation device for overlapping FIRST sets, effectively deferring the
 *             conflict to a read-only check) confirms it, and only then does `NodeParser<T>::parse`
 *             build the node.
 *          3. **The cursor advances only on success.** Every comparison and predicate above is
 *             read-only, so callers never save and restore `idx_` themselves; when no candidate
 *             matches, `match` consumes a single token, reports IEP34_1 and produces an `ErrorNode`
 *             (without throwing), which lets parsing continue past the error.
 *
 *          @par Two statement lanes (see @ref Parser::parseStatement)
 *          - **Instruction lane**: when the first token is a `KEYWORD` it goes to @ref dispatch
 *            (instruction_dispatcher.hpp), which expands every `InstructionKeyword` value at compile
 *            time and enters only the matching branch at run time; each operand is parsed with
 *            @ref Parser::matchOperand through `OperandType` → semantic variant → FIRST lookahead.
 *          - **Non-instruction lane**: every other statement (label definitions, `alias` / `define`
 *            directives, `#>` type annotations) goes through `matchVariant<FirstStatement>()`, again
 *            as FIRST-set lookahead.
 *
 *          @par Parsing flow:
 *          ```mermaid
 *          graph LR
 *              A[Token sequence] --> B{first token is KEYWORD ?}
 *              B -->|yes| C[dispatch expands keyword branches]
 *              C --> D[matchOperand per-operand lookahead]
 *              D --> G[ExecutableInstruction]
 *              B -->|no| E[matchVariant FirstStatement]
 *              E --> F[LabelDef / alias / define / annotations]
 *              F --> H[Program.statements]
 *              G --> H
 *          ```
 *
 *          @par Integration
 *          | Counterpart | Relationship |
 *          |:-----|:-----|
 *          | `lexer/lexer.hpp` | Upstream: produces the `std::vector<std::shared_ptr<Token>>`, terminated by `END` |
 *          | `parser/node_parser.hpp` | Downstream and mutual friend: `Parser` supplies cursor and diagnostics, `NodeParser<T>` supplies the `is` predicate and the `parse` body |
 *          | `parser/ast/node.hpp` | Node contract: the `HasFirst` concept and `AST<T>` (position, `toString`, `toJSON`) |
 *          | `parser/instruction_dispatcher.hpp` | Compile-time dispatch of the instruction lane |
 *          | `parser/ast/semantic_operand.hpp` | `OperandType` to semantic variant mapping |
 *          | `semantic/analyser.hpp` | Downstream: consumes the AST for symbols and types; not touched here |
 *          | `incremental/inc_parser.hpp` | Reuses this parser for statement-granular incremental re-parsing |
 *
 *          @par Error recovery (two levels, neither replaces the other)
 *          - **Statement level**: a failing `match` consumes one token and yields an `ErrorNode`;
 *            a failing `expect` throws @ref Error, which `matchArray` catches and turns into an
 *            `ErrorNode`.
 *          - **Line level**: a missing newline between statements reports IEP26 and @ref
 *            Parser::gotoNextLine skips to the next line, using `NEWLINE` as the synchronization
 *            point — this is why one broken line does not poison the next.
 *
 *          @attention This parser performs no semantic analysis: symbols, type inference and operand
 *                     legality all belong to the semantic stage (analyser.hpp / operand_check.hpp).
 *          @warning `debug_` is currently only assigned and **never read**, so the `debug` constructor
 *                   argument produces no output at all today; the historical claim "set
 *                   `debug_ = true` to output the parsing process" no longer holds.
 *          @note @ref detail::FirstLookaheadMatcher is a forward declaration only — no definition and
 *                no use site; it is a leftover.
 *
 *          @see node_parser.hpp per-node parsers and `NodeParserDispatcher`
 *          @see instruction_dispatcher.hpp compile-time dispatch of the instruction lane
 *          @see semantic_operand.hpp operand semantic variants
 *
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef IC10_COMPILER_CORE_PARSER_HPP
#define IC10_COMPILER_CORE_PARSER_HPP
#pragma once

#include "ic10_compiler/locals/local.hpp"
#include "ic10_compiler/pch/ast.hpp"
#include "ic10_compiler/pch/pch.hpp"
#include "node_parser.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace stationeers::ic10 {

    namespace detail {
        /**
         * @if zh
         * @brief （历史残留）未实现的 FIRST 前瞻匹配器前置声明
         * @details v3 重构期间作为设计占位保留：全仓既无定义、也无实例化点。真正的前瞻匹配是
         *          @ref Parser::matchFirst 与 @ref Parser::matchArray。
         * @note 若没有补齐计划，可以安全删除。
         * @elseif en
         * @brief (Leftover) forward declaration of an unimplemented FIRST-lookahead matcher
         * @details Kept as a design placeholder from the v3 rework: it has no definition and no
         *          instantiation anywhere in the tree. The real lookahead matching lives in
         *          @ref Parser::matchFirst and @ref Parser::matchArray.
         * @note Safe to delete if there is no plan to implement it.
         * @endif
         */
        template<HasFirst... Ts>
        struct FirstLookaheadMatcher;
    }  // namespace detail

    // LabelDef

    /**
     * @if zh
     * @brief @ref LabelDef 的解析器（形如 `main:`）
     * @details `LabelDef::FIRST` 是**两个 token** 的序列 `{ IDENTIFIER, COLON }`，是可变前瞻宽度 k
     *          的典型例子：只有同时看到标识符与冒号，@ref Parser::match 才会选中本解析器。
     * @note 这里没有需要自行消费的前缀关键字：`FIRST` 只用于预测，token 的实际消费一律由 `parse`
     *       主体完成（本例消费标识符与冒号）。对比 @ref NodeParser<AliasDirective>::parse，
     *       后者必须自己 `consume()` 掉 `alias` 关键字。
     * @note 缺少冒号时报告 IEP23 并返回 `ErrorNode`，且**不消费** token —— 交由 @ref Parser::parse
     *       主循环以 `NEWLINE` 为同步点跳到下一行。
     * @see Parser::parseStatement 进入本解析器的语句通道
     * @elseif en
     * @brief Parser for @ref LabelDef (e.g. `main:`)
     * @details `LabelDef::FIRST` is the **two-token** sequence `{ IDENTIFIER, COLON }`, the typical
     *          example of this project's variable lookahead width k: @ref Parser::match only selects
     *          this parser when both the identifier and the colon are visible.
     * @note There is no prefix keyword to consume here: `FIRST` is used for prediction only, and
     *       token consumption is always done by the `parse` body itself (identifier and colon in
     *       this case). Contrast @ref NodeParser<AliasDirective>::parse, which must `consume()` the
     *       `alias` keyword itself.
     * @note A missing colon reports IEP23 and returns an `ErrorNode` **without consuming** a token,
     *       leaving @ref Parser::parse's main loop to resynchronize on `NEWLINE`.
     * @see Parser::parseStatement the statement lane that reaches this parser
     * @endif
     */
    template<>
    struct NodeParser<LabelDef> {
        static ShallowErrorable<LabelDef> parse(Parser& parser);
    };

    // AliasDirective

    /**
     * @if zh
     * @brief @ref AliasDirective 的解析器（形如 `alias myDisp @Display`）
     * @details 依次解析：`alias` 关键字（自行消费）→ 标识符 → @ref OperandType::REG_OR_DEV 操作数
     *          → 可选的 `#:` 类型提示。
     * @note `FIRST` 为 `{ KEYWORD_ALIAS }`，只负责预测；关键字由本函数第一行 `consume()` 掉 ——
     *       这是「预测与消费分离」最容易踩错的地方。
     * @see NodeParser<TypeHint>::parse 可选的 `#:` 类型提示
     * @elseif en
     * @brief Parser for @ref AliasDirective (e.g. `alias myDisp @Display`)
     * @details Parses, in order: the `alias` keyword (consumed here) → identifier → a
     *          @ref OperandType::REG_OR_DEV operand → an optional `#:` type hint.
     * @note `FIRST` is `{ KEYWORD_ALIAS }` and only predicts; the keyword is consumed by this
     *       function's first line — the easiest place to trip over the predict/consume split.
     * @see NodeParser<TypeHint>::parse the optional `#:` type hint
     * @endif
     */
    template<>
    struct NodeParser<AliasDirective> {
        static AliasDirective parse(Parser& parser);
    };

    // DefineDirective

    /**
     * @if zh
     * @brief @ref DefineDirective 的解析器（形如 `define BUFFER_SIZE 1024`）
     * @details 依次解析：`define` 关键字（自行消费）→ 标识符 → 常量（数字走
     *          @ref OperandType::CONST_NUM，或字符串形式）→ 可选的 `#:` 类型提示。
     * @note 字符串 + `@builtin` 类型标签的组合是标准库表达难以写成字面量的常量（如 `rgas`、`tau`）
     *       的手段：此时字符串内容交给 @ref Parser::evaluateBuiltin 换算为浮点字面量，换算失败报
     *       IEP32_1；字符串配了非 `builtin` 的类型提示则报 IEP31。
     * @see Parser::evaluateBuiltin 内建常量表
     * @elseif en
     * @brief Parser for @ref DefineDirective (e.g. `define BUFFER_SIZE 1024`)
     * @details Parses, in order: the `define` keyword (consumed here) → identifier → the constant
     *          (a number through @ref OperandType::CONST_NUM, or a string form) → an optional `#:`
     *          type hint.
     * @note The string + `@builtin` tag combination is how the standard library expresses constants
     *       that are awkward as literals (`rgas`, `tau`, …): the string is converted to a float
     *       literal by @ref Parser::evaluateBuiltin, and a failed conversion reports IEP32_1; a
     *       string carrying a non-`builtin` type hint reports IEP31.
     * @see Parser::evaluateBuiltin the builtin constant table
     * @endif
     */
    template<>
    struct NodeParser<DefineDirective> {
        static DefineDirective parse(Parser& p);
    };

    /**
     * @class Parser
     * @if zh
     *
     * @brief IC10 语法分析器：持有 token 序列与游标，对外提供「前瞻匹配」与「节点分派」两组能力
     * @details 本类不实现任何节点的具体语法，节点知识都在 @ref NodeParser 的特化里；`Parser` 只负责
     *          三件事：**游标**（`idx_`）、**前瞻匹配**（`match*` 家族）、**诊断与错误恢复**。
     *          完整设计说明（FIRST 集、k 宽度、两条语句通道、集成关系）见本文件头部的 @c @details。
     *
     *          @par 匹配设施一览（谁消费 token）
     *          | 设施 | 作用 | 是否消费 token |
     *          |:-----|:-----|:-----|
     *          | @ref match | 在若干候选节点中预测并解析出**一个**，落空时产出 `ErrorNode` | 成功时由 `NodeParser<T>::parse` 消费；落空时消费 1 个 |
     *          | @ref matchFirst | 遍历单个节点的全部 FIRST 序列 | 否 |
     *          | @ref matchArray | 比对**一条** FIRST 序列，命中后跑语义谓词与 `parse` | 否（消费发生在 `parse` 内） |
     *          | @ref matchPredicate | 调用可选的 `NodeParser<T>::is` 语义谓词 | 否 |
     *          | @ref matchOperand / @ref matchVariant | 把 `OperandType` / 变体类型展平成候选列表后交给 @ref match | 同 @ref match |
     *          | @ref isMatch / @ref isAnyMatch / @ref isVariantMatch | 纯前瞻查询，**不解析** | 否 |
     *
     *          @par 使用示例:
     *          ```cpp
     *          using namespace stationeers::ic10;
     *
     *          auto tokens = Lexer::tokenize(source);       // 1. 词法分析
     *          auto program = Parser::parsing(tokens);      // 2. 语法分析（等价于 Parser{tokens}.parse()）
     *
     *          for (const auto& stmt : program.statements)  // 3. 遍历 AST
     *              std::cout << std::visit([](const auto& node) { return node.toString(); }, stmt)
     *                        << std::endl;
     *
     *          for (const auto& d : parser.getDiagnostics())  // 4. 取诊断（语法错误在这里）
     *              std::cout << d.message << std::endl;
     *          ```
     *
     *          @par 结果约定
     *          - **成功**：返回含全部语句的 `Program`。
     *          - **局部失败**：出错的语句位置是 `ErrorNode`，其余语句照常解析 —— 因此
     *            「`statements` 非空」不等于「没有语法错误」，必须查 @ref getDiagnostics。
     *
     *          @note 非线程安全：游标 `idx_` 由 @c const 成员函数通过 `mutable` 修改（见
     *                @ref skip / @ref consume / @ref gotoNextLine），同一个 `Parser` 实例不可并发使用。
     *          @note 构造参数 `debug` 当前无效果，见 @ref debug_。
     *          @see Lexer::tokenize 词法分析（上游）
     *          @see Analyser 语义分析（下游）
     *
     * @elseif en
     *
     * @brief IC10 parser: owns the token sequence and cursor, and exposes "lookahead matching" plus
     *        "node dispatch"
     * @details This class implements no node grammar itself — node knowledge lives in @ref NodeParser
     *          specializations. `Parser` owns exactly three things: the **cursor** (`idx_`), the
     *          **lookahead matching** family (`match*`), and **diagnostics / error recovery**. The
     *          full design (FIRST sets, width k, the two statement lanes, integration) is in this
     *          file's header @c @details.
     *
     *          @par Matching facilities at a glance (who consumes tokens)
     *          | Facility | Purpose | Consumes tokens? |
     *          |:-----|:-----|:-----|
     *          | @ref match | Predict and parse **one** of several candidate nodes; yields an `ErrorNode` on failure | yes, via `NodeParser<T>::parse` on success; 1 token on failure |
     *          | @ref matchFirst | Walk every FIRST sequence of one node | no |
     *          | @ref matchArray | Compare **one** FIRST sequence, then run the semantic predicate and `parse` | no (consumption happens inside `parse`) |
     *          | @ref matchPredicate | Invoke the optional `NodeParser<T>::is` semantic predicate | no |
     *          | @ref matchOperand / @ref matchVariant | Flatten an `OperandType` / variant into a candidate list and delegate to @ref match | as @ref match |
     *          | @ref isMatch / @ref isAnyMatch / @ref isVariantMatch | Pure lookahead query, **no parsing** | no |
     *
     *          @par Usage example:
     *          ```cpp
     *          using namespace stationeers::ic10;
     *
     *          auto tokens = Lexer::tokenize(source);       // 1. lex
     *          auto program = Parser::parsing(tokens);      // 2. parse (same as Parser{tokens}.parse())
     *
     *          for (const auto& stmt : program.statements)  // 3. walk the AST
     *              std::cout << std::visit([](const auto& node) { return node.toString(); }, stmt)
     *                        << std::endl;
     *
     *          for (const auto& d : parser.getDiagnostics())  // 4. diagnostics (syntax errors)
     *              std::cout << d.message << std::endl;
     *          ```
     *
     *          @par Result contract
     *          - **Success**: a `Program` holding every statement.
     *          - **Partial failure**: the offending position holds an `ErrorNode` while the rest of
     *            the program still parses — so a non-empty `statements` does *not* mean "no syntax
     *            errors"; always check @ref getDiagnostics.
     *
     *          @note Not thread-safe: the `idx_` cursor is mutated by @c const member functions
     *                through `mutable` (see @ref skip / @ref consume / @ref gotoNextLine), so one
     *                `Parser` instance must not be shared across threads.
     *          @note The `debug` constructor argument currently has no effect; see @ref debug_.
     *          @see Lexer::tokenize lexical analysis (upstream)
     *          @see Analyser semantic analysis (downstream)
     *
     * @endif
     */
    class Parser {
    public:
        /**
         * @if zh
         * @brief 调试模式标志（**当前无任何读取点**）
         * @warning 只被构造函数赋值，全仓没有任何地方读取它，因此传 `debug = true` 目前不会输出任何
         *          解析过程，也不改变任何行为。要恢复调试输出，需要真正使用本标志（例如在
         *          @ref parseStatement / @ref match 处打印游标与命中分支）。
         * @note 保留该成员是为了不破坏既有调用方签名（`Parser{tokens, debug}`）。
         * @elseif en
         * @brief Debug mode flag (**currently never read**)
         * @warning It is only assigned by the constructor and read nowhere in the tree, so passing
         *          `debug = true` currently prints nothing and changes no behaviour. Restoring debug
         *          output means actually using this flag (e.g. logging the cursor and the matched
         *          branch in @ref parseStatement / @ref match).
         * @note The member is kept so existing caller signatures (`Parser{tokens, debug}`) keep working.
         * @endif
         */
        bool debug_ = false;

        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        Parser() = default;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param tokens 词法标记向量
         * @param debug 是否启用调试模式
         *
         * @elseif en
         *
         * @brief Constructor
         * @param tokens Token vector
         * @param debug Whether to enable debug mode
         *
         * @endif
         */
        Parser(const std::vector<std::shared_ptr<Token>>& tokens, bool debug = false);

        /**
         * @if zh
         *
         * @brief 解析整个程序
         * @return 解析后的Program节点
         *
         * @elseif en
         *
         * @brief Parse the whole program
         * @return Parsed Program node
         *
         * @endif
         */
        Program parse();

        /**
         * @if zh
         *
         * @brief 获取诊断列表
         * @details 返回语法分析过程中收集到的所有诊断信息（包含错误、警告、提示）
         * @return 诊断列表的常量引用
         *
         * @elseif en
         *
         * @brief Get the list of diagnostics
         * @details Returns all diagnostics collected during parsing (including errors, warnings,
         * info)
         * @return Const reference to the diagnostic list
         *
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const noexcept {
            return reporter_.getDiagnostics();
        }

        /**
         * @if zh
         *
         * @brief 静态解析入口
         * @param tokens 词法标记向量
         * @param debug 是否启用调试模式
         * @return 解析后的Program节点
         *
         * @elseif en
         *
         * @brief Static parse entry point
         * @param tokens Token vector
         * @param debug Whether to enable debug mode
         * @return Parsed Program node
         *
         * @endif
         */
        static Program parsing(
            const std::vector<std::shared_ptr<Token>>& tokens, bool debug = false
        );

    private:
        /**
         * @if zh
         * @brief 当前标记索引
         * @elseif en
         * @brief Current token index
         * @endif
         */
        mutable std::size_t idx_ = 0;

        /**
         * @if zh
         * @brief 诊断报告器
         * @details 收集词法/语法分析过程中产生的错误、警告等诊断信息
         * @elseif en
         * @brief Diagnostic reporter
         * @details Collects errors, warnings and other diagnostic information during parsing
         * @endif
         */
        DiagnosticReporter<IC10CompilerMsgPack> reporter_;

        /**
         * @if zh
         * @brief 输入标记序列
         * @elseif en
         * @brief Input token sequence
         * @endif
         */
        std::vector<std::shared_ptr<Token>> tokens_{};

        /**
         * @if zh
         *
         * @brief 解析一条语句，并按首 token 选择语句通道
         * @details 本函数是两条通道的分岔口：
         *          - 首 token 为 `KEYWORD` → @ref parseExecutableInstruction（编译期展开的指令分派）；
         *          - 其余 → `matchVariant<FirstStatement>()`（FIRST 前瞻：标签定义、`alias` / `define`、
         *            `#>` 类型注解）。
         * @return 解析出的 `Statement`；本函数自身不产出 `ErrorNode`，错误节点由被调用的两方产出
         * @warning 越界保护分支（`!current()`）内部仍解引用 `current()`，属于**空指针解引用**；
         *          该路径在 `parse()` 的 @ref inScope 守卫下不可达，但本函数若被单独调用需自行保证
         *          游标有效。见 @ref Parser::parse 的循环条件。
         *
         * @elseif en
         *
         * @brief Parse one statement and pick its lane from the first token
         * @details This is the fork between the two lanes:
         *          - first token is a `KEYWORD` → @ref parseExecutableInstruction (compile-time
         *            expanded instruction dispatch);
         *          - otherwise → `matchVariant<FirstStatement>()` (FIRST lookahead: label definitions,
         *            `alias` / `define`, `#>` type annotations).
         * @return The parsed `Statement`; this function never produces an `ErrorNode` itself — the two
         *         callees do
         * @warning The out-of-range guard (`!current()`) still dereferences `current()`, i.e. a **null
         *          pointer dereference**. That path is unreachable under @ref inScope in @ref parse,
         *          but a direct caller must ensure the cursor is valid.
         *
         * @endif
         */
        Statement parseStatement();

        /**
         * @if zh
         *
         * @brief 解析一条指令语句（首 token 已是 `KEYWORD`）
         * @details 消费关键字后交给 @ref dispatch：后者把全部 `InstructionKeyword` 在编译期展开，
         *          运行时只进入命中的那一支。命中分支的回调会按该指令声明顺序，用
         *          @ref matchOperand 逐个解析操作数，再以关键字 token 的位置构造节点。
         * @return 解析出的 `ExecutableInstruction`；关键字无可分派目标时报告 IEP3_1、消费该 token
         *         并返回 `ErrorNode`
         * @note 分发目标由 `dispatch<ExecutableInstruction::Variant>` 指定，避免在分派处先构造一个
         *       覆盖全部指令的中间变体再整体转换（那会让每个分支都对 N 个候选做可构造性检查）。
         * @see dispatch 指令分派的实现与理由
         * @see semantic_operand.hpp 操作数类型到语义变体的映射
         *
         * @elseif en
         *
         * @brief Parse one instruction statement (the first token is already a `KEYWORD`)
         * @details After consuming the keyword it delegates to @ref dispatch, which expands every
         *          `InstructionKeyword` at compile time and enters only the matching branch at run
         *          time. The matching branch's callback parses its operands in declaration order with
         *          @ref matchOperand and builds the node at the keyword token's position.
         * @return The parsed `ExecutableInstruction`; when the keyword has no dispatch target it
         *         reports IEP3_1, consumes that token and returns an `ErrorNode`
         * @note The dispatch target is supplied as `dispatch<ExecutableInstruction::Variant>`, which
         *       avoids building an intermediate variant covering all instructions and converting it
         *       afterwards (that would force a constructibility check against N alternatives in every
         *       branch).
         * @see dispatch instruction dispatch and its rationale
         * @see semantic_operand.hpp operand type to semantic variant mapping
         *
         * @endif
         */
        ExecutableInstruction parseExecutableInstruction();

        /**
         * @if zh
         *
         * @brief 查询内建浮点常量表并换算其数值
         * @details 供 `define <名称> "<内建名>" #: @builtin` 这种写法使用：标准库用它表达难以写成
         *          字面量的常量。表内容为固定值，例如 `pi`、`tau`、`rgas`、`deg2rad`、`rad2deg`
         *          以及 `nan` / `pinf` / `ninf` / `epsilon`（后四者取自 `std::numeric_limits`）。
         * @param name 内建常量名（**不含**引号）
         * @return 命中时返回其数值；未命中返回 `std::nullopt`
         * @note `noexcept`：纯查表，不修改任何状态，也不产生诊断 —— 报错由调用方负责
         *       （见 @ref NodeParser<DefineDirective>::parse 的 IEP32_1）。
         * @elseif en
         *
         * @brief Look up a builtin float constant and produce its value
         * @details Used by the `define <name> "<builtin>" #: @builtin` form, which lets the standard
         *          library express constants that are awkward as literals. The table holds fixed
         *          values such as `pi`, `tau`, `rgas`, `deg2rad`, `rad2deg` plus `nan` / `pinf` /
         *          `ninf` / `epsilon` (the latter four from `std::numeric_limits`).
         * @param name Builtin name (**without** quotes)
         * @return Its value on a hit, `std::nullopt` otherwise
         * @note `noexcept`: a pure table lookup that mutates nothing and reports nothing — error
         *       reporting is the caller's job (see IEP32_1 in @ref NodeParser<DefineDirective>::parse).
         *
         * @endif
         */
        static std::optional<double> evaluateBuiltin(const std::string& name) noexcept;

        /**
         * @if zh
         *
         * @brief 检查是否在范围内
         * @return 如果还有标记则返回true
         *
         * @elseif en
         *
         * @brief Check if in range
         * @return true if there are still tokens
         *
         * @endif
         */
        bool inScope() const noexcept;

        /**
         * @if zh
         *
         * @brief 跳过连续的空行与注释
         * @details 语句解析的前后都由本函数对齐到「下一个有意义的 token」：换行与注释（含行内注释）
         *          在这里被整体略过，因此语句之间只允许出现它们。
         * @note 只前进 `idx_`，不消费其他 token，也不产生诊断。
         *
         * @elseif en
         *
         * @brief Skip consecutive newlines and comments
         * @details Statement parsing is aligned to "the next meaningful token" by this function, both
         *          before and after a statement: newlines and comments (including trailing ones) are
         *          skipped wholesale, so nothing else may appear between statements.
         * @note Only advances `idx_`; consumes no other token and reports nothing.
         *
         * @endif
         */
        void skip() noexcept;

        /**
         * @if zh
         *
         * @brief 前移游标一个 token
         * @note 仅在 @ref inScope 为真时前进 —— 游标因此永远不会越过 `END`，越界的读取请用
         *       @ref current / @ref peek 的 `nullptr` 返回值判断。
         * @note 声明为 `const` 却修改 `idx_`：本类是刻意设计为「const 方法也能推进游标」的
         *       （`idx_` 为 `mutable`），代价是实例不可并发使用。
         *
         * @elseif en
         *
         * @brief Advance the cursor by one token
         * @note It advances only when @ref inScope holds, so the cursor never moves past `END`; use
         *       the `nullptr` returns of @ref current / @ref peek to detect out-of-range reads.
         * @note Declared `const` yet mutating `idx_`: this class deliberately lets `const` methods
         *       advance the cursor (`idx_` is `mutable`); the price is that one instance is not
         *       safe to share across threads.
         *
         * @endif
         */
        void consume() const noexcept;

        /**
         * @if zh
         *
         * @brief 跳到下一行行首（**行级错误恢复的同步点**）
         * @details 一路跳过 token 直到 `NEWLINE` 并消费它。@ref Parser::parse 在「语句之间缺少换行
         *          分隔」（IEP26）时调用本函数，从而把一行内的语法错误限制在该行内 —— 这是错误恢复
         *          的第二级，与语句级恢复（`match` 落空产 `ErrorNode`）互补。
         * @note 若已到 `END`，循环结束且不会消费任何 token。
         *
         * @elseif en
         *
         * @brief Skip to the start of the next line (**the line-level recovery sync point**)
         * @details Skips tokens until `NEWLINE` and consumes it. @ref Parser::parse calls this when
         *          statements are not separated by a newline (IEP26), which confines a syntax error
         *          to its own line — the second recovery level, complementing the statement-level one
         *          (`match` yielding an `ErrorNode`).
         * @note At `END` the loop exits without consuming anything.
         *
         * @endif
         */
        void gotoNextLine() const noexcept;

        /**
         * @if zh
         *
         * @brief 读取游标处的 token
         * @param consume 为真时顺带前移游标（等价于读取后立即 @ref consume）
         * @return 游标处的 token；游标越界时返回 `nullptr`
         * @warning 调用方必须先判空：返回 `nullptr` 后解引用即为空指针解引用，
         *          `parseStatement` / `parseExecutableInstruction` 的报错分支目前正踩在这个边界上。
         *
         * @elseif en
         *
         * @brief Read the token at the cursor
         * @param consume When true, also advance the cursor (i.e. read then @ref consume)
         * @return The token at the cursor, or `nullptr` when the cursor is out of range
         * @warning Callers must null-check: dereferencing that `nullptr` is a null pointer
         *          dereference, which the error branches of `parseStatement` /
         *          `parseExecutableInstruction` currently sit right on top of.
         *
         * @endif
         */
        std::shared_ptr<Token> current(bool consume = false) const noexcept;

        /**
         * @if zh
         *
         * @brief 只读前瞻第 `offset` 个 token（默认下一个）
         * @details @ref matchArray 与 @ref isMatch 的前瞻比对完全建立在本函数之上；它们都先确认
         *          `idx_ + 序列长度 < tokens_.size()` 才逐项读取，因此不会读到 `END` 之后。
         * @param offset 相对游标的偏移量，`1` 表示紧随游标的下一个 token（**不是**当前 token）
         * @return 该位置的 token；越界返回 `nullptr`
         * @note 不修改游标，也不产生诊断。
         * @elseif en
         *
         * @brief Read-only lookahead of the `offset`-th token (the next one by default)
         * @details The lookahead comparisons in @ref matchArray and @ref isMatch are built entirely
         *          on this function; both first confirm `idx_ + sequence length < tokens_.size()`
         *          before reading item by item, so they never read past `END`.
         * @param offset Offset relative to the cursor; `1` is the token right after it (**not** the
         *        current token)
         * @return The token at that position, or `nullptr` when out of range
         * @note Neither mutates the cursor nor reports diagnostics.
         * @endif
         */
        std::shared_ptr<Token> peek(std::size_t offset = 1) const noexcept;

        /**
         * @if zh
         *
         * @brief 要求当前 token 为指定类型，否则报错并抛出
         * @param type 期望的 token 类型
         * @param skipWs 读取前是否先 @ref skip（期望 `NEWLINE` 时**必然不跳过**，否则会把自己跳过）
         * @param consume 失败时是否顺带消费掉当前 token，用于把错误限制在一个 token 内
         * @return 命中时返回该 token，并已前移游标
         * @throws Error 当前 token 类型不符时：先报 IEP1_1，再抛 `Error`
         * @note 出错 token 必须在消费之前取出，否则诊断的位置与类型会错位到下一个 token —— 这一顺序
         *       是本函数最容易被改坏的地方。
         * @note 抛出是刻意的：由 @ref matchArray 捕获并转换为 `ErrorNode`，从而让「一个节点解析失败」
         *       不中断整棵语法树；直接调用本函数的代码必须自行 `try` / `catch`。
         * @see matchArray 捕获 @ref Error 并转成 `ErrorNode`
         *
         * @elseif en
         *
         * @brief Require the current token to be of a given type, else report and throw
         * @param type Expected token type
         * @param skipWs Whether to @ref skip first (never done when `NEWLINE` is expected, or it
         *        would skip the very token being awaited)
         * @param consume Whether to also consume the current token on failure, keeping the error
         *        confined to a single token
         * @return The token on a hit, with the cursor already advanced
         * @throws Error when the current token has the wrong type: IEP1_1 is reported, then `Error`
         * @note The error token must be captured *before* consuming, or the reported position and
         *       type shift to the next token — the easiest thing to break in this function.
         * @note Throwing is deliberate: @ref matchArray catches it and converts it into an
         *       `ErrorNode`, so one failing node does not abort the whole tree; callers invoking
         *       this directly must `try` / `catch` themselves.
         * @see matchArray catches @ref Error and converts it to an `ErrorNode`
         *
         * @endif
         */
        std::shared_ptr<Token> expect(TokenType type, bool skipWs = true, bool consume = true);

        /**
         * @if zh
         *
         * @brief 按 `OperandType` 解析一个操作数
         * @details 两步映射：`OperandType` →（semantic_operand.hpp 的 `operand_type_t`）语义变体 →
         *          @ref matchVariant 展平成候选节点列表 → @ref match。指令节点的操作数类型由指令
         *          定义本身给出（`Ins<K, Vs...>` 的 `Vs...`），因此「改指令定义即改解析行为」。
         * @tparam O 语义操作数类型，见 @ref OperandType
         * @return 解析结果（`Errorable<...>`，可能含 `ErrorNode`）
         * @see semantic_operand.hpp `OperandType` 到语义变体的映射
         *
         * @elseif en
         *
         * @brief Parse one operand given its `OperandType`
         * @details Two hops: `OperandType` → (via `operand_type_t` in semantic_operand.hpp) a semantic
         *          variant → @ref matchVariant flattens it into candidate nodes → @ref match. An
         *          instruction's operand types come from the instruction definition itself (the
         *          `Vs...` of `Ins<K, Vs...>`), so "changing the definition changes the parsing".
         * @tparam O Semantic operand type, see @ref OperandType
         * @return The parse result (`Errorable<...>`, possibly holding an `ErrorNode`)
         * @see semantic_operand.hpp `OperandType` to semantic variant mapping
         *
         * @endif
         */
        template<OperandType O>
        auto matchOperand();

        /**
         * @if zh
         *
         * @brief 把一个变体的全部候选类型展平后交给 @ref match
         * @details 用变体指针做萃取（`static_cast<Variant*>(nullptr)` 重载决议），得到 `Ts...`
         *          后调用 `match<Ts...>()`，因此调用方写变体名而不是候选列表。
         * @tparam Variant `std::variant` 形态的候选集合
         * @return 解析结果，变体里已包含 `ErrorNode` 分支
         * @note `FirstStatement`、各操作数语义变体都是这样进入解析的（见 @ref Parser::parseStatement）。
         *
         * @elseif en
         *
         * @brief Flatten every alternative of a variant and hand it to @ref match
         * @details Extraction uses a variant pointer (`static_cast<Variant*>(nullptr)` overload
         *          resolution) to recover `Ts...`, then calls `match<Ts...>()`, so callers name the
         *          variant instead of the candidate list.
         * @tparam Variant A `std::variant`-shaped candidate set
         * @return The parse result; the variant already carries an `ErrorNode` alternative
         * @note `FirstStatement` and the per-operand semantic variants enter parsing this way (see
         *       @ref Parser::parseStatement).
         *
         * @endif
         */
        template<IsVariant Variant>
        auto matchVariant();

        /**
         * @if zh
         *
         * @brief 在若干候选节点中预测并解析出一个节点（前瞻匹配的主入口）
         * @details 对每个候选节点先跑 @ref matchFirst；全部落空时报 IEP34_1、消费一个 token，并返回
         *          类型名以 `|` 连接的 `ErrorNode`（不抛异常）。
         * @warning **候选顺序有意义**：按 @c reversed(types) 的语义，实参列表中**最后**一个类型最先被
         *          尝试。因此具体、易被抢走的节点类型应排在后面，通用类型排在前面。
         * @tparam Ts 候选节点类型，均需满足 @ref HasFirst，且应提供 `nodeName`（用于错误信息）
         * @return `Errorable<Ts...>`：命中则为对应节点，落空则为 `ErrorNode`
         * @see matchFirst / matchArray 预测与解析的两级
         *
         * @elseif en
         *
         * @brief Predict and parse one node out of several candidates (the main lookahead entry)
         * @details Each candidate runs @ref matchFirst; when all fail it reports IEP34_1, consumes one
         *          token and returns an `ErrorNode` whose type names are joined with `|` (no throw).
         * @warning **Candidate order matters**: following the `reversed(types)` semantics, the **last**
         *          type in the argument list is tried first. Put specific, easily-stolen node types
         *          later and general ones earlier.
         * @tparam Ts Candidate node types, each satisfying @ref HasFirst and expected to provide
         *         `nodeName` (used in the error message)
         * @return `Errorable<Ts...>`: the matching node, or an `ErrorNode` when nothing matched
         * @see matchFirst / matchArray the two stages of prediction and parsing
         *
         * @endif
         */
        template<HasFirst... Ts>
        auto match();

        /**
         * @if zh
         *
         * @brief 遍历某个节点的**每一条** FIRST 序列，命中即解析
         * @details 一条序列 = 一串连续 token 类型（`std::array<TokenType, N>`），长度即该分支的前瞻
         *          宽度。任一序列比对 + 谓词 + `parse` 成功即返回真。
         * @param result 解析结果写回目标（`Errorable<Ts...>` / 变体）
         * @return 是否已成功解析出节点
         * @note 长度守卫为 `idx_ + 序列长度 < tokens_.size()`，比严格所需多留一个 token —— 这正好
         *       保证 @ref matchArray 不会读到收尾的 `END` 之后。
         *
         * @elseif en
         *
         * @brief Walk **every** FIRST sequence of one node; the first hit is parsed
         * @details A sequence is a run of consecutive token types (`std::array<TokenType, N>`) whose
         *          length is that branch's lookahead width. Returns true as soon as one sequence
         *          compares, passes the predicate and parses.
         * @param result Target the parse result is written into (`Errorable<Ts...>` / a variant)
         * @return Whether a node was successfully parsed
         * @note The length guard is `idx_ + sequence length < tokens_.size()`, one token stricter than
         *       strictly needed — which is exactly what keeps @ref matchArray from reading past the
         *       trailing `END`.
         *
         * @endif
         */
        template<HasFirst T>
        [[nodiscard]] bool matchFirst(auto& result) noexcept;

        /**
         * @if zh
         *
         * @brief 比对**一条** FIRST 序列；命中则跑语义谓词并调用解析器
         * @details 三步：① 逐项比对 `Array[Is] == peek(Is)->type`（只读）；② @ref matchPredicate 询问
         *          可选的语义谓词；③ 调用 `NodeParser<T>::parse` 真正构造节点。
         * @param result 写回目标
         * @return 是否命中并成功解析
         * @note **本函数不消费 FIRST 序列本身**：token 的消费由 `parse` 主体负责（这正是
         *       `NodeParser<AliasDirective>::parse` 第一行要自己 `consume()` 的原因）。
         * @note `NodeParser<T>::parse` 抛出的 @ref Error 会在此被捕获并转成 `ErrorNode`，因此单个
         *       节点解析失败不会中断整棵语法树。
         *
         * @elseif en
         *
         * @brief Compare **one** FIRST sequence; on a hit run the semantic predicate and the parser
         * @details Three steps: ① compare item by item `Array[Is] == peek(Is)->type` (read-only);
         *          ② @ref matchPredicate asks the optional semantic predicate; ③ `NodeParser<T>::parse`
         *          actually builds the node.
         * @param result The write-back target
         * @return Whether it hit and parsed successfully
         * @note **This function does not consume the FIRST sequence itself**: consumption belongs to
         *       the `parse` body (which is why `NodeParser<AliasDirective>::parse` must `consume()` the
         *       keyword on its first line).
         * @note An @ref Error thrown by `NodeParser<T>::parse` is caught here and turned into an
         *       `ErrorNode`, so one failing node does not abort the whole tree.
         *
         * @endif
         */
        template<HasFirst T, std::array Array>
        [[nodiscard]] bool matchArray(auto& result) noexcept;

        /**
         * @if zh
         *
         * @brief 询问可选的语义谓词 `NodeParser<T>::is`
         * @details FIRST 集重叠时用来消歧：类型没有提供 `is` 时恒为真（用 `requires` 检测，
         *          因此「不写 `is`」即「不需要额外检查」）。
         * @return 谓词结果；无谓词时返回 `true`
         *
         * @elseif en
         *
         * @brief Ask the optional semantic predicate `NodeParser<T>::is`
         * @details The disambiguator for overlapping FIRST sets: true whenever the type provides no
         *          `is` (detected with `requires`, so "no `is`" means "no extra check needed").
         * @return The predicate's result, or `true` when there is none
         *
         * @endif
         */
        template<HasFirst T>
        [[nodiscard]] bool matchPredicate() noexcept;

        /**
         * @if zh
         *
         * @brief 纯前瞻查询：该节点的 FIRST 序列是否与接下来的 token 相符
         * @warning 与 @ref match 不同，本函数**不跑语义谓词**，也不解析、不消费。因此可能出现
         *          `isMatch<T>() == true` 而 @ref match 仍然落空（谓词否决）的情况 —— 需要「是否真的
         *          能解析」时请以 @ref match 的结果为准。
         * @return 是否有可能由该节点承接
         *
         * @elseif en
         *
         * @brief Pure lookahead query: does this node's FIRST sequence fit the upcoming tokens?
         * @warning Unlike @ref match, this does **not** run the semantic predicate, nor parse, nor
         *          consume. So `isMatch<T>() == true` can coexist with a failing @ref match (the
         *          predicate rejected it) — when you need "can this really be parsed", rely on
         *          @ref match's result.
         * @return Whether the node could possibly take over
         *
         * @endif
         */
        template<HasFirst T>
        [[nodiscard]] bool isMatch() noexcept;

        /**
         * @if zh
         *
         * @brief 纯前瞻查询：任一候选节点的 FIRST 序列是否相符
         * @note 同样按逆序探测（见 @ref match 的顺序说明），并与 @ref isMatch 一样不跑语义谓词。
         * @return 是否有可能由其中某个候选承接
         *
         * @elseif en
         *
         * @brief Pure lookahead query: does any candidate's FIRST sequence fit?
         * @note Probes in reverse order too (see the ordering note on @ref match) and, like
         *       @ref isMatch, runs no semantic predicate.
         * @return Whether any candidate could take over
         *
         * @endif
         */
        template<HasFirst... Ts>
        [[nodiscard]] bool isAnyMatch() noexcept;

        /**
         * @if zh
         *
         * @brief 纯前瞻查询：把一个变体的全部候选类型展平后询问 @ref isAnyMatch
         * @return 是否有可能由该变体中的某个候选承接
         * @elseif en
         * @brief Pure lookahead query: flatten a variant's alternatives and ask @ref isAnyMatch
         * @return Whether any alternative of that variant could take over
         * @endif
         */
        template<IsVariant Variant>
        [[nodiscard]] bool isVariantMatch() noexcept;

        template<typename T>
        friend struct NodeParser;

        template<typename T>
        friend struct NodeParserDispatcher;
    };

}  // namespace stationeers::ic10

#include "node_parser.inl"
#include "parser.inl"

#endif  // IC10_COMPILER_CORE_PARSER_HPP
