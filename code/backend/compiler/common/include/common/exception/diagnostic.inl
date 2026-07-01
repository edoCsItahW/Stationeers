// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file diagnostic.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/30 14:36
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_DIAGNOSTIC_INL
#define COMPILER_DIAGNOSTIC_INL
#pragma once

#include "common/locals/local.hpp"
#include "common/utils/enum_to_str.hpp"

namespace stationeers::ic10 {

    template<IsMsgPack P>
    const std::vector<Diagnostic>& DiagnosticReporter<P>::getDiagnostics() const {
        return diagnostics_;
    }

#ifndef _MSC_VER

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I>
    void DiagnosticReporter<P>::emplace(const Error& error) {
        diagnostics_.push_back({ DiagnosticLevel::Error, enumToStr(I), error.getStart(), error.getEnd(), error.message() });
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I>
    void DiagnosticReporter<P>::report(DiagnosticLevel level, Pos start, Pos end) {
        diagnostics_.push_back({level, enumToStr(I), start, end, Local<P>::template msgStr<I>()});
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I>
    void DiagnosticReporter<P>::error(Pos start, Pos end) {
        report<I>(DiagnosticLevel::Error, start, end);
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I>
    void DiagnosticReporter<P>::warning(Pos start, Pos end) {
        report<I>(DiagnosticLevel::Warning, start, end);
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I>
    void DiagnosticReporter<P>::info(Pos start, Pos end) {
        report<I>(DiagnosticLevel::Info, start, end);
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I, typename... Args>
    void DiagnosticReporter<P>::reportWith(
        DiagnosticLevel level, Pos start, Pos end, Args&&... args
    ) {
        diagnostics_.push_back(
            {level, enumToStr(I), start, end,
             Local<P>::template msgFormat<I>(std::forward<Args>(args)...)}
        );
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I, typename... Args>
    void DiagnosticReporter<P>::errorWith(Pos start, Pos end, Args&&... args) {
        reportWith<I, Args...>(DiagnosticLevel::Error, start, end, std::forward<Args>(args)...);
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I, typename... Args>
    void DiagnosticReporter<P>::warnWith(Pos start, Pos end, Args&&... args) {
        reportWith<I, Args...>(DiagnosticLevel::Warning, start, end, std::forward<Args>(args)...);
    }

    template<IsMsgPack P>
    template<typename DiagnosticReporter<P>::E I, typename... Args>
    void DiagnosticReporter<P>::infoWith(Pos start, Pos end, Args&&... args) {
        reportWith<I, Args...>(DiagnosticLevel::Info, start, end, std::forward<Args>(args)...);
    }

#endif

}  // namespace stationeers::ic10

#endif  // COMPILER_DIAGNOSTIC_INL
