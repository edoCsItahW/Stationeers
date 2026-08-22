/**
 * @file config.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 14:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_CONFIG_HPP
#define IC10_RUNTIME_CONFIG_HPP
#pragma once

namespace stationeers::ic10 {

    struct Config {
        double tickDuration = 0.5;
        int maxInstructions = 128;
        int maxStackSize = 512;
        /// runFull() 总指令数熔断阈值（防止死循环/无限 sleep），0 = 不限制
        int maxTotalInstructions = 1'000'000;
        bool allowErrorStatements = false;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_CONFIG_HPP
