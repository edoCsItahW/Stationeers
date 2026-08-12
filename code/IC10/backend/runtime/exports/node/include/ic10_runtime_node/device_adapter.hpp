// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 11:24
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_DEVICE_ADAPTER_HPP
#define IC10_RUNTIME_DEVICE_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10_runtime/device.hpp"

namespace stationeers::ic10 {

    class DeviceAdapter : public node::ObjectWrap<DeviceAdapter> {
    public:
        DeviceAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        EXPORT_D_FROM(IDevice*)

        static node::Value to(node::Env env, const IDevice* device);

    private:
        static node::FunctionReference constructor;

        IDevice* device_;

        EXPORT_D_METHOD_VALUE(readLogic)

        EXPORT_D_METHOD_VOID(writeLogic)

        EXPORT_D_METHOD_VALUE(canReadLogic)

        EXPORT_D_METHOD_VALUE(canWriteLogic)

        EXPORT_D_METHOD_VALUE(readStack)

        EXPORT_D_METHOD_VOID(writeStack)

        EXPORT_D_METHOD_VALUE(readSlot)

        EXPORT_D_METHOD_VOID(writeSlot)

        EXPORT_D_METHOD_VALUE(readReagent)

        EXPORT_D_METHOD_VALUE(queryReagentAmount)

        EXPORT_D_METHOD_VALUE(getTypeHash)

        EXPORT_D_METHOD_VALUE(getNameHash)

        EXPORT_D_METHOD_VOID(clearStack)

        EXPORT_D_METHOD_VOID(tick)
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_DEVICE_ADAPTER_HPP
