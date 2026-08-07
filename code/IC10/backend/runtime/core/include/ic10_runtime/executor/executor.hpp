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

#include "../context/context.hpp"

namespace stationeers::ic10 {

    class Executor {
    public:
        Executor(Context& ctx);

        bool execute();

    private:
        Context& ctx_;

        struct Flag {
            bool jumped = false;
            bool halted = false;
            bool paused = false;
        };

        std::optional<double> operandValue(const std::shared_ptr<Symbol>& symbol);

        template<IsVariant T>
        std::optional<double> operandValue(T&& op, bool throwError = true);

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

        void executeIns(const LabelDef&, Flag&);
        void executeIns(const AliasDirective&, Flag&);
        void executeIns(const DefineDirective&, Flag&);
        void executeIns(const EnumDocComment&, Flag&);
        void executeIns(const DeviceDocComment&, Flag&);
        void executeIns(const ErrorNode&, Flag&);

    };

}  // namespace stationeers::ic10

#include "executor.inl"

#endif  // IC10_RUNTIME_EXECUTOR_HPP
