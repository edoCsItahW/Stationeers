// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file semantic.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:28
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_SEMANTIC_HPP
#define COMPILER_SEMANTIC_HPP
#pragma once

#include "ic10/pch/ast.hpp"
#include "pch/async.hpp"
#include <expected>
#include <unordered_map>

namespace stationeers::ic10 {

    enum class Type { UNKNOWN, INTEGER, FLOAT, STRING, REGISTER, DEVICE, OTHER };

    struct Symbol {
        Type type;

        std::string name;

        [[nodiscard]] std::string toJSON() const;
    };

    template<typename>
    struct type_of_node {
        static constexpr auto value = Type::OTHER;
    };

    template<>
    struct type_of_node<Integer> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<Float> {
        static constexpr auto value = Type::FLOAT;
    };

    template<>
    struct type_of_node<String> {
        static constexpr auto value = Type::STRING;
    };

    template<>
    struct type_of_node<StrCall> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<HashCall> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<Register> {
        static constexpr auto value = Type::REGISTER;
    };

    template<>
    struct type_of_node<Device> {
        static constexpr auto value = Type::DEVICE;
    };

    template<>
    struct type_of_node<LabelDef> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<ErrorNode> {
        static constexpr auto value = Type::UNKNOWN;
    };

    template<typename T>
    inline constexpr auto type_of = type_of_node<T>::value;

    struct SymbolTable {
        struct Entry {
            Promise<std::shared_ptr<Symbol>> promise;

            Future<std::shared_ptr<Symbol>> future;

            Pos pos;

            [[nodiscard]] bool ready() const;
        };

    private:
        std::unordered_map<std::string, Entry> symbols_;

    public:
        SymbolTable() = default;

        /**
         * @brief 定义符号
         * @details 如果符号不存在，则将符号加入符号表中，
         *          如果符号已存在，有两种情况：
         *          1. 重复定义，则返回错误信息（此情况适用{@ref RedefinitionError}）；
         *          2. 虽然已定义，但是定义的是一个未决Future，表明之前已经有程序引用了
         *             这个符号，但是还没有得到真正的定义。这种情况是合法的，那么将符号
         *             加入符号表中，并通知所有等待者。
         *
         * @param name 符号名
         * @param symbol 符号
         * @return 如果符号确已存在，则返回错误信息，否则返回空值。
         *
         * @exception std::string 见 {@ref MsgId::IEA2_1}
         */
        [[nodiscard]] std::expected<void, std::string> define(
            const std::string& name, const std::shared_ptr<Symbol>& symbol
        );

        /**
         * @brief 查找符号
         * @details 查找符号，如果符号不存在，则将该符号初始化为未决Future，并加入符号表中，
         *          然后返回（无论存不存在）该符号的Future。
         *
         * @param name 符号名
         * @return 符号Future
         */
        template<typename Self>
        auto& resolve(this Self& self, std::string name, const Pos& pos);

        [[nodiscard]] bool contains(const std::string& name) const;

        template<typename Self>
        auto find(this Self& self, std::string name);

        template<typename Self>
        auto begin(this Self& self);

        template<typename Self>
        auto end(this Self& self);

        [[nodiscard]] std::string toJSON() const;

        void failAllPending();
    };

}  // namespace stationeers::ic10

#include "semantic.inl"

#endif  // COMPILER_SEMANTIC_HPP
