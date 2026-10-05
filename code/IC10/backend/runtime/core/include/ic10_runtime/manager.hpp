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
#include "device_type.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 端口名是否为静态端口
     * @details 静态端口是 `d0`-`d5`（可带引脚，如 `d0:1`）与自引用设备 `db`；动态端口
     *          （`dr0` / `drr0`）要等运行期读出寄存器才知道落在哪个端口，因此不算静态端口。
     *
     * @param name 端口名或设备别名指向的文本
     * @return 静态端口则为 true
     *
     * @else
     *
     * @brief Whether a port name is a static port
     * @details Static ports are `d0`-`d5` (optionally with a pin, e.g. `d0:1`) plus the self
     *          reference device `db`; a dynamic port (`dr0` / `drr0`) only resolves to a port once
     *          the register is read at run time, so it is not static.
     *
     * @param name Port name, or the text a device alias points at
     * @return true for a static port
     *
     * @endif
     */
    [[nodiscard]] bool isStaticPort(const std::string& name) noexcept;

    class Manager {
    public:
        Manager();

        IDevice* getDevice(const std::string& name);

        void setExternalDevice(const std::string& name, std::unique_ptr<IDevice> device);

        void setChipDevice(std::unique_ptr<IDevice> device) noexcept;

        /**
         * @if zh
         * @brief 设置设备型号表
         * @param registry 型号表（由执行上下文从类型表构建）
         * @else
         * @brief Set the device type table
         * @param registry The type table (built by the execution context from the type table)
         * @endif
         */
        void setRegistry(DeviceRegistry registry);

        /**
         * @if zh
         * @brief 设备型号表
         * @return 型号表引用
         * @else
         * @brief The device type table
         * @return Reference to the registry
         * @endif
         */
        [[nodiscard]] const DeviceRegistry& registry() const noexcept;

        /**
         * @if zh
         *
         * @brief 按型号名创建并绑定一个模拟设备
         *
         * @param name 端口名；`db` 表示自引用设备（替换芯片设备）
         * @param typeName 型号名
         *
         * @return 新建的设备
         *
         * @details 型号表里有该型号时创建型号感知的 @ref SimDevice，否则退化为无型号设备（型号名仍保留，
         *          便于宿主与调试器显示）。已绑定的同名端口会被替换。
         *
         * @else
         *
         * @brief Create and bind a simulated device by type name
         *
         * @param name Port name; `db` means the self reference device (replaces the chip device)
         * @param typeName Type name
         *
         * @return The newly created device
         *
         * @details Creates a type aware @ref SimDevice when the registry knows the type, otherwise an
         *          untyped device (the type name is still kept for host and debugger display). An
         *          existing binding under the same port is replaced.
         *
         * @endif
         */
        IDevice* bindTyped(const std::string& name, const std::string& typeName);

        /**
         * @if zh
         * @brief 已绑定的外部端口名（不含芯片设备 `db`），按名字排序
         * @return 端口名列表
         * @else
         * @brief Names of the bound external ports (chip device `db` excluded), sorted by name
         * @return The port names
         * @endif
         */
        [[nodiscard]] std::vector<std::string> ports() const;

        IDevice* findDeviceByType(int64_t typeHash) const;

        IDevice* findDeviceByTypeAndName(int64_t typeHash, int64_t nameHash) const;

        std::vector<IDevice*> findDevicesByType(int64_t typeHash) const;

        std::vector<IDevice*> findDevicesByTypeAndName(int64_t typeHash, int64_t nameHash) const;

        void tick();

    private:
        std::unordered_map<std::string, std::unique_ptr<IDevice>> devices_;

        std::unique_ptr<IDevice> chip_;

        /** @if zh @brief 型号表 @else @brief The device type table @endif */
        DeviceRegistry registry_;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_MANAGER_HPP
