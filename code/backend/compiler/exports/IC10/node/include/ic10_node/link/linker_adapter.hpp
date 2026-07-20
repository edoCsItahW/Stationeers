// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/20 22:18
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#ifndef EXPORT_LINKER_ADAPTER_HPP
#define EXPORT_LINKER_ADAPTER_HPP
#pragma once
#include "common_node/base.hpp"
#include "ic10/link/linker.hpp"

namespace stationeers::ic10 {

    class LinkerAdapter : public node::ObjectWrap<LinkerAdapter> {
    public:
        LinkerAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

    private:
        Linker linker_;

        node::Value addUnit(const node::CallbackInfo& info);

        node::Value link(const node::CallbackInfo& info);

        node::Value getDiagnostics(const node::CallbackInfo& info);

        node::Value getUnits(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_LINKER_ADAPTER_HPP