// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:40
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/device.hpp"

namespace stationeers::ic10 {

    void IDevice::tick() {}

    VirtualDevice::VirtualDevice()
        : stack_(512, 0.0) {}

    double VirtualDevice::readLogic(const std::string& prop) {
        if (const auto& it = logicProps_.find(prop); it != logicProps_.end()) return it->second;

        return 0.0;
    }

    void VirtualDevice::writeLogic(const std::string& prop, double value) { logicProps_[prop] = value; }

    bool VirtualDevice::canReadLogic(const std::string& prop) const {
        return logicProps_.contains(prop);
    }

    bool VirtualDevice::canWriteLogic(const std::string& prop [[maybe_unused]]) const { return true; }

    double VirtualDevice::readSlot(std::size_t index, const std::string& slot [[maybe_unused]]) {
        return readStack(index);
    }

    void VirtualDevice::writeSlot(std::size_t index, const std::string& slot [[maybe_unused]], double value) {
        writeStack(index, value);
    }

    double VirtualDevice::readReagent(int mode [[maybe_unused]]) { return 0.0; }

    double VirtualDevice::queryReagentAmount(int64_t reagentHash [[maybe_unused]]) { return 0.0; }

    int64_t VirtualDevice::getTypeHash() const { return 0; }

    int64_t VirtualDevice::getNameHash() const { return 0; }

    double VirtualDevice::readStack(std::size_t index) { return stack_[index]; }

    void VirtualDevice::writeStack(std::size_t index, double value) { stack_[index] = value; }

    void VirtualDevice::clearStack() { std::fill(stack_.begin(), stack_.end(), 0.0); }

}  // namespace stationeers::ic10
