// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_type_table.inl
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_J_TYPE_TABLE_INL
#define COMPILER_J_TYPE_TABLE_INL
#pragma once

namespace stationeers::ic10 {

    inline auto JTypeTable::to(const TypeTable& self) {
        auto obj = newObjectCxxArgs();

        obj->cthis()->typeTable_ = self;

        return obj;
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_J_TYPE_TABLE_INL
