// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file diagnostic.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/30 11:29
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_DIAGNOSTIC_HPP
#define COMPILER_DIAGNOSTIC_HPP
#pragma once

#include "common/exception/error.hpp"
#include "common/locals/local.hpp"
#include "common/utils/position.hpp"
#include <string>
#include <vector>

namespace stationeers::ic10 {

    enum class DiagnosticLevel { Error, Warning, Info };

    struct Diagnostic {
        DiagnosticLevel level;

        std::string id;

        Pos start;

        Pos end;

        std::string message;
    };

    template<IsMsgPack P>
    class DiagnosticReporter {
    public:
        using E = P::msg_id_type;

        const std::vector<Diagnostic>& getDiagnostics() const;

        template<E I>
        void emplace(const Error& error)
#ifdef _MSC_VER
        {
            diagnostics_.push_back(
                {DiagnosticLevel::Error, enumToStr(I), error.getStart(), error.getEnd(),
                 error.message()}
            );
        }
#else
            ;
#endif

        template<E I>
        void report(DiagnosticLevel level, Pos start, Pos end)
#ifdef _MSC_VER
        {
            diagnostics_.push_back(
                {level, enumToStr(I), start, end, Local<P>::template msgStr<I>()}
            );
        }
#else
            ;
#endif

        template<E I>
        void error(Pos start, Pos end)
#ifdef _MSC_VER
        {
            report<I>(DiagnosticLevel::Error, start, end);
        }
#else
            ;
#endif

        template<E I>
        void warn(Pos start, Pos end)
#ifdef _MSC_VER
        {
            report<I>(DiagnosticLevel::Warning, start, end);
        }
#else
            ;
#endif

        template<E I>
        void info(Pos start, Pos end)
#ifdef _MSC_VER
        {
            report<I>(DiagnosticLevel::Info, start, end);
        }

#else
            ;
#endif

        template<E I, typename... Args>
        void reportWith(DiagnosticLevel level, Pos start, Pos end, Args&&... args)
#ifdef _MSC_VER
        {
            diagnostics_.push_back(
                {level, enumToStr(I), start, end,
                 Local<P>::template msgFormat<I>(std::forward<Args>(args)...)}
            );
        }
#else
            ;
#endif

        template<E I, typename... Args>
        void errorWith(Pos start, Pos end, Args&&... args)
#ifdef _MSC_VER
        {
            reportWith<I, Args...>(DiagnosticLevel::Error, start, end, std::forward<Args>(args)...);
        }
#else
            ;
#endif

        template<E I, typename... Args>
        void warnWith(Pos start, Pos end, Args&&... args)
#ifdef _MSC_VER
        {
            reportWith<I, Args...>(
                DiagnosticLevel::Warning, start, end, std::forward<Args>(args)...
            );
        }
#else
            ;
#endif

        template<E I, typename... Args>
        void infoWith(Pos start, Pos end, Args&&... args)
#ifdef _MSC_VER
        {
            reportWith<I, Args...>(DiagnosticLevel::Info, start, end, std::forward<Args>(args)...);
        }
#else
            ;
#endif

    private:
        std::vector<Diagnostic> diagnostics_;
    };

}  // namespace stationeers::ic10

#include "diagnostic.inl"

#endif  // COMPILER_DIAGNOSTIC_HPP
