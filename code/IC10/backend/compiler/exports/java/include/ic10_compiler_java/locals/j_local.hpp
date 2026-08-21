// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_local.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10本地化Java绑定
 * @details 提供IC10编译器本地化(IC10Local)的JNI绑定，支持Java层设置编译器语言环境。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 localization Java binding
 * @details Provides JNI bindings for IC10 compiler localization, supporting Java layer to set compiler language.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_LOCAL_HPP
#define COMPILER_J_LOCAL_HPP
#pragma once

#include "ic10_compiler/pch/pch.hpp"
#include "common_java/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JIC10CompilerLocal
     * @brief IC10本地化Java适配器类
     * @details 提供IC10编译器本地化功能的Java绑定，支持Java层设置编译器语言环境。
     *          语言注册(中文简体、美式英语)在registerNatives中完成。
     * @elseif en
     * @class JIC10CompilerLocal
     * @brief IC10 localization Java adapter class
     * @details Provides Java bindings for IC10 compiler localization, supporting Java layer to set compiler language.
     *          Language registration (Simplified Chinese, American English) is done in registerNatives.
     * @endif
     */
    class JIC10CompilerLocal : public java::JavaClass<JIC10CompilerLocal> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/IC10CompilerLocal;";

        /**
         * @if zh
         * @brief 设置当前语言
         * @param clazz Java类引用(由fbjni自动传入)
         * @param name 语言名称标识(如"zh-hans"、"en-us")
         * @elseif en
         * @brief Set current language
         * @param clazz Java class reference (auto-passed by fbjni)
         * @param name Language name identifier (e.g. "zh-hans", "en-us")
         * @endif
         */
        static void setLanguage(java::alias_ref<java::JClass>, std::string name);

        /**
         * @if zh
         * @brief 注册本地化语言和native方法
         * @details 注册中文简体和美式英语语言包，设置默认语言为美式英语，
         *          并注册Java native方法。
         * @elseif en
         * @brief Register localization languages and native methods
         * @details Registers Simplified Chinese and American English language packs,
         *          sets default language to American English, and registers Java native methods.
         * @endif
         */
        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_LOCAL_HPP
