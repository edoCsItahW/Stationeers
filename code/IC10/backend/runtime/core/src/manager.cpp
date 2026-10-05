// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file manager.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:40
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/manager.hpp"
#include <algorithm>
#include <cctype>
#include <ranges>
#include <utility>

namespace stationeers::ic10 {

    bool isStaticPort(const std::string& name) noexcept {
        if (name == "db") return true;

        // d0-d5（可带引脚 d0:1）；dr0 / drr0 之类的动态端口在运行期换算后才落到具体端口
        if (name.size() < 2 || name.front() != 'd') return false;

        return std::isdigit(static_cast<unsigned char>(name[1])) != 0;
    }

    Manager::Manager()
        : chip_(std::make_unique<VirtualDevice>()) {}

    IDevice* Manager::getDevice(const std::string& name) {
        if (name == "db")
            return chip_.get();

        if (const auto& it = devices_.find(name); it != devices_.end())
            return it->second.get();

        return nullptr;
    }

    void Manager::setExternalDevice(const std::string& name, std::unique_ptr<IDevice> device) {
        devices_[name] = std::move(device);
    }

    void Manager::setChipDevice(std::unique_ptr<IDevice> device) noexcept {
        chip_ = std::move(device);
    }

    void Manager::setRegistry(DeviceRegistry registry) { registry_ = std::move(registry); }

    const DeviceRegistry& Manager::registry() const noexcept { return registry_; }

    IDevice* Manager::bindTyped(const std::string& name, const std::string& typeName) {
        // 型号表里没有该型号（例如注解缺失或名字写错）时退化为无型号设备：不改变执行语义
        const DeviceType* type = registry_.find(typeName);

        auto device = std::make_unique<SimDevice>(type ? *type : DeviceType{}, typeName);

        IDevice* raw = device.get();

        // 自引用设备由芯片设备承载，getDevice("db") 不会走 devices_ 表
        if (name == "db") chip_ = std::move(device);
        else devices_[name] = std::move(device);

        return raw;
    }

    std::vector<std::string> Manager::ports() const {
        std::vector<std::string> names;

        names.reserve(devices_.size());

        for (const auto& name : devices_ | std::views::keys)
            names.push_back(name);

        std::ranges::sort(names);

        return names;
    }

    IDevice* Manager::findDeviceByType(const int64_t typeHash) const {
        for (const auto& dev : devices_ | std::views::values)
            if (dev && dev->getTypeHash() == typeHash)
                return dev.get();

        return nullptr;
    }

    IDevice* Manager::findDeviceByTypeAndName(const int64_t typeHash, const int64_t nameHash) const {
        for (const auto& dev : devices_ | std::views::values)
            if (dev && dev->getTypeHash() == typeHash && dev->getNameHash() == nameHash)
                return dev.get();

        return nullptr;
    }

    std::vector<IDevice*> Manager::findDevicesByType(const int64_t typeHash) const {
        std::vector<IDevice*> result;
        for (const auto& dev : devices_ | std::views::values)
            if (dev && dev->getTypeHash() == typeHash)
                result.push_back(dev.get());
        if (chip_ && chip_->getTypeHash() == typeHash)
            result.push_back(chip_.get());
        return result;
    }

    std::vector<IDevice*> Manager::findDevicesByTypeAndName(
        const int64_t typeHash, const int64_t nameHash
    ) const {
        std::vector<IDevice*> result;
        for (const auto& dev : devices_ | std::views::values)
            if (dev && dev->getTypeHash() == typeHash && dev->getNameHash() == nameHash)
                result.push_back(dev.get());
        if (chip_ && chip_->getTypeHash() == typeHash && chip_->getNameHash() == nameHash)
            result.push_back(chip_.get());
        return result;
    }

    void Manager::tick() {
        for (auto& dev : devices_ | std::views::values)
            if (dev)
                dev->tick();

        if (chip_)
            chip_->tick();
    }

}  // namespace stationeers::ic10
