// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file manager_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 10:57
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_MANAGER_ADAPTER_HPP
#define IC10_RUNTIME_MANAGER_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10_runtime/manager.hpp"

namespace stationeers::ic10 {

    class ManagerAdapter : public node::ObjectWrap<ManagerAdapter> {
    public:
        ManagerAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        static Manager from(const node::Object& obj);

        static node::Object to(node::Env env, const Manager& manager);

    private:
        static node::FunctionReference constructor;

        Manager manager_;

        EXPORT_D_METHOD_VALUE(getDevice)

        EXPORT_D_METHOD_VOID(setExternalDevice)

        EXPORT_D_METHOD_VOID(setChipDevice)

        EXPORT_D_METHOD_VALUE(findDeviceByType)

        EXPORT_D_METHOD_VALUE(findDeviceByTypeAndName)

        EXPORT_D_METHOD_VALUE(findDevicesByType)

        EXPORT_D_METHOD_VALUE(findDevicesByTypeAndName)

        EXPORT_D_METHOD_VOID(tick)
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_MANAGER_ADAPTER_HPP
