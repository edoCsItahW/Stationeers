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

#include "common/async/task.hpp"

#include <napi.h>


namespace node = Napi;

namespace st = stationeers;


namespace stationeers {
    template<typename T>
    concept BaseOfValue = std::is_base_of_v<node::Value, T>;

    template<typename T>
    concept Serializable = requires(T node) {
        { node.toJSON() } -> std::convertible_to<std::string>;
    };

    template<typename T>
    node::Value to(node::Env env, const T& value);

    template<typename... Ts>
    node::Value to(node::Env env, const std::variant<Ts...>& var);

    template<typename T>
    node::Value to(node::Env env, const std::vector<T>& vec);

    class Arguments {
    public:
        Arguments(const node::CallbackInfo& info);

        const node::Value operator[](std::size_t index) const;

        template<BaseOfValue T>
        const T getWithCheck(std::size_t index) const;

        const node::Value get(std::size_t index) const;

    private:
        const node::CallbackInfo& info_;
    };

    template<typename T>
    concept Transformer = std::is_void_v<T> || requires {
        requires !std::is_void_v<decltype(T::from(std::declval<const node::Object&>()))>;

        {
            T::to(
                std::declval<node::Env>(),
                std::declval<const std::invoke_result_t<decltype(T::from), const node::Object&>&>()
            )
        } -> std::convertible_to<node::Object>;
    };

    template<Transformer T>
    class TaskWorker : public node::AsyncWorker {
    public:
        using TaskFactory = std::function<Task<T>()>;

        TaskWorker(node::Promise::Deferred deferred, TaskFactory factory);

        void Execute() override;

        void OnOK() override;

        void OnError(const Napi::Error& e) override;

    private:
        node::Promise::Deferred deferred_;

        TaskFactory factory_;

        T result_;

        bool flag_;
    };

    template<>
    class TaskWorker<void> : public node::AsyncWorker {
    public:
        using TaskFactory = std::function<Task<>()>;

        TaskWorker(node::Promise::Deferred deferred, TaskFactory factory);

        void Execute() override;

        void OnOK() override;

        void OnError(const Napi::Error& e) override;

    private:
        node::Promise::Deferred deferred_;

        TaskFactory factory_;

        bool flag_;
    };

}  // namespace stationeers

#define EXPORT_D_ATTR_GETTER(attr) node::Value get##attr(const node::CallbackInfo& info);

#define EXPORT_D_ATTR_SETTER(attr)                                                                 \
    void set##attr(const node::CallbackInfo& info, const node::Value& value);

#define EXPORT_D_ATTR(attr)                                                                        \
    EXPORT_D_ATTR_GETTER(attr)                                                                     \
    EXPORT_D_ATTR_SETTER(attr)


#define EXPORT_D_METHOD(name, result) result name(const node::CallbackInfo& info);

#define EXPORT_D_METHOD_VALUE(name)   EXPORT_D_METHOD(name, node::Value)

#define EXPORT_D_METHOD_VOID(name)    EXPORT_D_METHOD(name, void)

// #define EXPORT_ATTR(attr)

#include "base.inl"

#endif  // EXPORT_BASE_HPP
