// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file manager_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 10:57
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/manager_adapter.hpp"
#include "ic10_runtime_node/device_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference ManagerAdapter::constructor;

    ManagerAdapter::ManagerAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object ManagerAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Manager",
            {InstanceMethod<&ManagerAdapter::getDevice>("getDevice"),
             InstanceMethod<&ManagerAdapter::setExternalDevice>("setExternalDevice"),
             InstanceMethod<&ManagerAdapter::setChipDevice>("setChipDevice"),
             InstanceMethod<&ManagerAdapter::findDeviceByType>("findDeviceByType"),
             InstanceMethod<&ManagerAdapter::findDeviceByTypeAndName>("findDeviceByTypeAndName"),
             InstanceMethod<&ManagerAdapter::findDevicesByType>("findDevicesByType"),
             InstanceMethod<&ManagerAdapter::findDevicesByTypeAndName>("findDevicesByTypeAndName"),
             InstanceMethod<&ManagerAdapter::tick>("tick")}
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Manager", func);

        return exports;
    }

    Manager ManagerAdapter::from(const node::Object& obj) {
        ManagerAdapter* wrapper = Unwrap(obj);

        return std::move(wrapper->mgr());
    }

    node::Object ManagerAdapter::toExisting(node::Env env, Manager* manager) {
        node::Object obj = constructor.New({});

        ManagerAdapter* wrapper = Unwrap(obj);

        wrapper->mgrPtr_ = manager;

        return obj;
    }

    node::Object ManagerAdapter::to(node::Env env, const Manager& manager) {
        node::Object obj = constructor.New({});

        return obj;
    }

    node::Value ManagerAdapter::getDevice(const node::CallbackInfo& info) {
        Arguments args(info);

        IDevice* device = mgr().getDevice(args.getWithCheck<node::String>(0).Utf8Value());

        if (!device) return info.Env().Null();

        return DeviceAdapter::to(info.Env(), device);
    }

    void ManagerAdapter::setExternalDevice(const node::CallbackInfo& info) {
        Arguments args(info);

        mgr().setExternalDevice(
            args.getWithCheck<node::String>(0).Utf8Value(), std::make_unique<VirtualDevice>()
        );
    }

    void ManagerAdapter::setChipDevice(const node::CallbackInfo& info) {
        mgr().setChipDevice(std::make_unique<VirtualDevice>());
    }

    node::Value ManagerAdapter::findDeviceByType(const node::CallbackInfo& info) {
        Arguments args(info);

        IDevice* device =
            mgr().findDeviceByType(args.getWithCheck<node::Number>(0).Int64Value());

        if (!device) return info.Env().Null();

        return DeviceAdapter::to(info.Env(), device);
    }

    node::Value ManagerAdapter::findDeviceByTypeAndName(const node::CallbackInfo& info) {
        Arguments args(info);

        IDevice* device = mgr().findDeviceByTypeAndName(
            args.getWithCheck<node::Number>(0).Int64Value(),
            args.getWithCheck<node::Number>(1).Int64Value()
        );

        if (!device) return info.Env().Null();

        return DeviceAdapter::to(info.Env(), device);
    }

    node::Value ManagerAdapter::findDevicesByType(const node::CallbackInfo& info) {
        Arguments args(info);

        auto devices = mgr().findDevicesByType(args.getWithCheck<node::Number>(0).Int64Value());

        auto result = node::Array::New(info.Env());

        for (std::size_t i = 0; i < devices.size(); i++)
            result[i] = DeviceAdapter::to(info.Env(), devices[i]);

        return result;
    }

    node::Value ManagerAdapter::findDevicesByTypeAndName(const node::CallbackInfo& info) {
        Arguments args(info);

        auto devices = mgr().findDevicesByTypeAndName(
            args.getWithCheck<node::Number>(0).Int64Value(),
            args.getWithCheck<node::Number>(1).Int64Value()
        );

        auto result = node::Array::New(info.Env());

        for (std::size_t i = 0; i < devices.size(); i++)
            result[i] = DeviceAdapter::to(info.Env(), devices[i]);

        return result;
    }

    void ManagerAdapter::tick(const node::CallbackInfo& info) { mgr().tick(); }

}  // namespace stationeers::ic10
