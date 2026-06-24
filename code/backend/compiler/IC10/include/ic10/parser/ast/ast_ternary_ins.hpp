// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_ternary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:05
 * @if zh
 * @brief IC10三元指令AST定义
 * @details 定义IC10中的三元指令(含三个操作数的指令),如ADD、DIV、MUL、GET、PUT等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_ternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 ternary instruction AST definitions
 * @details Defines ternary instructions (instructions with three operands) in IC10, such as ADD, DIV, MUL, GET, PUT, etc.
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
 * @note Implementation in ast_ternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_TERNARY_INS_HPP
#define COMPILER_AST_TERNARY_INS_HPP
#pragma once

#include "ast_binary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        // TernaryInstructionBase（实现于ast.cpp）

        /**
         * @if zh
         *
         * @class TernaryInstructionBase
         * @brief 三元指令基类模板
         * @details 三元指令是带三个操作数的指令基类。
         *        继承自BinaryInstructionBase,添加第三个操作数。
         *
         * @tparam K 指令关键字的FString类型
         * @tparam O1 第一个操作数类型
         * @tparam O2 第二个操作数类型
         * @tparam O3 第三个操作数类型
         *
         * @elseif en
         *
         * @class TernaryInstructionBase
         * @brief Ternary instruction base class template
         * @details Ternary instructions are instruction base classes with three operands.
         *        Inherits from BinaryInstructionBase, adding third operand.
         *
         * @tparam K FString type of instruction keyword
         * @tparam O1 Type of first operand
         * @tparam O2 Type of second operand
         * @tparam O3 Type of third operand
         *
         * @endif
         */
        template<FString K, typename O1, typename O2, typename O3>
        struct TernaryInstructionBase : BinaryInstructionBase<K, O1, O2> {
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
             * @brief 第三个操作数
             * @elseif en
             * @brief Third operand
             * @endif
             */
            O3 operand3;

            /**
             * @if zh
             * @brief 参数类型元组
             * @elseif en
             * @brief Argument types tuple
             * @endif
             */
            using Args = std::tuple<O1, O2, O3>;

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
            TernaryInstructionBase() = default;

            /**
             * @if zh
             *
             * @brief 构造函数
             * @param pos 位置信息
             * @param op1 第一个操作数
             * @param op2 第二个操作数
             * @param op3 第三个操作数
             *
             * @elseif en
             *
             * @brief Constructor
             * @param pos Position information
             * @param op1 First operand
             * @param op2 Second operand
             * @param op3 Third operand
             *
             * @endif
             */
            TernaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3);

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

    /**
     * @def DEFINE_TERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义三元指令
     * @details 使用TernaryInstructionBase定义一个三元指令类型并注册到TypeMap
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define ternary instruction
     * @details Defines a ternary instruction type using TernaryInstructionBase and registers it in TypeMap
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_TERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                          \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, ic10::TernaryInstructionBase, __VA_ARGS__)

    // 三元指令别名
    DEFINE_TERNARY_INSTRUCTION(
        add, Add, ADD, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_TERNARY_INSTRUCTION(
        atan2, Atan2, ATAN2, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        div, Div, DIV, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        max, Max, MAX, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        min, Min, MIN, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        mod, Mod, MOD, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        mul, Mul, MUL, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        pow, Pow, POW, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(and, And, AND, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(
        nor, Nor, NOR, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(or, Or, OR, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(
        sla, Sla, SLA, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sll, Sll, SLL, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sra, Sra, SRA, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        srl, Srl, SRL, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        xor, Xor, XOR, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sapz, Sapz, SAPZ, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        snaz, Snaz, SNAZ, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        seq, Seq, SEQ, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sne, Sne, SNE, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sge, Sge, SGE, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sgt, Sgt, SGT, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        sle, Sle, SLE, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )
    DEFINE_TERNARY_INSTRUCTION(
        slt, Slt, SLT, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand
    )

#endif

    namespace ic10 {

        using TernaryInstructionMap_RI_O_O = TypeMap<
            TokenType, TokenType::KEYWORD_ADD
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_ATAN2, TokenType::KEYWORD_DIV, TokenType::KEYWORD_MAX,
            TokenType::KEYWORD_MIN, TokenType::KEYWORD_MOD, TokenType::KEYWORD_MUL,
            TokenType::KEYWORD_POW, TokenType::KEYWORD_AND, TokenType::KEYWORD_NOR,
            TokenType::KEYWORD_OR, TokenType::KEYWORD_SLA, TokenType::KEYWORD_SLL,
            TokenType::KEYWORD_SRA, TokenType::KEYWORD_SRL, TokenType::KEYWORD_XOR,
            TokenType::KEYWORD_SAPZ, TokenType::KEYWORD_SNAZ, TokenType::KEYWORD_SEQ,
            TokenType::KEYWORD_SNE, TokenType::KEYWORD_SGE, TokenType::KEYWORD_SGT,
            TokenType::KEYWORD_SLE, TokenType::KEYWORD_SLT
#endif
            >;

    }  // namespace ic10

    // 三元指令 - 操作数类型 ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::Operand
    DEFINE_TERNARY_INSTRUCTION(
        get, Get, GET, ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_TERNARY_INSTRUCTION(
        rmap, Rmap, RMAP, ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::Operand
    )

#endif

    namespace ic10 {

        using TernaryInstructionMap_RI_DR_O = TypeMap<
            TokenType, TokenType::KEYWORD_GET
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_RMAP
#endif
            >;

    }  // namespace ic10

    // 三元指令 - 操作数类型 ic10::DeviceReference, ic10::Operand, ic10::Operand
    DEFINE_TERNARY_INSTRUCTION(put, Put, PUT, ic10::DeviceReference, ic10::Operand, ic10::Operand)

    namespace ic10 {

        using TernaryInstructionMap_DR_O_O = TypeMap<TokenType, TokenType::KEYWORD_PUT>;

    }

    // 三元指令 - 操作数类型 ic10::RegisterOrIdentifier, ic10::DeviceReference, LogicType
    DEFINE_TERNARY_INSTRUCTION(
        l, L, L, ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::LogicType
    )

    namespace ic10 {

        using TernaryInstructionMap_RI_DR_LT = TypeMap<TokenType, TokenType::KEYWORD_L>;

    }

    // 三元指令 - 操作数类型 ic10::RegisterOrIdentifier, ic10::DeviceReference, ReagentMode
    DEFINE_TERNARY_INSTRUCTION(
        lr, Lr, LR, ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::ReagentMode
    )

    namespace ic10 {

        using TernaryInstructionMap_RI_DR_RM = TypeMap<TokenType, TokenType::KEYWORD_LR>;

    }

    // 三元指令 - 操作数类型 ic10::DeviceReference, LogicType, ic10::RegisterOrIdentifier
    DEFINE_TERNARY_INSTRUCTION(
        s, S, S, ic10::DeviceReference, ic10::LogicType, ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using TernaryInstructionMap_DR_LT_RI = TypeMap<TokenType, TokenType::KEYWORD_S>;

    }

    // 三元指令 - 操作数类型 ic10::Operand, LogicType, ic10::RegisterOrIdentifier
    DEFINE_TERNARY_INSTRUCTION(
        sb, Sb, SB, ic10::Operand, ic10::LogicType, ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using TernaryInstructionMap_O_LT_RI = TypeMap<TokenType, TokenType::KEYWORD_SB>;

    }

    // 三元指令 - 操作数类型 ic10::DeviceReference, LogicType, ic10::Operand
    DEFINE_TERNARY_INSTRUCTION(
        bdnvl, Bdnvl, BDNVL, ic10::DeviceReference, ic10::LogicType, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_TERNARY_INSTRUCTION(
        bdnvs, Bdnvs, BDNVS, ic10::DeviceReference, ic10::LogicType, ic10::Operand
    )

#endif

    namespace ic10 {

        using TernaryInstructionMap_DR_LT_O = TypeMap<
            TokenType, TokenType::KEYWORD_BDNVL
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BDNVS
#endif
            >;

    }  // namespace ic10

    // 三元指令 - 操作数类型 ic10::Operand, ic10::Operand, ic10::Operand
    DEFINE_TERNARY_INSTRUCTION(beq, Beq, BEQ, ic10::Operand, ic10::Operand, ic10::Operand)

#ifndef IC10_SIMPLE_MODE

    DEFINE_TERNARY_INSTRUCTION(beqal, Beqal, BEQAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bne, Bne, BNE, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bneal, Bneal, BNEAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bge, Bge, BGE, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bgeal, Bgeal, BGEAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bgt, Bgt, BGT, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bgtal, Bgtal, BGTAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(ble, Ble, BLE, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bleal, Bleal, BLEAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(blt, Blt, BLT, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bltal, Bltal, BLTAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bapz, Bapz, BAPZ, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bapzal, Bapzal, BAPZAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bnaz, Bnaz, BNAZ, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(bnazal, Bnazal, BNAZAL, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(breq, Breq, BREQ, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brne, Brne, BRNE, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brge, Brge, BRGE, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brgt, Brgt, BRGT, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brle, Brle, BRLE, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brlt, Brlt, BRLT, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brapz, Brapz, BRAPZ, ic10::Operand, ic10::Operand, ic10::Operand)
    DEFINE_TERNARY_INSTRUCTION(brnaz, Brnaz, BRNAZ, ic10::Operand, ic10::Operand, ic10::Operand)

#endif

    namespace ic10 {

        using TernaryInstructionMap_O_O_O = TypeMap<
            TokenType, TokenType::KEYWORD_BEQ
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BEQAL, TokenType::KEYWORD_BNE, TokenType::KEYWORD_BNEAL,
            TokenType::KEYWORD_BGE, TokenType::KEYWORD_BGEAL, TokenType::KEYWORD_BGT,
            TokenType::KEYWORD_BGTAL, TokenType::KEYWORD_BLE, TokenType::KEYWORD_BLEAL,
            TokenType::KEYWORD_BLT, TokenType::KEYWORD_BLTAL, TokenType::KEYWORD_BAPZ,
            TokenType::KEYWORD_BAPZAL, TokenType::KEYWORD_BNAZ, TokenType::KEYWORD_BNAZAL,
            TokenType::KEYWORD_BREQ, TokenType::KEYWORD_BRNE, TokenType::KEYWORD_BRGE,
            TokenType::KEYWORD_BRGT, TokenType::KEYWORD_BRLE, TokenType::KEYWORD_BRLT,
            TokenType::KEYWORD_BRAPZ, TokenType::KEYWORD_BRNAZ
#endif
            >;

        using TernaryInstruction = ShallowErrorable<
            AddInstruction, GetInstruction, PutInstruction, LInstruction, LrInstruction,
            SInstruction, SbInstruction, BeqInstruction, BdnvlInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            Atan2Instruction, DivInstruction, MaxInstruction, MinInstruction, ModInstruction,
            MulInstruction, PowInstruction, AndInstruction, NorInstruction, OrInstruction,
            SlaInstruction, SllInstruction, SraInstruction, SrlInstruction, XorInstruction,
            RmapInstruction, BeqalInstruction, BneInstruction, BnealInstruction, BgeInstruction,
            BgealInstruction, BgtInstruction, BgtalInstruction, BleInstruction, BlealInstruction,
            BltInstruction, BltalInstruction, BapzInstruction, BapzalInstruction, BnazInstruction,
            BnazalInstruction, BdnvsInstruction, BreqInstruction, BrneInstruction, BrgeInstruction,
            BrgtInstruction, BrleInstruction, BrltInstruction, BrapzInstruction, BrnazInstruction,
            SapzInstruction, SnazInstruction, SeqInstruction, SneInstruction, SgeInstruction,
            SgtInstruction, SleInstruction, SltInstruction
#endif
            >;

        using TernaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_ADD, TokenType::KEYWORD_GET, TokenType::KEYWORD_PUT,
            TokenType::KEYWORD_L, TokenType::KEYWORD_LR, TokenType::KEYWORD_S,
            TokenType::KEYWORD_SB, TokenType::KEYWORD_BEQ, TokenType::KEYWORD_BDNVL
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_ATAN2, TokenType::KEYWORD_DIV, TokenType::KEYWORD_MAX,
            TokenType::KEYWORD_MIN, TokenType::KEYWORD_MOD, TokenType::KEYWORD_MUL,
            TokenType::KEYWORD_POW, TokenType::KEYWORD_AND, TokenType::KEYWORD_NOR,
            TokenType::KEYWORD_OR, TokenType::KEYWORD_SLA, TokenType::KEYWORD_SLL,
            TokenType::KEYWORD_SRA, TokenType::KEYWORD_SRL, TokenType::KEYWORD_XOR,
            TokenType::KEYWORD_RMAP, TokenType::KEYWORD_BEQAL, TokenType::KEYWORD_BNE,
            TokenType::KEYWORD_BNEAL, TokenType::KEYWORD_BGE, TokenType::KEYWORD_BGEAL,
            TokenType::KEYWORD_BGT, TokenType::KEYWORD_BGTAL, TokenType::KEYWORD_BLE,
            TokenType::KEYWORD_BLEAL, TokenType::KEYWORD_BLT, TokenType::KEYWORD_BLTAL,
            TokenType::KEYWORD_BAPZ, TokenType::KEYWORD_BAPZAL, TokenType::KEYWORD_BNAZ,
            TokenType::KEYWORD_BNAZAL, TokenType::KEYWORD_BDNVS, TokenType::KEYWORD_BREQ,
            TokenType::KEYWORD_BRNE, TokenType::KEYWORD_BRGE, TokenType::KEYWORD_BRGT,
            TokenType::KEYWORD_BRLE, TokenType::KEYWORD_BRLT, TokenType::KEYWORD_BRAPZ,
            TokenType::KEYWORD_BRNAZ, TokenType::KEYWORD_SAPZ, TokenType::KEYWORD_SNAZ,
            TokenType::KEYWORD_SEQ, TokenType::KEYWORD_SNE, TokenType::KEYWORD_SGE,
            TokenType::KEYWORD_SGT, TokenType::KEYWORD_SLE, TokenType::KEYWORD_SLT
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_ternary_ins.inl"

#endif  // COMPILER_AST_TERNARY_INS_HPP
