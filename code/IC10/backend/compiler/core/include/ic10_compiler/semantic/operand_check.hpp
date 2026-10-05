// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file operand_check.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief 操作数的**语义合法性表**：按 `OperandType` 特化 `Analyser::IdentifierChecker`
 * @details 操作数分两半看：`parser/ast/semantic_operand.hpp` 决定「这个位置**能长成什么形状**」
 *          （FIRST 前瞻能匹配到哪些 AST 变体），本文件决定「已经解析出来的**标识符**在该位置是否
 *          **合法**」。换句话说：前者是语法层，后者是语义层；同一个 `OperandType` 在两边各出现一次，
 *          但回答的是不同问题。
 *
 *          @par 协议
 *          每个特化提供 `static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol,
 *          auto&& arg)`：
 *          - @p symbol 已经过符号解析（别名/常量/标签/内置符号），失败时**不会**调用本函数；
 *          - @p arg 是原始操作数节点，用于取位置（`arg.start()` / `arg.end()`）与拼写（`arg.value`）；
 *          - 返回值表示「本次是否已判为合法/已处理」，**并不**只是布尔判定：见下面的返回值约定。
 *
 *          @par 返回值约定（重要，别按「true = 合法」读）
 *          多个特化在**上报错误之后仍然返回 `true`**，目的是让调用方跳过后续检查、**避免同一处重复
 *          报错**（源码里写的就是「报告同时返回true避免重复报告」）。因此只应把返回 `false` 理解为
 *          「本特化没管这件事，让调用方按自己的规则收尾」。
 *
 *          @par 调用链（谁在什么时候调用）
 *          | 步骤 | 位置 |
 *          |:-----|:-----|
 *          | 遍历指令操作数 | `Analyser::operator()`（analyser.inl，折叠协程逐参数 `process<Vs>`） |
 *          | 单操作数分派 | `Analyser::handleOperand<Type>`（具名协程） |
 *          | 标准库型标识符 | `LOGIC_PROP` / `LOGIC_SLOT_PROP` / `REAGENT_MODE` / `AGG_MODE` **不走符号表**：构造 dummy Symbol 后先试 `checkWithDeviceContext`，**通过即跳过**本文件的通用检查 |
 *          | 设备引用 | `DEVICE_REF` 只负责解析并登记**设备上下文**（`pendingDeviceSymbol_`），不调用本文件 |
 *          | 其余类型 | `resolveSymbol` 成功后调用 `IdentifierChecker<Type>::check(...)` |
 *          | 共用判定设施 | `Analyser::checkOperandType<I, Vs...>`：把 `BasicType` / `TypeCategory` 逐个与 `symbol->type` 比对，不符则用消息编号 `I` 报错 |
 *
 *          @par 各操作数的接受范围与消息编号
 *          | `OperandType` | 接受的类型 | 不符时的消息 |
 *          |:-----|:-----|:-----|
 *          | `REG_TARGET` | 仅 `REGISTER` | IWA1_1 |
 *          | `NUM_VALUE` | `REGISTER`、`INTEGER`、`FLOAT`、`ENUM` 或类别 `NUMBER` | IWA3_1 |
 *          | `JUMP_LINE` | 类别 `LABEL`、类别 `NUMBER`、`REGISTER` | IWA7_1 |
 *          | `ADDRESS` | `REGISTER` 或类别 `NUMBER` | IWA19_1 |
 *          | `SLOT_IDX` | 设备注解时须命中该设备的 `slots`；否则类别 `NUMBER` | IWA16_2 / IWA9_1（负数另报 IEA9） |
 *          | `HARDWARE_ID` | `REGISTER`、`ENUM` 或类别 `NUMBER` | IWA20_1 |
 *          | `REAGENT_HASH` | `REGISTER`、`ENUM` 或类别 `NUMBER` | IWA21_1 |
 *          | `DEVICE_REF` | `DEVICE` 或 `REGISTER` | IWA4_1 |
 *          | `DEVICE_REF_STRICT` | 仅 `DEVICE` | IWA2_1 |
 *          | `LOGIC_PROP` | 设备注解的 `logics` 命中；否则全局枚举 `LogicType` 命中；再否则类别 `NUMBER` | IWA14_2 / IWA15_1 / IWA8_1（数值旧语法仅警告 IWA24） |
 *          | `LOGIC_SLOT_PROP` | 设备注解的 `logicSlots` 命中；否则枚举 `LogicSlotType`；再否则类别 `NUMBER` | IWA11_2 / IWA12_1 / IWA5_1 |
 *          | `AGG_MODE` | 枚举 `BatchMode` 命中；否则类别 `NUMBER` | IWA17_1 / IWA10_1 |
 *          | `REAGENT_MODE` | 枚举 `ReagentMode` 命中；否则类别 `NUMBER` | IWA13_1 / IWA6_1 |
 *          | `DEVICE_HASH` | `REGISTER` 或类别 `NUMBER` | IWA22_1 |
 *          | `NAME_HASH` | `REGISTER` 或类别 `NUMBER` | IWA23_1 |
 *          | `REG_OR_DEV` | **无特化**：`alias` 专用，标识符在该位置已由别处处理 | — |
 *
 *          @par 数据来源
 *          判定依赖 @ref TypeTable（`self->typeTable_->find(名字)`）：别名携带的设备注解
 *          （`DeviceAnnotation` 的 `logics` / `logicSlots` / `slots`）回答「这台设备有哪些属性」，
 *          全局枚举注解（`EnumAnnotation`：`LogicType` / `LogicSlotType` / `BatchMode` /
 *          `ReagentMode`）回答「没有设备上下文时哪些名字算合法」。两者都由
 *          `parser/ast/expand_node.hpp` 解析、`@ic10/metadata` 提供数据。
 *
 *          @warning **默认是「不检查」**：主模板 `IdentifierChecker<Type>::check` 直接返回 `true`
 *                   （operand_check.inl），因此新增一个 `OperandType` 而忘记补特化时**不会有任何
 *                   编译错误、也不会有任何检查** —— 这与 `semantic_operand.hpp` 那边「漏补特化即
 *                   编译失败」的 fail-fast 正好相反，是本项目里唯一需要靠人记住的对称性缺口。
 *          @warning `SLOT_IDX` 的负数检查用 `std::stoi(*symbol->value)`：值超出 `int` 范围时会抛
 *                   `std::out_of_range`，此处**未捕获**，属潜在崩溃点（未修，仅记录）。
 *          @todo `LOGIC_PROP` 里的**读写权限检查尚未实现**：源码中留有被注释的 `// TODO: 检查读写权限`
 *                分支，`DeviceAnnotationLogic` 的 `access` 字段目前未参与判定。
 *          @note 本文件的特化分两处书写：声明在 `operand_check.hpp`，定义在 `operand_check.inl`，
 *                避免把 400 多行实现塞进头文件（项目对 `@c .inl` 的一贯做法）。
 *          @see parser/ast/semantic_operand.hpp 操作数的形状/候选集合（语法层）
 *          @see analyser.hpp @ref Analyser 的调用方与 `checkOperandType`
 *          @see semantic/types.hpp `BasicType` / `TypeCategory` / `TypeTable`
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief The operand **semantic legality table**: `Analyser::IdentifierChecker` specialized per
 *        `OperandType`
 * @details Operands have two halves. `parser/ast/semantic_operand.hpp` decides **what shape a position
 *          may take** (which AST variants FIRST lookahead can match); this file decides whether an
 *          already-parsed **identifier is legal** at that position. The former is syntax, the latter is
 *          semantics: the same `OperandType` appears in both, but answers a different question.
 *
 *          @par Protocol
 *          Each specialization provides `static bool check(const Analyser* self, const
 *          std::shared_ptr<Symbol>& symbol, auto&& arg)`:
 *          - @p symbol is already resolved (alias/constant/label/builtin); on resolution failure this
 *            function is **not** called at all;
 *          - @p arg is the original operand node, used for position (`arg.start()` / `arg.end()`) and
 *            spelling (`arg.value`);
 *          - the return value means "did this specialization settle the matter", **not** merely
 *            "is it valid" — see the convention below.
 *
 *          @par Return-value convention (important: don't read it as "true = valid")
 *          Several specializations **report an error and still return `true`**, so the caller skips
 *          further checks and **the same spot is not reported twice** (the source says exactly
 *          "report and return true to avoid duplicate reporting"). Read `false` as "this specialization
 *          did not handle it; the caller finishes by its own rules".
 *
 *          @par Call chain (who calls what, when)
 *          | Step | Location |
 *          |:-----|:-----|
 *          | Iterate an instruction's operands | `Analyser::operator()` (analyser.inl; folding coroutine, `process<Vs>` per argument) |
 *          | Per-operand dispatch | `Analyser::handleOperand<Type>` (a named coroutine) |
 *          | Standard-library identifiers | `LOGIC_PROP` / `LOGIC_SLOT_PROP` / `REAGENT_MODE` / `AGG_MODE` **bypass the symbol table**: a dummy Symbol is built, `checkWithDeviceContext` is tried first, and **passing it skips** the generic checks in this file |
 *          | Device references | `DEVICE_REF` only resolves and records the **device context** (`pendingDeviceSymbol_`); it does not call this file |
 *          | Everything else | `IdentifierChecker<Type>::check(...)` after a successful `resolveSymbol` |
 *          | Shared machinery | `Analyser::checkOperandType<I, Vs...>`: compares each `BasicType` / `TypeCategory` against `symbol->type` and reports message `I` on mismatch |
 *
 *          @par Accepted types and message ids per operand
 *          | `OperandType` | Accepted | Message on mismatch |
 *          |:-----|:-----|:-----|
 *          | `REG_TARGET` | `REGISTER` only | IWA1_1 |
 *          | `NUM_VALUE` | `REGISTER`, `INTEGER`, `FLOAT`, `ENUM` or category `NUMBER` | IWA3_1 |
 *          | `JUMP_LINE` | category `LABEL`, category `NUMBER`, `REGISTER` | IWA7_1 |
 *          | `ADDRESS` | `REGISTER` or category `NUMBER` | IWA19_1 |
 *          | `SLOT_IDX` | with a device annotation: must hit that device's `slots`; otherwise category `NUMBER` | IWA16_2 / IWA9_1 (a negative additionally reports IEA9) |
 *          | `HARDWARE_ID` | `REGISTER`, `ENUM` or category `NUMBER` | IWA20_1 |
 *          | `REAGENT_HASH` | `REGISTER`, `ENUM` or category `NUMBER` | IWA21_1 |
 *          | `DEVICE_REF` | `DEVICE` or `REGISTER` | IWA4_1 |
 *          | `DEVICE_REF_STRICT` | `DEVICE` only | IWA2_1 |
 *          | `LOGIC_PROP` | hit in the device annotation's `logics`; else the global `LogicType` enum; else category `NUMBER` | IWA14_2 / IWA15_1 / IWA8_1 (the numeric old syntax only warns: IWA24) |
 *          | `LOGIC_SLOT_PROP` | hit in `logicSlots`; else `LogicSlotType`; else category `NUMBER` | IWA11_2 / IWA12_1 / IWA5_1 |
 *          | `AGG_MODE` | hit in the `BatchMode` enum; else category `NUMBER` | IWA17_1 / IWA10_1 |
 *          | `REAGENT_MODE` | hit in the `ReagentMode` enum; else category `NUMBER` | IWA13_1 / IWA6_1 |
 *          | `DEVICE_HASH` | `REGISTER` or category `NUMBER` | IWA22_1 |
 *          | `NAME_HASH` | `REGISTER` or category `NUMBER` | IWA23_1 |
 *          | `REG_OR_DEV` | **no specialization**: `alias`-only; identifiers there are handled elsewhere | — |
 *
 *          @par Where the data comes from
 *          The checks consult the @ref TypeTable (`self->typeTable_->find(name)`): an alias's device
 *          annotation (`DeviceAnnotation` with `logics` / `logicSlots` / `slots`) answers "which
 *          properties does this device have", while global enum annotations (`EnumAnnotation`:
 *          `LogicType` / `LogicSlotType` / `BatchMode` / `ReagentMode`) answer "which names are legal
 *          when no device context exists". Both are parsed by `parser/ast/expand_node.hpp` from data
 *          supplied by `@ic10/metadata`.
 *
 *          @warning **The default is "no check at all"**: the primary template
 *                   `IdentifierChecker<Type>::check` just returns `true` (operand_check.inl), so adding a
 *                   new `OperandType` and forgetting its specialization produces **no compile error and
 *                   no check whatsoever** — the exact opposite of the fail-fast in
 *                   `semantic_operand.hpp` (missing specialization = compile error). It is the one
 *                   symmetry gap this project expects maintainers to remember.
 *          @warning `SLOT_IDX`'s negative check uses `std::stoi(*symbol->value)`: a value beyond `int`
 *                   range throws `std::out_of_range`, which is **not caught** here — a potential crash
 *                   site (not fixed, recorded only).
 *          @todo The **read/write permission check in `LOGIC_PROP` is not implemented**: the source
 *                still carries a commented-out `// TODO: 检查读写权限` branch, and
 *                `DeviceAnnotationLogic`'s `access` field takes no part in the decision yet.
 *          @note Specializations are written in two places: declared in `operand_check.hpp`, defined in
 *                `operand_check.inl`, so that 400+ lines of implementation stay out of the header (the
 *                project's usual `@c .inl` practice).
 *          @see parser/ast/semantic_operand.hpp operand shapes/candidate sets (the syntax half)
 *          @see analyser.hpp the caller, @ref Analyser, and `checkOperandType`
 *          @see semantic/types.hpp `BasicType` / `TypeCategory` / `TypeTable`
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_OPERAND_CHECK_HPP
#define IC10_COMPILER_CORE_OPERAND_CHECK_HPP
#pragma once

namespace stationeers::ic10 {

    // 对标识符做具体的语义检查

    /**
     * @if zh
     * @brief `REG_TARGET`：写入目标必须是寄存器（别名/常量都不算）
     * @elseif en
     * @brief `REG_TARGET`: the write target must be a register (aliases and constants do not count)
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::REG_TARGET> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    // OperandType::REG_OR_DEV为alias专用，不允许标识符

    /**
     * @if zh
     * @brief `NUM_VALUE`：参与运算/比较的通用数值（寄存器、整数、浮点、枚举）
     * @elseif en
     * @brief `NUM_VALUE`: the general numeric value used in arithmetic/comparison (register, integer,
     *        float, enum)
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::NUM_VALUE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `JUMP_LINE`：跳转目标（标签、数字行号或寄存器）
     * @elseif en
     * @brief `JUMP_LINE`: the jump target (label, numeric line or register)
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::JUMP_LINE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `ADDRESS`：堆栈地址索引（`get`/`put`/`poke`），寄存器或数值
     * @elseif en
     * @brief `ADDRESS`: the stack address index (`get`/`put`/`poke`), a register or a number
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::ADDRESS> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `SLOT_IDX`：设备槽位索引；有设备注解时须命中该设备的 `slots`，数值形式还需非负
     * @details 本特化是**唯一做设备上下文相关检查**的一个（其余只比对类型/类别），并会额外上报
     *          「负数槽位」错误。注意返回值的例外语义：负数上报后仍返回 `true`。
     * @elseif en
     * @brief `SLOT_IDX`: the device slot index; with a device annotation it must hit that device's
     *        `slots`, and a numeric form must be non-negative
     * @details This is the **only specialization doing a device-context check** (the rest compare
     *          type/category only), and it additionally reports a negative slot. Note the return-value
     *          quirk: a negative is reported and `true` is still returned.
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::SLOT_IDX> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `HARDWARE_ID`：设备数字硬件 ID（`clrd`/`getd` 等），寄存器、枚举或数值
     * @elseif en
     * @brief `HARDWARE_ID`: the device numeric hardware id (`clrd`/`getd`, …), a register, enum or number
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::HARDWARE_ID> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `REAGENT_HASH`：试剂材料哈希（`rmap` 配方查询），寄存器、枚举或数值
     * @elseif en
     * @brief `REAGENT_HASH`: the reagent material hash (`rmap` recipe lookup), a register, enum or number
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::REAGENT_HASH> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `DEVICE_REF`：设备引用（静态/动态端口或设备别名），设备或寄存器
     * @note 正常路径下本特化很少被走到：`handleOperand` 对 `DEVICE_REF` 只做符号解析并登记设备
     *       上下文，已解析的设备符号天然满足类型要求。
     * @elseif en
     * @brief `DEVICE_REF`: a device reference (static/dynamic port or device alias), a device or register
     * @note This specialization is rarely reached on the normal path: `handleOperand` handles
     *       `DEVICE_REF` by resolving the symbol and recording device context only, and a resolved
     *       device symbol satisfies the type requirement by construction.
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::DEVICE_REF> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `DEVICE_REF_STRICT`：严格设备端口（`clr` 的 `d?`），**不接受别名**
     * @elseif en
     * @brief `DEVICE_REF_STRICT`: a strict device port (`clr`'s `d?`), **aliases not accepted**
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::DEVICE_REF_STRICT> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `LOGIC_PROP`：设备逻辑属性名（如 `Pressure`、`Activate`）
     * @details 三级判定：① 别名带设备注解 → 在 `logics` 中查名，未命中报 IWA14_2；② 无注解 →
     *          查全局枚举 `LogicType`，未命中报 IWA15_1；③ 再退化为「类别须为 `NUMBER`」（IWA8_1）。
     *          数值写法属旧语法，仅警告 IWA24。
     * @todo 读写权限检查尚未实现（源码留有待填的 `// TODO` 分支）。
     * @elseif en
     * @brief `LOGIC_PROP`: a device logic property name (e.g. `Pressure`, `Activate`)
     * @details Three-level decision: ① an alias carrying a device annotation → look the name up in
     *          `logics`, reporting IWA14_2 on a miss; ② no annotation → consult the global `LogicType`
     *          enum, reporting IWA15_1 on a miss; ③ otherwise fall back to "category must be `NUMBER`"
     *          (IWA8_1). The numeric spelling is old syntax and only warns (IWA24).
     * @todo The read/write permission check is not implemented yet (the source keeps an unfilled
     *       `// TODO` branch).
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::LOGIC_PROP> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `LOGIC_SLOT_PROP`：槽位物品逻辑属性名
     * @details 判定结构与 `LOGIC_PROP` 对称：设备注解的 `logicSlots`（IWA11_2）→ 全局枚举
     *          `LogicSlotType`（IWA12_1）→ 类别 `NUMBER`（IWA5_1）；数值写法警告 IWA24。
     * @elseif en
     * @brief `LOGIC_SLOT_PROP`: a slot item's logic property name
     * @details Mirrors `LOGIC_PROP`: the device annotation's `logicSlots` (IWA11_2) → the global
     *          `LogicSlotType` enum (IWA12_1) → category `NUMBER` (IWA5_1); the numeric spelling warns
     *          IWA24.
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::LOGIC_SLOT_PROP> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `AGG_MODE`：聚合模式（`Average`、`Sum` 等），查全局枚举 `BatchMode`
     * @details 未命中枚举则退化为类别 `NUMBER`（IWA10_1）；枚举本身缺失时报 IEA8_1。
     * @elseif en
     * @brief `AGG_MODE`: the aggregate mode (`Average`, `Sum`, …), looked up in the global `BatchMode` enum
     * @details A miss falls back to category `NUMBER` (IWA10_1); a missing enum itself reports IEA8_1.
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::AGG_MODE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `REAGENT_MODE`：试剂模式（`Contents`、`Recipe`），查全局枚举 `ReagentMode`
     * @elseif en
     * @brief `REAGENT_MODE`: the reagent mode (`Contents`, `Recipe`), looked up in the global
     *        `ReagentMode` enum
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::REAGENT_MODE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `DEVICE_HASH`：设备类型哈希（`lb`/`sb` 系列），寄存器或类别 `NUMBER`
     * @elseif en
     * @brief `DEVICE_HASH`: the device type hash (`lb`/`sb` family); a register or category `NUMBER`
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::DEVICE_HASH> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    /**
     * @if zh
     * @brief `NAME_HASH`：设备名称哈希（`lbn`/`sbn` 系列），寄存器或类别 `NUMBER`
     * @elseif en
     * @brief `NAME_HASH`: the device name hash (`lbn`/`sbn` family); a register or category `NUMBER`
     * @endif
     */
    template<>
    struct Analyser::IdentifierChecker<OperandType::NAME_HASH> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

}  // namespace stationeers::ic10

#include "operand_check.inl"

#endif  // IC10_COMPILER_CORE_OPERAND_CHECK_HPP
