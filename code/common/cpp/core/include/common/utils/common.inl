// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao
// Songtao. This software is protected by copyright law. Reproduction,
// distribution, or use for commercial purposes is prohibited without the
// author's permission. If you have any questions or require permission, please
// contact the author: edocsitahw@qq.com

/**
 * @file common.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/08 10:14
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMMON_COMMON_INL
#define COMMON_COMMON_INL
#pragma once

namespace stationeers {

    template<IsVariant NarrowType, IsVariant WideType>
    bool variantContains(const WideType& wide) {
        return std::visit(
            []<typename T, typename U = std::decay_t<T>>(T&&) {
                return variant_contains_v<NarrowType, U>;
            },
            wide
        );
    }

}  // namespace stationeers

#endif  // COMMON_COMMON_INL
