// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file value.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/08 16:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/value/value.hpp"
#include <limits>

namespace stationeers::ic10 {

    const std::unordered_map<std::string, double> CONSTANTS = {
        {"rgas",    8.31446261815324                         },
        {"deg2rad", 0.0174532923847437                       },
        {"tau",     6.28318530717959                         },
        {"epsilon", std::numeric_limits<double>::denorm_min()},
        {"nan",     std::numeric_limits<double>::quiet_NaN() },
        {"pinf",    std::numeric_limits<double>::infinity()  },
        {"ninf",    -std::numeric_limits<double>::infinity() },
        {"pi",      3.14159265358979                         },
        {"rad2deg", 57.2957801818848                         }
    };

    namespace detail {

        uint32_t crc32(const std::string& str) noexcept {
            uint32_t crc = 0xFFFFFFFF;
            for (unsigned char c : str) {  // UTF‑8 字节即为 char
                crc ^= c;

                for (int i = 0; i < 8; ++i)
                    if (crc & 1)
                        crc = (crc >> 1) ^ 0xEDB88320;
                    else
                        crc >>= 1;
            }

            return crc ^ 0xFFFFFFFF;  // 最终异或
        }

    }  // namespace detail

    int32_t hashValue(const std::string& content) noexcept {
        return static_cast<int32_t>(detail::crc32(content));
    }

    double strValue(const std::string& content) noexcept {
        uint64_t packed = 0;

        auto size = content.size();
        for (std::size_t i{0}; i < size; ++i)
            packed |= static_cast<uint64_t>(static_cast<unsigned char>(content[i])) << (i * 8);

        return static_cast<double>(packed);
    }

    std::optional<double> constantValue(const std::string& keyword) noexcept {
        if (const auto& it = CONSTANTS.find(keyword); it != CONSTANTS.end()) return it->second;

        return std::nullopt;
    }

    std::optional<double> macroCall(const MacroCall& macroCall) {
        return std::visit([&]<typename T, typename U = std::decay_t<T>>(const T& arg) -> std::optional<double> {
            if constexpr (std::is_same_v<U, ErrorNode>)
                return std::nullopt;

            else if constexpr (std::is_same_v<U, StrCall>)
                return strValue(getValue<String>(arg.value).value);

            else
                return hashValue(getValue<String>(arg.value).value);

            return std::nullopt;
        }, macroCall);
    }

    std::optional<double> directionValue(const Operand& operand) {
        return std::visit([&]<typename T, typename U = std::decay_t<T>>(const T& arg) -> std::optional<double> {
            if constexpr (std::is_same_v<T, Integer> || std::is_same_v<T, Float>)
                return std::stod(arg.value);

            else if constexpr (std::is_same_v<U, HexNumber>) {
                auto hex = arg.value;
                if (auto pos = hex.find('$'); pos != std::string::npos) hex.erase(pos, 1);

                return std::stoll(hex, nullptr, 16);
            }

            else if constexpr (std::is_same_v<U, BinaryNumber>) {
                auto bin = arg.value;
                if (auto pos = bin.find('%'); pos != std::string::npos) bin.erase(pos, 1);

                return std::stoll(bin, nullptr, 2);
            }

            else if constexpr (std::is_same_v<U, Constant>)
                return constantValue(arg.keyword);

            else if constexpr (std::is_same_v<U, HashCall> || std::is_same_v<U, StrCall>)
                return macroCall(arg);
            else
                return std::nullopt;

            return std::nullopt;
        }, operand);
    }

}  // namespace stationeers::ic10
