// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file base.inl 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/16 23:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_BASE_INL
#define EXPORT_BASE_INL
#pragma once

namespace Napi {

    template<BaseOfValue T>
    const T Arguments::getWithCheck(std::size_t index) const {
        if (auto num = info_.Length(); index >= num)
            Error::New(info_.Env(), std::format("expected {} arguments, but got {}", index + 1, num))
                .ThrowAsJavaScriptException();

        return info_[index].As<T>();
    }

}  // namespace Napi

#endif //EXPORT_BASE_INL
