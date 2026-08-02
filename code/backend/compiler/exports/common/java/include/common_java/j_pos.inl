/**
 * @file j_pos.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 20:50
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_J_POS_INL
#define COMPILER_J_POS_INL
#pragma once

namespace stationeers {

    inline auto JPos::to(const Pos& pos) {
        auto jpos = newObjectCxxArgs();

        jpos->cthis()->pos_ = pos;

        return jpos;
    }

}  // namespace stationeers

#endif  // COMPILER_J_POS_INL
