// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.cpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief @ref Analyser 的非模板实现：生命周期、遍历入口与各语句/叶节点访问器
 * @details 设计说明见 `analyser.hpp`。本文件三块内容：
 *          - **生命周期**：自持构造（新建符号表/类型表/报告器，`ownsResources_ = true`）与借用构造
 *            （@ref Linker 专用），析构仅在自持时释放。
 *          - **入口**：`analyse`（构造临时实例并 `visit`，**结果随实例一起丢弃**）、`visit`（逐条语句
 *            `std::visit` 到具名协程，Task 存入 `detachedTasks_`，最后按 `deferFailAllPending_`
 *            决定是否立即 `failAllPending`）、`resolveSymbol`（未决 Future → 前向引用；失败报 IEA3_1
 *            并返回 `nullptr` 让调用方跳过类型检查）、`defineSymbol`（重定义报 IEA2_1）。
 *          - **访问器**：语句类（`LabelDef` / `AliasDirective` / `DefineDirective` / 宏调用 /
 *            注解）做符号定义与类型注册；**叶节点访问器为空实现**，作用是不落入泛型兜底（否则误报
 *            IEA6），`ErrorNode` 更是刻意跳过以免重复上报语法阶段已报的错误。
 *          @note 语句转发用的是**非协程** lambda（转发到具名协程 `visitStatement`），原因见
 *                `analyser.hpp` 的协程帧生命周期说明 —— 这是本文件里最容易改坏的一处。
 *          @see analyser.hpp 设计说明与声明
 *          @see analyser.inl 模板与具名协程实现
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Non-template implementation of @ref Analyser: lifetime, traversal entry and the
 *        statement/leaf visitors
 * @details Design notes live in `analyser.hpp`. This file has three parts:
 *          - **Lifetime**: owning construction (new symbol table/type table/reporter,
 *            `ownsResources_ = true`) and borrowed construction (@ref Linker only); the destructor frees
 *            only when owning.
 *          - **Entry points**: `analyse` (builds a temporary and `visit`s it, **discarding the results
 *            with the instance**), `visit` (`std::visit`s each statement to a named coroutine, stores the
 *            Tasks in `detachedTasks_`, then decides from `deferFailAllPending_` whether to call
 *            `failAllPending` right away), `resolveSymbol` (a pending Future is a forward reference;
 *            failure reports IEA3_1 and returns `nullptr` so callers skip type checks) and `defineSymbol`
 *            (redefinition reports IEA2_1).
 *          - **Visitors**: statement-level ones (`LabelDef` / `AliasDirective` / `DefineDirective` / macro
 *            calls / annotations) define symbols and register types; **leaf visitors are intentionally
 *            empty**, their job being to keep calls off the generic fallback (which would report a
 *            spurious IEA6), and `ErrorNode` is skipped on purpose so errors already reported by the
 *            parser are not duplicated.
 *          @note Statement forwarding uses a **non-coroutine** lambda (forwarding to the named coroutine
 *                `visitStatement`); the reason is in the coroutine frame lifetime notes in
 *                `analyser.hpp` — the easiest thing in this file to break.
 *          @see analyser.hpp design notes and declarations
 *          @see analyser.inl template and named-coroutine implementations
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_compiler/locals/local.hpp"

namespace stationeers::ic10 {

    Analyser::Analyser(
        TypeTable& typeTable, SymbolTable& symbolTable,
        DiagnosticReporter<IC10CompilerMsgPack>& reporter, bool deferFailAllPending
    ) noexcept
        : typeTable_(&typeTable)
        , symbolTable_(&symbolTable)
        , reporter_(&reporter)
        , deferFailAllPending_(deferFailAllPending) {}

    Analyser::Analyser()
        : typeTable_(new TypeTable())
        , symbolTable_(new SymbolTable())
        , reporter_(new DiagnosticReporter<IC10CompilerMsgPack>())
        , ownsResources_(true) {}

    Analyser::~Analyser() {
        if (ownsResources_) {
            delete typeTable_;
            delete symbolTable_;
            delete reporter_;
        }
    }

    // 分析入口：构造临时分析器并访问程序根节点
    Task<> Analyser::analyse(const Program& program) {
        auto analyser = Analyser();

        (void)co_await analyser.visit(program);
    }

    SymbolTable& Analyser::getSymbolTable() const noexcept { return *symbolTable_; }

    // 获取诊断报告器累积的所有诊断信息
    const std::vector<Diagnostic>& Analyser::getDiagnostics() const noexcept {
        return reporter_->getDiagnostics();
    }

    TypeTable& Analyser::getTypeTable() const noexcept { return *typeTable_; }

    // 单条语句访问：具名协程成员函数（不用协程 lambda，原因见 handleOperand 声明处注释）
    template<typename T>
    Task<> Analyser::visitStatement(const T& arg) {
        (void)co_await this->operator()(arg);

        co_return;
    }

    // 访问 Program：逐条遍历语句，结束后清理未决 Future
    Task<> Analyser::visit(const Program& program) {
        for (const auto& stmt : program.statements)
            // 语句协程同样可能挂起后恢复，其Task必须被持有而非丢弃（见 detachedTasks_）；
            // 转发用非协程 lambda，处理体是具名协程 visitStatement
            detachedTasks_.push_back(std::visit(
                [this]<typename T>(const T& arg) -> Task<> {
                    return this->visitStatement<T>(arg);
                },
                stmt.raw()
            ));

        // 分析结束，此时依然悬而未决的 Future 被确定为未定义，向所有等待者返回错误
        // Linker 场景下推迟到所有单元处理完后统一调用
        if (!deferFailAllPending_) symbolTable_->failAllPending();

        co_return;
    }

    // 解析符号：从符号表取 Future 并等待结果，失败则转化为诊断
    Task<std::shared_ptr<Symbol>> Analyser::resolveSymbol(
        const std::string& name, const Pos& pos
    ) const {
        auto result = co_await std::move(symbolTable_->resolve(name, pos));

        // 解析失败：将异常重新抛出以捕获其消息
        if (!result.has_value()) {
            // 已知解析失败只有未定义一种情况，且对符号的引用存在多个位置
            reporter_->errorWith<ICMsgId::IEA3_1>(pos, endPos(pos, name), name);

            co_return nullptr;
        }

        co_return result.value();
    }

    // 定义符号：包装符号表 define，重定义时上报 IEA2_1
    void Analyser::defineSymbol(const Identifier& identifier, Symbol&& symbol) const {
        if (auto res = symbolTable_->define(identifier.value, std::make_shared<Symbol>(symbol));
            !res.has_value())
            reporter_->errorWith<ICMsgId::IEA2_1>(
                identifier.start(), identifier.end(), identifier.value
            );
    }

    // 标签定义：将标签名定义为 INTEGER 符号
    Task<> Analyser::operator()(const LabelDef& labelDef) {
        // Identifier: 定义为新符号
        if (std::holds_alternative<Identifier>(labelDef.identifier)) [[likely]] {
            const auto identifier = std::get<Identifier>(labelDef.identifier);

            defineSymbol(
                identifier,
                {identifier.value, type_of<LabelDef>, std::to_string(labelDef.position.line())}
            );
        }

        // ErrorNode: identifier 解析失败，上报类型不匹配
        else [[unlikely]]
            reporter_->errorWith<ICMsgId::IEA1_2>(
                labelDef.start(), labelDef.end(), Identifier::nodeName.value.data(),
                std::get<ErrorNode>(labelDef.identifier).nodeName.value.data()
            );

        co_return;
    }

    // alias 指令：为寄存器或设备定义别名
    Task<> Analyser::operator()(const AliasDirective& aliasDirective) {
        // Identifier: 按寄存器/设备类型定义别名
        if (std::holds_alternative<Identifier>(aliasDirective.identifier)) [[likely]] {
            // registerOrDevice 类型为 RegOrDev(Register|Device), 不含 Identifier, 无需预解析
            const auto identifier = std::get<Identifier>(aliasDirective.identifier);

            std::visit(
                [&]<typename T, typename U = std::decay_t<T>>(T&& ins) {
                    Symbol symbol;
                    symbol.name = identifier.value;

                    // 不允许为别名定义别名
                    if constexpr (std::is_same_v<U, Identifier>) {
                        reporter_->error<ICMsgId::IEA4>(
                            aliasDirective.start(), aliasDirective.end()
                        );
                        symbol.type = {};
                    }

                    // ErrorNode: Parser 已报错，此处跳过
                    else if constexpr (std::is_same_v<U, ErrorNode>)
                        symbol.type = {};

                    else if constexpr (
                        std::is_same_v<U, DynamicRegister> || std::is_same_v<U, DynamicDevice>
                        || std::is_same_v<U, StaticDevice>
                    ) {
                        symbol.type = type_of<U>;

                        symbol.value = ins.toString();
                    }

                    else if constexpr (
                        container_helper<Device>::contains_v<U>
                        || container_helper<Register>::contains_v<U>
                    ) {
                        symbol.type = type_of<U>;

                        symbol.value = ins.value;
                    }

                    else {
                        symbol.type = type_of<U>;
                    }

                    // 有类型注释
                    if (auto& typeHint = aliasDirective.typeHint; typeHint) {
                        if (typeHint->type) {
                            symbol.type.typeName = *typeHint->type;

                            if (symbol.value)
                                if (auto it = symbolTable_->builtinSymbols.find(*symbol.value);
                                    it != symbolTable_->builtinSymbols.end())
                                    it->second.type.typeName = *typeHint->type;
                        }

                        if (typeHint->desc) symbol.desc = *typeHint->desc;

                        symbol.isBuiltin = typeHint->builtin;
                    }

                    defineSymbol(identifier, std::move(symbol));
                },
                aliasDirective.registerOrDevice
            );

            // 如果存在类型注释，先检查是否给设备进行了错误的枚举注释
            if (auto& typeHint = aliasDirective.typeHint;
                typeHint && typeHint->type && typeHint->desc)
                if (auto* typePtr = typeTable_->find(*typeHint->type); typePtr)
                    std::visit(
                        [&]<typename T>(T&&) {
                            using U = std::remove_cvref_t<T>;

                            if constexpr (std::is_same_v<U, EnumAnnotation>)
                                reporter_->errorWith<ICMsgId::IEA7_1>(
                                    aliasDirective.start(), aliasDirective.end(), identifier.value
                                );
                        },
                        *typePtr
                    );

        }

        // ErrorNode: identifier 解析失败，上报类型不匹配
        else [[unlikely]]
            reporter_->errorWith<ICMsgId::IEA1_2>(
                aliasDirective.start(), aliasDirective.end(), Identifier::nodeName.value.data(),
                std::get<ErrorNode>(aliasDirective.identifier).nodeName.value.data()
            );

        co_return;
    }

    // define 指令：定义常量符号，其值由 operand 决定
    Task<> Analyser::operator()(const DefineDirective& defineDirective) {
        if (std::holds_alternative<Identifier>(defineDirective.identifier)) [[likely]] {
            // operand 类型为 ConstNum(仅 Number), 不含 Identifier, 无需预解析
            const auto identifier = std::get<Identifier>(defineDirective.identifier);

            std::visit(
                [&]<typename U, typename V = std::remove_cvref_t<U>>(U&& ins) {
                    Symbol symbol;
                    symbol.name = identifier.value;

                    // ErrorNode: Parser 已报错，此处跳过
                    if constexpr (std::is_same_v<V, ErrorNode>)
                        symbol.type = {};

                    else if constexpr (
                        std::is_same_v<V, HashMacro> || std::is_same_v<V, StrMacro>
                    ) {
                        symbol.type = type_of<V>;

                        if (auto value = std::get_if<String>(&ins.value); value)
                            symbol.value = value->value;
                    }

                    // Number (Integer/Float/HexNumber/BinaryNumber):
                    else {
                        symbol.type = type_of<V>;

                        symbol.value = ins.value;
                    }

                    if (auto& typeHint = defineDirective.typeHint; typeHint && typeHint->desc)
                        symbol.desc = *typeHint->desc;

                    defineSymbol(identifier, std::move(symbol));
                },
                defineDirective.operand
            );
        }

        // ErrorNode: identifier 解析失败，上报类型不匹配
        else [[unlikely]]
            reporter_->errorWith<ICMsgId::IEA1_2>(
                defineDirective.start(), defineDirective.end(), Identifier::nodeName.value.data(),
                std::get<ErrorNode>(defineDirective.identifier).nodeName.value.data()
            );

        co_return;
    }

    Task<> Analyser::operator()(const DeviceAnnotation& deviceAnnotation) {
        typeTable_->registerType(deviceAnnotation);

        co_return;
    }

    Task<> Analyser::operator()(const EnumAnnotation& enumAnnotation) {
        typeTable_->registerType(enumAnnotation);

        co_return;
    }

    // STR 宏调用：仅检查 value 是否为 ErrorNode
    Task<> Analyser::operator()(const StrMacro& strCall) {
        // value 解析失败，上报类型不匹配
        if (std::holds_alternative<ErrorNode>(strCall.value)) [[unlikely]]
            reporter_->errorWith<ICMsgId::IEA1_2>(
                strCall.start(), strCall.end(), String::nodeName.value.data(),
                std::get<ErrorNode>(strCall.value).nodeName.value.data()
            );

        co_return;
    }

    // HASH 宏调用：仅检查 value 是否为 ErrorNode
    Task<> Analyser::operator()(const HashMacro& hashCall) {
        // value 解析失败，上报类型不匹配
        if (std::holds_alternative<ErrorNode>(hashCall.value)) [[unlikely]]
            reporter_->errorWith<ICMsgId::IEA1_2>(
                hashCall.start(), hashCall.end(), String::nodeName.value.data(),
                std::get<ErrorNode>(hashCall.value).nodeName.value.data()
            );

        co_return;
    }

    // 以下为叶节点访问器：这些节点无子节点需要遍历，也无需符号解析，直接返回

    Task<> Analyser::operator()(const SelfReferenceDevice&) { co_return; }

    Task<> Analyser::operator()(const OrdinaryDevice&) { co_return; }

    Task<> Analyser::operator()(const StaticDevice&) { co_return; }

    Task<> Analyser::operator()(const DynamicDevice&) { co_return; }

    // Enum 叶节点访问器：`Foo.Bar` 这类枚举操作数暂不做语义校验，但必须显式处理，
    // 否则会落入泛型 fallback 误报 IEA6（未知语法类型）
    Task<> Analyser::operator()(const Enum&) { co_return; }

    Task<> Analyser::operator()(const GeneralPurposeRegister&) { co_return; }

    Task<> Analyser::operator()(const AddressRegister&) { co_return; }

    Task<> Analyser::operator()(const StackPointerRegister&) { co_return; }

    Task<> Analyser::operator()(const DynamicRegister&) { co_return; }

    Task<> Analyser::operator()(const String&) { co_return; }

    // Identifier 叶节点访问器：保留作为防御性兜底
    // 正常流程中 Identifier 会在指令访问器内被直接 resolveSymbol，不会走到此分支
    Task<> Analyser::operator()(const Identifier&) { co_return; }

    Task<> Analyser::operator()(const BinaryNumber&) { co_return; }

    Task<> Analyser::operator()(const HexNumber&) { co_return; }

    Task<> Analyser::operator()(const Float&) { co_return; }

    Task<> Analyser::operator()(const Integer&) { co_return; }

    // ErrorNode 叶节点访问器：Parser 已上报，此处跳过避免重复诊断
    Task<> Analyser::operator()(const ErrorNode&) { co_return; }

}  // namespace stationeers::ic10
