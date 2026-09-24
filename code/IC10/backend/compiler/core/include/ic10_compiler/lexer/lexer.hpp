// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file lexer.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief IC10 词法分析器：字符级状态机，把源代码切成以 `END` 收尾的 Token 序列
 * @details 编译器**第一阶段**。产物是一个 `std::vector<std::shared_ptr<Token>>`，**最后一项必定是
 *          `TokenType::END`**（@ref Lexer::scan 的循环条件就是「直到产出 END」），语法分析器的
 *          `inScope()` 正是依赖这一约定来判断「还有没有 token」。
 *
 *          @par 工作方式：按首字符分派 + 各自向前吃字符
 *          没有一张全局状态转移表：@ref Lexer::next 先跳过行内空格，然后**按当前字符分派**给一个
 *          `extractXxx` 成员，由它自己把该 token 的字符吃干净并返回 Token。分派顺序即优先级：
 *
 *          | 首字符 | 处理函数 | 产出 |
 *          |:-----|:-----|:-----|
 *          | `\n` | 内联 | `NEWLINE`（`pos_.newline()` 记账行号，类别 WHITESPACE） |
 *          | `$` | @ref Lexer::extractHexNumber | `HEX_NUMBER` |
 *          | `%` | @ref Lexer::extractBinaryNumber | `BINARY_NUMBER` |
 *          | 数字 | @ref Lexer::extractNumber | `INTEGER` / `FLOAT` |
 *          | `"` | @ref Lexer::extractString | `STRING`，未闭合则 `UNKNOWN` |
 *          | `#` | @ref Lexer::extractHash | `TYPE_ANNOTATION_PREFIX`（`#>`）/ `TYPE_HINT_PREFIX`（`#:`）/ `HEX_COMMENT`（其它） |
 *          | `@` | @ref Lexer::extractTag | `TAG`（注解标签，如 `@builtin`、`@enum`） |
 *          | 在 `SYMBOLS` 内 | @ref Lexer::extractSymbol | 由 `SYMBOL_MAP` 决定的符号 Token |
 *          | 其它 | @ref Lexer::extractLetter | 寄存器 / 设备 / 关键字 / 标识符 |
 *
 *          @par 两个不显眼但很关键的设计
 *          1. **词元边界校验**（@ref Lexer::next 末尾）：若刚产出的是 `LITERAL`，而紧随其后的字符既不是
 *             空白、也不属于 `():-./`，就报 IEL3_1。这能抓住 `r0x`、`12abc` 这类「数字/寄存器后面粘了
 *             东西」的输入 —— 单个 `extractXxx` 只顾自己吃字符，粘错字只能在这里兜住。
 *          2. **换行即同步点**（@ref Lexer::extractString）：字符串未闭合时不消费换行符，直接报 IEL2_1
 *             并返回 `UNKNOWN`，让下一行照常继续词法化 —— 这是「一个坏字符串不该吞掉整个文件」的来源，
 *             与语法分析器行级恢复用的是同一个同步点。
 *
 *          @par 集成关系
 *          | 协作方 | 关系 |
 *          |:-----|:-----|
 *          | `lexer/token.hpp` | 产物结构：`TokenType` / `TokenCategory` / 各类关键字映射表 |
 *          | `parser/parser.hpp` | 下游：消费本文件产出的向量，依赖结尾的 `END` 与语句间的 `NEWLINE` |
 *          | `incremental/inc_lexer.hpp` | 复用：按行缓存词法结果，只重扫改动行 |
 *          | `locals/local.hpp` | 诊断文案：IEL1_1（未知符号）、IEL2_1（未闭合字符串）、IEL3_1（边界异常） |
 *          | `common/exception/diagnostic.hpp` | @ref DiagnosticReporter 收集诊断，不抛异常 |
 *
 *          @warning **数字前缀是 `$` 与 `%`**（`$FF` / `%1010`），不是 C 风格的 `0x` / `0b`：后者会被
 *                   当成普通标识符。历史版本的本文件曾写作 `0x`/`0b`，属过时说明。
 *          @warning **`@` 开头的是注解标签（`TAG`），不是设备**：设备 Token（`DEVICE`）来自
 *                   `d0`…`d5` 与自引用设备 `db`。把 `@Display` 当设备是旧方言的写法。
 *          @note 诊断一律进 @ref DiagnosticReporter（@ref getDiagnostics 取回），词法器**不抛异常**。
 *          @see Parser 语法分析器（下游）
 *          @see IncLexer 增量词法分析
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 lexer: a character-level dispatcher that cuts source into an `END`-terminated token vector
 * @details The **first stage** of the compiler. Its product is a
 *          `std::vector<std::shared_ptr<Token>>` whose **last element is always `TokenType::END`**
 *          (@ref Lexer::scan loops until an END appears), and the parser's `inScope()` relies on exactly
 *          that convention to know whether tokens remain.
 *
 *          @par How it works: dispatch on the first character, then let each extractor consume
 *          There is no global transition table: @ref Lexer::next skips intra-line spaces and then
 *          **dispatches on the current character** to an `extractXxx` member, which consumes that
 *          token's characters itself and returns the Token. Dispatch order is priority:
 *
 *          | First char | Handler | Produces |
 *          |:-----|:-----|:-----|
 *          | `\n` | inline | `NEWLINE` (`pos_.newline()` tracks the line; category WHITESPACE) |
 *          | `$` | @ref Lexer::extractHexNumber | `HEX_NUMBER` |
 *          | `%` | @ref Lexer::extractBinaryNumber | `BINARY_NUMBER` |
 *          | digit | @ref Lexer::extractNumber | `INTEGER` / `FLOAT` |
 *          | `"` | @ref Lexer::extractString | `STRING`, or `UNKNOWN` when unterminated |
 *          | `#` | @ref Lexer::extractHash | `TYPE_ANNOTATION_PREFIX` (`#>`) / `TYPE_HINT_PREFIX` (`#:`) / `HEX_COMMENT` (anything else) |
 *          | `@` | @ref Lexer::extractTag | `TAG` (annotation tags such as `@builtin`, `@enum`) |
 *          | in `SYMBOLS` | @ref Lexer::extractSymbol | a symbol token chosen by `SYMBOL_MAP` |
 *          | anything else | @ref Lexer::extractLetter | register / device / keyword / identifier |
 *
 *          @par Two low-key but crucial designs
 *          1. **Token boundary validation** (end of @ref Lexer::next): if the token just produced is a
 *             `LITERAL` and the next character is neither whitespace nor one of `():-./`, IEL3_1 is
 *             reported. This catches input like `r0x` or `12abc` — an individual `extractXxx` only
 *             consumes its own characters, so glued-on junk can only be caught here.
 *          2. **Newline as a synchronization point** (@ref Lexer::extractString): an unterminated string
 *             does not consume the newline; IEL2_1 is reported and an `UNKNOWN` token returned, so the
 *             next line still tokenizes — this is why one broken string does not swallow the file, and
 *             it is the same synchronization point the parser's line-level recovery uses.
 *
 *          @par Integration
 *          | Counterpart | Relationship |
 *          |:-----|:-----|
 *          | `lexer/token.hpp` | Product structures: `TokenType` / `TokenCategory` / keyword maps |
 *          | `parser/parser.hpp` | Downstream: consumes the vector and relies on the trailing `END` and on `NEWLINE` between statements |
 *          | `incremental/inc_lexer.hpp` | Reuse: caches lexing per line and re-scans only changed lines |
 *          | `locals/local.hpp` | Diagnostics: IEL1_1 (unknown symbol), IEL2_1 (unterminated string), IEL3_1 (bad boundary) |
 *          | `common/exception/diagnostic.hpp` | @ref DiagnosticReporter collects diagnostics; nothing is thrown |
 *
 *          @warning **The number prefixes are `$` and `%`** (`$FF` / `%1010`), not C-style `0x` / `0b` —
 *                   the latter lex as ordinary identifiers. An older revision of this file claimed
 *                   `0x`/`0b`, which is stale.
 *          @warning **A leading `@` is an annotation tag (`TAG`), not a device**: `DEVICE` tokens come
 *                   from `d0`…`d5` and the self-reference device `db`. Treating `@Display` as a device is
 *                   the old dialect.
 *          @note Diagnostics always go to the @ref DiagnosticReporter (retrieved with
 *                @ref getDiagnostics); the lexer **throws nothing**.
 *          @see Parser the parser (downstream)
 *          @see IncLexer incremental lexing
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef IC10_COMPILER_CORE_LEXER_HPP
#define IC10_COMPILER_CORE_LEXER_HPP
#pragma once

#include "common/exception/diagnostic.hpp"
#include "ic10_compiler/locals/local.hpp"
#include "token.hpp"
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @class Lexer
     * @if zh
     *
     * @brief 词法分析器：持有「源码视图 + 游标」，把源码切成 Token 序列
     * @details 一次扫描只需三步：构造（只记住源码与游标，**不解析**）→ @ref scan（反复调 @ref next
     *          直到 `END`）→ 取 @ref getDiagnostics 看问题。设计思路与集成关系见本文件头部 @c @details。
     *
     *          @par 使用示例:
     *          ```cpp
     *          using namespace stationeers::ic10;
     *
     *          // 方式 1：静态便捷入口（内部即 Lexer{src, debug}.scan()）
     *          auto tokens = Lexer::tokenize("add r0 r1 r2");
     *
     *          // 方式 2：先构造再扫描（需要单独读诊断时用这种）
     *          Lexer lexer{src};
     *          auto tokens = lexer.scan();
     *          for (const auto& d : lexer.getDiagnostics())
     *              std::cout << d.message << std::endl;
     *          ```
     *
     *          @par 识别的词法单元
     *          | 单元 | 写法 | 产出 |
     *          |:-----|:-----|:-----|
     *          | 指令关键字 | `add`、`move`、`yield` | `KEYWORD`（带 @c keyword 字段，供语法分析器直接分派） |
     *          | 其它关键字 | `alias`、`define` 等 | 由 `OTHER_KEYWORD_MAP` 决定的类型 |
     *          | 寄存器 | `r0`…`r15`、`ra`、`sp` | `REGISTER` |
     *          | 设备 | `d0`…`d5`、自引用 `db` | `DEVICE` |
     *          | 注解标签 | `@builtin`、`@enum`、`@logic` | `TAG` |
     *          | 标识符 | 字母开头 | `IDENTIFIER` |
     *          | 字符串 | `"..."` | `STRING`（控制字符会被写成 `\x` 形式，见 @ref extractString） |
     *          | 注释 | `# ...` | `HEX_COMMENT`（类别 COMMENT，值含 `#`） |
     *          | 注解前缀 | `#>`、`#:` | `TYPE_ANNOTATION_PREFIX`、`TYPE_HINT_PREFIX` |
     *          | 符号 | `:`、`.`、`(`、`)`、`-`、`/` 等 | 由 `SYMBOL_MAP` 决定 |
     *
     *          @par 数字写法
     *          | 格式 | 前缀 | 示例 | 说明 |
     *          |:-----|:-----|:-----|:-----|
     *          | 十进制 | 无 | `42` | 仅数字与单个 `.` |
     *          | 浮点 | 无 | `3.14`、`1.5e-3` | 含 `.` 即为 `FLOAT`；支持 `e`/`E` 科学计数法 |
     *          | 十六进制 | `$` | `$FF`、`$1a` | 前缀是 `$`，不是 `0x` |
     *          | 二进制 | `%` | `%1010`、`%10_10` | 前缀是 `%`，不是 `0b`；允许 `_` 分隔 |
     *
     *          @note 负号**不属于**数字：`-7` 会切成符号 `-` 与 `INTEGER 7` 两个 Token。
     *          @note 科学计数法带**回溯**：`e`/`E` 后面没跟数字时（如 `12ef`），游标与已收集字符会
     *                回退到 `e` 之前，该字符留给后续处理（见 @ref extractNumber）。
     *
     *          @attention 构造函数**不会**执行词法分析，必须调用 @ref scan 或使用 @ref tokenize。
     *          @note 非线程安全：游标 `pos_` 由 @c const 成员函数通过 `mutable` 修改，与其他
     *                `const` 方法共同构成「const 也能前进」的写法（与 @ref Parser 一致），
     *                因此一个实例不可并发使用。
     *          @see tokenize 静态便捷入口
     *          @see Parser 下游消费者
     *
     * @elseif en
     *
     * @brief The lexer: owns "a source view plus a cursor" and cuts the source into tokens
     * @details A scan is three steps: construct (records source and cursor only, **no parsing**) →
     *          @ref scan (repeatedly call @ref next until `END`) → read @ref getDiagnostics. The design
     *          rationale and integration map are in this file's header @c @details.
     *
     *          @par Usage example:
     *          ```cpp
     *          using namespace stationeers::ic10;
     *
     *          // Option 1: the static convenience entry (internally Lexer{src, debug}.scan())
     *          auto tokens = Lexer::tokenize("add r0 r1 r2");
     *
     *          // Option 2: construct then scan (use this when you also want diagnostics)
     *          Lexer lexer{src};
     *          auto tokens = lexer.scan();
     *          for (const auto& d : lexer.getDiagnostics())
     *              std::cout << d.message << std::endl;
     *          ```
     *
     *          @par Recognized lexical units
     *          | Unit | Spelling | Produces |
     *          |:-----|:-----|:-----|
     *          | Instruction keyword | `add`, `move`, `yield` | `KEYWORD` (carries @c keyword so the parser can dispatch directly) |
     *          | Other keywords | `alias`, `define`, … | whichever type `OTHER_KEYWORD_MAP` maps to |
     *          | Register | `r0`…`r15`, `ra`, `sp` | `REGISTER` |
     *          | Device | `d0`…`d5`, self-reference `db` | `DEVICE` |
     *          | Annotation tag | `@builtin`, `@enum`, `@logic` | `TAG` |
     *          | Identifier | letter-initial | `IDENTIFIER` |
     *          | String | `"..."` | `STRING` (control characters are written as `\x`, see @ref extractString) |
     *          | Comment | `# ...` | `HEX_COMMENT` (category COMMENT; the value keeps the `#`) |
     *          | Annotation prefix | `#>`, `#:` | `TYPE_ANNOTATION_PREFIX`, `TYPE_HINT_PREFIX` |
     *          | Symbol | `:`, `.`, `(`, `)`, `-`, `/`, … | decided by `SYMBOL_MAP` |
     *
     *          @par Number spellings
     *          | Format | Prefix | Example | Notes |
     *          |:-----|:-----|:-----|:-----|
     *          | Decimal | none | `42` | digits and at most one `.` |
     *          | Float | none | `3.14`, `1.5e-3` | any `.` makes it `FLOAT`; `e`/`E` scientific notation supported |
     *          | Hexadecimal | `$` | `$FF`, `$1a` | prefix is `$`, not `0x` |
     *          | Binary | `%` | `%1010`, `%10_10` | prefix is `%`, not `0b`; `_` separators allowed |
     *
     *          @note A leading minus is **not** part of the number: `-7` lexes as the symbol `-` plus
     *                `INTEGER 7`.
     *          @note Scientific notation **backtracks**: when `e`/`E` is not followed by a digit (e.g.
     *                `12ef`) the cursor and the collected characters are rolled back to before the `e`,
     *                leaving that character for whatever comes next (see @ref extractNumber).
     *
     *          @attention The constructor does **not** scan; call @ref scan or use @ref tokenize.
     *          @note Not thread-safe: the `pos_` cursor is mutated by @c const member functions through
     *                `mutable` — the same "const can still advance" style as @ref Parser — so one
     *                instance must not be shared across threads.
     *          @see tokenize the static convenience entry
     *          @see Parser the downstream consumer
     *
     * @endif
     */
    class Lexer {
    public:
        /**
         * @if zh
         *
         * @brief 构造：只记录源码视图与游标，**不执行词法分析**
         * @param src 源代码字符串视图
         * @param debug 调试标志（当前无效，见 @ref debug_）
         * @warning @p src 以 `std::string_view` **借用**保存，本对象不复制内容：源码缓冲区的生命周期
         *          必须覆盖到 @ref scan 结束，否则是悬垂视图。临时字符串字面量（`"..."`）安全，
         *          局部 `std::string` 被销毁后不安全。
         * @note `noexcept`：构造只做成员初始化。
         *
         * @elseif en
         *
         * @brief Construct: records the source view and cursor only, **no lexing happens**
         * @param src The source string view
         * @param debug Debug flag (currently inert, see @ref debug_)
         * @warning @p src is **borrowed** as a `std::string_view` and the content is not copied: the
         *          source buffer must outlive @ref scan, or the view dangles. String literals
         *          (`"..."`) are safe; a local `std::string` that goes out of scope is not.
         * @note `noexcept`: construction only initialises members.
         *
         * @endif
         */
        explicit Lexer(std::string_view src, bool debug = false) noexcept;

        /**
         * @if zh
         * @brief 默认构造：得到一个空源码、零偏移、无诊断的实例
         * @note 若随后调用 @ref scan，源码为空时会立刻产出 `END`（@ref next 对空输入的处理）。
         * @elseif en
         * @brief Default construction: an instance with empty source, zero offset and no diagnostics
         * @note If @ref scan is then called, an empty source immediately yields `END` (see how
         *       @ref next handles empty input).
         * @endif
         */
        Lexer() = default;

        /**
         * @if zh
         *
         * @brief 反复调用 @ref next 扫完整个源码，直到产出 `END`
         * @details 循环条件是「刚产出的 Token 不是 `END`」，因此返回向量的**最后一项必定是 `END`**：
         *          语法分析器的 @c inScope() 就建立在这一点上。空源码也会返回只含一个 `END` 的向量。
         * @return Token 指针向量，结尾为 `END`
         * @note 词法错误不会中断扫描：出错处产出 `UNKNOWN` Token（或继续产出正确的 Token）并把诊断
         *       记入 @ref getDiagnostics，因此**向量非空不代表没有错误**。
         * @note `[[nodiscard]]`：丢弃结果等于白扫一遍。
         * @see next 单个 Token 的产出与边界校验
         *
         * @elseif en
         *
         * @brief Repeatedly call @ref next until `END` to scan the whole source
         * @details The loop condition is "the token just produced is not `END`", so the returned
         *          vector's **last element is always `END`** — the very fact the parser's
         *          @c inScope() is built on. Empty source yields a vector holding just `END`.
         * @return A vector of token pointers ending with `END`
         * @note Lexical errors do not abort the scan: the offending position yields an `UNKNOWN` token
         *       (or scanning simply continues) and the diagnostic lands in @ref getDiagnostics, so a
         *       non-empty vector does **not** mean "no errors".
         * @note `[[nodiscard]]`: discarding the result means scanning for nothing.
         * @see next how one token is produced and boundary-checked
         *
         * @endif
         */
        [[nodiscard]] std::vector<std::shared_ptr<Token>> scan();

        /**
         * @if zh
         *
         * @brief 获取本次扫描收集到的诊断
         * @details 包含错误、警告与提示（IEL1_1 未知符号、IEL2_1 未闭合字符串、IEL3_1 词元边界异常）。
         * @return 诊断列表的常量引用（生命周期随本对象）
         * @note 与 @ref Parser::getDiagnostics 一样，调用方应把「向量里有没有 Token」和「有没有错误」
         *       当成两件事判断。
         * @elseif en
         * @brief Get the diagnostics collected by this scan
         * @details Errors, warnings and info (IEL1_1 unknown symbol, IEL2_1 unterminated string, IEL3_1
         *          bad token boundary).
         * @return Const reference to the diagnostic list (lives as long as this object)
         * @note As with @ref Parser::getDiagnostics, "the vector has tokens" and "there are no errors"
         *       are two different questions.
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const noexcept { return reporter_.getDiagnostics(); }

        /**
         * @if zh
         *
         * @brief 静态便捷入口：构造临时实例并 @ref scan 一次
         * @param src 源代码字符串视图（生命周期只需覆盖本次调用）
         * @param debug 调试标志（当前无效）
         * @return Token 指针向量，结尾为 `END`
         * @note 等价于 `Lexer{src, debug}.scan()`；这条路径拿不到诊断，需要诊断时请自行构造实例。
         * @elseif en
         * @brief Static convenience entry: construct a temporary instance and @ref scan once
         * @param src The source string view (only needs to outlive this call)
         * @param debug Debug flag (currently inert)
         * @return A vector of token pointers ending with `END`
         * @note Equivalent to `Lexer{src, debug}.scan()`; this path gives you no diagnostics — construct
         *       an instance yourself when you need them.
         * @endif
         */
        static std::vector<std::shared_ptr<Token>> tokenize(
            std::string_view src, bool debug = false
        );

    private:
        /**
         * @if zh
         * @brief 当前扫描位置（偏移 + 行/列，由 `pos_.next(c)` 与 `pos_.newline()` 维护）
         * @note `mutable`：@ref skip 与各 `extractXxx` 都是 @c const 成员却要推进它 —— 这是本类
         *       「const 方法也能前进」的原因，也是实例不可并发共享的原因。
         * @elseif en
         * @brief The current scan position (offset plus line/column, maintained by `pos_.next(c)` and
         *        `pos_.newline()`)
         * @note `mutable`: @ref skip and every `extractXxx` are @c const members that still advance it —
         *       which is why `const` methods can move forward here, and why one instance cannot be
         *       shared across threads.
         * @endif
         */
        mutable Pos pos_;

        /**
         * @if zh
         * @brief 借用的源码视图（**不拥有内容**）
         * @warning 源码缓冲区的生命周期必须覆盖 @ref scan；否则是悬垂视图。见构造函数说明。
         * @elseif en
         * @brief A borrowed view of the source (**owns nothing**)
         * @warning The source buffer must outlive @ref scan, or the view dangles. See the constructor.
         * @endif
         */
        std::string_view src_;

        /**
         * @if zh
         * @brief 调试标志（**当前无任何读取点**）
         * @warning 只由构造函数赋值、全仓无处读取，因此 `debug = true` 目前不会输出任何东西。
         *          要恢复调试输出需真正使用它（例如在 @ref next 打印分派结果）。
         * @elseif en
         * @brief Debug flag (**currently never read**)
         * @warning Assigned by the constructor and read nowhere in the tree, so `debug = true` prints
         *          nothing today. Restoring output means actually using it (e.g. printing the dispatch
         *          decision in @ref next).
         * @endif
         */
        bool debug_;

        /**
         * @if zh
         * @brief 诊断报告器
         * @details 收集词法分析过程中产生的错误、警告等诊断信息
         * @elseif en
         * @brief Diagnostic reporter
         * @details Collects errors, warnings and other diagnostic information during lexical analysis
         * @endif
         */
        mutable DiagnosticReporter<IC10CompilerMsgPack> reporter_;

        /**
         * @if zh
         *
         * @brief 读取游标处字符
         * @return 该字符；已到源码末尾时返回 `std::nullopt`
         * @note 只读，不推进游标；判空是必须的（越界即 `nullopt`）。
         *
         * @elseif en
         *
         * @brief Read the character at the cursor
         * @return That character, or `std::nullopt` at end of source
         * @note Read-only and does not advance; the null check is mandatory.
         *
         * @endif
         */
        [[nodiscard]] std::optional<char> current() const noexcept;

        /**
         * @if zh
         *
         * @brief 产出**下一个** Token：本类唯一的按字符分派点
         * @details 顺序为：① @ref skip 掉行内空格；② 若已到末尾返回 `END`；③ `\n` 直接产出 `NEWLINE`
         *          （并调 `pos_.newline()` 记账行号）；④ 否则按当前字符分派给 `extractHexNumber`
         *          （`$`）/ `extractBinaryNumber`（`%`）/ `extractNumber` / `extractString` /
         *          `extractHash` / `extractTag` / `extractSymbol` / `extractLetter`；
         *          ⑤ **词元边界校验**：若产物是 `LITERAL`，而紧随其后的字符既非空白又不在 `():-./`
         *          之内，报 IEL3_1（抓 `r0x`、`12abc` 这类粘连）。
         * @return 产出的 Token；输入耗尽时返回类别为 `END` 的 Token
         * @note 本函数不抛异常：可恢复的错误都在对应 `extractXxx` 里报诊断并产出 `UNKNOWN`。
         * @see scan 反复调用本函数直到 `END`
         *
         * @elseif en
         *
         * @brief Produce the **next** token: the single character-level dispatch point
         * @details Order: ① @ref skip intra-line spaces; ② return `END` at end of input; ③ `\n` yields
         *          `NEWLINE` directly (and calls `pos_.newline()`); ④ otherwise dispatch on the current
         *          character to `extractHexNumber` (`$`) / `extractBinaryNumber` (`%`) /
         *          `extractNumber` / `extractString` / `extractHash` / `extractTag` / `extractSymbol` /
         *          `extractLetter`; ⑤ **token boundary validation**: if the product is a `LITERAL` and the
         *          following character is neither whitespace nor one of `():-./`, report IEL3_1 (catches
         *          glued input like `r0x` or `12abc`).
         * @return The produced token; an `END`-category token once the input is exhausted
         * @note No exceptions: recoverable problems are reported inside the relevant `extractXxx` and
         *       yield an `UNKNOWN` token.
         * @see scan calls this repeatedly until `END`
         *
         * @endif
         */
        Token next();

        /**
         * @if zh
         *
         * @brief 游标是否尚未越过源码末尾
         * @return 还有字符则为 `true`
         * @note 与 @ref Parser::inScope 不同：此处只看偏移，**没有**「END 哨兵」的概念 —— 词法阶段
         *       的末尾就是字符串末尾。
         * @elseif en
         * @brief Whether the cursor has not passed the end of the source
         * @return `true` while characters remain
         * @note Unlike @ref Parser::inScope this looks only at the offset: there is no "END sentinel"
         *       notion during lexing — the end of input is simply the end of the string.
         * @endif
         */
        [[nodiscard]] bool inScope() const noexcept;

        /**
         * @if zh
         *
         * @brief 只读前瞻游标之后的第 `offset` 个字符
         * @param offset 相对偏移，默认 `1` 表示**紧随游标的下一个**字符（不是当前字符）
         * @return 该位置字符；越界返回 `std::nullopt`
         * @note 不推进游标；本函数目前只在个别分支里用于判断「下一个字符是否像数字/字母」。
         *
         * @elseif en
         * @brief Read-only lookahead of the `offset`-th character after the cursor
         * @param offset Relative offset; the default `1` is the character **right after** the cursor
         *        (not the current one)
         * @return That character, or `std::nullopt` when out of range
         * @note Does not advance the cursor; today only a few branches use it to test whether the next
         *       character looks like a digit or letter.
         *
         * @endif
         */
        [[nodiscard]] std::optional<char> peek(int offset = 1) const noexcept;

        /**
         * @if zh
         *
         * @brief 跳过行内空白
         * @details 只跳 ASCII 空格类字符，且**遇到 `\n` 立即停止** —— 换行是 Token（`NEWLINE`），
         *          不能被当空白吞掉，否则语法分析器就失去了语句分隔与行级恢复的同步点。
         * @note @c const 但会推进 `pos_`（见 @ref pos_）。
         *
         * @elseif en
         * @brief Skip intra-line whitespace
         * @details Skips ASCII space characters only and **stops immediately at `\n`** — a newline is a
         *          token (`NEWLINE`) and must not be swallowed as whitespace, or the parser would lose
         *          both its statement separator and its line-level recovery sync point.
         * @note @c const yet it advances `pos_` (see @ref pos_).
         *
         * @endif
         */
        void skip() const noexcept;

        /**
         * @if zh
         *
         * @brief 提取「字母开头的一串字符」，并判定它是寄存器、设备、关键字还是标识符
         * @details 先吃掉连续的「非空白、非符号」字符，同时用一串布尔标志记录**拼写形状**：
         *          `r` 记 `true`，`a` 与数字记 `false`，其它字符则把首字符类型改成 `'*'`（即「不是
         *          寄存器/设备拼写」）。判定顺序：
         *          1. `sp` → `REGISTER`（栈指针）；`db` → `DEVICE`（自引用设备）—— 两个特例先短路；
         *          2. 首字符为 `r` 或 `d` 时校验形状：必须含非 `r` 字符、以 `a` 结尾时只允许一个 `a`、
         *             且**不允许非 `r` 之后又出现 `r`**（`adjacent_find` 检测），全通过才产出
         *             `REGISTER` / `DEVICE`；
         *          3. 否则查 `OTHER_KEYWORD_MAP`（如 `alias`、`define`）→ 命中的类型；
         *          4. 再查 `INS_KEYWORD_MAP` → `KEYWORD`，并**带上 `keyword` 字段**，让语法分析器免于
         *             再次查表即可直接分派（见 @ref dispatch）；
         *          5. 全不命中 → `IDENTIFIER`。
         * @return 上述五类之一
         * @note 这里**不校验寄存器编号范围**（`r99` 在词法阶段仍是 `REGISTER`），范围与合法性属于
         *       语义阶段（`operand_check.hpp`）。
         * @note 形状校验失败不是错误：会继续走关键字/标识符分支，因此 `r0r` 只是普通标识符。
         *
         * @elseif en
         *
         * @brief Consume a letter-initial run and decide whether it is a register, device, keyword or
         *        identifier
         * @details It first eats consecutive "non-space, non-symbol" characters while recording the
         *          **spelling shape** in a boolean vector: `r` maps to `true`, `a` and digits to `false`,
         *          and any other character rewrites the leading type to `'*'` (meaning "not a
         *          register/device spelling"). Decision order:
         *          1. `sp` → `REGISTER` (stack pointer); `db` → `DEVICE` (self-reference device) — two
         *             special cases short-circuit first;
         *          2. when the first character is `r` or `d`, validate the shape: it must contain a
         *             non-`r`, an `a` may only terminate the run once, and an `r` **must not follow a
         *             non-`r`** (`adjacent_find`); only then does it yield `REGISTER` / `DEVICE`;
         *          3. otherwise consult `OTHER_KEYWORD_MAP` (e.g. `alias`, `define`);
         *          4. then `INS_KEYWORD_MAP` → `KEYWORD`, **carrying the `keyword` field** so the parser
         *             can dispatch without a second lookup (see @ref dispatch);
         *          5. nothing matched → `IDENTIFIER`.
         * @return One of the five kinds above
         * @note Register **number ranges are not checked here** (`r99` is still a `REGISTER` at lex time);
         *       ranges and legality belong to the semantic stage (`operand_check.hpp`).
         * @note A failed shape check is not an error: it falls through to the keyword/identifier
         *       branches, so `r0r` is simply an identifier.
         *
         * @endif
         */
        Token extractLetter() const;

        /**
         * @if zh
         *
         * @brief 提取十进制数字，并区分整数与浮点
         * @details 吃「数字与 `.`」，但**最多一个点**（`pointCount <= 1`）。随后尝试科学计数法：
         *          见到 `e`/`E` 则先记下当前位置与已收集字符，再吃可选符号与指数数字；**若指数部分没有
         *          数字，就把游标与字符一起回退到 `e` 之前**（`12ef` 里的 `e` 留给后续处理，不会吞掉）。
         * @return 含 `.` 或走了科学计数法分支时为 `FLOAT`，否则 `INTEGER`
         * @note 前导负号不属于数字（`-7` 会先切成符号 `-`），因为分派只看首字符是否为数字。
         * @note `1.2.3` 在第二个点处停下，剩下的 `.3` 由后续 Token 处理。
         *
         * @elseif en
         *
         * @brief Consume decimal digits and distinguish integer from float
         * @details It eats digits and `.` but **at most one dot** (`pointCount <= 1`). It then tries
         *          scientific notation: on `e`/`E` it saves the position and collected characters,
         *          consumes an optional sign and exponent digits, and **if the exponent has no digits it
         *          rolls the cursor and the characters back to before the `e`** (`12ef` leaves the `e`
         *          for whatever comes next instead of swallowing it).
         * @return `FLOAT` when the value contains `.` or the scientific branch was taken, else `INTEGER`
         * @note A leading minus is not part of the number (`-7` lexes as the symbol `-` first), because
         *       dispatch only looks at whether the first character is a digit.
         * @note `1.2.3` stops at the second dot; the remaining `.3` is handled by later tokens.
         *
         * @endif
         */
        Token extractNumber() const;

        /**
         * @if zh
         *
         * @brief 提取字符串字面量，并处理未闭合的情况
         * @details 从开引号吃到闭引号。过程中**不解释转义**，反而把控制/空白字符**重新编码**成 `\x`
         *          形式写进 Token 值（查 `WHITESPACE_MAP`），保证 Token 文本本身是可安全打印的单行文本。
         * @return 正常闭合时 `STRING`（值含两侧引号）
         * @note **未闭合不是抛异常**：遇到换行或输入结束时报 IEL2_1，返回 `UNKNOWN`（类别 `INVALID`），
         *       并且**不消费那个换行符** —— 换行作为同步点留给后续行，这正是「一个坏字符串不该吞掉
         *       整个文件」的实现方式（@ref Lexer::next 的分派表里也能看到换行优先处理）。
         * @warning 换行之后的内容**照常继续**词法化，因此未闭合字符串只会污染所在行。
         *
         * @elseif en
         *
         * @brief Consume a string literal and handle the unterminated case
         * @details It eats from the opening to the closing quote. Escapes are **not interpreted**;
         *          instead control/whitespace characters are **re-encoded** as `\x` text into the token
         *          value (via `WHITESPACE_MAP`), keeping the token text a safely printable single line.
         * @return `STRING` when properly closed (value includes both quotes)
         * @note **Unterminated is not an exception**: on a newline or end of input it reports IEL2_1,
         *       returns `UNKNOWN` (category `INVALID`) and **does not consume that newline** — the
         *       newline stays as a sync point for the following lines, which is exactly how one broken
         *       string avoids swallowing the file (the newline-first dispatch in @ref Lexer::next is the
         *       other half of this).
         * @warning Lexing continues normally after the newline, so an unterminated string only poisons
         *          its own line.
         *
         * @endif
         */
        Token extractString();

        /**
         * @if zh
         *
         * @brief 提取单字符符号（`:`、`.`、`(`、`)`、`-`、`/` 等）
         * @details 用 `SYMBOL_MAP` 把字符映射为 Token 类型，一次只吃一个字符。
         * @return 命中时返回对应符号 Token（类别 `SYMBOL`）
         * @note 未登记的符号不是致命错误：报 IEL1_1 并返回 `UNKNOWN`（类别 `INVALID`），同时**照常
         *       前进一个字符**，避免卡死在同一位置。
         *
         * @elseif en
         *
         * @brief Consume a single-character symbol (`:`, `.`, `(`, `)`, `-`, `/`, …)
         * @details `SYMBOL_MAP` maps the character to a token type; exactly one character is consumed.
         * @return The matching symbol token (category `SYMBOL`)
         * @note An unregistered symbol is not fatal: IEL1_1 is reported and an `UNKNOWN` token (category
         *       `INVALID`) returned, while still **advancing one character** so the lexer cannot get
         *       stuck in place.
         *
         * @endif
         */
        Token extractSymbol();

        /**
         * @if zh
         *
         * @brief 提取 `#` 开头的三种东西：文档注解前缀、类型提示前缀、普通注释
         * @details 吃完 `#` 后看下一个字符：
         *          `>` → `TYPE_ANNOTATION_PREFIX`（`#>`，类型注解块的行首）；`:` → `TYPE_HINT_PREFIX`
         *          （`#:`，类型提示）；都不是 → 一路吃到行尾，产出 `HEX_COMMENT`（类别 `COMMENT`）。
         * @return 上述三者之一；注释 Token 的值包含 `#` 与注释正文（**不含**行尾换行）
         * @note 注释不会跨行：循环在 `\n` 前停下，换行仍由 @ref next 单独产出。
         *
         * @elseif en
         *
         * @brief Consume the three `#`-initiated forms: doc-annotation prefix, type-hint prefix, comment
         * @details After eating `#` it inspects the next character: `>` →
         *          `TYPE_ANNOTATION_PREFIX` (`#>`, the line head of a type-annotation block); `:` →
         *          `TYPE_HINT_PREFIX` (`#:`, a type hint); otherwise it consumes to the end of the line
         *          and yields `HEX_COMMENT` (category `COMMENT`).
         * @return One of the three; a comment token's value contains the `#` and the body (**excluding**
         *         the trailing newline)
         * @note Comments never span lines: the loop stops before `\n`, which @ref next still emits
         *       separately.
         *
         * @endif
         */
        Token extractHash() const;

        /**
         * @if zh
         *
         * @brief 提取十六进制数字（前缀 `$`）
         * @details 吃 `$` 及其后的十六进制数字，产出 `HEX_NUMBER`，值包含 `$` 前缀。
         * @return `HEX_NUMBER` Token
         * @note 前缀是 `$` 而**不是** `0x`：`0xFF` 会被当作标识符（`0` 开头则退化为数字 + 标识符）。
         * @note `$` 后面没有十六进制数字时，仍产出值为 `"$"` 的 `HEX_NUMBER`；紧邻的异常字符随后由
         *       @ref next 的边界校验报 IEL3_1 兜住。
         *
         * @elseif en
         *
         * @brief Consume a hexadecimal number (prefix `$`)
         * @details Eats `$` plus following hex digits and yields `HEX_NUMBER`, value including the `$`.
         * @return A `HEX_NUMBER` token
         * @note The prefix is `$`, **not** `0x`: `0xFF` is treated as an identifier (or, starting with
         *       `0`, as a number followed by an identifier).
         * @note With no hex digits after `$` it still yields a `HEX_NUMBER` whose value is `"$"`; the
         *       adjacent odd character is then caught by the boundary check in @ref next (IEL3_1).
         *
         * @endif
         */
        Token extractHexNumber() const;

        /**
         * @if zh
         *
         * @brief 提取二进制数字（前缀 `%`）
         * @details 吃 `%` 及其后的二进制数字与 `_` 分隔符，产出 `BINARY_NUMBER`，值包含 `%` 前缀。
         * @return `BINARY_NUMBER` Token
         * @note 前缀是 `%` 而**不是** `0b`；`_` 只是可读性分隔符，是否合法由语义阶段判定。
         *
         * @elseif en
         *
         * @brief Consume a binary number (prefix `%`)
         * @details Eats `%` plus binary digits and `_` separators and yields `BINARY_NUMBER`, value
         *          including the `%`.
         * @return A `BINARY_NUMBER` token
         * @note The prefix is `%`, **not** `0b`; `_` is only a readability separator and its legality is
         *       decided in the semantic stage.
         *
         * @endif
         */
        Token extractBinaryNumber() const;

        /**
         * @if zh
         *
         * @brief 提取 `@` 开头的注解标签
         * @details 吃 `@` 及其后的字母与 `-`（因此 `@logic-slot` 是一个标签），产出 `TAG`。
         * @return `TAG` Token，值包含 `@`
         * @note **`@` 开头不是设备**：设备 Token 由 @ref extractLetter 从 `d0`…`d5` / `db` 产出；
         *       本函数的产物是注解标签（`@builtin`、`@enum`、`@logic` 等），供注解解析使用。
         * @note 遇到数字即停：`@Device1` 只会产出 `@Device`，`1` 留给后续 Token。
         *
         * @elseif en
         *
         * @brief Consume an `@`-initiated annotation tag
         * @details Eats `@` plus following letters and `-` (so `@logic-slot` is one tag) and yields `TAG`.
         * @return A `TAG` token whose value includes the `@`
         * @note **A leading `@` is not a device**: `DEVICE` tokens come from @ref extractLetter reading
         *       `d0`…`d5` / `db`; this function produces annotation tags (`@builtin`, `@enum`, `@logic`,
         *       …) for annotation parsing.
         * @note It stops at a digit: `@Device1` yields only `@Device`, leaving `1` for the next token.
         *
         * @endif
         */
        Token extractTag() const;

    };

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_LEXER_HPP
