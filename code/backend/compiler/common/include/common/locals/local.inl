// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file local.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 23:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COMMON_LOCAL_INL
#define COMPILER_COMMON_LOCAL_INL
#pragma once

#include <format>
#include <stdexcept>

namespace stationeers {

    template<typename T, IsMsgPack P, typename P::msg_id_type I>
    auto getMsgFromMap() -> std::string_view {
        using MsgMapType = P::template msg_map_type<T, I>;

        return std::string_view(MsgMapType::msg.value.data(), MsgMapType::msg.size);
    }

    // LanguageBase

    template<IsEnum E>
    template<E I>
    std::string_view LanguageBase<E>::getMsg() const {
        return getMsgImpl(I);
    }

    // Language

    template<typename T, IsMsgPack P>
    constexpr std::size_t Language<T, P>::findIndex(E id) {
        std::size_t l = 0, r = msg_id_len<E> - 1;

        while (l <= r) {
            auto mid = l + (r - l) / 2;

            if (msg_id_list<E>[mid] == id) return mid;

            if (msg_id_list<E>[mid] < id)
                l = mid + 1;
            else
                r = mid - 1;
        }

        return -1;
    }

    template<typename T, IsMsgPack P>
    std::string_view Language<T, P>::getMsgImpl(E id) const {
        if (auto index = findIndex(id); index != -1) return msgFuncs[index]();

        throw std::runtime_error(std::format("No message found for id: {}", enumToStr(id)));
    }

    // Local

    template<IsMsgPack P>
    template<typename T>
    void Local<P>::registerLanguage(const std::string& name) {
        registry_[name] = [] { return std::make_unique<Language<T, P>>(); };
    }

    template<IsMsgPack P>
    void Local<P>::setLanguage(const std::string& name) {
        if (auto it = registry_.find(name); it != registry_.end()) {
            language_ = it->second();
            return;
        }

        throw std::runtime_error("Unknown language name: " + name);
    }
#ifndef _MSC_VER
    template<IsMsgPack P>
    template<typename Local<P>::E I, std::size_t N>
        requires number_of_args_check<typename Local<P>::E, I, N>
    auto Local<P>::msg() {
        return language_->template getMsg<I>();
    }

    template<IsMsgPack P>
    template<typename Local<P>::E I>
        requires number_of_args_check<typename Local<P>::E, I, 0>
    std::string Local<P>::msgStr() {
        return std::string(msg<I, 0>());
    }

    template<IsMsgPack P>
    template<typename Local<P>::E I, typename... Args>
        requires number_of_args_check<typename Local<P>::E, I, sizeof...(Args)>
    auto Local<P>::msgFormat(Args&&... args) {
        return std::apply(
            [&](const auto&... unpacked) {
                return std::vformat(msg<I, sizeof...(Args)>(), std::make_format_args(unpacked...));
            },
            std::tuple<Args...>(std::forward<Args>(args)...)
        );
    }
#endif

}  // namespace stationeers

#endif  // COMPILER_COMMON_LOCAL_INL
