// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file operand_check.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/04 11:30
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_OPERAND_CHECK_HPP
#define STATIONEERS_OPERAND_CHECK_HPP
#pragma once

namespace stationeers::ic10 {

    // 对标识符做具体的语义检查

    template<>
    struct Analyser::IdentifierChecker<OperandType::REG_TARGET> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    // OperandType::REG_OR_DEV为alias专用，不允许标识符

    template<>
    struct Analyser::IdentifierChecker<OperandType::NUM_VALUE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::JUMP_LINE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::ADDRESS> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::SLOT_IDX> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::HARDWARE_ID> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::REAGENT_HASH> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::DEVICE_REF> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::DEVICE_REF_STRICT> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::LOGIC_PROP> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::LOGIC_SLOT_PROP> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::AGG_MODE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::REAGENT_MODE> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::DEVICE_HASH> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

    template<>
    struct Analyser::IdentifierChecker<OperandType::NAME_HASH> {
        static bool check(const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg);
    };

}  // namespace stationeers::ic10

#include "operand_check.inl"

#endif  // STATIONEERS_OPERAND_CHECK_HPP
