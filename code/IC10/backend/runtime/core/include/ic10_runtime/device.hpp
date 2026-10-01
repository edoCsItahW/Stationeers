// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/10/05 12:00
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_DEVICE_HPP
#define IC10_RUNTIME_DEVICE_HPP
#pragma once

#include "ic10_runtime/device_type.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @struct DeviceMemberView
     * @brief 设备成员在某一时刻的状态
     *
     * @details 由 @ref IDevice::snapshot 产出，供宿主与调试器内省设备：既包含型号注解声明的成员
     *          （`isDeclared` 为 true），也包含程序写过而型号没有声明的成员（`isDeclared` 为 false）。
     *
     * @note 成员**只有被赋值过才有值**：`assigned` 为 false 时 `value` 为空，因为设备成员的默认值不
     *       一定是 0，用 0 冒充会把"从未赋值"与"赋值为 0"混为一谈。
     *
     * @else
     *
     * @struct DeviceMemberView
     * @brief State of one device member at a given instant
     *
     * @details Produced by @ref IDevice::snapshot for host and debugger introspection. It covers both
     *          members declared by the device annotation (`isDeclared` is true) and members the program
     *          wrote without any declaration (`isDeclared` is false).
     *
     * @note A member **has a value only once it was assigned**: when `assigned` is false `value` is
     *       empty, because a device member's default is not necessarily 0 and reporting 0 would
     *       conflate "never assigned" with "assigned 0".
     *
     * @endif
     */
    struct DeviceMemberView {
        /** @if zh @brief 成员名 @else @brief Member name @endif */
        std::string name;

        /** @if zh @brief 成员种类：`logic` / `logic-slot` / `slot` / `hash` / `reagent-hash` @else @brief Member kind: `logic` / `logic-slot` / `slot` / `hash` / `reagent-hash` @endif */
        std::string kind;

        /** @if zh @brief 注解声明的取值（未声明时为空） @else @brief Value declared by the annotation (empty when undeclared) @endif */
        std::string declaredValue;

        /** @if zh @brief 当前值；从未赋值时为 nullopt @else @brief Current value; nullopt when it was never assigned @endif */
        std::optional<double> value;

        /** @if zh @brief 该成员是否由型号注解声明 @else @brief Whether the member is declared by the device type @endif */
        bool isDeclared = true;

        /** @if zh @brief 该成员是否已被赋值 @else @brief Whether the member was assigned @endif */
        bool assigned = false;
    };

    class IDevice {
    public:
        virtual ~IDevice() = default;

        virtual double readLogic(const std::string& prop) = 0;

        virtual void writeLogic(const std::string& prop, double value) = 0;

        virtual bool canReadLogic(const std::string& prop) const = 0;

        virtual bool canWriteLogic(const std::string& prop) const = 0;

        virtual double readStack(std::size_t index) = 0;

        virtual void writeStack(std::size_t index, double value) = 0;

        virtual double readSlot(std::size_t index, const std::string& slot) = 0;

        virtual void writeSlot(std::size_t index, const std::string& slot, double value) = 0;

        virtual double readReagent(int mode) = 0;

        virtual double queryReagentAmount(int64_t reagentHash) = 0;

        virtual int64_t getTypeHash() const = 0;

        virtual int64_t getNameHash() const = 0;

        virtual void clearStack() = 0;

        virtual void tick();

        /**
         * @if zh
         * @brief 设备型号名
         * @return 型号名；未声明型号（或无型号信息的宿主设备）时为空字符串
         * @else
         * @brief Device type name
         * @return The type name; empty when no type is declared (or the host device carries none)
         * @endif
         */
        [[nodiscard]] virtual std::string typeName() const;

        /**
         * @if zh
         * @brief 成员与各自此刻的状态
         * @return 成员视图列表；不提供内省能力的宿主设备可以返回空列表
         * @else
         * @brief Members together with their current state
         * @return The member views; a host device without introspection may return an empty list
         * @endif
         */
        [[nodiscard]] virtual std::vector<DeviceMemberView> snapshot() const;
    };

    class VirtualDevice : public IDevice {
    public:
        VirtualDevice();

        double readLogic(const std::string& prop) override;

        void writeLogic(const std::string& prop, double value) override;

        bool canReadLogic(const std::string& prop) const override;

        bool canWriteLogic(const std::string& prop) const override;

        double readStack(std::size_t index) override;

        void writeStack(std::size_t index, double value) override;

        double readSlot(std::size_t index, const std::string& slot) override;

        void writeSlot(std::size_t index, const std::string& slot, double value) override;

        double readReagent(int mode) override;

        double queryReagentAmount(int64_t reagentHash) override;

        int64_t getTypeHash() const override;

        int64_t getNameHash() const override;

        void clearStack() override;

    protected:
        std::unordered_map<std::string, double> logicProps_;

        std::vector<double> stack_;
    };

    /**
     * @if zh
     *
     * @class SimDevice
     * @brief 型号感知的模拟设备
     *
     * @details 在 @ref VirtualDevice 之上引入设备**型号**（`#>` 设备注解，见 @ref DeviceType）：
     *          型号声明了该设备有哪些成员（注解给出的字段范围），@ref snapshot 如实报告每个成员
     *          是否被赋过值。型号哈希与名称哈希取自注解，因此按哈希查找设备
     *          （`lb` / `lbn` / `sb` / `sbn`）在型号化的端口上才有意义。
     *
     * @note 型号**只影响身份与内省，不影响执行语义**：读写逻辑属性仍沿用 @ref VirtualDevice 的行为
     *       （任意名字可写、读未赋过值的属性为 0、`canReadLogic` 表示是否被赋过值），因此现有程序
     *       与测试的语义不变；对未声明成员的约束与诊断留待后续阶段。
     *
     * @else
     *
     * @class SimDevice
     * @brief Type-aware simulated device
     *
     * @details Adds a device **type** (the `#>` device annotation, see @ref DeviceType) on top of
     *          @ref VirtualDevice: the type declares which members the device has (the field range
     *          given by the annotation) and @ref snapshot reports whether each member was assigned.
     *          Type and name hashes come from the annotation, which is what makes hash based lookups
     *          (`lb` / `lbn` / `sb` / `sbn`) meaningful on typed ports.
     *
     * @note The type **affects identity and introspection only, not execution semantics**: logic
     *       properties still behave as in @ref VirtualDevice (any name may be written, an unassigned
     *       property reads as 0, `canReadLogic` tells whether it was assigned), so existing programs
     *       and tests keep their semantics. Constraining or diagnosing undeclared members is left to
     *       a later stage.
     *
     * @endif
     */
    class SimDevice : public VirtualDevice {
    public:
        /**
         * @if zh
         * @brief 构造一个模拟设备
         * @param type 型号（声明成员集合）；空型号表示无型号设备
         * @param typeName 型号名；为空时取型号自身的名字（类型表里没有对应注解的名字也保留，供显示）
         * @else
         * @brief Construct a simulated device
         * @param type The device type (declared member set); an empty type means an untyped device
         * @param typeName Type name; defaults to the type's own name (a name without a matching
         *                 annotation is kept for display purposes)
         * @endif
         */
        explicit SimDevice(DeviceType type = {}, std::string typeName = {});

        int64_t getTypeHash() const override;

        int64_t getNameHash() const override;

        [[nodiscard]] std::string typeName() const override;

        [[nodiscard]] std::vector<DeviceMemberView> snapshot() const override;

        /**
         * @if zh
         * @brief 型号声明
         * @return 型号（无型号设备为空型号）
         * @else
         * @brief The device type
         * @return The type (an empty type for an untyped device)
         * @endif
         */
        [[nodiscard]] const DeviceType& type() const noexcept { return type_; }

    private:
        DeviceType type_;

        std::string typeName_;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_DEVICE_HPP
