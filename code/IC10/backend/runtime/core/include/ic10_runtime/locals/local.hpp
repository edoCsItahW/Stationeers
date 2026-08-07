// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/08 13:58
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_LOCAL_HPP
#define IC10_RUNTIME_LOCAL_HPP
#pragma once

#include "common/locals/local.hpp"

namespace stationeers::ic10 {

    enum class IC10RuntimeMsgId : uint32_t {
        IEM0,    // Stack Overflow Error.
        IEM1,    // Stack Underflow Error.
        IEM2_1,  // Unable to evaluate operand '{}'.
        IEM3_1,  // Index out of range.
        IEC4,    // There are incorrect statements present.
    };

    template<typename, IC10RuntimeMsgId>
    struct IC10RuntimeMsgMap;

    using IC10RuntimeMsgPack = MsgPack<IC10RuntimeMsgId, IC10RuntimeMsgMap>;

    class IC10RuntimeLocal : public Local<IC10RuntimeMsgPack> {};

    using IRMsgId = IC10RuntimeMsgId;

    using IRLoc = IC10RuntimeLocal;

#define IC10_RUNTIME_LOCAL_MSG(type, id, _msg)                                                     \
    template<>                                                                                     \
    struct IC10RuntimeMsgMap<type, IC10RuntimeMsgId::id> {                                         \
        static constexpr auto msg = _msg##_fs;                                                     \
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_LOCAL_HPP
