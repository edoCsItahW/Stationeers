// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file locals.hpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/16 17:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_LOCALS_HPP
#define EXPORT_LOCALS_HPP
#pragma once

#include "common_node/base.hpp"

node::Value setLanguage(const node::CallbackInfo& info);

void initLocals(node::Env& env, node::Object& exports);

#endif //EXPORT_LOCALS_HPP
