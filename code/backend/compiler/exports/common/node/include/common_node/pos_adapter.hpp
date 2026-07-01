// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file pos_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/17 13:26
 * @if zh
 * @brief Pos类型Node.js适配器
 * @details 提供C++ Pos位置类型与Node.js对象之间的双向转换适配器。
 *        封装位置信息(行号、列号、偏移量)的JS层表示和操作。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Pos type Node.js adapter
 * @details Provides bidirectional conversion adapter between C++ Pos position type and Node.js object.
 *        Encapsulates JS layer representation and operations of position information
 *        (line number, column number, offset).
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_POS_HPP
#define EXPORT_POS_HPP
#pragma once

#include "base.hpp"
#include "common/utils/position.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @class PosAdapter
     * @brief 位置适配器类
     * @details 提供C++ Pos类型与Node.js对象之间的双向转换。
     *          支持位置信息的属性访问和方法调用。
     *
     * @elseif en
     *
     * @class PosAdapter
     * @brief Position adapter class
     * @details Provides bidirectional conversion between C++ Pos type and Node.js object.
     *          Supports property access and method calls for position information.
     *
     * @endif
     */
    class PosAdapter : public node::ObjectWrap<PosAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 初始化位置适配器
         * @param env Node.js环境
         * @param exports 导出对象
         * @return 配置后的导出对象
         *
         * @elseif en
         *
         * @brief Initialize position adapter
         * @param env Node.js environment
         * @param exports Exports object
         * @return Configured exports object
         *
         * @endif
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将C++ Pos转换为Node.js对象
         * @param env Node.js环境
         * @param self C++ Pos对象
         * @return Node.js位置对象
         *
         * @elseif en
         *
         * @brief Convert C++ Pos to Node.js object
         * @param env Node.js environment
         * @param self C++ Pos object
         * @return Node.js position object
         *
         * @endif
         */
        static node::Object to(node::Env env, const Pos& self);

        /**
         * @if zh
         *
         * @brief 将Node.js对象转换为C++ Pos
         * @param obj Node.js对象
         * @return C++ Pos对象
         *
         * @elseif en
         *
         * @brief Convert Node.js object to C++ Pos
         * @param obj Node.js object
         * @return C++ Pos object
         *
         * @endif
         */
        static Pos from(const node::Object& obj);

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param info Node.js回调信息
         *
         * @elseif en
         *
         * @brief Constructor
         * @param info Node.js callback info
         *
         * @endif
         */
        PosAdapter(const node::CallbackInfo& info);

    private:
        /**
         * @if zh
         * @brief 内部C++ Pos对象
         * @elseif en
         * @brief Internal C++ Pos object
         * @endif
         */
        Pos pos_;

        /**
         * @if zh
         * @brief 构造函数引用
         * @elseif en
         * @brief Constructor reference
         * @endif
         */
        static node::FunctionReference constructor;

        EXPORT_D_ATTR_GETTER(Line)

        EXPORT_D_ATTR_GETTER(Column)

        EXPORT_D_ATTR_GETTER(Offset)

        EXPORT_D_METHOD_VOID(newline)

        EXPORT_D_METHOD_VOID(next)

        EXPORT_D_METHOD_VOID(move)
    };

}  // namespace stationeers

#endif  // EXPORT_POS_HPP
