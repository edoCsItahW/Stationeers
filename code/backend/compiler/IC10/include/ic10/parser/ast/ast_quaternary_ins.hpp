// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_quaternary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:12
 * @if zh
 * @brief IC10四元指令AST定义
 * @details 定义IC10中的四元指令(含四个操作数的指令),如LERP、EXT、INS、SAP等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_quaternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 quaternary instruction AST definitions
 * @details Defines quaternary instructions (instructions with four operands) in IC10, such as LERP, EXT, INS, SAP, etc.
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
        template<FString K, typename O1, typename O2, typename O3, typename O4>
        struct QuaternaryInstructionBase : TernaryInstructionBase<K, O1, O2, O3> {
            /**
             * @if zh
             * @brief 节点名称
             * @elseif en
             * @brief Node name
             * @endif
             */
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            /**
             * @if zh
             * @brief 第四个操作数
             * @elseif en
             * @brief Fourth operand
             * @endif
             */
            O4 operand4;

            /**
             * @if zh
             * @brief 参数类型元组
             * @elseif en
             * @brief Argument types tuple
             * @endif
             */
            using Args = std::tuple<O1, O2, O3, O4>;

            /**
             * @if zh
             * @brief 参数元组
             * @elseif en
             * @brief Argument tuple
             * @endif
             */
            Args args;

            /**
             * @if zh
             * @brief 默认构造函数
             * @elseif en
             * @brief Default constructor
             * @endif
             */
            QuaternaryInstructionBase() = default;

            /**
             * @if zh
             *
             * @brief 构造函数
             * @param pos 位置信息
             * @param op1 第一个操作数
             * @param op2 第二个操作数
             * @param op3 第三个操作数
             * @param op4 第四个操作数
             *
             * @elseif en
             *
             * @brief Constructor
             * @param pos Position information
             * @param op1 First operand
             * @param op2 Second operand
             * @param op3 Third operand
             * @param op4 Fourth operand
             *
             * @endif
             */
            QuaternaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3, O4 op4);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            /**
             * @if zh
             *
             * @brief JSON基类辅助函数
             * @tparam ...Ts 字段类型包
             * @param ... fields 字段名和值的对
             * @return JSON格式字符串
             *
             * @elseif en
             *
             * @brief JSON base helper function
             * @tparam ...Ts Field type pack
             * @param ... fields Pairs of field names and values
             * @return JSON format string
             *
             * @endif
             */
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

    // 四元指令 - 操作数类型 RegisterOrIdentifier, Operand, Operand, Operand
    DEFINE_QUATERNARY_INSTRUCTION(
        lerp, Lerp, LERP, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        ext, Ext, EXT, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        ins, Ins, INS, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        sap, Sap, SAP, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        sna, Sna, SNA, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        select, Select, SELECT, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand,
        ic10::Operand
    )

#endif


    namespace ic10 {

        using QuaternaryInstructionMap_RI_O_O_O = TypeMap<
            TokenType, TokenType::KEYWORD_LERP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_EXT, TokenType::KEYWORD_INS, TokenType::KEYWORD_SAP,
            TokenType::KEYWORD_SNA, TokenType::KEYWORD_SELECT
#endif
            >;

    }  // namespace ic10

    // 四元指令 - 操作数类型 DeviceReference, SlotIndex, LogicSlotType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        ss, Ss, SS, ic10::DeviceReference, ic10::SlotIndex, ic10::LogicSlotType,
        ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using QuaternaryInstructionMap_DR_SI_LS_RI = TypeMap<TokenType, TokenType::KEYWORD_SS>;

    }

    // 四元指令 - 操作数类型 RegisterOrIdentifier, Operand, LogicType, BatchMode
    DEFINE_QUATERNARY_INSTRUCTION(
        lb, Lb, LB, ic10::RegisterOrIdentifier, ic10::Operand, ic10::LogicType, ic10::BatchMode
    )

    namespace ic10 {

        using QuaternaryInstructionMap_RI_O_LT_BM = TypeMap<TokenType, TokenType::KEYWORD_LB>;

    }

    // 四元指令 - 操作数类型 Operand, Operand, LogicType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        sbn, Sbn, SBN, ic10::Operand, ic10::Operand, ic10::LogicType, ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using QuaternaryInstructionMap_O_O_LT_RI = TypeMap<TokenType, TokenType::KEYWORD_SBN>;

    }

    // 四元指令 - 操作数类型 Operand, SlotIndex, LogicSlotType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        sbs, Sbs, SBS, ic10::Operand, ic10::SlotIndex, ic10::LogicSlotType,
        ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using QuaternaryInstructionMap_O_SI_LS_RI = TypeMap<TokenType, TokenType::KEYWORD_SBS>;

    }

    // 四元指令 - 操作数类型 Operand, Operand, Operand, Operand
    DEFINE_QUATERNARY_INSTRUCTION(
        bap, Bap, BAP, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        bapal, Bapal, BAPAL, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        bna, Bna, BNA, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        bnaal, Bnaal, BNAAL, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        brap, Brap, BRAP, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        brna, Brna, BRNA, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )

#endif

    namespace ic10 {

        using QuaternaryInstructionMap_O_O_O_O = TypeMap<
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
        ls, Ls, LS, ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::SlotIndex,
        ic10::LogicSlotType
    )

    namespace ic10 {

        using QuaternaryInstructionMap_RI_DR_SI_LS = TypeMap<TokenType, TokenType::KEYWORD_LS>;

        using QuaternaryInstruction = ShallowErrorable<
            LerpInstruction, SsInstruction, LbInstruction, SbnInstruction, SbsInstruction,
            BapInstruction, LsInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            ExtInstruction, InsInstruction, BapalInstruction, BnaInstruction, BnaalInstruction,
            BrapInstruction, BrnaInstruction, SapInstruction, SnaInstruction, SelectInstruction
#endif
            >;

        using QuaternaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_LERP, TokenType::KEYWORD_SS, TokenType::KEYWORD_LB,
            TokenType::KEYWORD_SBN, TokenType::KEYWORD_SBS, TokenType::KEYWORD_BAP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_LS, TokenType::KEYWORD_EXT, TokenType::KEYWORD_INS,
            TokenType::KEYWORD_SAP, TokenType::KEYWORD_SNA, TokenType::KEYWORD_SELECT,
            TokenType::KEYWORD_BAPAL, TokenType::KEYWORD_BNA, TokenType::KEYWORD_BNAAL,
            TokenType::KEYWORD_BRAP, TokenType::KEYWORD_BRNA, TokenType::KEYWORD_SAP,
            TokenType::KEYWORD_SNA
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_quaternary_ins.inl"

#endif  // COMPILER_AST_QUATERNARY_INS_HPP
