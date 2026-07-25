/**
 * @file json.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/20 10:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_JSON_INL
#define COMPILER_JSON_INL
#pragma once

#include <sstream>
#include <utility>


namespace stationeers::ic10 {

    template<JsonStringAble T>
    std::string toJsonString(T value) {
        static auto fmt = [](const std::string& str) -> std::string {
            if (!str.empty()) {
                // 已经是 JSON 字面量（对象、数组、字符串），直接返回
                if (str[0] == '"' || str[0] == '[' || str[0] == '{')
                    return str;
                return '"' + str + '"';
            }
            return "\"\"";
        };
        using U = std::decay_t<T>;

        if constexpr (IsOptional<U>) {
            if (value.has_value()) return fmt(std::string(*value));
        }

        else if constexpr (std::is_arithmetic_v<U>)
            return std::to_string(value);

        else if constexpr (
            std::is_same_v<T, std::string> || std::is_constructible_v<std::string, T>
        )
            return fmt(std::string(value));

        else
            std::unreachable();

        return {};
    }

    template<FString... Vs, JsonArgsAble... Args>
        requires(sizeof...(Vs) == sizeof...(Args))
    std::string toJson(Args&&... args) {
        std::stringstream ss;

        ss << "{";

        bool first = true;

        (([&]<FString V>(auto&& value) {
             using T = std::decay_t<decltype(value)>;

             if constexpr (std::is_same_v<T, std::nullopt_t>) return;

             if constexpr (IsOptional<T>) {
                 if (value.has_value()) {
                     if (!first) ss << ", ";
                     ss << '"' << std::string(V) << "\": " << toJsonString(value);
                     first = false;
                 }
             }

             else if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string> || std::is_constructible_v<std::string, T>) {
                 if (!first) ss << ", ";

                 ss << '"' << std::string(V) << "\": " << toJsonString(value);

                 first = false;
             }

             else
                 std::unreachable();

         }.template operator()<Vs>(std::forward<Args>(args))),
         ...);

        ss << "}";

        return ss.str();
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_JSON_INL
