// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_symbol_table.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10符号表Java绑定
 * @details 提供IC10符号表(SymbolTable)的JNI绑定，支持Java层访问符号表的JSON序列化。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 symbol table Java binding
 * @details Provides JNI bindings for IC10 symbol table, supporting Java layer access to JSON serialization.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_SYMBOL_TABLE_HPP
#define COMPILER_J_SYMBOL_TABLE_HPP
#pragma once

#include "common_java/base.hpp"
#include "ic10/semantic/semantic.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JSymbolTable
     * @brief IC10符号表Java适配器类
     * @details 提供IC10符号表(SymbolTable)的Java绑定封装，
     *          支持C++ SymbolTable对象与Java对象之间的相互转换，
     *          以及符号表的JSON序列化功能。
     * @elseif en
     * @class JSymbolTable
     * @brief IC10 symbol table Java adapter class
     * @details Provides Java binding encapsulation for IC10 symbol table,
     *          supporting bidirectional conversion between C++ SymbolTable objects and Java objects,
     *          as well as JSON serialization functionality for symbol tables.
     * @endif
     */
    class JSymbolTable : public java::HybridClass<JSymbolTable> {
    public:
        static constexpr auto kJavaDescriptor = "Lcom/stationeers/ic10/SymbolTable;";

        SymbolTable symbolTable_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        std::string toJSON();

        static auto to(const SymbolTable& self);

        static SymbolTable from(java::alias_ref<javaobject> symbolTable);

        static SymbolTable from(java::alias_ref<java::JObject> obj);

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#include "j_symbol_table.inl"

#endif  // COMPILER_J_SYMBOL_TABLE_HPP
