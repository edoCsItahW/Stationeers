// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file executor.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:55
 * @brief Executor dispatches AST instructions to the runtime.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_EXECUTOR_HPP
#define IC10_RUNTIME_EXECUTOR_HPP
#pragma once

#include "ic10_runtime/context/context.hpp"
#include "common/exception/diagnostic.hpp"
#include <format>

namespace stationeers::ic10 {

    class Executor {
    public:
        Executor(Context& ctx) noexcept;

        void setReporter(DiagnosticReporter<IC10RuntimeMsgPack>* reporter) noexcept;

        bool execute();

    private:
        Context& ctx_;

        struct Flag {
            bool jumped = false;
            bool halted = false;
            bool paused = false;
        };

        DiagnosticReporter<IC10RuntimeMsgPack>* reporter_ = nullptr;

        std::optional<double> operandValue(const std::shared_ptr<Symbol>& symbol);

        template<IsVariant T>
        std::optional<double> operandValue(T&& op, bool throwError = true);

        // ---- 操作数解析辅助 -----------------------------------

        /**
         * @if zh
         * @brief 把写入目标解析为具体寄存器名
         * @details 目标既可以是寄存器（含 `rr?` 动态寄存器、`ra`、`sp`），也可以是指向寄存器的别名；
         *          别名经符号表解析后同样按寄存器文本递归求值。
         * @param op 目标操作数（`RegTarget`/`Register` 形态）
         * @param throwError 解析失败时是否上报 IEM2_1
         * @return 具体寄存器名（`r0`、`ra`、`sp`）；失败返回空值
         *
         * @else
         * @brief Resolve a write target to a concrete register name
         * @details The target is either a register (including dynamic `rr?`, `ra` and `sp`) or an alias
         *          pointing at a register; an alias is resolved through the symbol table and then
         *          evaluated as register text as well.
         * @param op The target operand (a `RegTarget`/`Register` shape)
         * @param throwError Whether to report IEM2_1 on failure
         * @return The concrete register name (`r0`, `ra`, `sp`), or an empty optional on failure
         *
         * @endif
         */
        template<IsVariant T>
        std::optional<std::string> targetRegister(T&& op, bool throwError = true);

        /**
         * @if zh
         * @brief 写入寄存器（`sp` 写入栈指针而非寄存器表）
         * @param op 目标操作数（`RegTarget`/`Register` 形态）
         * @param value 待写入的值
         *
         * @else
         * @brief Write a register (writing `sp` goes to the stack pointer, not the register map)
         * @param op The target operand (a `RegTarget`/`Register` shape)
         * @param value The value to write
         *
         * @endif
         */
        template<IsVariant T>
        void assignRegister(T&& op, double value);

        /**
         * @if zh
         * @brief 把设备引用解析为设备对象
         * @details 支持静态端口（`d0`、带引脚的 `d0:1`、自身引用 `db`）、动态端口（`dr0`、`drr0`
         *          按寄存器内容取端口号）以及设备别名；无法解析时上报 IEM2_1 并返回空指针。
         * @param op 设备引用操作数（`DeviceRef`/`Device` 形态）
         * @return 设备指针；引用非法或该端口未注册时为空
         *
         * @else
         * @brief Resolve a device reference to a device object
         * @details Supports static ports (`d0`, the pinned `d0:1`, the self reference `db`), dynamic
         *          ports (`dr0`, `drr0`, taking the port number from a register) and device aliases;
         *          an unresolvable reference reports IEM2_1 and yields a null pointer.
         * @param op The device reference operand (a `DeviceRef`/`Device` shape)
         * @return The device, or null when the reference is malformed or the port is not registered
         *
         * @endif
         */
        template<IsVariant T>
        IDevice* deviceRef(T&& op);

        /**
         * @if zh
         * @brief 把逻辑属性操作数解析为属性名
         * @details 正常形态是标识符（`Pressure`、`On`）；数字形态属旧语法，按十进制文本作为属性名传递。
         * @param op 逻辑属性操作数（`LogicProp`/`LogicSlotProp` 形态）
         * @param throwError 解析失败时是否上报 IEM2_1
         * @return 属性名；无法解析时返回空值
         *
         * @else
         * @brief Resolve a logic property operand to a property name
         * @details The normal form is an identifier (`Pressure`, `On`); the numeric form is legacy
         *          syntax and is passed along as its decimal text.
         * @param op The logic property operand (a `LogicProp`/`LogicSlotProp` shape)
         * @param throwError Whether to report IEM2_1 on failure
         * @return The property name, or an empty optional when it cannot be resolved
         *
         * @endif
         */
        template<IsVariant T>
        std::optional<std::string> propName(T&& op, bool throwError = true);

        /**
         * @if zh
         * @brief 读取寄存器（`ra` 取寄存器表、`sp` 取栈指针）
         * @param name 具体寄存器名
         * @return 寄存器当前值
         *
         * @else
         * @brief Read a register (`ra` from the register map, `sp` from the stack pointer)
         * @param name The concrete register name
         * @return The current value of the register
         *
         * @endif
         */
        double readRegister(const std::string& name);

        /**
         * @if zh
         * @brief 写入寄存器（`sp` 写入栈指针）
         * @param name 具体寄存器名
         * @param value 待写入的值
         *
         * @else
         * @brief Write a register (`sp` writes the stack pointer)
         * @param name The concrete register name
         * @param value The value to write
         *
         * @endif
         */
        void writeRegister(const std::string& name, double value);

        /**
         * @if zh
         * @brief 寄存器文本 → 具体寄存器名
         * @details 支持 `r?`、`ra`、`sp` 与其动态形式 `r<reg>`（内层寄存器的值即目标寄存器编号，
         *          对应语法中的 `rr0`）。
         * @param text 寄存器文本（符号表里别名的 value 或节点 toString() 的结果）
         * @return 具体寄存器名；文本非法或编号越界时返回空值
         *
         * @else
         * @brief Register text → concrete register name
         * @details Handles `r?`, `ra`, `sp` and their dynamic form `r<reg>` where the inner register
         *          holds the target register number (the `rr0` syntax).
         * @param text Register text (an alias value in the symbol table or a node's toString())
         * @return The concrete register name, or an empty optional for malformed text or a
         *         out-of-range index
         *
         * @endif
         */
        std::optional<std::string> resolveRegisterText(const std::string& text);

        /**
         * @if zh
         * @brief 设备文本 → 设备键
         * @details `db` 为自身引用、`d?`/`d?:?` 为静态端口、`d<reg>` 为动态端口（寄存器内容为端口号，
         *          `-1` 表示自身引用）。
         * @param text 设备文本（符号表里别名的 value 或节点 toString() 的结果）
         * @return 设备键（`Manager::getDevice` 的键）；无法解析时返回空值
         *
         * @else
         * @brief Device text → device key
         * @details `db` is the self reference, `d?`/`d?:?` are static ports and `d<reg>` is a dynamic
         *          port whose register holds the port number (`-1` meaning the self reference).
         * @param text Device text (an alias value in the symbol table or a node's toString())
         * @return The device key (the key of `Manager::getDevice`), or an empty optional when the
         *         text cannot be resolved
         *
         * @endif
         */
        std::optional<std::string> resolveDeviceKey(const std::string& text);

        /**
         * @if zh
         * @brief 枚举常量 → 数值
         * @details 枚举成员的值取自类型表中的枚举注解（如 `Color.Green` 取自 `@value Green 2`），
         *          因此需要 @ref Context::types。
         * @param node 枚举操作数节点（`Foo.Bar`）
         * @return 枚举成员的值；类型或成员缺失时返回空值
         *
         * @else
         * @brief Enum constant → value
         * @details A member's value comes from the enum annotation in the type table (for example
         *          `Color.Green` from `@value Green 2`), hence the dependency on @ref Context::types.
         * @param node The enum operand node (`Foo.Bar`)
         * @return The member value, or an empty optional when the type or member is missing
         *
         * @endif
         */
        std::optional<double> enumValue(const Enum& node);

        /**
         * @if zh
         * @brief 裸枚举成员名 → 数值
         * @details 逻辑属性/聚合模式/试剂模式等位置的裸成员名（`Pressure`、`Sum`、`Contents`）
         *          并非符号表条目，语义阶段按枚举成员校验，运行时据此在对应的操作数枚举里查找。
         * @param name 成员名
         * @return 成员值；四类操作数枚举中都没有该成员时返回空值
         *
         * @else
         * @brief Bare enum member name → value
         * @details Bare member names in logic property / aggregate mode / reagent mode positions
         *          (`Pressure`, `Sum`, `Contents`) are not symbol table entries: semantic analysis
         *          validates them as enum members, so the runtime looks them up in the corresponding
         *          operand enums.
         * @param name The member name
         * @return The member value, or an empty optional when none of the four operand enums has it
         *
         * @endif
         */
        std::optional<double> enumMember(const std::string& name);

        /**
         * @if zh
         * @brief 在指定枚举注解中查找成员值
         * @param enumName 枚举类型名（如 `BatchMode`）
         * @param member 成员名（如 `Sum`）
         * @return 成员值；类型或成员缺失时返回空值
         *
         * @else
         * @brief Look up a member value in the given enum annotation
         * @param enumName The enum type name (e.g. `BatchMode`)
         * @param member The member name (e.g. `Sum`)
         * @return The member value, or an empty optional when the type or member is missing
         *
         * @endif
         */
        std::optional<double> enumMemberOf(const std::string& enumName, const std::string& member);

        // ---- 零元 -----------------------------------

        void executeIns(const HcfInstruction& ins, Flag& flag);

        void executeIns(const YieldInstruction& ins, Flag& flag);

        // ---- 一元 — RI 组 -----------------------------------

        void executeIns(const PeekInstruction& ins, Flag& flag);

        void executeIns(const RandInstruction& ins, Flag& flag);

        void executeIns(const PopInstruction& ins, Flag& flag);

        // ---- 一元 — DAR 组 -----------------------------------

        void executeIns(const ClrInstruction& ins, Flag& flag);

        // ---- 一元 — RON 组 -----------------------------------

        void executeIns(const SleepInstruction& ins, Flag& flag);

        void executeIns(const ClrdInstruction& ins, Flag& flag);

        void executeIns(const PushInstruction& ins, Flag& flag);

        // ---- 一元 — JT 组 -----------------------------------

        void executeIns(const JalInstruction& ins, Flag& flag);

        void executeIns(const JrInstruction& ins, Flag& flag);

        void executeIns(const JInstruction& ins, Flag& flag);

        // ---- 二元 — RI_RON 组 -----------------------------------
        // 一元数学 / 位运算 / set-if / move

        void executeIns(const AbsInstruction& ins, Flag& flag);

        void executeIns(const AcosInstruction& ins, Flag& flag);

        void executeIns(const AsinInstruction& ins, Flag& flag);

        void executeIns(const AtanInstruction& ins, Flag& flag);

        void executeIns(const CeilInstruction& ins, Flag& flag);

        void executeIns(const CosInstruction& ins, Flag& flag);

        void executeIns(const ExpInstruction& ins, Flag& flag);

        void executeIns(const FloorInstruction& ins, Flag& flag);

        void executeIns(const LogInstruction& ins, Flag& flag);

        void executeIns(const RoundInstruction& ins, Flag& flag);

        void executeIns(const SinInstruction& ins, Flag& flag);

        void executeIns(const SqrtInstruction& ins, Flag& flag);

        void executeIns(const TanInstruction& ins, Flag& flag);

        void executeIns(const TruncInstruction& ins, Flag& flag);

        void executeIns(const NotInstruction& ins, Flag& flag);

        void executeIns(const MoveInstruction& ins, Flag& flag);

        void executeIns(const SgnInstruction& ins, Flag& flag);

        void executeIns(const SeqzInstruction& ins, Flag& flag);

        void executeIns(const SnezInstruction& ins, Flag& flag);

        void executeIns(const SgezInstruction& ins, Flag& flag);

        void executeIns(const SgtzInstruction& ins, Flag& flag);

        void executeIns(const SlezInstruction& ins, Flag& flag);

        void executeIns(const SltzInstruction& ins, Flag& flag);

        void executeIns(const SnanInstruction& ins, Flag& flag);

        void executeIns(const SnanzInstruction& ins, Flag& flag);

        // ---- 二元 — DR_RON 组 (设备分支) -----------------------

        void executeIns(const BdnsInstruction& ins, Flag& flag);

        void executeIns(const BdnsalInstruction& ins, Flag& flag);

        void executeIns(const BdseInstruction& ins, Flag& flag);

        void executeIns(const BdsealInstruction& ins, Flag& flag);

        void executeIns(const BrdnsInstruction& ins, Flag& flag);

        void executeIns(const BrdseInstruction& ins, Flag& flag);

        // ---- 二元 — RI_DR 组 -----------------------------------

        void executeIns(const SdnsInstruction& ins, Flag& flag);

        void executeIns(const SdseInstruction& ins, Flag& flag);

        // ---- 二元 — RON_RON 组 (分支零 / poke) ------------------

        void executeIns(const PokeInstruction& ins, Flag& flag);

        void executeIns(const BeqzInstruction& ins, Flag& flag);

        void executeIns(const BeqzalInstruction& ins, Flag& flag);

        void executeIns(const BnezInstruction& ins, Flag& flag);

        void executeIns(const BnezalInstruction& ins, Flag& flag);

        void executeIns(const BgezInstruction& ins, Flag& flag);

        void executeIns(const BgezalInstruction& ins, Flag& flag);

        void executeIns(const BgtzInstruction& ins, Flag& flag);

        void executeIns(const BgtzalInstruction& ins, Flag& flag);

        void executeIns(const BlezInstruction& ins, Flag& flag);

        void executeIns(const BlezalInstruction& ins, Flag& flag);

        void executeIns(const BltzInstruction& ins, Flag& flag);

        void executeIns(const BltzalInstruction& ins, Flag& flag);

        void executeIns(const BnanInstruction& ins, Flag& flag);

        void executeIns(const BreqzInstruction& ins, Flag& flag);

        void executeIns(const BrnezInstruction& ins, Flag& flag);

        void executeIns(const BrgezInstruction& ins, Flag& flag);

        void executeIns(const BrgtzInstruction& ins, Flag& flag);

        void executeIns(const BrlezInstruction& ins, Flag& flag);

        void executeIns(const BrltzInstruction& ins, Flag& flag);

        void executeIns(const BrnanInstruction& ins, Flag& flag);

        // ---- 三元 — RI_RON_RON 组 (二元数学 / 位运算 / set-if) ----

        void executeIns(const AddInstruction& ins, Flag& flag);

        void executeIns(const Atan2Instruction& ins, Flag& flag);

        void executeIns(const DivInstruction& ins, Flag& flag);

        void executeIns(const MaxInstruction& ins, Flag& flag);

        void executeIns(const MinInstruction& ins, Flag& flag);

        void executeIns(const ModInstruction& ins, Flag& flag);

        void executeIns(const MulInstruction& ins, Flag& flag);

        void executeIns(const PowInstruction& ins, Flag& flag);

        void executeIns(const SubInstruction& ins, Flag& flag);

        void executeIns(const RolInstruction& ins, Flag& flag);

        void executeIns(const RorInstruction& ins, Flag& flag);

        void executeIns(const AndInstruction& ins, Flag& flag);

        void executeIns(const NorInstruction& ins, Flag& flag);

        void executeIns(const OrInstruction& ins, Flag& flag);

        void executeIns(const SlaInstruction& ins, Flag& flag);

        void executeIns(const SllInstruction& ins, Flag& flag);

        void executeIns(const SraInstruction& ins, Flag& flag);

        void executeIns(const SrlInstruction& ins, Flag& flag);

        void executeIns(const XorInstruction& ins, Flag& flag);

        void executeIns(const SapzInstruction& ins, Flag& flag);

        void executeIns(const SnazInstruction& ins, Flag& flag);

        void executeIns(const SeqInstruction& ins, Flag& flag);

        void executeIns(const SneInstruction& ins, Flag& flag);

        void executeIns(const SgeInstruction& ins, Flag& flag);

        void executeIns(const SgtInstruction& ins, Flag& flag);

        void executeIns(const SleInstruction& ins, Flag& flag);

        void executeIns(const SltInstruction& ins, Flag& flag);

        // ---- 三元 — RI_DR_RON 组 (get) --------------------------

        void executeIns(const GetInstruction& ins, Flag& flag);

        // ---- 三元 — RI_DAR_RON 组 (rmap) ------------------------

        void executeIns(const RmapInstruction& ins, Flag& flag);

        // ---- 三元 — DR_RON_RON 组 (put) -------------------------

        void executeIns(const PutInstruction& ins, Flag& flag);

        // ---- 三元 — RI_DR_LT 组 (l) -----------------------------

        void executeIns(const LInstruction& ins, Flag& flag);

        // ---- 三元 — DR_LT_RI 组 (s) -----------------------------

        void executeIns(const SInstruction& ins, Flag& flag);

        // ---- 三元 — RON_LT_RI 组 (sb) ---------------------------

        void executeIns(const SbInstruction& ins, Flag& flag);

        // ---- 三元 — DR_LT_RON 组 (bdnvl / bdnvs) ----------------

        void executeIns(const BdnvlInstruction& ins, Flag& flag);

        void executeIns(const BdnvsInstruction& ins, Flag& flag);

        // ---- 三元 — RON_RON_RON 组 (条件分支) --------------------

        void executeIns(const BeqInstruction& ins, Flag& flag);

        void executeIns(const BeqalInstruction& ins, Flag& flag);

        void executeIns(const BneInstruction& ins, Flag& flag);

        void executeIns(const BnealInstruction& ins, Flag& flag);

        void executeIns(const BgeInstruction& ins, Flag& flag);

        void executeIns(const BgealInstruction& ins, Flag& flag);

        void executeIns(const BgtInstruction& ins, Flag& flag);

        void executeIns(const BgtalInstruction& ins, Flag& flag);

        void executeIns(const BleInstruction& ins, Flag& flag);

        void executeIns(const BlealInstruction& ins, Flag& flag);

        void executeIns(const BltInstruction& ins, Flag& flag);

        void executeIns(const BltalInstruction& ins, Flag& flag);

        void executeIns(const BapzInstruction& ins, Flag& flag);

        void executeIns(const BapzalInstruction& ins, Flag& flag);

        void executeIns(const BnazInstruction& ins, Flag& flag);

        void executeIns(const BnazalInstruction& ins, Flag& flag);

        void executeIns(const BreqInstruction& ins, Flag& flag);

        void executeIns(const BrneInstruction& ins, Flag& flag);

        void executeIns(const BrgeInstruction& ins, Flag& flag);

        void executeIns(const BrgtInstruction& ins, Flag& flag);

        void executeIns(const BrleInstruction& ins, Flag& flag);

        void executeIns(const BrltInstruction& ins, Flag& flag);

        void executeIns(const BrapzInstruction& ins, Flag& flag);

        void executeIns(const BrnazInstruction& ins, Flag& flag);

        // ---- 四元 -- RI_RON_RON_RON 组 (clamp / lerp / ext / ins / set-if / select)

        void executeIns(const ClampInstruction& ins, Flag& flag);

        void executeIns(const LerpInstruction& ins, Flag& flag);

        void executeIns(const ExtInstruction& ins, Flag& flag);

        void executeIns(const InsInstruction& ins, Flag& flag);

        void executeIns(const SapInstruction& ins, Flag& flag);

        void executeIns(const SnaInstruction& ins, Flag& flag);

        void executeIns(const SelectInstruction& ins, Flag& flag);

        // ---- 四元 — DR_SI_LS_RI 组 (ss) --------------------------

        void executeIns(const SsInstruction& ins, Flag& flag);

        // ---- 四元 — RI_RON_LT_BM 组 (lb) -------------------------

        void executeIns(const LbInstruction& ins, Flag& flag);

        // ---- 四元 — RON_RON_LT_RI 组 (sbn) -----------------------

        void executeIns(const SbnInstruction& ins, Flag& flag);

        // ---- 四元 — RON_SI_LS_RI 组 (sbs) ------------------------

        void executeIns(const SbsInstruction& ins, Flag& flag);

        // ---- 四元 — RON_RON_RON_RON 组 (近似分支) ----------------

        void executeIns(const BapInstruction& ins, Flag& flag);

        void executeIns(const BapalInstruction& ins, Flag& flag);

        void executeIns(const BnaInstruction& ins, Flag& flag);

        void executeIns(const BnaalInstruction& ins, Flag& flag);

        void executeIns(const BrapInstruction& ins, Flag& flag);

        void executeIns(const BrnaInstruction& ins, Flag& flag);

        // ---- 四元 — RI_DR_SI_LS 组 (ls) --------------------------

        void executeIns(const LsInstruction& ins, Flag& flag);

        // ---- 四元 — RI_DR_RM_JT 组 (lr) --------------------------

        void executeIns(const LrInstruction& ins, Flag& flag);

        // ---- 五元 -----------------------------------

        void executeIns(const LbnInstruction& ins, Flag& flag);

        void executeIns(const LbsInstruction& ins, Flag& flag);

        // ---- 六元 -----------------------------------

        void executeIns(const LbnsInstruction& ins, Flag& flag);

        // ---- 非指令类型 no-op -------------------------------

        void executeIns(const LabelDef&, Flag&) noexcept;
        void executeIns(const AliasDirective&, Flag&) noexcept;
        void executeIns(const DefineDirective&, Flag&) noexcept;
        void executeIns(const EnumAnnotation&, Flag&) noexcept;
        void executeIns(const DeviceAnnotation&, Flag&) noexcept;
        void executeIns(const ErrorNode&, Flag&) noexcept;

    };

}  // namespace stationeers::ic10

#include "executor.inl"

#endif  // IC10_RUNTIME_EXECUTOR_HPP
