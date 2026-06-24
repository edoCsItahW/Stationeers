// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file base.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 23:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_BASE_INL
#define EXPORT_BASE_INL
#pragma once

namespace stationeers {

    template<typename T>
    node::Value to(node::Env env, const T& value) {
        if constexpr (Serializable<T>) {
            node::HandleScope scope(env);

            if (auto jsonObj = env.Global().Get("JSON"); jsonObj.IsObject())
                if (auto jsonParse = jsonObj.As<node::Object>().Get("parse");
                    jsonParse.IsFunction())
                    return jsonParse.As<node::Function>().Call(
                        jsonObj, {node::String::New(env, value.toJSON())}
                    );

            return env.Undefined();
        }

        else if constexpr (std::is_arithmetic_v<T>)
            return node::Number::New(env, value);

        else if constexpr (std::is_convertible_v<T, std::string>)
            return node::String::New(env, value);

        else
            static_assert(!std::is_same_v<T, T>, "Unsupported type");

        return env.Undefined();
    }

    template<typename... Ts>
    node::Value to(node::Env env, const std::variant<Ts...>& var) {
        return std::visit([&](const auto& v) -> node::Value { return to(env, v); }, var);
    }

    template<typename T>
    node::Value to(node::Env env, const std::vector<T>& vec) {
        node::Array arr = node::Array::New(env, vec.size());

        for (size_t i = 0; i < vec.size(); ++i) arr[i] = to(env, vec[i]);

        return arr;
    }

    // Arguments

    template<BaseOfValue T>
    const T Arguments::getWithCheck(std::size_t index) const {
        if (auto num = info_.Length(); index >= num)
            node::Error::New(
                info_.Env(), std::format("expected {} arguments, but got {}", index + 1, num)
            )
                .ThrowAsJavaScriptException();

        return info_[index].As<T>();
    }

    // TaskWorker

    template<Transformer T>
    TaskWorker<T>::TaskWorker(node::Promise::Deferred deferred, TaskFactory factory)
        : AsyncWorker(deferred.Env())
        , deferred_(deferred)
        , factory_(std::move(factory))
        , flag_(false) {}

    template<Transformer T>
    void TaskWorker<T>::Execute() {
        try {
            Task<T> task = factory_();

            auto future = task.getFuture();

            if (auto result = future.get(); result.has_value()) {
                result_ = std::move(result.value());

                flag_ = true;
            }

            else {
                if (result.error()) std::rethrow_exception(result.error());

                throw std::runtime_error("Unknown error");
            }

        } catch (const std::exception& e) { SetError(e.what()); } catch (...) {
            SetError("Unknown error");
        }
    }

    template<Transformer T>
    void TaskWorker<T>::OnOK() {
        if (flag_)
            deferred_.Resolve(T::to(Env(), result_));

        else
            deferred_.Reject(node::Error::New(Env(), "Task failed without exception").Value());
    }

    template<Transformer T>
    void TaskWorker<T>::OnError(const Napi::Error& e) {
        deferred_.Reject(e.Value());
    }

}  // namespace stationeers

#endif  // EXPORT_BASE_INL
