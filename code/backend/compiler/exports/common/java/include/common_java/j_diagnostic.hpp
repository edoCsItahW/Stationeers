// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_diagnostic.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 16:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_J_DIAGNOSTIC_HPP
#define EXPORT_J_DIAGNOSTIC_HPP
#pragma once

#include "base.hpp"
#include "common/exception/diagnostic.hpp"

namespace stationeers {

    class JDiagnostic : public java::JavaClass<JDiagnostic> {
    public:
        static constexpr auto kJavaDescriptor = "Lcom/stationeers/ic10/Diagnostic;";

        static java::local_ref<javaobject> to(const Diagnostic& d);

    };

}  // namespace stationeers::ic10

#endif  // EXPORT_J_DIAGNOSTIC_HPP
