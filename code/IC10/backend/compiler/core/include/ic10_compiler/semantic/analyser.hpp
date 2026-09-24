// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief IC10 语义分析器：遍历 AST，产出**符号表 + 类型表 + 诊断**
 * @details 编译器第三阶段。输入是语法分析产出的 `Program`，产物有三样：@ref SymbolTable（别名、
 *          常量、标签、内置符号）、@ref TypeTable（`#>` 注解块声明的设备/枚举类型）与诊断列表。
 *          本阶段**不做代码生成**，也不修改 AST。
 *
 *          @par 工作方式：访问者 + 具名协程
 *          语句逐条经 `std::visit` 分派到匹配的 `operator()(const T&)` 重载（叶节点也有空实现，
 *          见下）；每个处理体都是返回 `Task<>` 的**具名协程**，可以中途挂起等待前向引用。
 *
 *          @par 前向引用：整套设计的核心
 *          1. `resolveSymbol` 不会「查不到就报错」，而是 `co_await` 符号表返回的 `Future`：名字尚未
 *             定义时，符号表给出一个**未决** Future，引用方就此挂起。
 *          2. 定义出现在后面时，符号表完成该 Future，挂起的协程被恢复 —— 因此 IC10 里「先用后定义」
 *             天然成立。
 *          3. 全部语句处理完后，仍然未决的 Future 由 `SymbolTable::failAllPending()` 判定为未定义，
 *             唤醒所有等待者，最终经 `resolveSymbol` 上报 IEA3_1。
 *          4. **链接场景会推迟这次清算**：@ref Linker 以 `deferFailAllPending = true` 构造分析器，
 *             等所有编译单元合并后再自己调用 `failAllPending` —— 跨单元前向引用才能解析成功。
 *
 *          @par 为什么协程必须被持有（`detachedTasks_`）
 *          语句协程与指令操作数折叠协程都是「发后即忘」，但它们在挂起后仍需被恢复。若像早期实现那样
 *          丢弃 `std::visit` / `std::apply` 返回的 `Task`，编译器会认为协程已结束并复用其帧存储，
 *          恢复时读到被破坏的帧（GCC 实测段错误，MSVC 不复用故不显形）。因此所有 Task 都塞进
 *          @ref detachedTasks_ 持有到分析结束 —— 详见该成员的说明。
 *
 *          @par 同一原因：具名协程，不要改回协程 lambda
 *          @ref handleOperand / @ref visitStatement 被刻意写成成员协程函数，转发处用**非协程**的普通
 *          lambda。把它们改回协程 lambda 会让闭包随创建者帧失效，前向引用恢复时读到已返回的栈帧
 *          （stack-use-after-return）。这是本文件里最容易「顺手改坏」的地方，请不要动。
 *
 *          @par 指令与操作数
 *          指令走泛型重载 `operator()(Ins<V, Vs...>)`：重置设备上下文后，对 `ins.args()` 折叠调用
 *          `process<Vs>` → @ref handleOperand。操作数的**合法性**判定在
 *          `semantic/operand_check.hpp`（按 `OperandType` 特化的 `IdentifierChecker`），本文件只负责
 *          「取符号、给上下文、调用检查器」。
 *
 *          @par 集成关系
 *          | 协作方 | 关系 |
 *          |:-----|:-----|
 *          | `parser/parser.hpp` | 上游：提供 `Program`（含 `ErrorNode`，本阶段跳过而不重复报错） |
 *          | `semantic/semantic.hpp` | @ref SymbolTable / @ref Symbol：定义、解析、未决 Future 与 `failAllPending` |
 *          | `semantic/types.hpp` | @ref TypeTable / `BasicType` / `TypeCategory`：类型注册与查询 |
 *          | `semantic/operand_check.hpp` | 操作数合法性表（`IdentifierChecker` 特化） |
 *          | `link/linker.hpp` | @ref Linker：友元，共享符号表/类型表/报告器，并推迟未决清算 |
 *          | `common/async/task.hpp` | `Task` / `Promise` / `Future` 原语 |
 *          | `locals/local.hpp` | 消息编号（IEA1_2、IEA2_1、IEA3_1、IEA4、IEA6、IEA7_1…） |
 *
 *          @warning 静态入口 @ref analyse 会**丢弃**它构造的分析器实例，因此**拿不到符号表与诊断**。
 *                   需要结果时请自己构造 `Analyser` 再 `co_await visit(program)`，然后用
 *                   @ref getSymbolTable / @ref getTypeTable / @ref getDiagnostics 取。
 *          @note 非线程安全：符号表、类型表与报告器都是共享可变状态，且 `failAllPending` 恢复挂起的
 *                协程，一个实例不可并发使用。
 *          @see operand_check.hpp 操作数的语义合法性判定
 *          @see link/linker.hpp 链接场景下的构造方式
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analyser: walks the AST and produces a **symbol table, a type table and
 *        diagnostics**
 * @details The third compiler phase. Its input is the `Program` from the parser, and it produces three
 *          things: the @ref SymbolTable (aliases, constants, labels, builtins), the @ref TypeTable
 *          (device/enum types declared in `#>` annotation blocks) and a diagnostics list. This phase
 *          performs **no code generation** and does not mutate the AST.
 *
 *          @par How it works: visitors plus named coroutines
 *          Statements are dispatched one by one through `std::visit` to the matching
 *          `operator()(const T&)` overload (leaf nodes have empty bodies too — see below), and every
 *          handler is a **named coroutine** returning `Task<>` that may suspend on a forward reference.
 *
 *          @par Forward references: the core of the design
 *          1. `resolveSymbol` does not "fail when not found"; it `co_await`s a `Future` owned by the
 *             symbol table. For an as-yet undefined name the table hands out a **pending** future and the
 *             referencing site suspends.
 *          2. When the definition appears later, the table completes that future and the suspended
 *             coroutine resumes — which is why "use before define" works naturally in IC10.
 *          3. After every statement has been handled, whatever is still pending is declared undefined by
 *             `SymbolTable::failAllPending()`, waking all waiters so `resolveSymbol` reports IEA3_1.
 *          4. **Linking defers that sweep**: @ref Linker constructs the analyser with
 *             `deferFailAllPending = true` and calls `failAllPending` itself once all compilation units
 *             are merged — that is what lets cross-unit forward references resolve.
 *
 *          @par Why the coroutines must be held (`detachedTasks_`)
 *          Statement coroutines and instruction operand-fold coroutines are fire-and-forget, yet they
 *          still have to be resumed after suspension. Discarding the `Task` returned by `std::visit` /
 *          `std::apply` (as an earlier implementation did) lets the compiler treat the coroutine as
 *          finished and reuse its frame storage, so resumption reads a corrupted frame (reproduced as a
 *          segfault with GCC; MSVC does not reuse and hides it). Every Task therefore goes into
 *          @ref detachedTasks_ until the analysis ends — see that member.
 *
 *          @par Same reason: named coroutines, never turn them back into coroutine lambdas
 *          @ref handleOperand / @ref visitStatement are deliberately member coroutine functions and the
 *          forwarding sites use **non-coroutine** plain lambdas. Turning them back into coroutine
 *          lambdas lets the closure die with the creating frame, so a forward-reference resumption reads
 *          an already-returned stack frame (stack-use-after-return). This is the easiest thing to break
 *          while "tidying up" in this file — please leave it alone.
 *
 *          @par Instructions and operands
 *          Instructions go through the generic `operator()(Ins<V, Vs...>)`: it resets the device context
 *          and then folds over `ins.args()` calling `process<Vs>` → @ref handleOperand. Operand
 *          **legality** is decided in `semantic/operand_check.hpp` (the `IdentifierChecker`
 *          specializations per `OperandType`); this file only resolves symbols, supplies context and
 *          invokes the checker.
 *
 *          @par Integration
 *          | Counterpart | Relationship |
 *          |:-----|:-----|
 *          | `parser/parser.hpp` | Upstream: supplies `Program` (including `ErrorNode`s, which this phase skips instead of re-reporting) |
 *          | `semantic/semantic.hpp` | @ref SymbolTable / @ref Symbol: definition, resolution, pending futures and `failAllPending` |
 *          | `semantic/types.hpp` | @ref TypeTable / `BasicType` / `TypeCategory`: type registration and lookup |
 *          | `semantic/operand_check.hpp` | The operand legality table (`IdentifierChecker` specializations) |
 *          | `link/linker.hpp` | @ref Linker: friend, shares symbol table/type table/reporter and defers the pending sweep |
 *          | `common/async/task.hpp` | `Task` / `Promise` / `Future` primitives |
 *          | `locals/local.hpp` | Message ids (IEA1_2, IEA2_1, IEA3_1, IEA4, IEA6, IEA7_1, …) |
 *
 *          @warning The static entry @ref analyse **discards** the analyser instance it constructs, so it
 *                   gives you **no symbol table and no diagnostics**. When you need results, construct an
 *                   `Analyser` yourself, `co_await visit(program)`, then read @ref getSymbolTable /
 *                   @ref getTypeTable / @ref getDiagnostics.
 *          @note Not thread-safe: the symbol table, type table and reporter are shared mutable state, and
 *                `failAllPending` resumes suspended coroutines — one instance must not be shared.
 *          @see operand_check.hpp operand semantic legality
 *          @see link/linker.hpp how the linker constructs an analyser
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef IC10_COMPILER_CORE_ANALYSER_HPP
#define IC10_COMPILER_CORE_ANALYSER_HPP
#pragma once

#include "common/async/task.hpp"
#include "ic10_compiler/locals/local.hpp"
#include "ic10_compiler/pch/ast.hpp"
#include "ic10_compiler/pch/pch.hpp"
#include "semantic.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 把 `OperandType` 映射为它在**类型表里的全局枚举名**（主模板用哨兵 `"~"`）
     * @details 标准库给四类操作数定了同名全局枚举（`LogicType`、`LogicSlotType`、`BatchMode`、
     *          `ReagentMode`），用于「没有设备上下文时，哪些名字算合法」的判定。主模板的 `"~"` 是
     *          **约定哨兵**，表示「该操作数没有全局枚举」，由 `Analyser::checkGlobalEnum` 以
     *          `enumName != "~"` 判断是否继续查表（见 analyser.inl）。
     * @note 只有这四个特化；新增标准库枚举时需同时补这里的名字与他人共用同一名字。
     * @tparam 见 @ref OperandType
     * @see operand_type_name_v 取值入口
     * @see Analyser::checkGlobalEnum 使用点
     *
     * @elseif en
     *
     * @brief Maps an `OperandType` to its **global enum name in the type table** (primary template uses
     *        the `"~"` sentinel)
     * @details The standard library gives four operand kinds a same-named global enum (`LogicType`,
     *          `LogicSlotType`, `BatchMode`, `ReagentMode`) used to decide "which names are legal when
     *          there is no device context". The primary template's `"~"` is a **sentinel** meaning "this
     *          operand has no global enum"; `Analyser::checkGlobalEnum` tests `enumName != "~"` to
     *          decide whether to look it up (see analyser.inl).
     * @note Only these four specializations exist; adding a standard-library enum means adding its name
     *       here as well.
     * @tparam See @ref OperandType
     * @see operand_type_name_v the value accessor
     * @see Analyser::checkGlobalEnum the use site
     *
     * @endif
     */
    template<OperandType>
    struct operand_type_name {
        static constexpr auto value = "~"_fs;
    };

    template<>
    struct operand_type_name<OperandType::LOGIC_SLOT_PROP> {
        static constexpr auto value = "LogicSlotType"_fs;
    };

    template<>
    struct operand_type_name<OperandType::LOGIC_PROP> {
        static constexpr auto value = "LogicType"_fs;
    };

    template<>
    struct operand_type_name<OperandType::AGG_MODE> {
        static constexpr auto value = "BatchMode"_fs;
    };

    template<>
    struct operand_type_name<OperandType::REAGENT_MODE> {
        static constexpr auto value = "ReagentMode"_fs;
    };

    /**
     * @if zh
     * @brief @ref operand_type_name 的取值别名（编译期常量）
     * @elseif en
     * @brief Value alias of @ref operand_type_name (a compile-time constant)
     * @endif
     */
    template<OperandType V>
    inline constexpr auto operand_type_name_v = operand_type_name<V>::value;

    /**
     * @if zh
     *
     * @class Analyser
     * @brief IC10 语义分析器：把 AST 走一遍，填出符号表与类型表
     * @details 设计动机、前向引用机制、协程帧生命周期约束与集成关系都在本文件头部的 @c @details
     *          里，这里是使用面的摘要：**一个入口（@ref visit）、三份产物（符号表 / 类型表 /
     *          诊断）**，所有节点处理体都是 `operator()` 重载上的具名协程。
     *
     *          @par 使用示例（需要结果时用这种写法）
     *          @code
     *          auto tokens  = Lexer::tokenize(source);
     *          auto program = Parser::parsing(tokens);
     *
     *          Analyser analyser;                      // 自持符号表/类型表/报告器
     *          co_await analyser.visit(program);       // 分析（可挂起，等待前向引用）
     *
     *          for (const auto& d : analyser.getDiagnostics())  // 语义错误在这里
     *              std::cout << d.message << std::endl;
     *
     *          auto& symbols = analyser.getSymbolTable();       // 别名/常量/标签/内置符号
     *          auto& types   = analyser.getTypeTable();         // 设备/枚举注解类型
     *          @endcode
     *
     *          @par 静态入口（只跑不管结果时用）
     *          @code
     *          co_await Analyser::analyse(program);    // 内部构造临时实例，结束后即析构
     *          @endcode
     *
     *          @warning @ref analyse 丢弃其内部实例，**取不到符号表与诊断**；要结果请按上面的「使用
     *                   示例」自行构造。另外 @ref visit 是协程，必须在协程上下文中 `co_await`。
     *          @note 资源归属由 @ref ownsResources_ 决定：默认构造时自持三份资源并在析构时释放；
     *                @ref Linker 通过私有构造函数**借用**自己那套，故不会被本类释放。
     *          @note 非线程安全，理由见文件头。
     *          @see visit 分析入口
     *          @see operand_check.hpp 操作数合法性
     *          @see Linker 共享资源的链接场景
     *
     * @elseif en
     *
     * @class Analyser
     * @brief IC10 semantic analyser: walk the AST once and fill in the symbol and type tables
     * @details The motivation, the forward-reference machinery, the coroutine frame lifetime constraints
     *          and the integration map all live in this file's header @c @details; here is the usage
     *          summary: **one entry (@ref visit), three products (symbol table / type table /
     *          diagnostics)**, with every node handler being a named coroutine on an `operator()`
     *          overload.
     *
     *          @par Usage example (use this shape when you need the results)
     *          @code
     *          auto tokens  = Lexer::tokenize(source);
     *          auto program = Parser::parsing(tokens);
     *
     *          Analyser analyser;                      // owns its symbol/type tables and reporter
     *          co_await analyser.visit(program);       // may suspend on forward references
     *
     *          for (const auto& d : analyser.getDiagnostics())  // semantic errors land here
     *              std::cout << d.message << std::endl;
     *
     *          auto& symbols = analyser.getSymbolTable();       // aliases/constants/labels/builtins
     *          auto& types   = analyser.getTypeTable();         // device/enum annotation types
     *          @endcode
     *
     *          @par Static entry (for "just run it")
     *          @code
     *          co_await Analyser::analyse(program);    // builds a temporary, destroyed on return
     *          @endcode
     *
     *          @warning @ref analyse discards its internal instance, so it yields **no symbol table and no
     *                   diagnostics**; construct your own as shown above when you need them. Also,
     *                   @ref visit is a coroutine and must be `co_await`ed from a coroutine context.
     *          @note Ownership is decided by @ref ownsResources_: default construction owns the three
     *                resources and frees them in the destructor, while @ref Linker **borrows** its own set
     *                through the private constructor, so this class never frees those.
     *          @note Not thread-safe; see the file header for why.
     *          @see visit the analysis entry
     *          @see operand_check.hpp operand legality
     *          @see Linker the linking scenario with shared resources
     *
     * @endif
     */
    class Analyser : public std::enable_shared_from_this<Analyser> {
    public:
        /**
         * @if zh
         * @brief 构造一个**自持资源**的分析器
         * @details 内部新建 @ref SymbolTable、@ref TypeTable 与 @ref DiagnosticReporter，并把
         *          @ref ownsResources_ 置为 `true`，三者随本对象析构释放。需要读取分析结果时用这个
         *          构造函数。
         * @note 链接场景不走这里：@ref Linker 用私有构造函数借用共享资源，见文件头说明。
         * @see ~Analyser 释放时机
         * @elseif en
         * @brief Construct an analyser that **owns its resources**
         * @details Internally allocates a @ref SymbolTable, a @ref TypeTable and a
         *          @ref DiagnosticReporter, sets @ref ownsResources_ to `true`, and frees all three in
         *          the destructor. Use this constructor when you need to read the results.
         * @note Linking does not go through here: @ref Linker uses the private constructor to borrow
         *       shared resources, as described in the file header.
         * @see ~Analyser when they are released
         * @endif
         */
        Analyser();

        /**
         * @if zh
         * @brief 析构：**仅当自己是资源的拥有者时**释放符号表、类型表与报告器
         * @warning 借用构造（@ref Linker）的实例不会释放任何东西；同时析构前应确保挂起的协程已
         *          被 `failAllPending` 恢复完毕（见 @ref detachedTasks_），否则会销毁仍在挂起中的
         *          协程帧。
         * @elseif en
         * @brief Destructor: frees the symbol table, type table and reporter **only when this instance
         *        owns them**
         * @warning A borrowed-construction instance (@ref Linker) frees nothing; also make sure suspended
         *          coroutines have been resumed by `failAllPending` before destruction (see
         *          @ref detachedTasks_), or frames of still-suspended coroutines are destroyed.
         * @endif
         */
        ~Analyser();

        /**
         * @if zh
         *
         * @brief 静态分析入口：构造临时实例并走完整棵树
         * @details 等价于 `co_await Analyser{}.visit(program)`，实例在返回时即析构。
         * @param program 待分析的程序 AST
         * @return 协程任务
         * @warning **本入口丢弃实例，因此拿不到符号表、类型表与诊断** —— 只适合「跑一遍看看有没有
         *          崩溃」的场景。要结果请自行构造 `Analyser` 并 `co_await visit(program)`。
         * @note 必须在协程上下文中 `co_await`。
         *
         * @elseif en
         *
         * @brief Static analysis entry: build a temporary instance and walk the whole tree
         * @details Equivalent to `co_await Analyser{}.visit(program)`; the instance is destroyed on
         *          return.
         * @param program Program AST to analyze
         * @return Coroutine task
         * @warning **This entry discards the instance, so you get no symbol table, no type table and no
         *          diagnostics** — it only suits "run it and see whether it crashes". For results,
         *          construct an `Analyser` yourself and `co_await visit(program)`.
         * @note Must be `co_await`ed from a coroutine context.
         *
         * @endif
         */
        static Task<> analyse(const Program& program);

        /**
         * @if zh
         *
         * @brief 获取符号表
         * @return 符号表引用；默认构造时为本对象所有，@ref Linker 场景下为共享的那一份
         * @note `const` 只约束本类指针不被改写，**返回的是可变引用** —— 符号表本身仍可被修改。
         * @elseif en
         * @brief Get the symbol table
         * @return A reference; owned by this object under default construction, or the shared one in the
         *         @ref Linker scenario
         * @note `const` only means this class's pointer is not reassigned — **a mutable reference is
         *       returned**, so the table itself can still be modified.
         * @endif
         */
        SymbolTable& getSymbolTable() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取本次分析累积的诊断
         * @return 诊断列表的常引用（错误、警告、提示；消息编号见 `locals/local.hpp`）
         * @note 与语法阶段同理：**符号表非空不代表没有错误**，语义错误只在这里体现。
         * @elseif en
         * @brief Get the diagnostics accumulated by this analysis
         * @return Const reference to the list (errors, warnings, info; message ids in `locals/local.hpp`)
         * @note As in the syntax stage, **a non-empty symbol table does not mean "no errors"** — semantic
         *       errors show up only here.
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取类型表
         * @return 类型表引用；来源同 @ref getSymbolTable
         * @details 装载 `#>` 注解块声明的设备/枚举类型（由 @ref operator()(const DeviceAnnotation&) 与
         *          @ref operator()(const EnumAnnotation&) 注册），是操作数合法性判定的数据来源。
         * @elseif en
         * @brief Get the type table
         * @return A reference; sourced like @ref getSymbolTable
         * @details Holds the device/enum types declared in `#>` blocks (registered by
         *          @ref operator()(const DeviceAnnotation&) and @ref operator()(const EnumAnnotation&)) and
         *          is the data source for operand legality checks.
         * @endif
         */
        TypeTable& getTypeTable() const noexcept;

        /**
         * @if zh
         *
         * @brief 遍历整个程序（分析入口）
         * @details 对每条语句 `std::visit` 到对应的 `operator()` 具名协程，把返回的 `Task` 存入
         *          @ref detachedTasks_（**必须持有**，理由见该成员）；遍历结束后，若
         *          @ref deferFailAllPending_ 为 `false`，调用 `SymbolTable::failAllPending()` 把仍未决
         *          的符号判为未定义。
         * @param program 程序 AST
         * @return 协程任务；`co_await` 返回时**所有语句协程均已结束**（否则析构会销毁挂起中的帧）
         * @note 语句处理体是具名协程 @ref visitStatement，转发用的 lambda 刻意**不是**协程，
         *       原因见文件头「不要改回协程 lambda」。
         * @see analyse 丢弃结果的静态入口
         *
         * @elseif en
         *
         * @brief Walk the whole program (the analysis entry)
         * @details Each statement is `std::visit`ed to the matching named `operator()` coroutine and the
         *          returned `Task` is stored in @ref detachedTasks_ (**holding it is mandatory**, see that
         *          member). After the walk, if @ref deferFailAllPending_ is `false`,
         *          `SymbolTable::failAllPending()` declares every still-pending symbol undefined.
         * @param program The program AST
         * @return A coroutine task; when the `co_await` returns, **every statement coroutine has
         *         finished** (otherwise destruction would destroy suspended frames)
         * @note The statement handler is the named coroutine @ref visitStatement and the forwarding
         *       lambda is deliberately **not** a coroutine; see "never turn them back into coroutine
         *       lambdas" in the file header.
         * @see analyse the static entry that discards results
         *
         * @endif
         */
        Task<> visit(const Program& program);

    private:
        friend class Linker;

        /**
         * @if zh
         * @brief **借用**外部资源构造（仅供 @ref Linker 使用，故为私有）
         * @details 链接器需要多个编译单元共用同一套符号表/类型表/报告器，因此这里只存指针而不接管
         *          所有权（@ref ownsResources_ 保持 `false`）。
         * @param typeTable 借用的类型表
         * @param symbolTable 借用的符号表
         * @param reporter 借用的诊断报告器
         * @param deferFailAllPending 为 `true` 时**不在本单元结束时**清算未决符号，交由 @ref Linker
         *        在所有单元合并后统一调用 `failAllPending`；这是跨单元前向引用能解析成功的前提
         * @warning 三个引用对象的生命周期必须覆盖本分析器；借来的资源不会被本类释放。
         * @elseif en
         * @brief Construct by **borrowing** external resources (private because it is @ref Linker-only)
         * @details The linker needs several compilation units to share one symbol table, type table and
         *          reporter, so this only stores pointers and takes no ownership
         *          (@ref ownsResources_ stays `false`).
         * @param typeTable Borrowed type table
         * @param symbolTable Borrowed symbol table
         * @param reporter Borrowed diagnostic reporter
         * @param deferFailAllPending When `true`, pending symbols are **not** swept at the end of this
         *        unit; @ref Linker calls `failAllPending` itself once every unit is merged, which is what
         *        lets cross-unit forward references resolve
         * @warning The three referenced objects must outlive this analyser, and are never freed here.
         * @endif
         */
        Analyser(
            TypeTable& typeTable, SymbolTable& symbolTable,
            DiagnosticReporter<IC10CompilerMsgPack>& reporter, bool deferFailAllPending = false
        ) noexcept;

        /**
         * @if zh
         * @brief 本对象是否拥有 @ref symbolTable_ / @ref typeTable_ / @ref reporter_
         * @details 默认构造（@ref Analyser）置 `true`，借用构造（@ref Linker）保持 `false`；
         *          @ref ~Analyser 据此决定是否 `delete`。
         * @elseif en
         * @brief Whether this object owns @ref symbolTable_ / @ref typeTable_ / @ref reporter_
         * @details Default construction (@ref Analyser) sets `true`; borrowed construction
         *          (@ref Linker) leaves it `false`. @ref ~Analyser decides whether to `delete` from it.
         * @endif
         */
        bool ownsResources_ = false;

        /**
         * @if zh
         * @brief 是否推迟「未决符号」清算（链接场景为 `true`）
         * @details `false` 时 @ref visit 在遍历完语句后立刻调用 `SymbolTable::failAllPending()`；
         *          `true` 时由 @ref Linker 在合并完所有单元后再调用。设为 `true` 而忘记调用
         *          `failAllPending`，会让未定义符号**静默消失**（既不报 IEA3_1，等待者永远挂起）。
         * @elseif en
         * @brief Whether to defer the pending-symbol sweep (the linker passes `true`)
         * @details When `false`, @ref visit calls `SymbolTable::failAllPending()` right after walking the
         *          statements; when `true`, @ref Linker calls it after merging every unit. Setting `true`
         *          and forgetting that call makes undefined symbols **silently vanish** (no IEA3_1 is
         *          reported and waiters stay suspended forever).
         * @endif
         */
        bool deferFailAllPending_ = false;

        /**
         * @if zh
         * @brief 符号表（拥有或借用，见 @ref ownsResources_）
         * @elseif en
         * @brief The symbol table (owned or borrowed, see @ref ownsResources_)
         * @endif
         */
        SymbolTable* symbolTable_;

        /**
         * @if zh
         * @brief 类型表（拥有或借用，见 @ref ownsResources_）
         * @elseif en
         * @brief The type table (owned or borrowed, see @ref ownsResources_)
         * @endif
         */
        TypeTable* typeTable_;

        /**
         * @if zh
         * @brief 已解析的设备符号 + 它在源码中的范围
         * @details `pendingDeviceSymbol_` 表示「当前指令的设备上下文」：指令的第一个设备型操作数
         *          建立它，随后的逻辑属性/槽位类操作数据此判定（见 @ref handleOperand），
         *          @ref operator()(const Ins<V, Vs...>&) 在处理每条指令前会重置它。
         * @elseif en
         * @brief A resolved device symbol plus its source range
         * @details `pendingDeviceSymbol_` is "the device context of the current instruction": the first
         *          device-like operand establishes it and subsequent logic-property/slot operands are
         *          judged against it (see @ref handleOperand);
         *          @ref operator()(const Ins<V, Vs...>&) resets it before each instruction.
         * @endif
         */
        struct DeviceSymbol {
            std::shared_ptr<Symbol> symbol;

            Pos start;

            Pos end;
        };

        /**
         * @if zh
         * @brief 当前指令的设备上下文（可为空：无设备上下文时走标准库全局枚举判定）
         * @elseif en
         * @brief The current instruction's device context (may be empty: without one, the decision falls
         *        back to the standard library's global enums)
         * @endif
         */
        std::optional<DeviceSymbol> pendingDeviceSymbol_;

        /**
         * @if zh
         *
         * @brief 发后即忘协程的持有者
         * @details 语句协程与指令操作数折叠协程在挂起后需要被恢复（前向引用），
         *          因此它们的Task必须一直存活到分析结束：若像早期实现那样直接丢弃
         *          `std::apply`/`std::visit` 返回的Task，编译器会认为该协程生命周期已结束，
         *          进而复用其帧内存储（GCC实测在恢复时读到被破坏的帧并段错误；MSVC不复用
         *          故不显形）。持有Task即让"帧生命周期"对优化器可见。
         * @note 在分析结束（failAllPending 之后）随Analyser析构释放
         *
         * @elseif en
         *
         * @brief Owner of fire-and-forget coroutines
         * @details Statement coroutines and instruction operand-fold coroutines must be resumed
         *          after suspension (forward references), so their Tasks must stay alive until the
         *          end of the analysis. Discarding the Task returned by `std::apply`/`std::visit`
         *          (as an earlier implementation did) lets the compiler treat the coroutine as
         *          finished and reuse its frame storage; resumed later, it then reads a corrupted
         *          frame (reproduced as a segfault with GCC; MSVC does not reuse and hides it).
         *          Holding the Task makes the frame's lifetime visible to the optimizer.
         * @note Released with the Analyser after the analysis ends (after failAllPending)
         *
         * @endif
         */
        std::vector<Task<>> detachedTasks_;

        mutable DiagnosticReporter<IC10CompilerMsgPack>* reporter_;

        /**
         * @if zh
         *
         * @brief 解析符号
         * @param name 符号名
         * @param pos 引用位置
         * @return 符号指针的Future
         *
         * @elseif en
         *
         * @brief Resolve symbol
         * @param name Symbol name
         * @param pos Reference position
         * @return Future of symbol pointer
         *
         * @endif
         */
        Task<std::shared_ptr<Symbol>> resolveSymbol(const std::string& name, const Pos& pos) const;

        /**
         * @if zh
         *
         * @brief 定义符号
         * @param identifier 标识符节点
         * @param symbol 符号实例
         *
         * @elseif en
         *
         * @brief Define symbol
         * @param identifier Identifier node
         * @param symbol Symbol instance
         *
         * @endif
         */
        void defineSymbol(const Identifier& identifier, Symbol&& symbol) const;

        /**
         * @if zh
         *
         * @brief 访问标签定义节点
         * @param labelDef 标签定义节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit label definition node
         * @param labelDef Label definition node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const LabelDef& labelDef);

        /**
         * @if zh
         *
         * @brief 访问alias指令节点
         * @param aliasDirective alias指令节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit alias directive node
         * @param aliasDirective Alias directive node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const AliasDirective& aliasDirective);

        /**
         * @if zh
         *
         * @brief 访问define指令节点
         * @param defineDirective define指令节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit define directive node
         * @param defineDirective Define directive node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const DefineDirective& defineDirective);

        /**
         * @if zh
         * @brief 注册 `@device` 注解：把设备类型（含 `logics` / `logicSlots` / `slots`）写入类型表
         * @param deviceAnnotation 设备注解节点
         * @return 协程任务（无挂起点）
         * @see TypeTable::registerType
         * @elseif en
         * @brief Register a `@device` annotation: writes the device type (with `logics` / `logicSlots` /
         *        `slots`) into the type table
         * @param deviceAnnotation The device annotation node
         * @return A coroutine task (no suspension point)
         * @see TypeTable::registerType
         * @endif
         */
        Task<> operator()(const DeviceAnnotation& deviceAnnotation);

        /**
         * @if zh
         * @brief 注册 `@enum` 注解：把枚举类型（标准库 `LogicType` / `BatchMode` 等）写入类型表
         * @param enumAnnotation 枚举注解节点
         * @return 协程任务（无挂起点）
         * @see TypeTable::registerType
         * @elseif en
         * @brief Register an `@enum` annotation: writes the enum type (standard-library `LogicType` /
         *        `BatchMode`, …) into the type table
         * @param enumAnnotation The enum annotation node
         * @return A coroutine task (no suspension point)
         * @see TypeTable::registerType
         * @endif
         */
        Task<> operator()(const EnumAnnotation& enumAnnotation);

        /**
         * @if zh
         *
         * @brief 访问通用指令节点
         * @tparam Ins 指令模板类型
         * @tparam V 指令名FString值
         * @tparam Args 操作数类型包
         * @param ins 指令节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit generic instruction node
         * @tparam Ins Instruction template type
         * @tparam V Instruction name FString value
         * @tparam Args Operand type pack
         * @param ins Instruction node
         * @return Coroutine task
         *
         * @endif
         */
        template<template<auto, auto...> class Ins, FString V, OperandType... Vs>
            requires IsInstruction<Ins<V, Vs...>>
        Task<> operator()(const Ins<V, Vs...>& ins);

        /**
         * @if zh
         *
         * @brief 访问str宏调用节点
         * @param strCall str宏调用节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit str macro call node
         * @param strCall Str macro call node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const StrMacro& strCall);

        /**
         * @if zh
         *
         * @brief 访问hash宏调用节点
         * @param hashCall hash宏调用节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit hash macro call node
         * @param hashCall Hash macro call node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const HashMacro& hashCall);

        /**
         * @if zh
         * @brief 设备叶节点访问器（自引用 `db` / 普通端口 / 静态 `d0`…`d5` / 动态端口）
         * @details 四者都是空实现：设备本身的语义（类型、可访问属性）由**别名定义处**与
         *          @ref handleOperand 的设备上下文判定负责，这里只保证「有对应重载、不落入泛型
         *          fallback」。**不要删除**：删掉会走 @ref operator()(T&&) 并误报 IEA6。
         * @return 协程任务（无挂起点）
         * @elseif en
         * @brief Device leaf visitors (self-reference `db` / ordinary port / static `d0`…`d5` / dynamic port)
         * @details All four are empty: a device's semantics (type, accessible properties) belong to the
         *          **alias definition site** and to @ref handleOperand's device-context decision; these
         *          exist so an overload is found and the generic fallback is not. **Do not delete them** —
         *          without them the call lands on @ref operator()(T&&) and a spurious IEA6 is reported.
         * @return A coroutine task (no suspension point)
         * @endif
         */
        Task<> operator()(const SelfReferenceDevice& device);

        Task<> operator()(const OrdinaryDevice& device);

        Task<> operator()(const StaticDevice& device);

        Task<> operator()(const DynamicDevice& device);

        /**
         * @if zh
         *
         * @brief 访问枚举操作数节点（如 @c Foo.Bar ）
         * @details 目前不做语义校验，仅用于避免落入泛型 fallback 误报 IEA6。
         * @param enumNode 枚举节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit an enum operand node (e.g. @c Foo.Bar )
         * @details No semantic validation is performed yet; this visitor only keeps enum operands
         *          from falling into the generic fallback that reports a spurious IEA6.
         * @param enumNode Enum node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Enum& enumNode);

        /**
         * @if zh
         * @brief 寄存器叶节点访问器（`r0`…`r15` / `ra` / `sp` / 动态寄存器）
         * @details 与设备叶节点同理：寄存器名无需符号解析，这里只做「有重载」的占位，避免落入
         *          @ref operator()(T&&) 误报 IEA6。**不要删除**。
         * @return 协程任务（无挂起点）
         * @elseif en
         * @brief Register leaf visitors (`r0`…`r15` / `ra` / `sp` / dynamic register)
         * @details As with the device leaves: register names need no symbol resolution, so these are
         *          placeholders that keep the call off @ref operator()(T&&) and away from a spurious IEA6.
         *          **Do not delete them.**
         * @return A coroutine task (no suspension point)
         * @endif
         */
        Task<> operator()(const GeneralPurposeRegister& register_);

        Task<> operator()(const AddressRegister& register_);

        Task<> operator()(const StackPointerRegister& register_);

        Task<> operator()(const DynamicRegister& register_);

        /**
         * @if zh
         *
         * @brief 访问字符串节点
         * @param string 字符串节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit string node
         * @param string String node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const String& string);

        /**
         * @if zh
         *
         * @brief 访问标识符节点
         * @param identifier 标识符节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit identifier node
         * @param identifier Identifier node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Identifier& identifier);

        /**
         * @if zh
         *
         * @brief 访问二进制数节点
         * @param binaryNumber 二进制数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit binary number node
         * @param binaryNumber Binary number node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const BinaryNumber& binaryNumber);

        /**
         * @if zh
         *
         * @brief 访问十六进制数节点
         * @param hexNumber 十六进制数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit hexadecimal number node
         * @param hexNumber Hexadecimal number node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const HexNumber& hexNumber);

        /**
         * @if zh
         *
         * @brief 访问浮点数节点
         * @param float_ 浮点数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit float number node
         * @param float_ Float number node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Float& float_);

        /**
         * @if zh
         *
         * @brief 访问整数节点
         * @param integer 整数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit integer node
         * @param integer Integer node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Integer& integer);

        /**
         * @if zh
         *
         * @brief 访问错误节点
         * @param errorNode 错误节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit error node
         * @param errorNode Error node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const ErrorNode& errorNode);

        /**
         * @if zh
         *
         * @brief 泛型节点访问器
         * @tparam T 节点类型
         * @param node 节点引用
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Generic node visitor
         * @tparam T Node type
         * @param node Node reference
         * @return Coroutine task
         *
         * @endif
         */
        template<typename T>
        Task<> operator()(T&& node);

        /**
         * @if zh
         * @brief 单操作数处理入口：按操作数**变体**萃取真实节点类型后转交 @ref handleOperand
         * @details 操作数的候选集合是变体（`Errorable<...>`），这里用 `std::visit` 把变体里的实际类型
         *          提取出来，再以 `Type` 作为该位置的语义操作数类型调用 @ref handleOperand<Type>。
         * @tparam Type 该操作数位置的语义类型（来自指令定义 `Ins<K, Vs...>` 的 `Vs`）
         * @param variant 已解析出的操作数变体
         * @return 协程任务
         * @see handleOperand 具体处理体
         * @elseif en
         * @brief The per-operand entry: extract the real node type out of the operand **variant** and
         *        hand it to @ref handleOperand
         * @details An operand's candidate set is a variant (`Errorable<...>`), so `std::visit` pulls out
         *          the concrete type and then calls @ref handleOperand<Type> with `Type` as this
         *          position's semantic operand type.
         * @tparam Type The semantic type of this operand position (the `Vs` of the instruction's
         *         `Ins<K, Vs...>`)
         * @param variant The parsed operand variant
         * @return A coroutine task
         * @see handleOperand the body that does the work
         * @endif
         */
        template<OperandType Type>
        Task<> process(const auto& variant);

        /**
         * @if zh
         *
         * @brief 单操作数处理体（具名协程）
         * @details 由 process 转发调用。**不得改写回协程 lambda**：协程 lambda 的闭包存放在
         *          创建者的帧里，创建者一旦返回（或其帧被编译器省略到栈上），闭包即失效；
         *          而本协程会在前向引用处挂起、稍后被恢复，届时读取闭包会命中已返回的栈帧
         *          （GCC 下实测为 stack-use-after-return 导致段错误；MSVC 对闭包的处理不同，
         *          故该缺陷只在部分编译器上暴露）。具名协程的参数直接存放在自身帧中，
         *          不依赖创建者帧的存活。
         *
         * @elseif en
         *
         * @brief Single-operand handling body (named coroutine)
         * @details Called by process. **Must not be turned back into a coroutine lambda**: a
         *          coroutine lambda's closure lives in the creating frame; once that creator
         *          returns (or its frame is elided onto the stack) the closure is dead, while this
         *          coroutine is suspended on a forward reference and resumed later, so reading the
         *          closure hits a returned stack frame (measured as a stack-use-after-return
         *          segfault with GCC; MSVC treats closures differently, so it only shows on some
         *          compilers). A named coroutine keeps its parameters in its own frame.
         *
         * @endif
         */
        template<OperandType Type, typename T>
        Task<> handleOperand(const T& arg);

        /**
         * @if zh
         *
         * @brief 单条语句访问（具名协程）
         * @details 由 visit 转发调用，理由同 handleOperand：避免协程 lambda 的闭包随创建者帧失效。
         *
         * @elseif en
         *
         * @brief Single-statement visit (named coroutine)
         * @details Called by visit, for the same reason as handleOperand: avoid a coroutine lambda
         *          whose closure dies with the creating frame.
         *
         * @endif
         */
        template<typename T>
        Task<> visitStatement(const T& arg);

        template<OperandType>
        struct IdentifierChecker {
            static bool check(
                const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
            ) noexcept;
        };

        template<ICMsgId I, auto... Vs>
            requires(
                ...
                && (std::is_same_v<decltype(Vs), BasicType>
                    || std::is_same_v<decltype(Vs), TypeCategory>)
            )
        bool checkOperandType(const std::shared_ptr<Symbol>& symbol, auto&& arg) const;

        /**
         * @if zh
         * @brief 用**设备上下文**判定标准库型标识符（`LOGIC_PROP` 等）
         * @details 当别名携带设备注解时，按该设备的 `logics` / `logicSlots` / `slots` 判定名字是否合法；
         *          返回 `true` 表示**已判定完毕**，调用方应跳过通用 `IdentifierChecker`（analyser.inl 中
         *          `handleOperand` 的 `if (… && checkWithDeviceContext<Type>(…)) co_return;`）。
         * @tparam Type 操作数语义类型
         * @param currentSym 待判定的符号（无设备上下文时是 dummy）
         * @param devSym 提供上下文的设备符号
         * @param start 操作数起始位置（用于诊断）
         * @param end 操作数结束位置（用于诊断）
         * @return `true` 表示本函数已给出结论（可能已报错），调用方不应再走通用检查
         * @elseif en
         * @brief Judge a standard-library identifier (`LOGIC_PROP`, …) against the **device context**
         * @details When the alias carries a device annotation, the name is checked against that device's
         *          `logics` / `logicSlots` / `slots`; returning `true` means **the matter is settled** and
         *          the caller must skip the generic `IdentifierChecker` (see `handleOperand`'s
         *          `if (… && checkWithDeviceContext<Type>(…)) co_return;` in analyser.inl).
         * @tparam Type The operand semantic type
         * @param currentSym The symbol under test (a dummy when there is no device context)
         * @param devSym The device symbol supplying context
         * @param start Operand start (for diagnostics)
         * @param end Operand end (for diagnostics)
         * @return `true` when this call has concluded the matter (possibly reporting an error), in which
         *         case the caller must not run the generic checks
         * @endif
         */
        template<OperandType Type>
        bool checkWithDeviceContext(
            const Symbol& currentSym, const Symbol& devSym, const Pos& start, const Pos& end
        ) const;

        /**
         * @if zh
         * @brief 用**全局枚举**判定标准库型标识符（无设备上下文时的兜底）
         * @details 枚举名取自 @ref operand_type_name_v；为其哨兵 `"~"` 时直接返回
         *          `false`（表示该操作数没有全局枚举，交给通用检查）。
         * @param name 待查的名字
         * @param start 位置（诊断用）
         * @param end 位置（诊断用）
         * @return `true` 表示已判定完毕或名字命中；`false` 表示无法据此判定
         * @see operand_type_name_v 枚举名映射与 `"~"` 哨兵
         * @elseif en
         * @brief Judge a standard-library identifier against a **global enum** (the fallback when there is
         *        no device context)
         * @details The enum name comes from @ref operand_type_name_v; when it is the `"~"`
         *          sentinel this returns `false` immediately, meaning the operand has no global enum and
         *          the generic checks should decide.
         * @param name The name to look up
         * @param start Position (for diagnostics)
         * @param end Position (for diagnostics)
         * @return `true` when the matter is settled or the name hit; `false` when nothing can be decided
         *         this way
         * @see operand_type_name_v the enum-name mapping and its `"~"` sentinel
         * @endif
         */
        template<OperandType Type>
        bool checkGlobalEnum(const std::string& name, const Pos& start, const Pos& end) const;

        /**
         * @if zh
         * @brief 用设备上下文校验槽位索引是否落在该设备的 `slots` 内
         * @param number 槽位索引操作数（数字/寄存器等）
         * @param devSym 提供上下文的设备符号
         * @return `true` 表示已判定完毕（命中或不适用），`false` 表示仍需通用检查
         * @elseif en
         * @brief Validate a slot index against the device context's `slots`
         * @param number The slot operand (number/register/…)
         * @param devSym The device symbol supplying context
         * @return `true` when the matter is settled (hit or not applicable), `false` when the generic
         *         checks still apply
         * @endif
         */
        bool checkSlotIndexWithDevice(const auto& number, const std::shared_ptr<Symbol>& devSym);
    };

}  // namespace stationeers::ic10

#include "operand_check.hpp"
#include "analyser.inl"

#endif  // IC10_COMPILER_CORE_ANALYSER_HPP
