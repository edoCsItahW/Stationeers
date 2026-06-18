// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file base.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 17:22
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_BASE_HPP
#define EXPORT_BASE_HPP
#pragma once

#include "common/utils/common.hpp"
#include <napi.h>

namespace node = Napi;

namespace st = stationeers;

template<typename T>
concept BaseOfValue = std::is_base_of_v<Napi::Value, T>;

namespace Napi {

    class Arguments {
    public:
        Arguments(const CallbackInfo& info);

        const Value operator[](std::size_t index) const;

        template<BaseOfValue T>
        const T getWithCheck(std::size_t index) const;

        const Value get(std::size_t index) const;

    private:
        const CallbackInfo& info_;
    };

}  // namespace Napi

#define EXPORT_D_ATTR_GETTER(attr) node::Value get##attr(const node::CallbackInfo& info);

#define EXPORT_D_ATTR_SETTER(attr)                                                                 \
    void set##attr(const node::CallbackInfo& info, const node::Value& value);

#define EXPORT_D_ATTR(attr)                                                                        \
    EXPORT_D_ATTR_GETTER(attr)                                                                     \
    EXPORT_D_ATTR_SETTER(attr)


#define EXPORT_D_METHOD(name, result) result name(const node::CallbackInfo& info);

#define EXPORT_D_METHOD_VALUE(name) EXPORT_D_METHOD(name, node::Value)

#define EXPORT_D_METHOD_VOID(name) EXPORT_D_METHOD(name, void)

// #define EXPORT_ATTR(attr)

#include "base.inl"

#endif  // EXPORT_BASE_HPP
