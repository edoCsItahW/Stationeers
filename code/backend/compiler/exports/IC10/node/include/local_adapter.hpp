// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file local_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 17:12
 * @if zh
 * @brief IC10本地化Node.js适配器
 * @details 提供IC10编译器本地化(L10n)的Node.js绑定。
 *        支持设置编译器的语言环境和初始化本地资源。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 localization Node.js adapter
 * @details Provides Node.js bindings for IC10 compiler localization (L10n).
 *        Supports setting compiler language environment and initializing local resources.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_LOCALS_HPP
#define EXPORT_LOCALS_HPP
#pragma once

#include "common_node/base.hpp"

namespace stationeers::ic10 {

    /**
     * @class IC10LocalAdapter
     * @if zh
     *
     * @brief IC10本地化Node.js适配器类
     * @details 提供IC10编译器本地化功能的Node.js绑定封装，
     *          支持JavaScript层设置编译器语言环境和管理本地化资源。
     *
     *
     * @elseif en
     *
     * @brief IC10 localization Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 compiler localization functionality,
     *          supporting JavaScript layer to set compiler language environment and manage localization resources.
     *
     *
     * @endif
     */
    class IC10LocalAdapter : public node::ObjectWrap<IC10LocalAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建IC10LocalAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates an IC10LocalAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof IC10LocalAdapter
         */
        IC10LocalAdapter(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 初始化适配器并导出到Node.js模块
         * @details 注册类到Node.js环境并将其添加到exports对象
         *
         * @param env Node.js环境对象
         * @param exports Node.js模块导出对象
         * @return 注册后的类构造函数对象
         *
         *
         * @elseif en
         *
         * @brief Initialize the adapter and export to Node.js module
         * @details Registers the class to the Node.js environment and adds it to the exports object
         *
         * @param env Node.js environment object
         * @param exports Node.js module exports object
         * @return Registered class constructor object
         *
         *
         * @endif
         *
         * @public @memberof IC10LocalAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

    private:
        /**
         * @if zh
         *
         * @brief 设置语言
         * @details 静态方法，设置编译器的当前语言环境
         *
         * @param info Node.js回调信息对象，包含语言代码参数
         *
         *
         * @elseif en
         *
         * @brief Set language
         * @details Static method, sets the current language environment of the compiler
         *
         * @param info Node.js callback information object, containing language code parameter
         *
         *
         * @endif
         *
         * @private @memberof IC10LocalAdapter
         */
        static void setLanguage(const node::CallbackInfo& info);
    };

    /**
     * @if zh
     *
     * @brief 设置语言的全局函数
     * @details 独立的全局函数，用于设置IC10编译器的语言环境
     *
     * @param info Node.js回调信息对象，包含语言代码参数
     * @return Node.js值对象
     *
     *
     * @elseif en
     *
     * @brief Global function for setting language
     * @details Independent global function used to set the language environment of the IC10 compiler
     *
     * @param info Node.js callback information object, containing language code parameter
     * @return Node.js value object
     *
     *
     * @endif
     */
    node::Value setLanguage(const node::CallbackInfo& info);

    /**
     * @if zh
     *
     * @brief 初始化本地资源
     * @details 将本地化相关功能初始化并添加到Node.js模块导出对象中
     *
     * @param env Node.js环境对象引用
     * @param exports Node.js模块导出对象引用
     *
     *
     * @elseif en
     *
     * @brief Initialize local resources
     * @details Initializes localization-related functionality and adds it to the Node.js module exports object
     *
     * @param env Node.js environment object reference
     * @param exports Node.js module exports object reference
     *
     *
     * @endif
     */
    void initLocals(node::Env& env, node::Object& exports);

}  // namespace stationeers::ic10

#endif  // EXPORT_LOCALS_HPP
