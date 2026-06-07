// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file type_list.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:54
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_TYPE_LIST_HPP
#define COMPILER_TYPE_LIST_HPP
#pragma once

#include <variant>
#include <tuple>


namespace stationeers {

    template<typename...>
    struct type_list {};

    template<typename>
    struct is_type_list : std::false_type {};

    template<typename... Ts>
    struct is_type_list<type_list<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_type_list_v = is_type_list<T>::value;

    template<typename T>
    concept IsTypeList = is_type_list_v<T>;

    template<typename>
    struct type_list_size;

    template<typename... Ts>
    struct type_list_size<type_list<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

    template<typename T>
    inline constexpr std::size_t type_list_size_v = type_list_size<T>::value;

    template<IsTypeList List, typename T>
    struct type_list_contains;

    template<typename... Ts, typename T>
    struct type_list_contains<type_list<Ts...>, T> : std::disjunction<std::is_same<T, Ts>...> {};

    template<IsTypeList List, typename T>
    inline constexpr bool type_list_contains_v = type_list_contains<List, T>::value;

    template<IsTypeList List, typename... Ts>
    struct type_list_contains_any;

    template<typename... Ts, typename... Us>
    struct type_list_contains_any<type_list<Ts...>, Us...>
        : std::disjunction<type_list_contains<type_list<Ts...>, Us>...> {};

    template<IsTypeList List, typename... Ts>
    inline constexpr bool type_list_contains_any_v = type_list_contains_any<List, Ts...>::value;

    template<IsTypeList List, typename... Ts>
    struct type_list_push_front;

    template<typename... Ts, typename... Us>
    struct type_list_push_front<type_list<Ts...>, Us...> {
        using type = type_list<Us..., Ts...>;
    };

    template<IsTypeList List, typename... Ts>
    using type_list_push_front_t = type_list_push_front<List, Ts...>::type;

    template<IsTypeList List, typename... Ts>
    struct type_list_push_back;

    template<typename... Ts, typename... Us>
    struct type_list_push_back<type_list<Ts...>, Us...> {
        using type = type_list<Ts..., Us...>;
    };

    template<IsTypeList List, typename... Ts>
    using type_list_push_back_t = type_list_push_back<List, Ts...>::type;

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

    template<IsTypeList... List>
    using type_list_concat_t = type_list_concat<List...>::type;

    namespace detail {
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

    template<IsTypeList List, typename T>
    struct type_list_exclude : detail::type_list_exclude_impl<List, T> {};

    template<IsTypeList List, typename T>
    using type_list_exclude_t = type_list_exclude<List, T>::type;

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

    template<IsTypeList List>
    using type_list_unique_t = type_list_unique<List>::type;

    template<typename, template<typename...> class = type_list>
    struct extract_arguments;

    template<template<typename...> class T1, template<typename...> class T2, typename... Ts>
    struct extract_arguments<T1<Ts...>, T2> {
        using type = T2<Ts...>;
    };

    template<typename T, template<typename...> class List = type_list>
    using extract_arguments_t = extract_arguments<T, List>::type;

    template<typename>
    struct is_flattenable : std::false_type {};

    template<typename... Ts>
    struct is_flattenable<type_list<Ts...>> : std::true_type {};

    template<typename... Ts>
    struct is_flattenable<std::tuple<Ts...>> : std::true_type {};

    template<typename... Ts>
    struct is_flattenable<std::variant<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_flattenable_v = is_flattenable<T>::value;

    template<typename T>
    concept IsFlattenable = is_flattenable_v<T>;

    template<IsTypeList List>
    struct type_list_is_nested;

    template<typename... Ts>
    struct type_list_is_nested<type_list<Ts...>> : std::disjunction<is_flattenable<Ts>...> {};

    template<IsTypeList List>
    inline constexpr bool type_list_is_nested_v = type_list_is_nested<List>::value;

    template<typename T>
    struct type_list_flatten_single {
        using type = type_list<T>;
    };

    template<template<typename...> class C, typename... Ts>
        requires(is_flattenable_v<C<Ts...>>)
    struct type_list_flatten_single<C<Ts...>> {
        using type = type_list_concat_t<typename type_list_flatten_single<Ts>::type...>;
    };

    template<IsTypeList List>
    struct type_list_flatten;

    template<typename... Ts>
    struct type_list_flatten<type_list<Ts...>> {
        using type = type_list_concat_t<typename type_list_flatten_single<Ts>::type...>;
    };

    template<IsTypeList List>
    using type_list_flatten_t = type_list_flatten<List>::type;

    template<IsTypeList List>
    using type_list_flatten_with_unique_t = type_list_unique_t<type_list_flatten_t<List>>;

    namespace typelist {

        template<typename T>
        inline constexpr bool is_v = is_type_list_v<T>;

        template<typename T>
        inline constexpr std::size_t size_v = is_type_list_v<T>;

        template<IsTypeList List, typename T>
        inline constexpr bool contains_v = type_list_contains_v<List, T>;

        template<IsTypeList List, typename... Ts>
        inline constexpr bool contains_any_v = type_list_contains_any_v<List, Ts...>;

        template<IsTypeList List>
        inline constexpr bool is_nested_v = type_list_is_nested_v<List>;

        template<IsTypeList List, typename... Ts>
        using push_front_t = type_list_push_front_t<List, Ts...>;

        template<IsTypeList List, typename... Ts>
        using push_back_t = type_list_push_back_t<List, Ts...>;

        template<IsTypeList... Lists>
        using concat_t = type_list_concat_t<Lists...>;

        template<IsTypeList List, typename T>
        using exclude_t = type_list_exclude_t<List, T>;

        template<IsTypeList List>
        using unique_t = type_list_unique_t<List>;

        template<typename T>
        using flatten_single_t = type_list_flatten_single<T>::type;

        template<IsTypeList List>
        using flatten_t = type_list_flatten_t<List>;

        template<IsTypeList List>
        using flatten_with_unique_t = type_list_flatten_with_unique_t<List>;

    }  // namespace typelist

    template<typename>
    struct container_helper;

    template<template<typename...> class C, typename... Ts>
    struct container_helper<C<Ts...>> {
    private:
        using list = type_list<Ts...>;

    public:
        using type = C<Ts...>;

        static constexpr bool is_nested_v = typelist::is_nested_v<list>;

        static constexpr std::size_t size_v = typelist::size_v<list>;

        template<typename... Us>
        static constexpr bool contains_v = typelist::contains_any_v<list, Us...>;

        template<typename... Us>
        using concat_t = extract_arguments_t<typelist::concat_t<list, type_list<Us...>>, C>;

        using flatten_t = extract_arguments_t<typelist::flatten_t<list>, C>;

        using flatten_with_unique_t = extract_arguments_t<typelist::flatten_with_unique_t<list>, C>;

        template<typename T>
        using exclude_t = extract_arguments_t<typelist::exclude_t<list, T>, C>;

        using unique_t = extract_arguments_t<typelist::unique_t<list>, C>;
    };

}  // namespace stationeers

#endif  // COMPILER_TYPE_LIST_HPP
