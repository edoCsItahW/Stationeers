// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file base.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 22:41
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_node/base.hpp"

namespace Napi {

    Arguments::Arguments(const CallbackInfo& info)
        : info_(info) {}

    const Value Arguments::operator[](std::size_t index) const {
        return info_[index];
    }

    const Value Arguments::get(std::size_t index) const {
        if (index < info_.Length())
            return info_[index];

        return info_.Env().Undefined();
    }

}  // namespace Napi
