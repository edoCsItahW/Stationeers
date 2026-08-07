// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:38
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_DEVICE_HPP
#define IC10_RUNTIME_DEVICE_HPP
#pragma once

#include <string>
#include <unordered_map>

namespace stationeers::ic10 {

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

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_DEVICE_HPP
