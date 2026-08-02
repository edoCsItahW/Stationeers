// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file build.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 18:46
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_java/build.hpp"
#include "common_java/j_pos.hpp"

namespace stationeers {

    void registerNatives() { JPos::registerNatives(); }

}  // namespace stationeers::ic10
