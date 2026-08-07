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
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_CONFIG_HPP
