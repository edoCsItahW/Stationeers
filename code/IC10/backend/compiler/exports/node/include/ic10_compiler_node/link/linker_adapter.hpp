// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/20 22:18
 * @if zh
 * @brief IC10链接器Node.js适配器
 * @details 提供IC10 Linker类的Node-API绑定，将C++ Linker类暴露给JavaScript。
 *          支持添加编译单元、执行链接、获取诊断信息等操作。
 *
 * @note 使用N-API (Node-API)实现跨版本兼容的Node.js绑定。
 *
 * @par 使用方式:
 * @code
 * const { Linker } = require('ic10');
 *
 * const linker = new Linker();
 * // 添加编译单元
 * linker.addUnit(program);
 * linker.addUnit(sourceCode, 'file.ic');
 * // 执行链接
 * const symbolTable = linker.link();
 * // 获取诊断信息
 * const diagnostics = linker.diagnostics;
 * @endcode
 *
 * @see Linker C++链接器类
 * @see common_node/base.hpp Node.js绑定基类
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 linker Node.js adapter
 * @details Provides Node-API bindings for the IC10 Linker class, exposing the C++
 *          Linker class to JavaScript. Supports adding compilation units, performing
 *          linking, and retrieving diagnostic information.
 *
 * @note Uses N-API (Node-API) for cross-version compatible Node.js bindings.
 *
 * @par Usage:
 * @code
 * const { Linker } = require('ic10');
 *
 * const linker = new Linker();
 * // Add compilation units
 * linker.addUnit(program);
 * linker.addUnit(sourceCode, 'file.ic');
 * // Perform linking
 * const symbolTable = linker.link();
 * // Get diagnostics
 * const diagnostics = linker.diagnostics;
 * @endcode
 *
 * @see Linker C++ linker class
 * @see common_node/base.hpp Node.js binding base class
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */

#ifndef EXPORT_LINKER_ADAPTER_HPP
#define EXPORT_LINKER_ADAPTER_HPP
#pragma once

#include "ic10_compiler/link/linker.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class LinkerAdapter
     * @brief Linker类的Node.js适配器
     * @details 继承自node::ObjectWrap，将C++ Linker类封装为JavaScript对象。
     *          提供构造函数、addUnit方法、link方法和diagnostics/units只读属性。
     *
     * @elseif en
     * @class LinkerAdapter
     * @brief Node.js adapter for Linker class
     * @details Inherits from node::ObjectWrap, wrapping the C++ Linker class as a JavaScript object.
     *          Provides constructor, addUnit method, link method, and diagnostics/units readonly properties.
     *
     * @endif
     */
    class LinkerAdapter : public node::ObjectWrap<LinkerAdapter> {
    public:
        /**
         * @if zh
         * @brief 构造函数
         * @details 创建LinkerAdapter实例，内部初始化Linker对象。
         *
         * @param info Node.js回调信息对象
         *
         * @elseif en
         * @brief Constructor
         * @details Creates a LinkerAdapter instance, initializing the internal Linker object.
         *
         * @param info Node.js callback info object
         *
         * @endif
         */
        LinkerAdapter(const node::CallbackInfo& info);

        /**
         * @if zh
         * @brief 初始化Linker类的Node.js绑定
         * @details 在Node.js模块中注册Linker类，暴露构造函数、方法和属性。
         *
         * @param env Node.js环境对象
         * @param exports 模块导出对象
         * @return 更新后的导出对象
         *
         * @elseif en
         * @brief Initialize Node.js bindings for Linker class
         * @details Registers the Linker class in the Node.js module, exposing constructor,
         *          methods, and properties.
         *
         * @param env Node.js environment object
         * @param exports Module exports object
         * @return Updated exports object
         *
         * @endif
         */
        static node::Object init(node::Env env, node::Object exports);

    private:
        /** @if zh @brief 内部Linker实例 @else @brief Internal Linker instance @endif */
        Linker linker_;

        /**
         * @if zh
         * @brief 添加编译单元
         * @details JavaScript方法，支持两种调用方式：
         *          - addUnit(program) - 添加已编译的Program对象
         *          - addUnit(source, path?) - 添加源代码字符串，可选路径
         *
         * @param info Node.js回调信息对象
         * @return undefined
         *
         * @elseif en
         * @brief Add compilation unit
         * @details JavaScript method supporting two calling patterns:
         *          - addUnit(program) - Add compiled Program object
         *          - addUnit(source, path?) - Add source code string with optional path
         *
         * @param info Node.js callback info object
         * @return undefined
         *
         * @endif
         */
        node::Value addUnit(const node::CallbackInfo& info);

        /**
         * @if zh
         * @brief 执行链接操作
         * @details JavaScript方法，执行链接并返回合并后的符号表。
         *
         * @param info Node.js回调信息对象
         * @return SymbolTable对象
         *
         * @elseif en
         * @brief Perform linking
         * @details JavaScript method that performs linking and returns the merged symbol table.
         *
         * @param info Node.js callback info object
         * @return SymbolTable object
         *
         * @endif
         */
        node::Value link(const node::CallbackInfo& info);

        /**
         * @if zh
         * @brief 获取所有诊断信息
         * @details JavaScript getter，返回所有编译单元的诊断信息数组。
         *
         * @param info Node.js回调信息对象
         * @return 诊断信息数组
         *
         * @elseif en
         * @brief Get all diagnostics
         * @details JavaScript getter that returns an array of diagnostic information from all units.
         *
         * @param info Node.js callback info object
         * @return Diagnostics array
         *
         * @endif
         */
        node::Value getDiagnostics(const node::CallbackInfo& info);

        /**
         * @if zh
         * @brief 获取所有编译单元信息
         * @details JavaScript getter，返回所有编译单元的信息数组，包含路径和诊断信息。
         *
         * @param info Node.js回调信息对象
         * @return 编译单元信息数组
         *
         * @elseif en
         * @brief Get all compilation units
         * @details JavaScript getter that returns an array of unit information including paths and diagnostics.
         *
         * @param info Node.js callback info object
         * @return Unit information array
         *
         * @endif
         */
        node::Value getUnits(const node::CallbackInfo& info);

        /**
         * @if zh
         * @brief 获取全局类型表
         * @details JavaScript getter，返回链接过程中合并的全局类型表。
         *
         * @param info Node.js回调信息对象
         * @return TypeTable对象
         *
         * @elseif en
         * @brief Get global type table
         * @details JavaScript getter that returns the merged global type table from linking.
         *
         * @param info Node.js callback info object
         * @return TypeTable object
         *
         * @endif
         */
        node::Value getTypeTable(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_LINKER_ADAPTER_HPP
