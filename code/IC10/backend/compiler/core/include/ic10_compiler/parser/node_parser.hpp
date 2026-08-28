// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file node_parser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/29 21:51
 * @brief
 * @warning 必须使用Parser的match机制解析，未预检的情况下不能直接使用NodeParser<T>::parse解析，
 * 因为parse中没有错误处理机制，依赖match的前瞻机制
 * @note 由于Parser的match前瞻，因此NodeParser的parse方法可能为noexcept
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_NODE_PARSER_HPP
#define STATIONEERS_NODE_PARSER_HPP
#pragma once

namespace stationeers::ic10 {

    class Parser;

    template<typename>
    struct NodeParser;

    template<typename T>
    struct NodeParserDispatcher {
        enum class Cardinality { REQUIRED, OPTIONAL, AT_LEAST_ONCE, REPEATED };

        template<FString N, Cardinality C, typename... Args>
        NodeParserDispatcher& add(Args&&... args)
        #ifdef _MSC_VER
        {
            rules_.emplace_back(std::string(N), C, std::forward<Args>(args)...);

            return *this;
        }
        #else
        ;
        #endif

        void setPrefix(TokenType prefix);

        void until(std::function<bool(Parser&)> condition);

        NodeParserDispatcher(Parser& parser, T& result);

    private:

        Parser& parser_;

        T& result_;

        struct Rule {
            std::string name;
            Cardinality cardinality;
            std::function<bool (Parser&)> matcher;
            std::function<void (Parser&, T&)> parser;
        };

        std::vector<Rule> rules_;

        std::unordered_map<std::string, int> counts_;

        std::optional<TokenType> prefix_;

    };

    // Integer

    template<>
    struct NodeParser<Integer> {
        static Integer parse(Parser& parser) noexcept;
    };

    // Float

    template<>
    struct NodeParser<Float> {
        static Float parse(Parser& parser) noexcept;
    };

    // BinaryNumber

    template<>
    struct NodeParser<BinaryNumber> {
        static ShallowErrorable<BinaryNumber> parse(Parser& parser) noexcept;
    };

    // HexNumber

    template<>
    struct NodeParser<HexNumber> {
        static HexNumber parse(Parser& parser) noexcept;
    };

    // Identifier

    template<>
    struct NodeParser<Identifier> {
        static Identifier parse(Parser& parser) noexcept;
    };

    // String

    template<>
    struct NodeParser<String> {
        static String parse(Parser& parser) noexcept;
    };

    // Register

    template<>
    struct NodeParser<GeneralPurposeRegister> {
        static GeneralPurposeRegister parse(Parser& p) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<AddressRegister> {
        static AddressRegister parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<StackPointerRegister> {
        static StackPointerRegister parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<DynamicRegister> {
        static DynamicRegister parse(Parser& parser);

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    // Device

    template<>
    struct NodeParser<SelfReferenceDevice> {
        static SelfReferenceDevice parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<OrdinaryDevice> {
        static OrdinaryDevice parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<StaticDevice> {
        static StaticDevice parse(Parser& parser) noexcept;

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    template<>
    struct NodeParser<DynamicDevice> {
        static DynamicDevice parse(Parser& parser);

        [[nodiscard]] static bool is(const Parser& parser) noexcept;
    };

    // Enum

    template<>
    struct NodeParser<Enum> {
        static Enum parse(Parser& parser) noexcept;
    };

    // HashMacro

    template<>
    struct NodeParser<HashMacro> {
        static ShallowErrorable<HashMacro> parse(Parser& parser) noexcept;
    };

    // StrMacro

    template<>
    struct NodeParser<StrMacro> {
        static ShallowErrorable<StrMacro> parse(Parser& parser) noexcept;
    };

    // ErrorNode

    template<>
    struct NodeParser<ErrorNode> {
        static ErrorNode parse(Parser& parser) noexcept;
    };

}  // namespace stationeers::ic10

#include "node_parser.inl"

#endif  // STATIONEERS_NODE_PARSER_HPP
