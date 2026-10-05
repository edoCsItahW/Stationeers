// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semantic_operand.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief 操作数的**语义分类**及其到具体 AST 变体的映射表
 * @details 指令定义只声明「这个位置要什么语义操作数」（例如 `Ins<"add", OperandType::REG_TARGET,
 *          OperandType::NUM_VALUE, OperandType::NUM_VALUE>`），而**不**书写具体 AST 类型名。
 *          本文件就是这两者之间的唯一桥梁：`OperandType` 枚举 + `operand_type<O>::type` 特化表。
 *
 *          @par 为什么要多这一层
 *          1. **指令定义保持可读**：`Ins` 的模板参数直接写出 `NUM_VALUE` 这类语义名，比写
 *             `Errorable<Number, Register, Identifier, Enum>` 清楚得多。
 *          2. **候选集合只在一处维护**：某个操作数「允许哪些写法」是语法层的事实，改动只需动本文件
 *             （必要时加一个枚举值 + 一个特化），所有引用它的指令自动跟随。
 *          3. **解析与校验共用一套词汇**：语法阶段按 @ref Parser::matchOperand 用本表选出候选变体；
 *             语义阶段（`semantic/operand_check.hpp`）再用同一枚举判断操作数在**该指令、该位置**上
 *             是否合法 —— 因此「解析形态」与「语义合法性」是两个独立问题，别混在一起找 bug。
 *
 *          @par 集成关系
 *          | 协作方 | 关系 |
 *          |:-----|:-----|
 *          | `ast/ins_*.hpp` / `ast/instructions.hpp` | 上游：`Ins<K, Vs...>` 用 `operand_type_t<V>` 取候选变体，供 `toJSON` / 类型表使用 |
 *          | `ast/node.hpp` | 别名定义处：`RegTarget`、`NumValue`、`ConstNum` … 都定义在那里 |
 *          | `parser.hpp` | @ref Parser::matchOperand 以 `operand_type_t<O>` 展平候选后走 FIRST 前瞻 |
 *          | `semantic/operand_check.hpp` | 下游：消费解析结果，判定「此操作数在此处是否合法」 |
 *
 *          @par 完整性约束（刻意的 fail-fast）
 *          枚举的 17 个取值与 17 个特化**一一对应**：新增枚举值而不补特化时，`operand_type_t<V>` 会因为
 *          找不到 `operand_type<V>::type` 而编译失败，而不是静默退化 —— 这正是想要的报错时机。
 *          @note 表中若干取值（`ADDRESS` / `SLOT_IDX` / `HARDWARE_ID` / `REAGENT_HASH`）的**解析形态
 *                完全相同**，其差别不在语法层，而由语义阶段判定（见 `operand_check.hpp`）。
 *          @see operand_check.hpp 语义合法性判定
 *          @see parser.hpp @ref Parser::matchOperand 解析入口
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief The **semantic taxonomy** of operands and its mapping to concrete AST variants
 * @details Instruction definitions only state "which semantic operand belongs here" (e.g.
 *          `Ins<"add", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE>`) and
 *          never spell out AST type names. This file is the single bridge between the two: the
 *          `OperandType` enum plus the `operand_type<O>::type` specialization table.
 *
 *          @par Why this extra layer exists
 *          1. **Instruction definitions stay readable**: writing `NUM_VALUE` as a template argument
 *             beats writing `Errorable<Number, Register, Identifier, Enum>`.
 *          2. **Candidate sets are maintained in one place**: what an operand may look like is a
 *             syntactic fact, so a change touches this file only (one enum value plus one
 *             specialization when needed) and every instruction using it follows automatically.
 *          3. **Parsing and validation share one vocabulary**: the syntax stage selects the candidate
 *             variant through @ref Parser::matchOperand using this table, while the semantic stage
 *             (`semantic/operand_check.hpp`) uses the same enum to judge whether an operand is legal
 *             **at that position of that instruction** — so "parse shape" and "semantic legality" are
 *             two separate questions; don't hunt for one bug in the other.
 *
 *          @par Integration
 *          | Counterpart | Relationship |
 *          |:-----|:-----|
 *          | `ast/ins_*.hpp` / `ast/instructions.hpp` | Upstream: `Ins<K, Vs...>` uses `operand_type_t<V>` for `toJSON` / type tables |
 *          | `ast/node.hpp` | Where the aliases live: `RegTarget`, `NumValue`, `ConstNum`, … |
 *          | `parser.hpp` | @ref Parser::matchOperand flattens candidates via `operand_type_t<O>` and runs FIRST lookahead |
 *          | `semantic/operand_check.hpp` | Downstream: consumes the parse result and judges legality here |
 *
 *          @par Completeness invariant (deliberate fail-fast)
 *          The 17 enumerators and the 17 specializations correspond one to one: adding an enumerator
 *          without its specialization makes `operand_type_t<V>` fail to compile because
 *          `operand_type<V>::type` is missing, rather than degrading silently — which is exactly when
 *          you want the error.
 *          @note Several enumerators (`ADDRESS` / `SLOT_IDX` / `HARDWARE_ID` / `REAGENT_HASH`) share an
 *                **identical parse shape**; their difference is not syntactic and is decided by the
 *                semantic stage (see `operand_check.hpp`).
 *          @see operand_check.hpp semantic legality checking
 *          @see parser.hpp the @ref Parser::matchOperand entry point
 *          @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_SEMANTIC_OPERAND_HPP
#define IC10_COMPILER_CORE_SEMANTIC_OPERAND_HPP
#pragma once

#include "node.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 操作数在**语义上**的分类：指令定义用它声明「这个位置接受什么」，解析器用它选候选变体
     * @details 每个取值都对应 `operand_type` 的一个特化（见本文件下方），两者必须成对存在。
     *          下表给出全部取值及其候选集合（别名定义于 `ast/node.hpp`，`Errorable` 表示「展平嵌套变体
     *          并唯一化，且含 `ErrorNode` 分支」）：
     *
     *          | 取值 | 别名 | 候选集合 |
     *          |:-----|:-----|:-----|
     *          | `REG_TARGET` | `RegTarget` | `Errorable`：`Register`、`Identifier` |
     *          | `REG_OR_DEV` | `RegOrDev` | `Errorable`：`Register`、`Device`（**`alias` 指令专用**） |
     *          | `NUM_VALUE` | `NumValue` | `Errorable`：`Number`、`Register`、`Identifier`、`Enum` |
     *          | `JUMP_LINE` | `JumpLine` | `Errorable`：`Number`、`Register`、`Identifier`（跳转目标：行号/标签） |
     *          | `ADDRESS` | `Address` | 同 `NumValue` |
     *          | `SLOT_IDX` | `SlotIdx` | 同 `NumValue`（槽位序号） |
     *          | `HARDWARE_ID` | `HardwareId` | 同 `NumValue`（硬件/设备哈希） |
     *          | `REAGENT_HASH` | `ReagentHash` | 同 `NumValue`（试剂哈希） |
     *          | `DEVICE_REF` | `DeviceRef` | `Errorable`：`Device`、`Identifier`（设备或别名） |
     *          | `DEVICE_REF_STRICT` | `DeviceRefStrict` | `Errorable`：仅 `Device`（不接受别名） |
     *          | `LOGIC_PROP` | `LogicProp` | `Errorable`：`Identifier`、`Number`（逻辑属性名或哈希） |
     *          | `LOGIC_SLOT_PROP` | `LogicSlotProp` | 同 `LogicProp`（槽位逻辑属性） |
     *          | `AGG_MODE` | `AggMode` | `Errorable`：`Number`、`Identifier`、`Enum`（聚合模式） |
     *          | `REAGENT_MODE` | `ReagentMode` | 同 `AggMode`（试剂模式） |
     *          | `DEVICE_HASH` | `DeviceHash` | `Errorable`：`Number`、`Register`、`Identifier`、`HashMacro` |
     *          | `NAME_HASH` | `NameHash` | `Errorable`：`Number`、`Register`、`Identifier`、`HashMacro` |
     *          | `CONST_NUM` | `ConstNum` | `Errorable`：`Number`、`Macro`（`define` 的常量值） |
     *
     *          @note 「同 `NumValue`」的四类取值**解析结果形态一致**，差别体现在语义阶段对该位置
     *                取值范围的判定（见 `semantic/operand_check.hpp`）。
     *          @note 枚举值本身不带节点类型信息：真正的类型由 `operand_type` 特化给出，因此本枚举可以
     *                被语义层当作「纯标签」使用，而不牵连 AST 头文件。
     * @elseif en
     * @brief The **semantic** classification of operands: instruction definitions use it to state what
     *        a position accepts, the parser uses it to pick candidate variants
     * @details Every enumerator pairs with one `operand_type` specialization (below in this file); the
     *          two must exist together. The table lists all enumerators with their candidate sets
     *          (aliases are defined in `ast/node.hpp`; `Errorable` means "nested variants flattened and
     *          deduplicated, including an `ErrorNode` alternative"):
     *
     *          | Enumerator | Alias | Candidate set |
     *          |:-----|:-----|:-----|
     *          | `REG_TARGET` | `RegTarget` | `Errorable`: `Register`, `Identifier` |
     *          | `REG_OR_DEV` | `RegOrDev` | `Errorable`: `Register`, `Device` (**`alias` directive only**) |
     *          | `NUM_VALUE` | `NumValue` | `Errorable`: `Number`, `Register`, `Identifier`, `Enum` |
     *          | `JUMP_LINE` | `JumpLine` | `Errorable`: `Number`, `Register`, `Identifier` (jump target: line/label) |
     *          | `ADDRESS` | `Address` | same as `NumValue` |
     *          | `SLOT_IDX` | `SlotIdx` | same as `NumValue` (slot index) |
     *          | `HARDWARE_ID` | `HardwareId` | same as `NumValue` (hardware/device hash) |
     *          | `REAGENT_HASH` | `ReagentHash` | same as `NumValue` (reagent hash) |
     *          | `DEVICE_REF` | `DeviceRef` | `Errorable`: `Device`, `Identifier` (device or alias) |
     *          | `DEVICE_REF_STRICT` | `DeviceRefStrict` | `Errorable`: `Device` only (no alias) |
     *          | `LOGIC_PROP` | `LogicProp` | `Errorable`: `Identifier`, `Number` (logic property name or hash) |
     *          | `LOGIC_SLOT_PROP` | `LogicSlotProp` | same as `LogicProp` (slot logic property) |
     *          | `AGG_MODE` | `AggMode` | `Errorable`: `Number`, `Identifier`, `Enum` (aggregate mode) |
     *          | `REAGENT_MODE` | `ReagentMode` | same as `AggMode` (reagent mode) |
     *          | `DEVICE_HASH` | `DeviceHash` | `Errorable`: `Number`, `Register`, `Identifier`, `HashMacro` |
     *          | `NAME_HASH` | `NameHash` | `Errorable`: `Number`, `Register`, `Identifier`, `HashMacro` |
     *          | `CONST_NUM` | `ConstNum` | `Errorable`: `Number`, `Macro` (the value of a `define`) |
     *
     *          @note The four "same as `NumValue`" enumerators **parse identically**; their difference
     *                lies in the semantic stage's range judgement for that position (see
     *                `semantic/operand_check.hpp`).
     *          @note The enum itself carries no node type information — the types come from the
     *                `operand_type` specializations, so the semantic layer can use this enum as a pure
     *                label without pulling in AST headers.
     * @endif
     */
    enum class OperandType {
        REG_TARGET,
        REG_OR_DEV,
        NUM_VALUE,
        JUMP_LINE,
        ADDRESS,
        SLOT_IDX,
        HARDWARE_ID,
        REAGENT_HASH,
        DEVICE_REF,
        DEVICE_REF_STRICT,
        LOGIC_PROP,
        LOGIC_SLOT_PROP,
        AGG_MODE,
        REAGENT_MODE,
        DEVICE_HASH,
        NAME_HASH,
        CONST_NUM
    };

    /**
     * @if zh
     * @brief 操作数语义类型到候选变体别名的映射（**主模板故意无定义**）
     * @details 本模板只在具化时给出 `type`；缺少对应特化即编译失败，从而保证「枚举取值」与
     *          「候选集合」不会失去同步（见文件头的完整性约束）。
     * @tparam O 操作数语义类型
     * @see operand_type_t 取用方式
     * @elseif en
     * @brief Maps an operand semantic type to its candidate variant alias (**primary template is
     *        deliberately undefined**)
     * @details The template only provides `type` when specialized; a missing specialization fails to
     *          compile, which keeps "enumerators" and "candidate sets" from drifting apart (see the
     *          completeness invariant in the file header).
     * @tparam O The operand semantic type
     * @see operand_type_t how it is consumed
     * @endif
     */
    template<OperandType>
    struct operand_type;

    template<>
    struct operand_type<OperandType::REG_TARGET> {
        using type = RegTarget;
    };

    template<>
    struct operand_type<OperandType::REG_OR_DEV> {  // alias专用
        using type = RegOrDev;
    };

    template<>
    struct operand_type<OperandType::NUM_VALUE> {
        using type = NumValue;
    };

    template<>
    struct operand_type<OperandType::JUMP_LINE> {
        using type = JumpLine;
    };

    template<>
    struct operand_type<OperandType::ADDRESS> {
        using type = Address;
    };

    template<>
    struct operand_type<OperandType::SLOT_IDX> {
        using type = SlotIdx;
    };

    template<>
    struct operand_type<OperandType::HARDWARE_ID> {
        using type = HardwareId;
    };

    template<>
    struct operand_type<OperandType::REAGENT_HASH> {
        using type = ReagentHash;
    };

    template<>
    struct operand_type<OperandType::DEVICE_REF> {
        using type = DeviceRef;
    };

    template<>
    struct operand_type<OperandType::DEVICE_REF_STRICT> {
        using type = DeviceRefStrict;
    };

    template<>
    struct operand_type<OperandType::LOGIC_PROP> {
        using type = LogicProp;
    };

    template<>
    struct operand_type<OperandType::LOGIC_SLOT_PROP> {
        using type = LogicSlotProp;
    };

    template<>
    struct operand_type<OperandType::AGG_MODE> {
        using type = AggMode;
    };

    template<>
    struct operand_type<OperandType::REAGENT_MODE> {
        using type = ReagentMode;
    };

    template<>
    struct operand_type<OperandType::DEVICE_HASH> {
        using type = DeviceHash;
    };

    template<>
    struct operand_type<OperandType::NAME_HASH> {
        using type = NameHash;
    };

    template<>
    struct operand_type<OperandType::CONST_NUM> {
        using type = ConstNum;
    };

    /**
     * @if zh
     * @brief 取出某个操作数语义类型的候选变体别名
     * @details 指令定义与解析器的统一入口：`Ins<K, Vs...>` 以它产出类型信息，
     *          @ref Parser::matchOperand 以它选候选集合。
     * @tparam V 操作数语义类型
     * @note 需要的是**别名**（`RegTarget` 等），不是 `operand_type<V>` 本身；
     *       因此这里不做任何包装，直接暴露 `::type`。
     * @elseif en
     * @brief The candidate variant alias of an operand semantic type
     * @details The shared entry point of instruction definitions and the parser: `Ins<K, Vs...>` uses
     *          it to produce type information and @ref Parser::matchOperand to pick the candidate set.
     * @tparam V The operand semantic type
     * @note What callers want is the **alias** (`RegTarget`, …), not `operand_type<V>` itself, so this
     *       exposes `::type` without any wrapper.
     * @endif
     */
    template<OperandType V>
    using operand_type_t = operand_type<V>::type;

    /**
     * @if zh
     * @brief 把一串操作数语义类型映射成对应的候选变体别名元组
     * @details 用于按指令声明顺序批量取得操作数的类型信息（例如类型表/JSON 输出）。
     * @tparam Vs 操作数语义类型包
     * @elseif en
     * @brief Map a pack of operand semantic types to a tuple of candidate variant aliases
     * @details Used to obtain operand type information in instruction declaration order (e.g. for type
     *          tables / JSON output).
     * @tparam Vs The operand semantic type pack
     * @endif
     */
    template<OperandType... Vs>
    using operand_type_list = std::tuple<operand_type_t<Vs>...>;

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_SEMANTIC_OPERAND_HPP
