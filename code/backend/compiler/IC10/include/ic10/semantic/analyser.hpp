// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file analyser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:47
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ANALYSER_HPP
#define COMPILER_ANALYSER_HPP
#pragma once

#include "common/exception/error.hpp"
#include "semantic.hpp"

namespace stationeers::ic10 {

    class Analyser : std::enable_shared_from_this<Analyser> {
    public:
        static Task<> analyse(const Program& program);

        SymbolTable& getSymbolTable();

        std::vector<Error>& getErrors();

        Task<> visit(const Program& program);

    private:
        SymbolTable symbolTable_{};

        std::vector<Error> errors_;

        template<typename T, typename... Args>
        void reportError(Args&&... args);

        Task<std::shared_ptr<Symbol>> resolveSymbol(const std::string& name, const Pos& pos);

        void defineSymbol(const Identifier& identifier, Symbol&& symbol);

        Task<> operator()(const LabelDef& labelDef);

        Task<> operator()(const AliasDirective& aliasDirective);

        Task<> operator()(const DefineDirective& defineDirective);

        template<template<auto, typename...> class Ins, FString V, typename... Args>
        Task<> operator()(const Ins<V, Args...>& ins);

        Task<> operator()(const StrCall& strCall);

        Task<> operator()(const HashCall& hashCall);

        Task<> operator()(const Constant& constant);

        Task<> operator()(const Device& device);

        Task<> operator()(const Register& register_);

        Task<> operator()(const String& string);

        Task<> operator()(const Identifier& identifier);

        Task<> operator()(const BinaryNumber& binaryNumber);

        Task<> operator()(const HexNumber& hexNumber);

        Task<> operator()(const Float& float_);

        Task<> operator()(const Integer& integer);

        Task<> operator()(const ErrorNode& errorNode);

        template<typename T>
        Task<> operator()(T&& node);
    };

}  // namespace stationeers::ic10

#include "analyser.inl"

#endif  // COMPILER_ANALYSER_HPP
