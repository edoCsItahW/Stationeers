// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_token.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 19:11
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_J_TOKEN_HPP
#define COMPILER_J_TOKEN_HPP
#pragma once

#include "ic10_compiler/pch/pch.hpp"
#include "common_java/base.hpp"

namespace stationeers::ic10 {

    class JToken : public java::HybridClass<JToken> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/Token;";

        Token token_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        jint getType() noexcept;

        auto getPos();

        std::string getLexeme();

        jint getCategory() noexcept;

        std::string toString();

        std::string toJSON();

        static auto to(const Token& token);

        static Token from(java::alias_ref<javaobject> token);

        static Token from(java::alias_ref<java::JObject> obj);

        static void registerNatives();

    };

}  // namespace stationeers::ic10

#include "j_token.inl"

#endif  // COMPILER_J_TOKEN_HPP
