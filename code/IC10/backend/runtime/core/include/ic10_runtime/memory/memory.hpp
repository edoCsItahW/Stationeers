// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 17:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_MEMORY_HPP
#define IC10_RUNTIME_MEMORY_HPP
#pragma once

#include "common/exception/error.hpp"
#include "common/utils/default_unordered_map.hpp"
#include "ic10_runtime/config.hpp"
#include <functional>
#include <string>

namespace stationeers::ic10 {

    class Memory {
    public:
        Memory(const Config& config);

        Config cfg;

        template<typename T>
            requires std::is_arithmetic_v<std::decay_t<T>>
        void setReg(const std::string& name, T&& val);

        template<typename T = double>
            requires std::is_arithmetic_v<std::decay_t<T>>
        T getReg(const std::string& name);

        template<typename T = double>
            requires std::is_arithmetic_v<std::decay_t<T>>
        T getSP();

        template<typename T>
            requires std::is_arithmetic_v<std::decay_t<T>>
        void setSP(T&& val);

        template<typename T>
            requires std::is_arithmetic_v<std::decay_t<T>>
        double getStack(T index);

        template<typename T>
            requires std::is_arithmetic_v<std::decay_t<T>>
        void setStack(std::size_t index, T&& val);

        void push(
            double val,
            const std::optional<std::function<StackOverflowError(std::string)>>& onOverflow =
                std::nullopt
        );

        double pop(
            const std::optional<std::function<StackOverflowError(std::string)>>& onOverflow =
                std::nullopt
        );

        double peek(
            const std::optional<std::function<StackOverflowError(std::string)>>& onOverflow =
                std::nullopt
        ) const;

        void poke(
            std::size_t idx, double val,
            const std::optional<std::function<RangeError(std::string)>>& onRange = std::nullopt
        );

        [[nodiscard]] std::string toJSON() const;

    private:
        DefaultUnorderedMap<std::string, double> registers_;

        double sp_;

        std::vector<double> stack_;
    };

}  // namespace stationeers::ic10

#include "memory.inl"

#endif  // IC10_RUNTIME_MEMORY_HPP
