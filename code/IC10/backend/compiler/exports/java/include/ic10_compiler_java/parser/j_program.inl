// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_program.inl
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_J_PROGRAM_INL
#define COMPILER_J_PROGRAM_INL
#pragma once

namespace stationeers::ic10 {

    inline auto JProgram::to(const Program& program) {
        auto jprogram = newObjectCxxArgs();

        jprogram->cthis()->program_ = program;

        return jprogram;
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_J_PROGRAM_INL
