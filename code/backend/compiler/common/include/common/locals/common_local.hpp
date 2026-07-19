/**
 * @file common_local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:36
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COMMON_LOCAL_PRIVATE_HPP
#define COMPILER_COMMON_LOCAL_PRIVATE_HPP
#pragma once

#include "local.hpp"

namespace stationeers {

    enum class CommonMsgId : uint32_t {
        CEF1_1,  // could not open file '{}' - file system.
        CEF2_1,  // failed to read file '{}' - file system.
        CEF3_1   // unable to write to file '{}' - file system.

    };

    template<typename, CommonMsgId>
    struct CommonMsgMap;

    using CommonMsgPack = MsgPack<CommonMsgId, CommonMsgMap>;

    class CommonLocal : public Local<CommonMsgPack> {};

    using CMsgId = CommonMsgId;

    using CLoc = CommonLocal;

#define COMMON_LOCAL_MSG(type, id, _msg)                                                           \
    template<>                                                                                     \
    struct CommonMsgMap<type, CommonMsgId::id> {                                                   \
        static constexpr auto msg = _msg##_fs;                                                     \
    };

}  // namespace stationeers

#endif  // COMPILER_COMMON_LOCAL_PRIVATE_HPP
