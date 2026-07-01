// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file build.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 12:57
 * @if zh
 * @brief common模块Node.js导出初始化
 * @details 提供common模块在Node.js环境中的初始化入口函数。
 *        该函数由N-API调用,注册所有导出的类和函数。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief common module Node.js export initialization
 * @details Provides initialization entry function for common module in Node.js environment.
 *        This function is called by N-API to register all exported classes and functions.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_BUILD_HPP
#define EXPORT_BUILD_HPP
#pragma once

#include "base.hpp"

node::Object commonInit(const node::Env env, const node::Object exports);

#endif  // EXPORT_BUILD_HPP
