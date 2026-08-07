// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 11:24
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/device_adapter.hpp"

namespace stationeers::ic10 {

    DeviceAdapter::DeviceAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info)
        , device_(nullptr) {}

    node::FunctionReference DeviceAdapter::constructor;

    node::Object DeviceAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Device",
            {InstanceMethod<&DeviceAdapter::readLogic>("readLogic"),
             InstanceMethod<&DeviceAdapter::writeLogic>("writeLogic"),
             InstanceMethod<&DeviceAdapter::canReadLogic>("canReadLogic"),
             InstanceMethod<&DeviceAdapter::canWriteLogic>("canWriteLogic"),
             InstanceMethod<&DeviceAdapter::readStack>("readStack"),
             InstanceMethod<&DeviceAdapter::writeStack>("writeStack"),
             InstanceMethod<&DeviceAdapter::readSlot>("readSlot"),
             InstanceMethod<&DeviceAdapter::writeSlot>("writeSlot"),
             InstanceMethod<&DeviceAdapter::readReagent>("readReagent"),
             InstanceMethod<&DeviceAdapter::queryReagentAmount>("queryReagentAmount"),
             InstanceMethod<&DeviceAdapter::getTypeHash>("getTypeHash"),
             InstanceMethod<&DeviceAdapter::getNameHash>("getNameHash"),
             InstanceMethod<&DeviceAdapter::clearStack>("clearStack"),
             InstanceMethod<&DeviceAdapter::tick>("tick")}
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Device", func);

        return exports;
    }

    IDevice* DeviceAdapter::from(const node::Object& obj) { return Unwrap(obj)->device_; }

    node::Value DeviceAdapter::to(node::Env env, const IDevice* device) {
        if (!device) return env.Null();

        node::Object obj = constructor.New({});

        DeviceAdapter* wrapper = Unwrap(obj);

        wrapper->device_ = const_cast<IDevice*>(device);

        return obj;
    }

    node::Value DeviceAdapter::readLogic(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Number::New(
            info.Env(), device_->readLogic(args.getWithCheck<node::String>(0).Utf8Value())
        );
    }

    void DeviceAdapter::writeLogic(const node::CallbackInfo& info) {
        Arguments args(info);

        device_->writeLogic(
            args.getWithCheck<node::String>(0).Utf8Value(),
            args.getWithCheck<node::Number>(1).DoubleValue()
        );
    }

    node::Value DeviceAdapter::canReadLogic(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Boolean::New(
            info.Env(), device_->canReadLogic(args.getWithCheck<node::String>(0).Utf8Value())
        );
    }

    node::Value DeviceAdapter::canWriteLogic(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Boolean::New(
            info.Env(), device_->canWriteLogic(args.getWithCheck<node::String>(0).Utf8Value())
        );
    }

    node::Value DeviceAdapter::readStack(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Number::New(
            info.Env(), device_->readStack(args.getWithCheck<node::Number>(0).Uint32Value())
        );
    }

    void DeviceAdapter::writeStack(const node::CallbackInfo& info) {
        Arguments args(info);

        device_->writeStack(
            args.getWithCheck<node::Number>(0).Uint32Value(),
            args.getWithCheck<node::Number>(1).DoubleValue()
        );
    }

    node::Value DeviceAdapter::readSlot(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Number::New(
            info.Env(), device_->readSlot(
                            args.getWithCheck<node::Number>(0).Uint32Value(),
                            args.getWithCheck<node::String>(1).Utf8Value()
                        )
        );
    }

    void DeviceAdapter::writeSlot(const node::CallbackInfo& info) {
        Arguments args(info);

        device_->writeSlot(
            args.getWithCheck<node::Number>(0).Uint32Value(),
            args.getWithCheck<node::String>(1).Utf8Value(),
            args.getWithCheck<node::Number>(2).DoubleValue()
        );
    }

    node::Value DeviceAdapter::readReagent(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Number::New(
            info.Env(), device_->readReagent(args.getWithCheck<node::Number>(0).Int32Value())
        );
    }

    node::Value DeviceAdapter::queryReagentAmount(const node::CallbackInfo& info) {
        Arguments args(info);

        return node::Number::New(
            info.Env(), device_->queryReagentAmount(args.getWithCheck<node::Number>(0).Int64Value())
        );
    }

    node::Value DeviceAdapter::getTypeHash(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), static_cast<double>(device_->getTypeHash()));
    }

    node::Value DeviceAdapter::getNameHash(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), static_cast<double>(device_->getNameHash()));
    }

    void DeviceAdapter::clearStack(const node::CallbackInfo& info) { device_->clearStack(); }

    void DeviceAdapter::tick(const node::CallbackInfo& info) { device_->tick(); }

}  // namespace stationeers::ic10
