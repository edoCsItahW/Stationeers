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
#include <ranges>

namespace stationeers::ic10 {

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

    void Manager::setChipDevice(std::unique_ptr<IDevice> device) {
        chip_ = std::move(device);
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
