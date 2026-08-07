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
 * @if zh
 * @brief 诊断信息系统
 * @details 定义诊断信息结构、诊断级别枚举和诊断报告器模板类。
 *          提供类型安全的诊断消息报告机制，支持错误、警告和信息三种级别。
 *
 * @note 诊断报告器是模板类，通过消息打包类型(P)实现编译期类型检查。
 *
 * @par 使用方式:
 * @code
 * DiagnosticReporter<IC10MsgPack> reporter;
 *
 * // 报告错误（无参数）
 * reporter.error<IC10MsgId::IEA3_1>(startPos, endPos);
 *
 * // 报告错误（带参数）
 * reporter.errorWith<IC10MsgId::IEL1_1>(startPos, endPos, operatorName);
 *
 * // 报告警告
 * reporter.warn<IC10MsgId::IWL2>(pos, pos);
 *
 * // 获取所有诊断
 * auto diagnostics = reporter.getDiagnostics();
 * @endcode
 *
 * @see DiagnosticLevel 诊断级别枚举
 * @see Diagnostic 诊断信息结构
 * @see Error 错误异常类
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Diagnostic information system
 * @details Defines diagnostic information structure, diagnostic level enum, and diagnostic
 *          reporter template class. Provides type-safe diagnostic message reporting mechanism
 *          supporting three levels: error, warning, and info.
 *
 * @note The diagnostic reporter is a template class, achieving compile-time type checking
 *       through message pack type (P).
 *
 * @par Usage:
 * @code
 * DiagnosticReporter<IC10MsgPack> reporter;
 *
 * // Report error (no parameters)
 * reporter.error<IC10MsgId::IEA3_1>(startPos, endPos);
 *
 * // Report error (with parameters)
 * reporter.errorWith<IC10MsgId::IEL1_1>(startPos, endPos, operatorName);
 *
 * // Report warning
 * reporter.warn<IC10MsgId::IWL2>(pos, pos);
 *
 * // Get all diagnostics
 * auto diagnostics = reporter.getDiagnostics();
 * @endcode
 *
 * @see DiagnosticLevel Diagnostic level enum
 * @see Diagnostic Diagnostic information structure
 * @see Error Error exception class
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_DIAGNOSTIC_HPP
#define COMPILER_DIAGNOSTIC_HPP
#pragma once

#include "common/exception/error.hpp"
#include "common/locals/local.hpp"
#include "common/utils/position.hpp"
#include <string>
#include <vector>

namespace stationeers {

    /**
     * @if zh
     * @enum DiagnosticLevel
     * @brief 诊断级别枚举
     * @details 定义诊断信息的严重程度，用于区分错误、警告和信息消息。
     *
     * @elseif en
     * @enum DiagnosticLevel
     * @brief Diagnostic level enum
     * @details Defines the severity level of diagnostic information, used to distinguish
     *          errors, warnings, and informational messages.
     *
     * @endif
     */
    enum class DiagnosticLevel {
        /** @if zh @brief 错误级别，表示编译无法继续 @else @brief Error level, indicating compilation cannot continue @endif */
        Error,

        /** @if zh @brief 警告级别，表示潜在问题但不阻止编译 @else @brief Warning level, indicating potential issues but not blocking compilation @endif */
        Warning,

        /** @if zh @brief 信息级别，表示一般提示信息 @else @brief Info level, indicating general informational messages @endif */
        Info
    };

    /**
     * @if zh
     * @struct Diagnostic
     * @brief 诊断信息结构
     * @details 包含单个诊断消息的完整信息，包括级别、ID、位置和消息内容。
     *          用于在编译过程中传递和存储诊断结果。
     *
     * @elseif en
     * @struct Diagnostic
     * @brief Diagnostic information structure
     * @details Contains complete information for a single diagnostic message, including
     *          level, ID, position, and message content. Used for passing and storing
     *          diagnostic results during compilation.
     *
     * @endif
     */
    struct Diagnostic {
        /** @if zh @brief 诊断级别（Error/Warning/Info） @else @brief Diagnostic level (Error/Warning/Info) @endif */
        DiagnosticLevel level;

        /** @if zh @brief 诊断消息ID，用于唯一标识消息类型 @else @brief Diagnostic message ID, uniquely identifying the message type @endif */
        std::string id;

        /** @if zh @brief 诊断消息起始位置 @else @brief Diagnostic message start position @endif */
        Pos start;

        /** @if zh @brief 诊断消息结束位置 @else @brief Diagnostic message end position @endif */
        Pos end;

        /** @if zh @brief 本地化后的诊断消息文本 @else @brief Localized diagnostic message text @endif */
        std::string message;
    };

    /**
     * @if zh
     * @class DiagnosticReporter
     * @brief 诊断报告器模板类
     * @details 提供类型安全的诊断消息报告机制，支持错误、警告和信息三种级别。
     *          通过模板参数P指定消息打包类型，实现编译期类型检查。
     *
     * @tparam P 消息打包类型，必须满足IsMsgPack概念
     *
     * @note MSVC编译器下使用内联实现，其他编译器使用inl文件实现。
     *
     * @elseif en
     * @class DiagnosticReporter
     * @brief Diagnostic reporter template class
     * @details Provides type-safe diagnostic message reporting mechanism supporting
     *          three levels: error, warning, and info. Uses template parameter P to
     *          specify message pack type, achieving compile-time type checking.
     *
     * @tparam P Message pack type, must satisfy IsMsgPack concept
     *
     * @note Inline implementation is used for MSVC compiler, inl file implementation
     *       for other compilers.
     *
     * @endif
     */
    template<IsMsgPack P>
    class DiagnosticReporter {
    public:
        /** @if zh @brief 消息ID类型，从消息打包类型P中提取 @else @brief Message ID type, extracted from message pack type P @endif */
        using E = P::msg_id_type;

        /**
         * @if zh
         * @brief 获取所有诊断信息
         * @details 返回当前报告器收集的所有诊断信息列表。
         *
         * @return 诊断信息列表的常量引用
         *
         * @elseif en
         * @brief Get all diagnostics
         * @details Returns all diagnostic information collected by the current reporter.
         *
         * @return Const reference to diagnostic information list
         *
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const;

        /**
         * @if zh
         * @brief 合并另一个报告器的诊断到当前报告器
         * @details 用于多编译单元场景下，将各单元独立报告器的诊断汇总到主报告器。
         *          源报告器的诊断会被追加到当前报告器的诊断列表末尾。
         *
         * @param other 源报告器，其诊断会被追加到当前报告器
         *
         * @elseif en
         * @brief Merge diagnostics from another reporter into this one
         * @details Used in multi-unit scenarios to aggregate diagnostics from per-unit
         *          reporters into the main reporter. Diagnostics from the source reporter
         *          will be appended to the end of this reporter's diagnostic list.
         *
         * @param other Source reporter whose diagnostics will be appended to this reporter
         *
         * @endif
         */
        void mergeFrom(const DiagnosticReporter& other);

        /**
         * @if zh
         * @brief 从错误异常构造诊断
         * @details 将Error异常对象转换为诊断信息并添加到报告器。
         *          自动提取异常中的位置信息和消息内容。
         *
         * @tparam I 消息ID枚举值
         * @param error Error异常对象，包含位置和消息信息
         *
         * @elseif en
         * @brief Emplace diagnostic from error exception
         * @details Converts an Error exception object to diagnostic information and adds
         *          it to the reporter. Automatically extracts position information and
         *          message content from the exception.
         *
         * @tparam I Message ID enum value
         * @param error Error exception object containing position and message information
         *
         * @endif
         */
        template<E I>
        void emplace(const Error& error)
#ifdef _MSC_VER
        {
            diagnostics_.push_back(
                {DiagnosticLevel::Error, std::string(enumToStr(I)), *error.getStart(), *error.getEnd(),
                 error.message()}
            );
        }
#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告诊断消息（无参数）
         * @details 根据指定的消息ID和级别报告诊断消息，消息内容从本地化系统获取。
         *
         * @tparam I 消息ID枚举值
         * @param level 诊断级别
         * @param start 消息起始位置
         * @param end 消息结束位置
         *
         * @elseif en
         * @brief Report diagnostic message (no parameters)
         * @details Reports a diagnostic message based on the specified message ID and level,
         *          with message content retrieved from the localization system.
         *
         * @tparam I Message ID enum value
         * @param level Diagnostic level
         * @param start Message start position
         * @param end Message end position
         *
         * @endif
         */
        template<E I>
        void report(DiagnosticLevel level, Pos start, Pos end)
#ifdef _MSC_VER
        {
            diagnostics_.push_back(
                {level, std::string(enumToStr(I)), start, end, Local<P>::template msgStr<I>()}
            );
        }
#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告错误消息（无参数）
         * @details 便捷方法，以Error级别报告诊断消息。
         *
         * @tparam I 消息ID枚举值
         * @param start 消息起始位置
         * @param end 消息结束位置
         *
         * @elseif en
         * @brief Report error message (no parameters)
         * @details Convenience method to report diagnostic message at Error level.
         *
         * @tparam I Message ID enum value
         * @param start Message start position
         * @param end Message end position
         *
         * @endif
         */
        template<E I>
        void error(Pos start, Pos end)
#ifdef _MSC_VER
        {
            report<I>(DiagnosticLevel::Error, start, end);
        }
#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告警告消息（无参数）
         * @details 便捷方法，以Warning级别报告诊断消息。
         *
         * @tparam I 消息ID枚举值
         * @param start 消息起始位置
         * @param end 消息结束位置
         *
         * @elseif en
         * @brief Report warning message (no parameters)
         * @details Convenience method to report diagnostic message at Warning level.
         *
         * @tparam I Message ID enum value
         * @param start Message start position
         * @param end Message end position
         *
         * @endif
         */
        template<E I>
        void warn(Pos start, Pos end)
#ifdef _MSC_VER
        {
            report<I>(DiagnosticLevel::Warning, start, end);
        }
#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告信息消息（无参数）
         * @details 便捷方法，以Info级别报告诊断消息。
         *
         * @tparam I 消息ID枚举值
         * @param start 消息起始位置
         * @param end 消息结束位置
         *
         * @elseif en
         * @brief Report info message (no parameters)
         * @details Convenience method to report diagnostic message at Info level.
         *
         * @tparam I Message ID enum value
         * @param start Message start position
         * @param end Message end position
         *
         * @endif
         */
        template<E I>
        void info(Pos start, Pos end)
#ifdef _MSC_VER
        {
            report<I>(DiagnosticLevel::Info, start, end);
        }

#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告诊断消息（带参数）
         * @details 根据指定的消息ID和级别报告诊断消息，支持格式化参数。
         *          消息模板中的{}占位符会被参数值替换。
         *
         * @tparam I 消息ID枚举值
         * @tparam Args 参数类型列表
         * @param level 诊断级别
         * @param start 消息起始位置
         * @param end 消息结束位置
         * @param args 格式化参数，按顺序替换消息模板中的{}占位符
         *
         * @elseif en
         * @brief Report diagnostic message (with parameters)
         * @details Reports a diagnostic message based on the specified message ID and level,
         *          supporting format parameters. {} placeholders in the message template
         *          will be replaced with parameter values.
         *
         * @tparam I Message ID enum value
         * @tparam Args Parameter type list
         * @param level Diagnostic level
         * @param start Message start position
         * @param end Message end position
         * @param args Format parameters, replacing {} placeholders in message template in order
         *
         * @endif
         */
        template<E I, typename... Args>
        void reportWith(DiagnosticLevel level, Pos start, Pos end, Args&&... args)
#ifdef _MSC_VER
        {
            diagnostics_.push_back(
                {level, std::string(enumToStr(I)), start, end,
                 Local<P>::template msgFormat<I>(std::forward<Args>(args)...)}
            );
        }
#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告错误消息（带参数）
         * @details 便捷方法，以Error级别报告诊断消息，支持格式化参数。
         *
         * @tparam I 消息ID枚举值
         * @tparam Args 参数类型列表
         * @param start 消息起始位置
         * @param end 消息结束位置
         * @param args 格式化参数，按顺序替换消息模板中的{}占位符
         *
         * @elseif en
         * @brief Report error message (with parameters)
         * @details Convenience method to report diagnostic message at Error level,
         *          supporting format parameters.
         *
         * @tparam I Message ID enum value
         * @tparam Args Parameter type list
         * @param start Message start position
         * @param end Message end position
         * @param args Format parameters, replacing {} placeholders in message template in order
         *
         * @endif
         */
        template<E I, typename... Args>
        void errorWith(Pos start, Pos end, Args&&... args)
#ifdef _MSC_VER
        {
            reportWith<I, Args...>(DiagnosticLevel::Error, start, end, std::forward<Args>(args)...);
        }
#else
            ;
#endif

        /**
         * @if zh
         * @brief 报告警告消息（带参数）
         * @details 便捷方法，以Warning级别报告诊断消息，支持格式化参数。
         *
         * @tparam I 消息ID枚举值
         * @tparam Args 参数类型列表
         * @param start 消息起始位置
         * @param end 消息结束位置
         * @param args 格式化参数，按顺序替换消息模板中的{}占位符
         *
         * @elseif en
         * @brief Report warning message (with parameters)
         * @details Convenience method to report diagnostic message at Warning level,
         *          supporting format parameters.
         *
         * @tparam I Message ID enum value
         * @tparam Args Parameter type list
         * @param start Message start position
         * @param end Message end position
         * @param args Format parameters, replacing {} placeholders in message template in order
         *
         * @endif
         */
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

        /**
         * @if zh
         * @brief 报告信息消息（带参数）
         * @details 便捷方法，以Info级别报告诊断消息，支持格式化参数。
         *
         * @tparam I 消息ID枚举值
         * @tparam Args 参数类型列表
         * @param start 消息起始位置
         * @param end 消息结束位置
         * @param args 格式化参数，按顺序替换消息模板中的{}占位符
         *
         * @elseif en
         * @brief Report info message (with parameters)
         * @details Convenience method to report diagnostic message at Info level,
         *          supporting format parameters.
         *
         * @tparam I Message ID enum value
         * @tparam Args Parameter type list
         * @param start Message start position
         * @param end Message end position
         * @param args Format parameters, replacing {} placeholders in message template in order
         *
         * @endif
         */
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
        /** @if zh @brief 诊断信息列表，存储所有报告的诊断消息 @else @brief Diagnostic information list storing all reported diagnostic messages @endif */
        std::vector<Diagnostic> diagnostics_;
    };

}  // namespace stationeers::ic10

#include "diagnostic.inl"

#endif  // COMPILER_DIAGNOSTIC_HPP
