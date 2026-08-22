// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file manager.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:39
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_MANAGER_HPP
#define IC10_RUNTIME_MANAGER_HPP
#pragma once

#include "device.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

namespace stationeers::ic10 {

    class Manager {
    public:
        Manager();

        IDevice* getDevice(const std::string& name);

        void setExternalDevice(const std::string& name, std::unique_ptr<IDevice> device);

        void setChipDevice(std::unique_ptr<IDevice> device) noexcept;

        IDevice* findDeviceByType(int64_t typeHash) const;

        IDevice* findDeviceByTypeAndName(int64_t typeHash, int64_t nameHash) const;

        std::vector<IDevice*> findDevicesByType(int64_t typeHash) const;

        std::vector<IDevice*> findDevicesByTypeAndName(int64_t typeHash, int64_t nameHash) const;

        void tick();

    private:
        std::unordered_map<std::string, std::unique_ptr<IDevice>> devices_;

        std::unique_ptr<IDevice> chip_;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_MANAGER_HPP
