// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file local_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/23 14:01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_LOCAL_ADAPTER_HPP
#define STATIONEERS_LOCAL_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"

namespace stationeers::ic10 {

    class IC10RuntimeLocalAdapter : public node::ObjectWrap<IC10RuntimeLocalAdapter> {
    public:
        IC10RuntimeLocalAdapter(const node::CallbackInfo& info);

        EXPORT_D_INIT()

    private:
        static void setLanguage(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // STATIONEERS_LOCAL_ADAPTER_HPP
