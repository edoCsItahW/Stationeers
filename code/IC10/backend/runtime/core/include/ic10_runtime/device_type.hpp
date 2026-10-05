// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device_type.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 12:00
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_DEVICE_TYPE_HPP
#define IC10_RUNTIME_DEVICE_TYPE_HPP
#pragma once

#include "ic10_compiler/pch/ast.hpp"
#include "ic10_compiler/semantic/semantic.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @struct DeviceMemberDecl
     * @brief 设备注解里声明的一项成员
     *
     * @details 对应 `#>` 设备块里的一行：`@logic Setting 12`（逻辑属性）、`@logic-slot Quantity 3`
     *          （槽位逻辑属性）、`@slot Slot0 0`（槽位）、`@device-hash 12345`（型号哈希）、
     *          `@name-hash 777`（名称哈希）、`@reagent-hash 1680000`（试剂哈希）。
     *
     * @note 成员在运行时按**名字**寻址（与源码里的写法一致），`value` 只原样保留注解中声明的取值
     *       （属性编号、槽位序号或哈希值），不参与寻址。
     *
     * @else
     *
     * @struct DeviceMemberDecl
     * @brief One member declared by a device annotation
     *
     * @details Corresponds to one line of a `#>` device block: `@logic Setting 12` (logic property),
     *          `@logic-slot Quantity 3` (slot logic property), `@slot Slot0 0` (slot),
     *          `@device-hash 12345` (type hash), `@name-hash 777` (name hash) and
     *          `@reagent-hash 1680000` (reagent hash).
     *
     * @note Members are addressed by **name** at run time (exactly as written in the source); `value`
     *       merely preserves what the annotation declares (property index, slot number or hash) and
     *       takes no part in addressing.
     *
     * @endif
     */
    struct DeviceMemberDecl {
        /** @if zh @brief 成员名（源码里的写法，如 `Setting`） @else @brief Member name (e.g. `Setting`) @endif */
        std::string name;

        /** @if zh @brief 注解声明的取值（属性编号、槽位序号或哈希值） @else @brief Value declared by the annotation (index, slot number or hash) @endif */
        std::string value;

        /** @if zh @brief 成员种类：`logic` / `logic-slot` / `slot` / `hash` / `reagent-hash` @else @brief Member kind: `logic` / `logic-slot` / `slot` / `hash` / `reagent-hash` @endif */
        std::string kind;

        /**
         * @if zh
         * @brief 注解声明的默认值（成员行末追加的那个取值）
         *
         * @details 写作 `@logic Setting 12 1` 时默认值为 1；不写则**没有默认值**（`nullopt`），
         *          而不是 0——设备的默认值不一定是 0，用 0 冒充会把"未声明默认值"与"默认值为 0"
         *          混为一谈。成员被赋值前，`IDevice::readLogic` 就以该值作答。
         *
         * @else
         * @brief Default declared by the annotation (the value appended to the member line)
         *
         * @details `@logic Setting 12 1` declares 1; omitting it means **no default**
         *          (`nullopt`) rather than 0, because a member's default is not necessarily 0 and
         *          reporting 0 would conflate "no default declared" with "defaults to 0". Before the
         *          member is assigned, `IDevice::readLogic` answers with this value.
         *
         * @endif
         */
        std::optional<double> defaultValue = std::nullopt;
    };

    /**
     * @if zh
     *
     * @struct DeviceType
     * @brief 设备型号：由一条 `#> @device` 注解描述的**可能成员集合**
     *
     * @details 注解相当于设备的类型（类似联合类型），一个具体设备实例在某一时刻只提供其中的一部分
     *          成员（见 `IDevice::setActive` 与 `IDevice::snapshot`）。
     *
     * @else
     *
     * @struct DeviceType
     * @brief A device type: the **set of possible members** described by one `#> @device` annotation
     *
     * @details The annotation plays the role of the device's type (a union type, so to speak); a
     *          concrete instance provides only part of those members at any instant (see
     *          `IDevice::setActive` and `IDevice::snapshot`).
     *
     * @endif
     */
    struct DeviceType {
        /** @if zh @brief 型号名（`@name` 行） @else @brief Type name (the `@name` line) @endif */
        std::string name;

        /** @if zh @brief 型号哈希（`@device-hash`，缺失或非数字时为 0） @else @brief Type hash (`@device-hash`, 0 when missing or non-numeric) @endif */
        std::int64_t deviceHash = 0;

        /** @if zh @brief 名称哈希（`@name-hash`，缺失或非数字时为 0） @else @brief Name hash (`@name-hash`, 0 when missing or non-numeric) @endif */
        std::int64_t nameHash = 0;

        /** @if zh @brief 声明的成员，按注解中的书写顺序 @else @brief Declared members, in annotation order @endif */
        std::vector<DeviceMemberDecl> members;

        /**
         * @if zh
         * @brief 查找声明成员
         * @param name 成员名
         * @return 成员声明，未声明时为空指针
         * @else
         * @brief Find a declared member
         * @param name Member name
         * @return The declaration, or nullptr when the member is not declared
         * @endif
         */
        [[nodiscard]] const DeviceMemberDecl* findMember(const std::string& name) const noexcept;

        /**
         * @if zh
         * @brief 是否声明了该成员
         * @param name 成员名
         * @return 已声明则为 true
         * @else
         * @brief Whether the member is declared
         * @param name Member name
         * @return true when declared
         * @endif
         */
        [[nodiscard]] bool contains(const std::string& name) const noexcept;

        /** @if zh @brief 是否没有任何声明成员 @else @brief Whether there is no declared member @endif */
        [[nodiscard]] bool empty() const noexcept;
    };

    /**
     * @if zh
     *
     * @class DeviceRegistry
     * @brief 设备型号表：运行时的设备类型注册处
     *
     * @details 由执行上下文从编译产物的类型表（`TypeTable`，即 `#>` 设备块）构建，供 `Manager` 按型号名
     *          创建模拟设备。型号表按值持有，因此可以随 `Manager` 一起移动，实例内保存的是型号副本，
     *          不存在指向注册处的悬垂引用。
     *
     * @else
     *
     * @class DeviceRegistry
     * @brief Device type table: the run time's registry of device types
     *
     * @details Built by the execution context from the compiler's type table (`TypeTable`, i.e. the
     *          `#>` device blocks) and used by `Manager` to create simulated devices by type name.
     *          The registry holds its types by value so it can be moved together with the `Manager`,
     *          and every instance keeps a copy of its type — no reference back into the registry.
     *
     * @endif
     */
    class DeviceRegistry {
    public:
        DeviceRegistry() = default;

        /**
         * @if zh
         * @brief 登记一条设备注解
         * @param annotation 设备注解节点
         * @else
         * @brief Register one device annotation
         * @param annotation The device annotation node
         * @endif
         */
        void add(const DeviceAnnotation& annotation);

        /**
         * @if zh
         * @brief 按名字从类型表登记型号
         * @param types 类型表（语义分析的产物，可能包含其它编译单元的声明）
         * @param name 型号名
         * @return 类型表中存在该设备注解并登记成功则为 true
         * @else
         * @brief Register a type looked up by name in the type table
         * @param types The type table (semantic analysis output; may hold other units' declarations)
         * @param name Type name
         * @return true when the type table holds such a device annotation and it was registered
         * @endif
         */
        bool addFrom(const TypeTable& types, const std::string& name);

        /**
         * @if zh
         * @brief 按型号名查找
         * @param name 型号名
         * @return 型号，未登记时为空指针
         * @else
         * @brief Look a type up by name
         * @param name Type name
         * @return The type, or nullptr when it is not registered
         * @endif
         */
        [[nodiscard]] const DeviceType* find(const std::string& name) const noexcept;

        /** @if zh @brief 已登记型号数量 @else @brief Number of registered types @endif */
        [[nodiscard]] std::size_t size() const noexcept;

        /** @if zh @brief 型号表是否为空 @else @brief Whether the registry is empty @endif */
        [[nodiscard]] bool empty() const noexcept;

    private:
        /** @if zh @brief 型号名 → 型号 @else @brief Type name → type @endif */
        std::unordered_map<std::string, DeviceType> types_;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_DEVICE_TYPE_HPP
