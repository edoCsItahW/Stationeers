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

#include <format>
#include <sstream>
#include <utility>

namespace stationeers {

    /**
     * @if zh
     * @brief 对字符串中的特殊字符进行 JSON 转义
     * @param str 原始字符串
     * @return 转义后的字符串（不含外层引号）
     * @elseif en
     * @brief Escape special characters in a string for JSON
     * @param str Raw string
     * @return Escaped string (without outer quotes)
     * @endif
     */
    inline std::string escapeJsonString(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20)
                        result += std::format("\\u{:04x}", static_cast<unsigned char>(c));
                    else
                        result += c;
                    break;
            }
        }
        return result;
    }

    template<JsonStringAble T>
    std::string toJsonString(T value) {
        static auto fmt = [](const std::string& str) -> std::string {
            if (!str.empty()) {
                // 已经是 JSON 字面量（对象、数组、字符串），直接返回
                if (str[0] == '"' || str[0] == '[' || str[0] == '{') return str;
                return '"' + escapeJsonString(str) + '"';
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

    template<JsonStringAble T>
    static void helper(std::stringstream& ss, const std::string& key, T&& value, bool& first) {
        if (!first) ss << ", ";
        ss << '"' << key << "\": " << toJsonString(std::forward<T>(value));
        first = false;
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
                 if (value.has_value()) helper(ss, std::string(V), value, first);
             }

             else if constexpr (
                 std::is_arithmetic_v<T> || std::is_same_v<T, std::string>
                 || std::is_constructible_v<std::string, T>
             )
                 helper(ss, std::string(V), value, first);

             else
                 std::unreachable();
         }.template operator()<Vs>(std::forward<Args>(args))),
         ...);

        ss << "}";

        return ss.str();
    }

    template<JsonArgsAble T>
    std::string toJson(std::unordered_map<std::string, T> map) {
        std::stringstream ss;

        ss << "{";

        bool first = true;

        if constexpr (std::is_same_v<T, std::nullopt_t>) {
            ss << "}";

            return ss.str();
        }

        for (const auto& [key, value] : map)
            if constexpr (IsOptional<T>) {
                if (value.has_value()) helper(ss, key, value, first);
            }

            else if constexpr (
                std::is_arithmetic_v<T> || std::is_same_v<T, std::string>
                || std::is_constructible_v<std::string, T>
            )
                helper(ss, key, value, first);

            else
                std::unreachable();

        ss << "}";

        return ss.str();
    }

    template<JsonArgsAble T>
    std::string toJson(std::vector<T> vec) {
        std::stringstream ss;

        ss << "[";

        bool first = true;

        if constexpr (std::is_same_v<T, std::nullopt_t>) {
            ss << "]";

            return ss.str();
        }

        for (const auto& value : vec)
            if constexpr (IsOptional<T>) {
                if (value.has_value()) {
                    if (!first) ss << ", ";
                    ss << toJsonString(value);
                    first = false;
                }
            }

            else if constexpr (
                std::is_arithmetic_v<T> || std::is_same_v<T, std::string>
                || std::is_constructible_v<std::string, T>
            ) {
                if (!first) ss << ", ";
                ss << toJsonString(value);
                first = false;
            }

            else
                std::unreachable();

        ss << "]";

        return ss.str();
    }

}  // namespace stationeers

#endif  // COMPILER_JSON_INL
