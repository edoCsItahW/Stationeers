// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file node_parser.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief 各 AST 节点的解析器协议：`NodeParser` 特化注册表
 * @details 本文件存放「语法分析」里**每个节点各自那一小块知识**。@ref Parser 只管游标、前瞻与
 *          诊断，「这个节点长什么样、怎样把 token 变成节点」一律由 `NodeParser` 的特化回答 ——
 *          解析逻辑因此与节点定义（`ast/node.hpp`、`ast/ast.hpp`、`ast/expand_node.hpp`）就近放置，
 *          而不是集中成一条巨大的 `switch`。
 *
 *          @par `NodeParser` 的契约
 *          - `static R parse(Parser&)`：构造节点。**必须在 @ref Parser::match 前瞻预检之后调用**，
 *            因为其内部不判断「首部 token 是否正确」，只负责消费与构造。
 *          - `static bool is(const Parser&)`（**可选**）：语义谓词，用于 FIRST 集重叠时的消歧，
 *            由 @ref Parser::matchPredicate 调用。
 *          - 返回类型约定：单节点用 `ShallowErrorable<T>`（即 `std::variant<T, ErrorNode>`）；
 *            候选可能嵌套时用 `Errorable<...>`（展平嵌套变体并唯一化）。
 *
 *          @par 为什么有些 `parse` 可以 `noexcept`
 *          `parse` 中出现的 token 已被 FIRST 序列保证存在；若它只需按序消费这些 token，便可标
 *          `noexcept`（如 `Integer`、`String`、`GeneralPurposeRegister`）。反之，**还需进一步判断**
 *          或可能报错抛出的节点就不能（`DynamicRegister`、`DynamicDevice`、`Enum`、`LabelDef`…）。
 *          换句话说：`noexcept` 在这里是「没有额外失败面」的信号，不是随手加的修饰。
 *
 *          @par 特化分布（按职责分在三个头文件，找实现时别只翻一个）
 *          | 头文件 | 覆盖的节点 |
 *          |:-----|:-----|
 *          | 本文件 | 操作数叶子：`Integer`、`Float`、`HexNumber`、`BinaryNumber`、`Identifier`、`String`、四种寄存器、四种设备、`Enum`、`HashMacro`、`StrMacro`、`ErrorNode` |
 *          | `parser.hpp` | 语句级：`LabelDef`、`AliasDirective`、`DefineDirective` |
 *          | `expand_node_parser.hpp` | `@` 注解类复合节点：`Link`、`TypeHint`、`EnumAnnotation`、`DeviceAnnotation` 及子节点 |
 *
 *          @par 为何 `is` 只出现在寄存器与设备
 *          四种寄存器共享同一 FIRST 集 `{ { TokenType::REGISTER } }`，四种设备共享
 *          `{ { TokenType::DEVICE } }`：仅凭前瞻无法区分，于是它们各自提供 `is`，把区分推迟到一次
 *          只读检查（`GeneralPurposeRegister` / `AddressRegister` / `StackPointerRegister` /
 *          `DynamicRegister`，以及 `SelfReferenceDevice` / `OrdinaryDevice` / `StaticDevice` /
 *          `DynamicDevice`）。不需要消歧的节点则不写 `is` —— @ref Parser::matchPredicate 以
 *          `requires` 探测，缺失即视为「恒真」。
 *
 *          @warning 未经 @ref Parser::match 预检直接调用 `NodeParser<T>::parse` 属**契约违规**：
 *                   `parse` 内部没有首部检查，会读到不匹配的 token 并让诊断位置错位。
 *          @note `parse` 抛出的 @ref Error 由 `Parser::matchArray` 捕获并转成 `ErrorNode`
 *                （见 parser.inl），因此单个节点失败不会中断整棵语法树。
 *          @see parser.hpp 前瞻匹配机制与两条语句通道
 *          @see expand_node_parser.hpp 注解类节点与 `NodeParserDispatcher` 的用法
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Per-node parser protocol: the `NodeParser` specialization registry
 * @details This file holds **each node's own slice of parsing knowledge**. @ref Parser only owns the
 *          cursor, lookahead and diagnostics; "what this node looks like and how tokens become a
 *          node" is always answered by a `NodeParser` specialization — so the parsing logic sits
 *          next to the node definition (`ast/node.hpp`, `ast/ast.hpp`, `ast/expand_node.hpp`) instead
 *          of being collected into one giant `switch`.
 *
 *          @par The `NodeParser` contract
 *          - `static R parse(Parser&)`: builds the node. It **must be called after a
 *            @ref Parser::match lookahead pre-check**, because it does not itself decide whether the
 *            head token is right — it only consumes and constructs.
 *          - `static bool is(const Parser&)` (**optional**): a semantic predicate used to
 *            disambiguate overlapping FIRST sets, invoked by @ref Parser::matchPredicate.
 *          - Return conventions: `ShallowErrorable<T>` (i.e. `std::variant<T, ErrorNode>`) for a single
 *            node; `Errorable<...>` (nested variants flattened and deduplicated) when candidates nest.
 *
 *          @par Why some `parse` functions can be `noexcept`
 *          The tokens a `parse` uses are already guaranteed to exist by the FIRST sequence, so if it
 *          merely consumes them in order it can be `noexcept` (`Integer`, `String`,
 *          `GeneralPurposeRegister`). Nodes that must **inspect further** or can report and throw
 *          cannot (`DynamicRegister`, `DynamicDevice`, `Enum`, `LabelDef`, …). In other words
 *          `noexcept` here signals "no additional failure surface", not decoration.
 *
 *          @par Where the specializations live (three headers, so don't look in just one)
 *          | Header | Nodes covered |
 *          |:-----|:-----|
 *          | this file | operand leaves: `Integer`, `Float`, `HexNumber`, `BinaryNumber`, `Identifier`, `String`, the four registers, the four devices, `Enum`, `HashMacro`, `StrMacro`, `ErrorNode` |
 *          | `parser.hpp` | statement level: `LabelDef`, `AliasDirective`, `DefineDirective` |
 *          | `expand_node_parser.hpp` | `@`-annotation compound nodes: `Link`, `TypeHint`, `EnumAnnotation`, `DeviceAnnotation` and their children |
 *
 *          @par Why `is` exists only for registers and devices
 *          The four register types share one FIRST set `{ { TokenType::REGISTER } }` and the four
 *          device types share `{ { TokenType::DEVICE } }`: lookahead alone cannot tell them apart, so
 *          each provides `is` to defer the distinction to a read-only check
 *          (`GeneralPurposeRegister` / `AddressRegister` / `StackPointerRegister` / `DynamicRegister`,
 *          and `SelfReferenceDevice` / `OrdinaryDevice` / `StaticDevice` / `DynamicDevice`). Nodes
 *          that need no disambiguation simply omit `is` — @ref Parser::matchPredicate detects it with
 *          `requires` and treats absence as "always true".
 *
 *          @warning Calling `NodeParser<T>::parse` directly without a prior @ref Parser::match
 *                   pre-check **violates the contract**: `parse` performs no head check and will read
 *                   mismatched tokens, shifting diagnostic positions.
 *          @note An @ref Error thrown by `parse` is caught by `Parser::matchArray` and turned into an
 *                `ErrorNode` (see parser.inl), so one failing node does not abort the tree.
 *          @see parser.hpp the lookahead mechanism and the two statement lanes
 *          @see expand_node_parser.hpp annotation nodes and how they use `NodeParserDispatcher`
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_NODE_PARSER_HPP
#define IC10_COMPILER_CORE_NODE_PARSER_HPP
#pragma once

namespace stationeers::ic10 {

    class Parser;

    template<typename>
    struct NodeParser;

    /**
     * @if zh
     *
     * @brief 规则表驱动的「有序小节」解析器：用于 `@` 注解这类**多段、可重复、带固定前缀**的结构
     * @details 注解类节点（`#:` 类型提示、`@enum` / `@device` 注解块）的语法是「一串带前缀的条目，
     *          顺序不固定、部分必需、部分可重复」。手写会变成又长又难改的 if 链，于是这里把它做成
     *          一张**规则表**：每条规则 = 一个名字 + 一个前瞻匹配器 + 一个写结果的回调，
     *          @ref until 负责循环取用规则。
     *
     *          @par 用法（见 expand_node_parser.cpp）
     *          @code{.cpp}
     *          NodeParserDispatcher<EnumAnnotation> units{parser, result};
     *          units.setPrefix(TokenType::TYPE_ANNOTATION_PREFIX);          // 每条前都要求 `#>`
     *          units.add<"name", Cardinality::REQUIRED>(matcher, writer);   // 声明小节
     *          units.add<"values", Cardinality::AT_LEAST_ONCE>(...);
     *          units.until([](Parser& p) { return isEnd(p); });            // 直到块尾
     *          @endcode
     *
     *          @par 逐条规则的匹配顺序
     *          @ref until 每轮按 @c rules_ 的**登记顺序**依次询问 @c matcher，第一个命中的规则执行其
     *          @c parser 并计数，然后 `break`。因此**规则顺序即优先级**：前缀更特殊、更容易被误吞的
     *          规则要登记在前面。
     *
     *          @par 错误恢复
     *          一轮下来没有任何规则命中时报 IEP27 并消费一个 token，然后继续下一轮 —— 即
     *          「丢弃一个无法识别的条目，保住整块注解」。
     *
     *          @warning @ref Cardinality **目前只是声明、并未被本类强制执行**：`until` 会把每条规则的
     *                   命中次数记进 `counts_`，但 `cardinality` 与 `counts_` 全仓都没有读取点。
     *                   现实中 `REQUIRED` / `AT_LEAST_ONCE` 由各节点自己的 `parse` 主体检查
     *                   （例如 expand_node_parser.cpp 用「name 置空」作为违反 REQUIRED 的标志）。
     *                   因此**不要**误以为把 `Cardinality::REQUIRED` 写进 `add<>` 就获得了校验。
     *          @note MSVC 与其它编译器下 @ref add 的定义位置不同（MSVC 走类内定义，其余走
     *                node_parser.inl），这是为绕开编译器差异，语义一致。
     *          @see expand_node_parser.cpp 全部实际使用点
     *          @see Parser::expect `setPrefix` 所依赖的前缀检查
     *
     * @elseif en
     *
     * @brief Rule-table driven parser for "ordered sections": `@` annotations with several parts,
     *        repeats and a fixed prefix
     * @details Annotation nodes (`#:` type hints, `@enum` / `@device` blocks) have the grammar "a
     *          sequence of prefixed items, in loose order, some required, some repeatable". Hand
     *          writing that becomes a long, brittle if-chain, so it is expressed as a **rule table**:
     *          each rule is a name plus a lookahead matcher plus a callback that writes the result,
     *          and @ref until drives the loop.
     *
     *          @par Usage (see expand_node_parser.cpp)
     *          @code{.cpp}
     *          NodeParserDispatcher<EnumAnnotation> units{parser, result};
     *          units.setPrefix(TokenType::TYPE_ANNOTATION_PREFIX);          // require `#>` before each
     *          units.add<"name", Cardinality::REQUIRED>(matcher, writer);   // declare a section
     *          units.add<"values", Cardinality::AT_LEAST_ONCE>(...);
     *          units.until([](Parser& p) { return isEnd(p); });            // until end of block
     *          @endcode
     *
     *          @par Per-rule matching order
     *          Each @ref until round asks the rules in **registration order** and runs the first
     *          match's @c parser, counts it and `break`s. So **rule order is priority**: register the
     *          more specific, easily-swallowed rules first.
     *
     *          @par Error recovery
     *          When no rule matches in a round, IEP27 is reported, one token is consumed and the loop
     *          continues — i.e. "drop one unrecognizable item, keep the whole annotation".
     *
     *          @warning @ref Cardinality is **declared but not enforced by this class today**: @ref
     *                   until records per-rule hit counts in `counts_`, but neither `cardinality` nor
     *                   `counts_` is read anywhere in the tree. In practice `REQUIRED` /
     *                   `AT_LEAST_ONCE` are checked by each node's own `parse` body (e.g.
     *                   expand_node_parser.cpp uses an emptied `name` as the REQUIRED-violation flag).
     *                   So do **not** assume that writing `Cardinality::REQUIRED` in `add<>` buys you
     *                   validation.
     *          @note @ref add is defined in-class for MSVC and out-of-line in node_parser.inl
     *                elsewhere, to work around a compiler difference; semantics are identical.
     *          @see expand_node_parser.cpp every real use site
     *          @see Parser::expect the prefix check `setPrefix` relies on
     *
     * @endif
     */
    template<typename T>
    struct NodeParserDispatcher {
        /**
         * @if zh
         * @brief 小节的重复性声明（**当前仅供阅读，未被强制校验**，见类级 @c @warning）
         * @details `REQUIRED` 恰好一次、`OPTIONAL` 至多一次、`AT_LEAST_ONCE` 至少一次、
         *          `REPEATED` 任意次。
         * @elseif en
         * @brief Repeatability declaration of a section (**documentation only today, not enforced**,
         *        see the class-level @c @warning)
         * @details `REQUIRED` exactly once, `OPTIONAL` at most once, `AT_LEAST_ONCE` at least once,
         *          `REPEATED` any number of times.
         * @endif
         */
        enum class Cardinality { REQUIRED, OPTIONAL, AT_LEAST_ONCE, REPEATED };

        /**
         * @if zh
         * @brief 登记一条小节规则
         * @tparam N 小节名，仅用于 `counts_` 计数与阅读（出现在 `add<"name", ...>` 里）
         * @tparam C 重复性声明，见 @ref Cardinality（当前不被强制）
         * @param args 依次为 `matcher`（`std::function<bool(Parser&)>`，判断本轮是否命中本规则）与
         *             `parser`（`std::function<void(Parser&, T&)>`，命中后写结果）
         * @return `*this`，便于链式登记
         * @note 登记顺序即匹配优先级。
         * @elseif en
         * @brief Register one section rule
         * @tparam N Section name, used only for `counts_` bookkeeping and readability (appears as
         *         `add<"name", ...>`)
         * @tparam C Repeatability declaration, see @ref Cardinality (not enforced today)
         * @param args A `matcher` (`std::function<bool(Parser&)>`, does this round hit this rule?)
         *             followed by a `parser` (`std::function<void(Parser&, T&)>`, writes the result)
         * @return `*this`, so registrations can be chained
         * @note Registration order is matching priority.
         * @endif
         */
        template<FString N, Cardinality C, typename... Args>
        NodeParserDispatcher& add(Args&&... args)
        #ifdef _MSC_VER
        {
            rules_.emplace_back(std::string(N), C, std::forward<Args>(args)...);

            return *this;
        }
        #else
        ;
        #endif

        /**
         * @if zh
         * @brief 要求每一轮小节之前都出现指定前缀 token
         * @param prefix 前缀类型，例如注解块用的 `TYPE_ANNOTATION_PREFIX`（`#>`）
         * @note 前缀由 @ref until 在每轮开始时用 @ref Parser::expect 校验：缺失即报 IEP1_1 并抛出，
         *       由调用方的 `matchArray` 层转成 `ErrorNode`。
         * @elseif en
         * @brief Require a prefix token before every section round
         * @param prefix Prefix type, e.g. `TYPE_ANNOTATION_PREFIX` (`#>`) for annotation blocks
         * @note @ref until checks it at the start of each round with @ref Parser::expect: a missing
         *       prefix reports IEP1_1 and throws, converted to an `ErrorNode` by the caller's
         *       `matchArray` level.
         * @endif
         */
        void setPrefix(TokenType prefix);

        /**
         * @if zh
         * @brief 循环匹配小节，直到条件成立或输入结束
         * @param condition 终止条件：接到解析器后返回 `true` 即停止（例如「已到注解块结尾」）
         * @details 每轮顺序：① 若设了 @ref setPrefix 则校验前缀；② 询问 @c condition，成立即退出；
         *          ③ 按登记顺序找第一条命中的规则并执行；④ 都不命中则报 IEP27 并消费一个 token。
         * @note 该循环的终止还受 @ref Parser::inScope 保护，不会越过 `END`。
         * @see Cardinality 计数与「未被强制」的说明
         * @elseif en
         * @brief Loop over sections until a condition holds or the input ends
         * @param condition Stop condition: returning `true` after receiving the parser stops the loop
         *        (e.g. "reached the end of the annotation block")
         * @details Each round: ① validate the prefix if @ref setPrefix was used; ② ask `condition` and
         *          exit on true; ③ run the first rule that matches, in registration order; ④ if none
         *          matched, report IEP27 and consume one token.
         * @note The loop is also guarded by @ref Parser::inScope, so it never passes `END`.
         * @see Cardinality the counting and "not enforced" note
         * @endif
         */
        void until(std::function<bool(Parser&)> condition);

        /**
         * @if zh
         * @brief 构造：绑定要读取的解析器与要写入的结果对象
         * @param parser 借用的 @ref Parser 引用（**不接管所有权**，生命周期须长于本对象）
         * @param result 借用的结果对象引用，各规则的 `parser` 回调直接写入它
         * @warning 两个引用成员都按引用长期持有，调用方必须保证二者在 @ref until 期间存活。
         * @elseif en
         * @brief Constructor: bind the parser to read and the result object to write
         * @param parser A borrowed @ref Parser reference (**ownership is not taken**; it must outlive
         *        this object)
         * @param result A borrowed result reference that each rule's `parser` callback writes into
         * @warning Both members are held by reference for the object's lifetime, so the caller must
         *          keep both alive across @ref until.
         * @endif
         */
        NodeParserDispatcher(Parser& parser, T& result);

    private:

        /**
         * @if zh
         * @brief 被读取的解析器（借用引用）
         * @elseif en
         * @brief The parser being read (borrowed reference)
         * @endif
         */
        Parser& parser_;

        /**
         * @if zh
         * @brief 被写入的结果对象（借用引用）
         * @elseif en
         * @brief The result object being written (borrowed reference)
         * @endif
         */
        T& result_;

        /**
         * @if zh
         * @brief 一条规则：名称、重复性声明、前瞻匹配器与写结果回调
         * @note `matcher` 只做判断、不消费 token；消费由 `parser` 回调里的节点解析器负责。
         * @elseif en
         * @brief One rule: name, repeatability declaration, lookahead matcher and result writer
         * @note `matcher` only decides and consumes nothing; consumption belongs to the node parsers
         *       inside the `parser` callback.
         * @endif
         */
        struct Rule {
            std::string name;
            Cardinality cardinality;
            std::function<bool (Parser&)> matcher;
            std::function<void (Parser&, T&)> parser;
        };

        /**
         * @if zh
         * @brief 规则表，顺序即匹配优先级
         * @elseif en
         * @brief The rule table; order is matching priority
         * @endif
         */
        std::vector<Rule> rules_;

        /**
         * @if zh
         * @brief 各小节命中次数
         * @warning 目前**只写不读**：没有任何地方校验它，也未据此判定 @ref Cardinality。
         * @elseif en
         * @brief Per-section hit counts
         * @warning Currently **written but never read**: nothing validates it and @ref Cardinality is
         *          not decided from it.
         * @endif
         */
        std::unordered_map<std::string, int> counts_;

        /**
         * @if zh
         * @brief 可选的小节前缀 token（见 @ref setPrefix）
         * @elseif en
         * @brief Optional section prefix token (see @ref setPrefix)
         * @endif
         */
        std::optional<TokenType> prefix_;

    };

    // Integer

    template<>
    struct NodeParser<Integer> {
        static Integer parse(Parser& parser) noexcept;
    };

    // Float

    template<>
    struct NodeParser<Float> {
        static Float parse(Parser& parser) noexcept;
    };

    // BinaryNumber

    template<>
    struct NodeParser<BinaryNumber> {
        static ShallowErrorable<BinaryNumber> parse(Parser& parser) noexcept;
    };

    // HexNumber

    template<>
    struct NodeParser<HexNumber> {
        static HexNumber parse(Parser& parser) noexcept;
    };

    // Identifier

    template<>
    struct NodeParser<Identifier> {
        static Identifier parse(Parser& parser) noexcept;
    };

    // String

    template<>
    struct NodeParser<String> {
        static String parse(Parser& parser) noexcept;
    };

    // Register

    template<>
    struct NodeParser<GeneralPurposeRegister> {
        static GeneralPurposeRegister parse(Parser& p) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<AddressRegister> {
        static AddressRegister parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<StackPointerRegister> {
        static StackPointerRegister parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<DynamicRegister> {
        static DynamicRegister parse(Parser& parser);

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    // Device

    template<>
    struct NodeParser<SelfReferenceDevice> {
        static SelfReferenceDevice parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<OrdinaryDevice> {
        static OrdinaryDevice parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<StaticDevice> {
        static StaticDevice parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<DynamicDevice> {
        static DynamicDevice parse(Parser& parser);

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    // Enum

    template<>
    struct NodeParser<Enum> {
        static Enum parse(Parser& parser) noexcept;
    };

    // HashMacro

    template<>
    struct NodeParser<HashMacro> {
        static ShallowErrorable<HashMacro> parse(Parser& parser) noexcept;
    };

    // StrMacro

    template<>
    struct NodeParser<StrMacro> {
        static ShallowErrorable<StrMacro> parse(Parser& parser) noexcept;
    };

    // ErrorNode

    template<>
    struct NodeParser<ErrorNode> {
        static ErrorNode parse(Parser& parser) noexcept;
    };

}  // namespace stationeers::ic10

#include "node_parser.inl"

#endif  // IC10_COMPILER_CORE_NODE_PARSER_HPP
