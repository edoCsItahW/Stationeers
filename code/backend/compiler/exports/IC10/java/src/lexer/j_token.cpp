// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_token.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 19:42
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_java/lexer/j_token.hpp"
#include "common_java/j_pos.hpp"

namespace stationeers::ic10 {

    JToken::jhybriddata JToken::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    jint JToken::getType() { return static_cast<jint>(token_.type); }

    auto JToken::getPos() { return JPos::to(token_.pos); }

    std::string JToken::getLexeme() { return token_.lexeme; }

    jint JToken::getCategory() { return static_cast<jint>(token_.category); }

    std::string JToken::toString() { return token_.toString(); }

    std::string JToken::toJSON() { return token_.toJSON(); }

    Token JToken::from(java::alias_ref<javaobject> token) { return token->cthis()->token_; }

    Token JToken::from(java::alias_ref<java::JObject> obj) {
        auto jtoken = java::dynamic_ref_cast<javaobject>(obj);

        return jtoken->cthis()->token_;
    }

    void JToken::registerNatives() {
        registerHybrid(
            {makeNativeMethod("initHybrid", JToken::initHybrid),
             makeNativeMethod("getType", JToken::getType),
             makeNativeMethod("getPos", JToken::getPos),
             makeNativeMethod("getLexeme", JToken::getLexeme),
             makeNativeMethod("getCategory", JToken::getCategory),
             makeNativeMethod("toString", JToken::toString),
             makeNativeMethod("toJSON", JToken::toJSON)}
        );
    }

}  // namespace stationeers::ic10
