// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:39
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/memory/memory.hpp"
#include "common/utils/json.hpp"
#include "ic10_runtime/locals/local.hpp"

namespace stationeers::ic10 {

    Memory::Memory(const Config& config)
        : registers_([] { return 0.0; })
        , sp_(0)
        , stack_(config.maxStackSize, 0.0)
        , cfg(config) {}

    void Memory::setReporter(DiagnosticReporter<IC10RuntimeMsgPack>* reporter) noexcept {
        reporter_ = reporter;
    }

    void Memory::push(
        const double val,
        const std::optional<std::function<StackOverflowError(std::string)>>& onOverflow
    ) noexcept {
        if (sp_ >= cfg.maxStackSize) [[unlikely]]
            reporter_->emplace<IRMsgId::IEM0>(
                {(onOverflow ? (*onOverflow)(IRLoc::msgStr<IRMsgId::IEM0>())
                             : StackOverflowError(IRLoc::msgStr<IRMsgId::IEM0>()))}
            );

        stack_[static_cast<std::size_t>(sp_)] = val;

        ++sp_;
    }

    double Memory::pop(
        const std::optional<std::function<StackOverflowError(std::string)>>& onOverflow
    ) noexcept {
        if (sp_ <= 0) [[unlikely]]
            reporter_->emplace<IRMsgId::IEM1>(
                {onOverflow ? (*onOverflow)(IRLoc::msgStr<IRMsgId::IEM1>())
                            : StackOverflowError(IRLoc::msgStr<IRMsgId::IEM1>())}
            );

        --sp_;

        return stack_[static_cast<std::size_t>(sp_)];
    }

    double Memory::peek(
        const std::optional<std::function<StackOverflowError(std::string)>>& onOverflow
    ) const noexcept {
        if (sp_ <= 0) [[unlikely]]
            reporter_->emplace<IRMsgId::IEM0>(
                {onOverflow ? (*onOverflow)(IRLoc::msgStr<IRMsgId::IEM0>())
                            : StackOverflowError(IRLoc::msgStr<IRMsgId::IEM0>())}
            );

        return stack_[static_cast<std::size_t>(sp_ - 1)];
    }

    void Memory::poke(
        const std::size_t idx, const double val,
        const std::optional<std::function<RangeError(std::string)>>& onRange
    ) noexcept {
        if (idx >= cfg.maxStackSize) [[unlikely]]
            reporter_->emplace<IRMsgId::IEM3>(
                {onRange ? (*onRange)(IRLoc::msgStr<IRMsgId::IEM3>())
                         : RangeError(IRLoc::msgStr<IRMsgId::IEM3>())}
            );

        stack_[idx] = val;
    }

    std::string Memory::toJSON() const {
        return toJson<"registers", "stack", "sp">(toJson(registers_.getMap()), toJson(stack_), sp_);
    }

}  // namespace stationeers::ic10
