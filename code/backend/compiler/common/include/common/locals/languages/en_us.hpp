/**
 * @file en_us.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:42
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COMMON_EN_US_HPP
#define COMPILER_COMMON_EN_US_HPP
#pragma once

#include "../common_local.hpp"
#include "common/utils/fstr.hpp"

namespace stationeers {

    struct EnUs;

#define COMMON_LOCAL_MSG_EN_US(id, msg) COMMON_LOCAL_MSG(EnUs, id, msg)

    COMMON_LOCAL_MSG_EN_US(CEF1_1, "could not open file '{}' - file system.")

    COMMON_LOCAL_MSG_EN_US(CEF2_1, "failed to read file '{}' - file system.")

    COMMON_LOCAL_MSG_EN_US(CEF3_1, "unable to write to file '{}' - file system.")

}  // namespace stationeers


#endif  // COMPILER_COMMON__EN_US_HPP
