// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_quaternary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10四元指令AST定义
 * @details 定义IC10中的四元指令(含四个操作数的指令),按操作数类型分组:
 *        - RI_RON_RON_RON: clamp, lerp, ext, ins, sap, sna, select
 *        - DR_SI_LS_RI: ss
 *        - RI_RON_LT_BM: lb
 *        - RON_RON_LT_RI: sbn
 *        - RON_SI_LS_RI: sbs
 *        - RON_RON_RON_RON: bap, bapal, bna, bnaal, brap, brna
 *        - RI_DR_SI_LS: ls
 *        - RI_DR_RM_JT: lr
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_quaternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 quaternary instruction AST definitions
 * @details Defines quaternary instructions (instructions with four operands) in IC10, grouped by operand types:
 *        - RI_RON_RON_RON: clamp, lerp, ext, ins, sap, sna, select
 *        - DR_SI_LS_RI: ss
 *        - RI_RON_LT_BM: lb
 *        - RON_RON_LT_RI: sbn
 *        - RON_SI_LS_RI: sbs
 *        - RON_RON_RON_RON: bap, bapal, bna, bnaal, brap, brna
 *        - RI_DR_SI_LS: ls
 *        - RI_DR_RM_JT: lr
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
 * @note Implementation in ast_quaternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_QUATERNARY_INS_HPP
#define COMPILER_AST_QUATERNARY_INS_HPP
#pragma once

#include "ast_ternary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        // QuaternaryInstructionBase（实现于ast.cpp）

        /**
         * @if zh
         *
         * @class QuaternaryInstructionBase
         * @brief 四元指令基类模板
         * @details 四元指令是带四个操作数的指令基类。
         *        继承自TernaryInstructionBase,添加第四个操作数。
         *
         * @tparam K 指令关键字的FString类型
         * @tparam O1 第一个操作数类型
         * @tparam O2 第二个操作数类型
         * @tparam O3 第三个操作数类型
         * @tparam O4 第四个操作数类型
         *
         * @elseif en
         *
         * @class QuaternaryInstructionBase
         * @brief Quaternary instruction base class template
         * @details Quaternary instructions are instruction base classes with four operands.
         *        Inherits from TernaryInstructionBase, adding fourth operand.
         *
         * @tparam K FString type of instruction keyword
         * @tparam O1 Type of first operand
         * @tparam O2 Type of second operand
         * @tparam O3 Type of third operand
         * @tparam O4 Type of fourth operand
         *
         * @endif
         */
        template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
        struct QuaternaryInstructionBase : TernaryInstructionBase<K, V1, V2, V3> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            static constexpr auto type4 = V4;

            using O1 = operand_type_t<V1>;

            using O2 = operand_type_t<V2>;

            using O3 = operand_type_t<V3>;

            using O4 = operand_type_t<V4>;

            O4 operand4;

            using Args = operand_type_list<V1, V2, V3, V4>;

            Args args;

            QuaternaryInstructionBase() = default;

            QuaternaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3, O4 o4);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

// 四元指令别名
    /**
     * @def DEFINE_QUATERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义四元指令
     * @details 使用QuaternaryInstructionBase定义一个四元指令类型并注册到TypeMap
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define quaternary instruction
     * @details Defines a quaternary instruction type using QuaternaryInstructionBase and registers it in TypeMap
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_QUATERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                       \
    DEFINE_INSTRUCTION(                                                                            \
        lowerCase, pascalCase, upperCase, ic10::QuaternaryInstructionBase, __VA_ARGS__             \
    )

    // 四元指令 - 操作数类型 RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber,
    // RegisterOrNumber
    DEFINE_QUATERNARY_INSTRUCTION(
        clamp, Clamp, CLAMP, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        lerp, Lerp, LERP, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        ext, Ext, EXT, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        ins, Ins, INS, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        sap, Sap, SAP, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        sna, Sna, SNA, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        select, Select, SELECT, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM
    )

#endif


    namespace ic10 {

        /**
         * @if zh
         * @brief RI_RON_RON_RON 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)。
         *        包含指令: clamp, lerp, ext, ins, sap, sna, select。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RI_RON_RON_RON operand types
         * @details Operand types: (RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber).
         *        Contains instructions: clamp, lerp, ext, ins, sap, sna, select.
         * @endif
         */
        using QuaternaryInstructionMap_RI_RON_RON_RON = TypeMap<
            TokenType, TokenType::KEYWORD_CLAMP, TokenType::KEYWORD_LERP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_EXT, TokenType::KEYWORD_INS, TokenType::KEYWORD_SAP,
            TokenType::KEYWORD_SNA, TokenType::KEYWORD_SELECT
#endif
            >;

    }  // namespace ic10

    // 四元指令 - 操作数类型 DeviceReference, SlotIndex, LogicSlotType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        ss, Ss, SS, ic10::OperandType::DEV_REF, ic10::OperandType::SLOT_IDX, ic10::OperandType::LOGIC_SLOT,
        ic10::OperandType::REG_IDENT
    )

    namespace ic10 {

        /**
         * @if zh
         * @brief DR_SI_LS_RI 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (DeviceReference, SlotIndex, LogicSlotType, RegisterOrIdentifier)。
         *        包含指令: ss。
         * @elseif en
         * @brief Quaternary instruction TypeMap for DR_SI_LS_RI operand types
         * @details Operand types: (DeviceReference, SlotIndex, LogicSlotType, RegisterOrIdentifier).
         *        Contains instructions: ss.
         * @endif
         */
        using QuaternaryInstructionMap_DR_SI_LS_RI = TypeMap<TokenType, TokenType::KEYWORD_SS>;

    }

    // 四元指令 - 操作数类型 RegisterOrIdentifier, RegisterOrNumber, LogicType, BatchMode
    DEFINE_QUATERNARY_INSTRUCTION(
        lb, Lb, LB, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM, ic10::OperandType::LOGIC_TYPE,
        ic10::OperandType::BATCH_MODE
    )

    namespace ic10 {

        /**
         * @if zh
         * @brief RI_RON_LT_BM 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrIdentifier, RegisterOrNumber, LogicType, BatchMode)。
         *        包含指令: lb。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RI_RON_LT_BM operand types
         * @details Operand types: (RegisterOrIdentifier, RegisterOrNumber, LogicType, BatchMode).
         *        Contains instructions: lb.
         * @endif
         */
        using QuaternaryInstructionMap_RI_RON_LT_BM = TypeMap<TokenType, TokenType::KEYWORD_LB>;

    }

    // 四元指令 - 操作数类型 RegisterOrNumber, RegisterOrNumber, LogicType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        sbn, Sbn, SBN, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::LOGIC_TYPE,
        ic10::OperandType::REG_IDENT
    )

    namespace ic10 {

        /**
         * @if zh
         * @brief RON_RON_LT_RI 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrNumber, RegisterOrNumber, LogicType, RegisterOrIdentifier)。
         *        包含指令: sbn。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RON_RON_LT_RI operand types
         * @details Operand types: (RegisterOrNumber, RegisterOrNumber, LogicType, RegisterOrIdentifier).
         *        Contains instructions: sbn.
         * @endif
         */
        using QuaternaryInstructionMap_RON_RON_LT_RI = TypeMap<TokenType, TokenType::KEYWORD_SBN>;

    }

    // 四元指令 - 操作数类型 RegisterOrNumber, SlotIndex, LogicSlotType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        sbs, Sbs, SBS, ic10::OperandType::REG_NUM, ic10::OperandType::SLOT_IDX, ic10::OperandType::LOGIC_SLOT,
        ic10::OperandType::REG_IDENT
    )

    namespace ic10 {

        /**
         * @if zh
         * @brief RON_SI_LS_RI 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrNumber, SlotIndex, LogicSlotType, RegisterOrIdentifier)。
         *        包含指令: sbs。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RON_SI_LS_RI operand types
         * @details Operand types: (RegisterOrNumber, SlotIndex, LogicSlotType, RegisterOrIdentifier).
         *        Contains instructions: sbs.
         * @endif
         */
        using QuaternaryInstructionMap_RON_SI_LS_RI = TypeMap<TokenType, TokenType::KEYWORD_SBS>;

    }

    // 四元指令 - 操作数类型 RegisterOrNumber, RegisterOrNumber, RegisterOrNumber,
    // RegisterOrNumber
    DEFINE_QUATERNARY_INSTRUCTION(
        bap, Bap, BAP, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        bapal, Bapal, BAPAL, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        bna, Bna, BNA, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        bnaal, Bnaal, BNAAL, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        brap, Brap, BRAP, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        brna, Brna, BRNA, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM
    )

#endif

    namespace ic10 {

        /**
         * @if zh
         * @brief RON_RON_RON_RON 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)。
         *        包含指令: bap, bapal, bna, bnaal, brap, brna。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RON_RON_RON_RON operand types
         * @details Operand types: (RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber).
         *        Contains instructions: bap, bapal, bna, bnaal, brap, brna.
         * @endif
         */
        using QuaternaryInstructionMap_RON_RON_RON_RON = TypeMap<
            TokenType, TokenType::KEYWORD_BAP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BAPAL, TokenType::KEYWORD_BNA, TokenType::KEYWORD_BNAAL,
            TokenType::KEYWORD_BRAP, TokenType::KEYWORD_BRNA
#endif
            >;

    }  // namespace ic10

    // 四元指令 - 操作数类型 RegisterOrIdentifier, DeviceReference, SlotIndex, LogicSlotType
    DEFINE_QUATERNARY_INSTRUCTION(
        ls, Ls, LS, ic10::OperandType::REG_IDENT, ic10::OperandType::DEV_REF, ic10::OperandType::SLOT_IDX,
        ic10::OperandType::LOGIC_SLOT
    )

    namespace ic10 {

        /**
         * @if zh
         * @brief RI_DR_SI_LS 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrIdentifier, DeviceReference, SlotIndex, LogicSlotType)。
         *        包含指令: ls。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RI_DR_SI_LS operand types
         * @details Operand types: (RegisterOrIdentifier, DeviceReference, SlotIndex, LogicSlotType).
         *        Contains instructions: ls.
         * @endif
         */
        using QuaternaryInstructionMap_RI_DR_SI_LS = TypeMap<TokenType, TokenType::KEYWORD_LS>;

    }  // namespace ic10

    // 四元指令 - 操作数类型 RegisterOrIdentifier, DeviceReference, ReagentMode, JumpTarget
    DEFINE_QUATERNARY_INSTRUCTION(
        lr, Lr, LR, ic10::OperandType::REG_IDENT, ic10::OperandType::DEV_REF, ic10::OperandType::REAGENT_MODE,
        ic10::OperandType::JUMP_TARGET
    )

    namespace ic10 {

        /**
         * @if zh
         * @brief RI_DR_RM_JT 操作数类型的四元指令 TypeMap
         * @details 操作数类型为 (RegisterOrIdentifier, DeviceReference, ReagentMode, JumpTarget)。
         *        包含指令: lr。
         * @note lr 从三元指令移至四元指令。
         * @elseif en
         * @brief Quaternary instruction TypeMap for RI_DR_RM_JT operand types
         * @details Operand types: (RegisterOrIdentifier, DeviceReference, ReagentMode, JumpTarget).
         *        Contains instructions: lr.
         * @note lr moved from ternary to quaternary instructions.
         * @endif
         */
        using QuaternaryInstructionMap_RI_DR_RM_JT = TypeMap<TokenType, TokenType::KEYWORD_LR>;

        using QuaternaryInstruction = ShallowErrorable<
            ClampInstruction, LerpInstruction, SsInstruction, LbInstruction, SbnInstruction,
            SbsInstruction, BapInstruction, LsInstruction, LrInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            ExtInstruction, InsInstruction, BapalInstruction, BnaInstruction, BnaalInstruction,
            BrapInstruction, BrnaInstruction, SapInstruction, SnaInstruction, SelectInstruction
#endif
            >;

        using QuaternaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_CLAMP, TokenType::KEYWORD_LERP, TokenType::KEYWORD_SS,
            TokenType::KEYWORD_LB, TokenType::KEYWORD_SBN, TokenType::KEYWORD_SBS,
            TokenType::KEYWORD_BAP, TokenType::KEYWORD_LR
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_LS, TokenType::KEYWORD_EXT, TokenType::KEYWORD_INS,
            TokenType::KEYWORD_SAP, TokenType::KEYWORD_SNA, TokenType::KEYWORD_SELECT,
            TokenType::KEYWORD_BAPAL, TokenType::KEYWORD_BNA, TokenType::KEYWORD_BNAAL,
            TokenType::KEYWORD_BRAP, TokenType::KEYWORD_BRNA
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_quaternary_ins.inl"

#endif  // COMPILER_AST_QUATERNARY_INS_HPP
