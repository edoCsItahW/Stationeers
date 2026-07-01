// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_list.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:54
 * @if zh
 * @brief 类型列表元编程工具
 * @details 提供编译期类型列表操作工具,包括类型列表的构造、查询、合并、过滤、去重、平展等功能。
 *        主要用于模板元编程中处理类型集合。
 * @note 所有操作均在编译期完成,不产生运行时开销
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Type list metaprogramming utilities
 * @details Provides compile-time type list manipulation tools, including construction, querying,
 *        merging, filtering, deduplication, flattening, and other operations on type lists.
 *        Mainly used for handling type collections in template metaprogramming.
 * @note All operations are performed at compile-time, no runtime overhead
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_TYPE_LIST_HPP
#define COMPILER_TYPE_LIST_HPP
#pragma once

#include <variant>
#include <tuple>


namespace stationeers {

    /**
     * @if zh
     *
     * @brief 类型列表基本结构
     * @details 用于封装任意数量类型的编译期容器
     * @tparam ... 类型模板参数
     *
     * @elseif en
     *
     * @brief Basic type list structure
     * @details Compile-time container for encapsulating any number of types
     * @tparam ... Type template parameters
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename...>
    struct type_list {};

    /**
     * @if zh
     *
     * @brief 类型列表检测特化
     * @details 当T为type_list类型时,is_type_list<T>特化派生自std::true_type
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Type list detection specialization
     * @details When T is type_list type, is_type_list<T> specialization derives from std::true_type
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename>
    struct is_type_list : std::false_type {};

    template<typename... Ts>
    struct is_type_list<type_list<Ts...>> : std::true_type {};

    /**
     * @if zh
     *
     * @brief 类型列表检测变量模板
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Type list detection variable template
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    inline constexpr bool is_type_list_v = is_type_list<T>::value;

    /**
     * @if zh
     *
     * @brief 类型列表约束概念
     * @details 用于约束模板参数必须为type_list类型
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Type list constraint concept
     * @details Used to constrain template parameters to be type_list types
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    concept IsTypeList = is_type_list_v<T>;

    /**
     * @if zh
     *
     * @brief 类型列表大小计算
     * @details 编译期计算type_list中类型的数量
     * @tparam T 特化用的type_list类型
     *
     * @elseif en
     *
     * @brief Type list size calculation
     * @details Compile-time calculation of the number of types in type_list
     * @tparam T Type to specialize with type_list
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename>
    struct type_list_size;

    template<typename... Ts>
    struct type_list_size<type_list<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

    /**
     * @if zh
     *
     * @brief 类型列表大小变量模板
     * @tparam T type_list类型
     *
     * @elseif en
     *
     * @brief Type list size variable template
     * @tparam T type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    inline constexpr std::size_t type_list_size_v = type_list_size<T>::value;

    /**
     * @if zh
     *
     * @brief 类型列表包含检测
     * @details 编译期检测type_list是否包含指定类型T
     * @tparam List type_list类型
     * @tparam T 要查找的类型
     *
     * @elseif en
     *
     * @brief Type list contains detection
     * @details Compile-time check if type_list contains type T
     * @tparam List type_list type
     * @tparam T Type to find
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename T>
    struct type_list_contains;

    template<typename... Ts, typename T>
    struct type_list_contains<type_list<Ts...>, T> : std::disjunction<std::is_same<T, Ts>...> {};

    /**
     * @if zh
     *
     * @brief 类型列表包含检测变量模板
     * @tparam List type_list类型
     * @tparam T 要查找的类型
     *
     * @elseif en
     *
     * @brief Type list contains detection variable template
     * @tparam List type_list type
     * @tparam T Type to find
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename T>
    inline constexpr bool type_list_contains_v = type_list_contains<List, T>::value;

    /**
     * @if zh
     *
     * @brief 类型列表包含任意检测
     * @details 编译期检测type_list是否包含任意一个指定的类型
     * @tparam List type_list类型
     * @tparam ... Ts 要查找的类型包
     *
     * @elseif en
     *
     * @brief Type list contains any detection
     * @details Compile-time check if type_list contains any of the specified types
     * @tparam List type_list type
     * @tparam ... Ts Type pack to find
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename... Ts>
    struct type_list_contains_any;

    template<typename... Ts, typename... Us>
    struct type_list_contains_any<type_list<Ts...>, Us...>
        : std::disjunction<type_list_contains<type_list<Ts...>, Us>...> {};

    /**
     * @if zh
     *
     * @brief 类型列表包含任意检测变量模板
     * @tparam List type_list类型
     * @tparam ... Ts 要查找的类型包
     *
     * @elseif en
     *
     * @brief Type list contains any detection variable template
     * @tparam List type_list type
     * @tparam ... Ts Type pack to find
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename... Ts>
    inline constexpr bool type_list_contains_any_v = type_list_contains_any<List, Ts...>::value;

    /**
     * @if zh
     *
     * @brief 类型列表前置操作
     * @details 将指定类型前置到type_list头部
     * @tparam List 原始type_list类型
     * @tparam ... Ts 要前置的类型包
     *
     * @elseif en
     *
     * @brief Type list push front operation
     * @details Prepends specified types to the front of type_list
     * @tparam List Original type_list type
     * @tparam ... Ts Type pack to prepend
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename... Ts>
    struct type_list_push_front;

    template<typename... Ts, typename... Us>
    struct type_list_push_front<type_list<Ts...>, Us...> {
        using type = type_list<Us..., Ts...>;
    };

    /**
     * @if zh
     *
     * @brief 类型列表前置操作类型别名
     * @tparam List 原始type_list类型
     * @tparam ... Ts 要前置的类型包
     *
     * @elseif en
     *
     * @brief Type list push front operation type alias
     * @tparam List Original type_list type
     * @tparam ... Ts Type pack to prepend
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename... Ts>
    using type_list_push_front_t = type_list_push_front<List, Ts...>::type;

    /**
     * @if zh
     *
     * @brief 类型列表后置操作
     * @details 将指定类型追加到type_list尾部
     * @tparam List 原始type_list类型
     * @tparam ... Ts 要追加的类型包
     *
     * @elseif en
     *
     * @brief Type list push back operation
     * @details Appends specified types to the end of type_list
     * @tparam List Original type_list type
     * @tparam ... Ts Type pack to append
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename... Ts>
    struct type_list_push_back;

    template<typename... Ts, typename... Us>
    struct type_list_push_back<type_list<Ts...>, Us...> {
        using type = type_list<Ts..., Us...>;
    };

    /**
     * @if zh
     *
     * @brief 类型列表后置操作类型别名
     * @tparam List 原始type_list类型
     * @tparam ... Ts 要追加的类型包
     *
     * @elseif en
     *
     * @brief Type list push back operation type alias
     * @tparam List Original type_list type
     * @tparam ... Ts Type pack to append
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename... Ts>
    using type_list_push_back_t = type_list_push_back<List, Ts...>::type;

    /**
     * @if zh
     *
     * @brief 类型列表连接操作
     * @details 将多个type_list连接成一个
     * @tparam ... List 要连接的类型列表包
     *
     * @elseif en
     *
     * @brief Type list concatenation operation
     * @details Concatenates multiple type_lists into one
     * @tparam ... List Type list pack to concatenate
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList...>
    struct type_list_concat;

    template<>
    struct type_list_concat<> {
        using type = type_list<>;
    };

    template<typename... Ts>
    struct type_list_concat<type_list<Ts...>> {
        using type = type_list<Ts...>;
    };

    template<typename... Ts, typename... Us, typename... Rest>
    struct type_list_concat<type_list<Ts...>, type_list<Us...>, Rest...> {
        using type = type_list_concat<type_list<Ts..., Us...>, Rest...>::type;
    };

    /**
     * @if zh
     *
     * @brief 类型列表连接操作类型别名
     * @tparam ... List 要连接的类型列表包
     *
     * @elseif en
     *
     * @brief Type list concatenation operation type alias
     * @tparam ... List Type list pack to concatenate
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList... List>
    using type_list_concat_t = type_list_concat<List...>::type;

    namespace detail {
        /**
         * @if zh
         *
         * @brief 类型列表排除实现(内部辅助)
         * @details 递归实现从type_list中排除指定类型的逻辑
         * @tparam List 当前的type_list类型
         * @tparam T 要排除的类型
         *
         * @elseif en
         *
         * @brief Type list exclude implementation (internal helper)
         * @details Recursively implements logic to exclude a specified type from type_list
         * @tparam List Current type_list type
         * @tparam T Type to exclude
         *
         * @endif
         *
         * @private @memberof stationeers::detail
         */
        template<typename, typename>
        struct type_list_exclude_impl;

        template<typename T>
        struct type_list_exclude_impl<type_list<>, T> {
            using type = type_list<>;
        };

        template<typename Head, typename... Tail, typename T>
        struct type_list_exclude_impl<type_list<Head, Tail...>, T> {
            using tail_res = type_list_exclude_impl<type_list<Tail...>, T>::type;
            using type     = std::conditional_t<
                std::is_same_v<Head, T>, tail_res, type_list_push_front_t<tail_res, Head>>;
        };

    }  // namespace detail

    /**
     * @if zh
     *
     * @brief 类型列表排除操作
     * @details 从type_list中移除所有T类型的实例
     * @tparam List 原始type_list类型
     * @tparam T 要排除的类型
     *
     * @elseif en
     *
     * @brief Type list exclude operation
     * @details Removes all instances of type T from type_list
     * @tparam List Original type_list type
     * @tparam T Type to exclude
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename T>
    struct type_list_exclude : detail::type_list_exclude_impl<List, T> {};

    /**
     * @if zh
     *
     * @brief 类型列表排除操作类型别名
     * @tparam List 原始type_list类型
     * @tparam T 要排除的类型
     *
     * @elseif en
     *
     * @brief Type list exclude operation type alias
     * @tparam List Original type_list type
     * @tparam T Type to exclude
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List, typename T>
    using type_list_exclude_t = type_list_exclude<List, T>::type;

    /**
     * @if zh
     *
     * @brief 类型列表去重操作
     * @details 移除type_list中的重复类型,保持首次出现的顺序
     * @tparam List 原始type_list类型
     *
     * @elseif en
     *
     * @brief Type list deduplication operation
     * @details Removes duplicate types from type_list, preserving the order of first occurrence
     * @tparam List Original type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    struct type_list_unique;

    template<>
    struct type_list_unique<type_list<>> {
        using type = type_list<>;
    };

    template<typename Head, typename... Tail>
    struct type_list_unique<type_list<Head, Tail...>> {
        using tail_unique = type_list_unique<type_list<Tail...>>::type;

        using type = std::conditional_t<
            type_list_contains_v<tail_unique, Head>, tail_unique,
            type_list_push_front_t<tail_unique, Head>>;
    };

    /**
     * @if zh
     *
     * @brief 类型列表去重操作类型别名
     * @tparam List 原始type_list类型
     *
     * @elseif en
     *
     * @brief Type list deduplication operation type alias
     * @tparam List Original type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    using type_list_unique_t = type_list_unique<List>::type;

    /**
     * @if zh
     *
     * @brief 从模板实例提取类型参数
     * @details 将模板类的类型参数提取为type_list
     * @tparam T 模板实例类型
     * @tparam List 目标列表模板,默认为type_list
     *
     * @elseif en
     *
     * @brief Extract type arguments from template instantiation
     * @details Extracts type parameters of a template class into a type_list
     * @tparam T Template instance type
     * @tparam List Target list template, defaults to type_list
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename, template<typename...> class = type_list>
    struct extract_arguments;

    template<template<typename...> class T1, template<typename...> class T2, typename... Ts>
    struct extract_arguments<T1<Ts...>, T2> {
        using type = T2<Ts...>;
    };

    /**
     * @if zh
     *
     * @brief 从模板实例提取类型参数字别名
     * @tparam T 模板实例类型
     * @tparam List 目标列表模板,默认为type_list
     *
     * @elseif en
     *
     * @brief Extract type arguments type alias
     * @tparam T Template instance type
     * @tparam List Target list template, defaults to type_list
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T, template<typename...> class List = type_list>
    using extract_arguments_t = extract_arguments<T, List>::type;

    /**
     * @if zh
     *
     * @brief 可平展类型检测
     * @details 检测类型是否可以展平为其内部元素的type_list
     * @tparam T 待检测的类型
     *
     * @note 可平展的类型包括:type_list、std::tuple、std::variant
     *
     * @elseif en
     *
     * @brief Flatttenable type detection
     * @details Checks if a type can be flattened into a type_list of its inner elements
     * @tparam T Type to check
     *
     * @note Flattenable types include: type_list, std::tuple, std::variant
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename>
    struct is_flattenable : std::false_type {};

    template<typename... Ts>
    struct is_flattenable<type_list<Ts...>> : std::true_type {};

    template<typename... Ts>
    struct is_flattenable<std::tuple<Ts...>> : std::true_type {};

    template<typename... Ts>
    struct is_flattenable<std::variant<Ts...>> : std::true_type {};

    /**
     * @if zh
     *
     * @brief 可平展类型检测变量模板
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Flatttenable type detection variable template
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    inline constexpr bool is_flattenable_v = is_flattenable<T>::value;

    /**
     * @if zh
     *
     * @brief 可平展类型约束概念
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Flatttenable type constraint concept
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    concept IsFlattenable = is_flattenable_v<T>;

    /**
     * @if zh
     *
     * @brief 类型列表嵌套检测
     * @details 检测type_list中是否包含任何可平展的类型
     * @tparam List type_list类型
     *
     * @elseif en
     *
     * @brief Type list nested detection
     * @details Checks if type_list contains any flattenable types
     * @tparam List type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    struct type_list_is_nested;

    template<typename... Ts>
    struct type_list_is_nested<type_list<Ts...>> : std::disjunction<is_flattenable<Ts>...> {};

    /**
     * @if zh
     *
     * @brief 类型列表嵌套检测变量模板
     * @tparam List type_list类型
     *
     * @elseif en
     *
     * @brief Type list nested detection variable template
     * @tparam List type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    inline constexpr bool type_list_is_nested_v = type_list_is_nested<List>::value;

    /**
     * @if zh
     *
     * @brief 单类型平展操作
     * @details 将单一类型平展为type_list,如果该类型本身可平展则递归展开
     * @tparam T 要平展的类型
     *
     * @elseif en
     *
     * @brief Single type flatten operation
     * @details Flattens a single type into type_list, recursively expanding if the type is flattenable
     * @tparam T Type to flatten
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    struct type_list_flatten_single {
        using type = type_list<T>;
    };

    template<template<typename...> class C, typename... Ts>
        requires(is_flattenable_v<C<Ts...>>)
    struct type_list_flatten_single<C<Ts...>> {
        using type = type_list_concat_t<typename type_list_flatten_single<Ts>::type...>;
    };

    /**
     * @if zh
     *
     * @brief 类型列表平展操作
     * @details 将嵌套的type_list递归平展为一层type_list
     * @tparam List 原始type_list类型
     *
     * @elseif en
     *
     * @brief Type list flatten operation
     * @details Recursively flattens a nested type_list into a single-level type_list
     * @tparam List Original type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    struct type_list_flatten;

    template<typename... Ts>
    struct type_list_flatten<type_list<Ts...>> {
        using type = type_list_concat_t<typename type_list_flatten_single<Ts>::type...>;
    };

    /**
     * @if zh
     *
     * @brief 类型列表平展操作类型别名
     * @tparam List 原始type_list类型
     *
     * @elseif en
     *
     * @brief Type list flatten operation type alias
     * @tparam List Original type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    using type_list_flatten_t = type_list_flatten<List>::type;

    /**
     * @if zh
     *
     * @brief 类型列表平展并去重操作类型别名
     * @details 先平展再去重,得到不含重复类型的扁平type_list
     * @tparam List 原始type_list类型
     *
     * @elseif en
     *
     * @brief Type list flatten with unique operation type alias
     * @details First flattens then deduplicates, resulting in a flat type_list without duplicate types
     * @tparam List Original type_list type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsTypeList List>
    using type_list_flatten_with_unique_t = type_list_unique_t<type_list_flatten_t<List>>;

    /**
     * @if zh
     *
     * @brief 类型列表操作便捷命名空间
     * @details 提供类型列表操作的便捷访问方式,包含所有核心操作的变量模板和类型别名
     *
     * @elseif en
     *
     * @brief Type list operations convenience namespace
     * @details Provides convenient access to type list operations, containing variable templates
     *        and type aliases for all core operations
     *
     * @endif
     */
    namespace typelist {

        /**
         * @if zh
         *
         * @brief 类型列表检测变量
         * @tparam T 待检测的类型
         *
         * @elseif en
         *
         * @brief Type list detection variable
         * @tparam T Type to check
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<typename T>
        inline constexpr bool is_v = is_type_list_v<T>;

        /**
         * @if zh
         *
         * @brief 类型列表大小变量
         * @tparam T type_list类型
         *
         * @elseif en
         *
         * @brief Type list size variable
         * @tparam T type_list type
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<typename T>
        inline constexpr std::size_t size_v = is_type_list_v<T>;

        /**
         * @if zh
         *
         * @brief 类型列表包含检测变量
         * @tparam List type_list类型
         * @tparam T 要查找的类型
         *
         * @elseif en
         *
         * @brief Type list contains detection variable
         * @tparam List type_list type
         * @tparam T Type to find
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List, typename T>
        inline constexpr bool contains_v = type_list_contains_v<List, T>;

        /**
         * @if zh
         *
         * @brief 类型列表包含任意检测变量
         * @tparam List type_list类型
         * @tparam ... Ts 要查找的类型包
         *
         * @elseif en
         *
         * @brief Type list contains any detection variable
         * @tparam List type_list type
         * @tparam ... Ts Type pack to find
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List, typename... Ts>
        inline constexpr bool contains_any_v = type_list_contains_any_v<List, Ts...>;

        /**
         * @if zh
         *
         * @brief 类型列表嵌套检测变量
         * @tparam List type_list类型
         *
         * @elseif en
         *
         * @brief Type list nested detection variable
         * @tparam List type_list type
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List>
        inline constexpr bool is_nested_v = type_list_is_nested_v<List>;

        /**
         * @if zh
         *
         * @brief 类型列表前置操作类型别名
         * @tparam List 原始type_list类型
         * @tparam ... Ts 要前置的类型包
         *
         * @elseif en
         *
         * @brief Type list push front operation type alias
         * @tparam List Original type_list type
         * @tparam ... Ts Type pack to prepend
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List, typename... Ts>
        using push_front_t = type_list_push_front_t<List, Ts...>;

        /**
         * @if zh
         *
         * @brief 类型列表后置操作类型别名
         * @tparam List 原始type_list类型
         * @tparam ... Ts 要追加的类型包
         *
         * @elseif en
         *
         * @brief Type list push back operation type alias
         * @tparam List Original type_list type
         * @tparam ... Ts Type pack to append
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List, typename... Ts>
        using push_back_t = type_list_push_back_t<List, Ts...>;

        /**
         * @if zh
         *
         * @brief 类型列表连接操作类型别名
         * @tparam ... Lists 要连接的类型列表包
         *
         * @elseif en
         *
         * @brief Type list concatenation operation type alias
         * @tparam ... Lists Type list pack to concatenate
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList... Lists>
        using concat_t = type_list_concat_t<Lists...>;

        /**
         * @if zh
         *
         * @brief 类型列表排除操作类型别名
         * @tparam List 原始type_list类型
         * @tparam T 要排除的类型
         *
         * @elseif en
         *
         * @brief Type list exclude operation type alias
         * @tparam List Original type_list type
         * @tparam T Type to exclude
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List, typename T>
        using exclude_t = type_list_exclude_t<List, T>;

        /**
         * @if zh
         *
         * @brief 类型列表去重操作类型别名
         * @tparam List 原始type_list类型
         *
         * @elseif en
         *
         * @brief Type list deduplication operation type alias
         * @tparam List Original type_list type
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List>
        using unique_t = type_list_unique_t<List>;

        /**
         * @if zh
         *
         * @brief 单类型平展操作类型别名
         * @tparam T 要平展的类型
         *
         * @elseif en
         *
         * @brief Single type flatten operation type alias
         * @tparam T Type to flatten
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<typename T>
        using flatten_single_t = type_list_flatten_single<T>::type;

        /**
         * @if zh
         *
         * @brief 类型列表平展操作类型别名
         * @tparam List 原始type_list类型
         *
         * @elseif en
         *
         * @brief Type list flatten operation type alias
         * @tparam List Original type_list type
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List>
        using flatten_t = type_list_flatten_t<List>;

        /**
         * @if zh
         *
         * @brief 类型列表平展并去重操作类型别名
         * @tparam List 原始type_list类型
         *
         * @elseif en
         *
         * @brief Type list flatten with unique operation type alias
         * @tparam List Original type_list type
         *
         * @endif
         *
         * @public @memberof stationeers::typelist
         */
        template<IsTypeList List>
        using flatten_with_unique_t = type_list_flatten_with_unique_t<List>;

    }  // namespace typelist

    /**
     * @if zh
     *
     * @brief 容器类型辅助工具
     * @details 从模板容器类型提取类型列表并提供便捷操作接口
     * @tparam T 模板容器类型(如std::tuple<int, double>)
     *
     * @elseif en
     *
     * @brief Container type helper utility
     * @details Extracts type list from template container type and provides convenient operation interface
     * @tparam T Template container type (e.g., std::tuple<int, double>)
     *
     * @endif
     */
    template<typename>
    struct container_helper;

    template<template<typename...> class C, typename... Ts>
    struct container_helper<C<Ts...>> {
    private:
        using list = type_list<Ts...>;

    public:
        /**
         * @if zh
         *
         * @brief 原始容器类型
         *
         * @elseif en
         *
         * @brief Original container type
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        using type = C<Ts...>;

        /**
         * @if zh
         *
         * @brief 嵌套检测变量
         *
         * @elseif en
         *
         * @brief Nested detection variable
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        static constexpr bool is_nested_v = typelist::is_nested_v<list>;

        /**
         * @if zh
         *
         * @brief 大小变量
         *
         * @elseif en
         *
         * @brief Size variable
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        static constexpr std::size_t size_v = typelist::size_v<list>;

        /**
         * @if zh
         *
         * @brief 包含检测变量
         * @tparam ... Us 要查找的类型包
         *
         * @elseif en
         *
         * @brief Contains detection variable
         * @tparam ... Us Type pack to find
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        template<typename... Us>
        static constexpr bool contains_v = typelist::contains_any_v<list, Us...>;

        /**
         * @if zh
         *
         * @brief 连接操作类型别名
         * @tparam ... Us 要追加的类型包
         *
         * @elseif en
         *
         * @brief Concatenation operation type alias
         * @tparam ... Us Type pack to append
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        template<typename... Us>
        using concat_t = extract_arguments_t<typelist::concat_t<list, type_list<Us...>>, C>;

        /**
         * @if zh
         *
         * @brief 平展操作类型别名
         *
         * @elseif en
         *
         * @brief Flatten operation type alias
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        using flatten_t = extract_arguments_t<typelist::flatten_t<list>, C>;

        /**
         * @if zh
         *
         * @brief 平展并去重操作类型别名
         *
         * @elseif en
         *
         * @brief Flatten with unique operation type alias
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        using flatten_with_unique_t = extract_arguments_t<typelist::flatten_with_unique_t<list>, C>;

        /**
         * @if zh
         *
         * @brief 排除操作类型别名
         * @tparam T 要排除的类型
         *
         * @elseif en
         *
         * @brief Exclude operation type alias
         * @tparam T Type to exclude
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        template<typename T>
        using exclude_t = extract_arguments_t<typelist::exclude_t<list, T>, C>;

        /**
         * @if zh
         *
         * @brief 去重操作类型别名
         *
         * @elseif en
         *
         * @brief Deduplication operation type alias
         *
         * @endif
         *
         * @public @memberof container_helper
         */
        using unique_t = extract_arguments_t<typelist::unique_t<list>, C>;
    };

}  // namespace stationeers

#endif  // COMPILER_TYPE_LIST_HPP
