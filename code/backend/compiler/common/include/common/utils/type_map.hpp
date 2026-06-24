// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file type_map.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:55
 * @if zh
 * @brief 编译期类型映射表实现
 * @details 提供编译期键值类型映射表,支持通过枚举键在O(1)时间内查找对应的类型。
 *        主要用于编译器中操作码到具体指令类型的映射等场景。
 * @note 所有操作均在编译期完成,不产生运行时开销
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Compile-time type map implementation
 * @details Provides compile-time key-value type mapping table, supporting O(1) lookup of corresponding
 *        types through enum keys. Mainly used in compilers for mapping opcodes to specific instruction types.
 * @note All operations are performed at compile-time, no runtime overhead
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_TYPE_MAP_HPP
#define COMPILER_TYPE_MAP_HPP
#pragma once

#include <array>
#include <tuple>
#include <variant>

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 编译期类型查找辅助结构
     * @details 通过枚举键查找对应类型的辅助结构,需要配合TypeMap使用
     * @tparam K 枚举键值
     *
     * @elseif en
     *
     * @brief Compile-time type lookup helper structure
     * @details Helper structure for looking up corresponding types through enum keys,
     *        needs to be used with TypeMap
     * @tparam K Enum key value
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<auto K>
    struct TypeFor;

    /**
     * @if zh
     *
     * @class TypeMap
     * @brief 编译期类型映射表
     * @details 通过枚举键索引到对应类型的映射表。提供编译期类型查找和运行时Variant构造功能。
     *
     * @tparam T 枚举类型
     * @tparam ... Ks 枚举键值包
     *
     * @par 使用示例:
     * @code
     * enum class Op { Add, Sub, Mul };
     *
     * template<>
     * struct TypeFor<Op::Add> { using type = AddInstruction; };
     * template<>
     * struct TypeFor<Op::Sub> { using type = SubInstruction; };
     *
     * using MyMap = TypeMap<Op, Op::Add, Op::Sub, Op::Mul>;
     * static_assert(MyMap::contains(Op::Add)); // true
     * @endcode
     *
     * @elseif en
     *
     * @class TypeMap
     * @brief Compile-time type mapping table
     * @details Maps enum keys to corresponding types. Provides compile-time type lookup
     *        and runtime Variant construction functionality.
     *
     * @tparam T Enum type
     * @tparam ... Ks Enum key value pack
     *
     * @par Usage example:
     * @code
     * enum class Op { Add, Sub, Mul };
     *
     * template<>
     * struct TypeFor<Op::Add> { using type = AddInstruction; };
     * template<>
     * struct TypeFor<Op::Sub> { using type = SubInstruction; };
     *
     * using MyMap = TypeMap<Op, Op::Add, Op::Sub, Op::Mul>;
     * static_assert(MyMap::contains(Op::Add)); // true
     * @endcode
     *
     * @endif
     */
    template<typename T, T... Ks>
    struct TypeMap {
    private:
        template<T K>
        using MappedType = TypeFor<K>::type;

        using Variant = std::variant<MappedType<Ks>...>;

        using TypeList = std::tuple<MappedType<Ks>...>;

        static constexpr std::array<T, sizeof...(Ks)> keys{Ks...};

        static constexpr std::size_t operator[](T key) noexcept;

// reslove MSVC C3577 C3528 C2977 error
#ifdef _MSC_VER
        template<typename, typename, typename...>
        struct MakerArray;

        template<std::size_t I, typename... Args>
        static Variant construct(Args&&... args) noexcept {
            return Variant{std::in_place_index<I>, std::forward<Args>(args)...};
        }

        template<std::size_t... Is, typename... Args>
        struct MakerArray<std::index_sequence<Is...>, Variant, Args...> {
            static constexpr std::array<Variant (*)(Args&&...), sizeof...(Is)> value = {
                &TypeMap::construct<Is, Args...>...
            };
        };
#endif

    public:
        /**
         * @if zh
         *
         * @brief 构造Variant实例
         * @details 根据给定的枚举键和参数构造对应的Variant实例
         * @tparam ... Args 构造参数类型包
         * @param e 枚举键
         * @param ... args 构造参数
         * @return 构造的Variant实例
         *
         * @elseif en
         *
         * @brief Construct a Variant instance
         * @details Constructs the corresponding Variant instance based on the given enum key and arguments
         * @tparam ... Args Constructor argument type pack
         * @param e Enum key
         * @param ... args Constructor arguments
         * @return Constructed Variant instance
         *
         * @endif
         *
         * @public @memberof TypeMap
         */
        template<typename... Args>
        static Variant make(T e, Args&&... args) noexcept;

        /**
         * @if zh
         *
         * @brief 检查枚举键是否存在
         * @param key 要检查的枚举键
         * @return 如果键存在返回true,否则返回false
         *
         * @elseif en
         *
         * @brief Check if enum key exists
         * @param key Enum key to check
         * @return true if key exists, false otherwise
         *
         * @endif
         *
         * @public @memberof TypeMap
         */
        static bool contains(T key) noexcept;

        /**
         * @if zh
         *
         * @brief 检查映射表中是否包含指定类型
         * @tparam U 要检查的类型
         * @return 如果任意键映射到U则返回true
         *
         * @elseif en
         *
         * @brief Check if the mapping table contains the specified type
         * @tparam U Type to check
         * @return true if any key maps to U
         *
         * @endif
         *
         * @public @memberof TypeMap
         */
        template<typename U>
        static constexpr bool contains_v = (... || std::is_same_v<U, MappedType<Ks>>);
    };

}  // namespace stationeers

#include "type_map.inl"

#endif  // COMPILER_TYPE_MAP_HPP
