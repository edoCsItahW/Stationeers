// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_type_table.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10类型表Java绑定
 * @details 提供IC10类型表(TypeTable)的JNI绑定，支持Java层访问类型表的JSON序列化。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 type table Java binding
 * @details Provides JNI bindings for IC10 type table, supporting Java layer access to JSON serialization.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_TYPE_TABLE_HPP
#define COMPILER_J_TYPE_TABLE_HPP
#pragma once

#include "common_java/base.hpp"
#include "ic10_compiler/semantic/types.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JTypeTable
     * @brief IC10类型表Java适配器类
     * @details 提供IC10类型表(TypeTable)的Java绑定封装，
     *          支持C++ TypeTable对象与Java对象之间的相互转换，
     *          以及类型表的JSON序列化功能。
     * @elseif en
     * @class JTypeTable
     * @brief IC10 type table Java adapter class
     * @details Provides Java binding encapsulation for IC10 type table,
     *          supporting bidirectional conversion between C++ TypeTable objects and Java objects,
     *          as well as JSON serialization functionality for type tables.
     * @endif
     */
    class JTypeTable : public java::HybridClass<JTypeTable> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/TypeTable;";

        TypeTable typeTable_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        std::string toJSON();

        static auto to(const TypeTable& self);

        static TypeTable from(java::alias_ref<javaobject> typeTable);

        static TypeTable from(java::alias_ref<java::JObject> obj);

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#include "j_type_table.inl"

#endif  // COMPILER_J_TYPE_TABLE_HPP
