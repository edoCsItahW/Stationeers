// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file node_parser.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/03 13:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_NODE_PARSER_INL
#define STATIONEERS_NODE_PARSER_INL
#pragma once
#include "parser.hpp"

namespace stationeers::ic10 {

    template<typename T>
    NodeParserDispatcher<T>::NodeParserDispatcher(Parser& parser, T& result)
        : parser_(parser)
        , result_(result) {}

#ifndef _MSC_VER

    template<typename T>
    template<FString N, typename NodeParserDispatcher<T>::Cardinality C, typename... Args>
    NodeParserDispatcher<T>& NodeParserDispatcher<T>::add(Args&&... args) {
        rules_.emplace_back(std::string(N), C, std::forward<Args>(args)...);

        return *this;
    }

#endif

    template<typename T>
    void NodeParserDispatcher<T>::setPrefix(TokenType prefix) {
        prefix_ = prefix;
    }

    template<typename T>
    void NodeParserDispatcher<T>::until(std::function<bool(Parser& parser)> condition) {
        while (parser_.inScope()) {
            if (prefix_) parser_.expect(*prefix_);

            if (condition(parser_))
                break;

            bool matched = false;

            for (auto& rule : rules_)
                if (rule.matcher(parser_)) {
                    rule.parser(parser_, result_);

                    ++counts_[rule.name];

                    matched = true;

                    break;
                }

            if (!matched) {
                parser_.reporter_.error<ICMsgId::IEP27>(
                    parser_.current()->pos, endPos(*parser_.current())
                );

                parser_.consume();
            }
        }
    }

}  // namespace stationeers::ic10

#endif  // STATIONEERS_NODE_PARSER_INL
