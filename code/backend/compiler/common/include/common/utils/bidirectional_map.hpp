// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file bidirectional_map.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:09
 * @if zh
 * @brief 双向映射容器实现
 * @details 提供双向映射数据结构,支持通过Key查找Value和通过Value查找Key的双向查找功能。
 *        内部使用两个std::unordered_map分别存储正向和反向映射,实现O(1)时间复杂度的查找。
 * @note 该容器不是线程安全的,如需在多线程环境下使用,请自行添加同步机制
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Bidirectional map container implementation
 * @details Provides a bidirectional mapping data structure that supports looking up values by keys
 *        and keys by values with O(1) time complexity. Internally uses two std::unordered_map
 *        instances to store forward and reverse mappings.
 * @note This container is not thread-safe. If you need to use it in a multi-threaded environment,
 *       please add your own synchronization mechanisms
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_BIDIRECTIONAL_MAP_HPP
#define COMPILER_BIDIRECTIONAL_MAP_HPP
#pragma once

#include <unordered_map>
#include <stdexcept>


namespace stationeers {

    /**
     * @if zh
     *
     * @brief BiMap键类型约束概念
     * @details 约束类型T必须与键类型K相同
     * @tparam T 待检测类型
     * @tparam K 键类型
     *
     * @elseif en
     *
     * @brief BiMap key type constraint concept
     * @details Constrains type T to be the same as key type K
     * @tparam T Type to check
     * @tparam K Key type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T, typename K>
    concept IsBiMapKey = std::is_same_v<T, K>;

    /**
     * @if zh
     *
     * @brief BiMap值类型约束概念
     * @details 约束类型T必须与值类型V相同
     * @tparam T 待检测类型
     * @tparam V 值类型
     *
     * @elseif en
     *
     * @brief BiMap value type constraint concept
     * @details Constrains type T to be the same as value type V
     * @tparam T Type to check
     * @tparam V Value type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T, typename V>
    concept IsBiMapValue = std::is_same_v<T, V>;

    /**
     * @if zh
     *
     * @brief BiMap键或值类型约束概念
     * @details 约束类型T必须是键类型K或值类型V之一
     * @tparam T 待检测类型
     * @tparam K 键类型
     * @tparam V 值类型
     *
     * @elseif en
     *
     * @brief BiMap key or value type constraint concept
     * @details Constrains type T to be either key type K or value type V
     * @tparam T Type to check
     * @tparam K Key type
     * @tparam V Value type
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T, typename K, typename V>
    concept IsBiMapKeyOrValue = IsBiMapKey<T, K> || IsBiMapValue<T, V>;

    /**
     * @if zh
     *
     * @brief 标记结构:表示查询结果到达末尾
     * @details 用于BiMapFindResult与末尾标记的比较操作
     *
     * @elseif en
     *
     * @brief Tag struct: indicates the query result has reached the end
     * @details Used for comparison operations between BiMapFindResult and end marker
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    struct BiMapEndTag {};

    // BiMapFindResult

    /**
     * @if zh
     *
     * @class BiMapFindResult
     * @brief 双向映射查询结果包装类
     * @details 封装双向映射的查询结果,提供安全的迭代器访问和布尔值转换。
     *        可表示查询成功(找到元素)或失败(未找到)两种状态。
     *
     * @elseif en
     *
     * @class BiMapFindResult
     * @brief Bidirectional map query result wrapper class
     * @details Wraps the query result of a bidirectional map, providing safe iterator access and
     *        boolean conversion. Can represent two states: success (element found) or failure (not found).
     *
     * @endif
     */
    template<typename Iterator>
    class BiMapFindResult {
    public:
        /**
         * @if zh
         *
         * @brief 默认构造函数
         * @details 构造一个表示"未找到"的空结果
         *
         * @elseif en
         *
         * @brief Default constructor
         * @details Constructs an empty result representing "not found"
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        BiMapFindResult();

        /**
         * @if zh
         *
         * @brief 带迭代器的构造函数
         * @param iter 有效的迭代器
         *
         * @elseif en
         *
         * @brief Constructor with iterator
         * @param iter A valid iterator
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        BiMapFindResult(Iterator iter);

        /**
         * @if zh
         *
         * @brief 布尔值转换运算符
         * @return 如果找到元素返回true,否则返回false
         *
         * @elseif en
         *
         * @brief Boolean conversion operator
         * @return true if element found, false otherwise
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        explicit operator bool() const;

        /**
         * @if zh
         *
         * @brief 解引用运算符
         * @return 迭代器指向的元素的引用
         * @warning 调用前请确保结果有效(通过bool转换或find()返回值检查)
         *
         * @elseif en
         *
         * @brief Dereference operator
         * @return Reference to the element pointed to by the iterator
         * @warning Ensure the result is valid before calling (check via bool conversion or find() return value)
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        auto &operator*() const;

        /**
         * @if zh
         *
         * @brief 箭头运算符
         * @return 迭代器指向的元素的指针
         *
         * @elseif en
         *
         * @brief Arrow operator
         * @return Pointer to the element pointed to by the iterator
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        auto operator->() const;

        /**
         * @if zh
         *
         * @brief 相等比较运算符
         * @param other 另一个BiMapFindResult
         * @return 如果两者状态相同返回true
         *
         * @elseif en
         *
         * @brief Equality comparison operator
         * @param other Another BiMapFindResult
         * @return true if both have the same state
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        bool operator==(const BiMapFindResult &other) const;

        /**
         * @if zh
         *
         * @brief 与末尾标记的相等比较
         * @param 末端标记
         * @return 如果结果无效(未找到)返回true
         *
         * @elseif en
         *
         * @brief Equality comparison with end tag
         * @param End tag marker
         * @return true if result is invalid (not found)
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        bool operator==(const BiMapEndTag &) const;

        /**
         * @if zh
         *
         * @brief 显式转换为迭代器类型
         * @return 存储的迭代器副本
         * @note 即使结果无效也会返回迭代器,调用者需要自行检查有效性
         *
         * @elseif en
         *
         * @brief Explicit conversion to iterator type
         * @return Copy of stored iterator
         * @note Even if the result is invalid, the iterator will be returned. Callers need to check validity themselves.
         *
         * @endif
         *
         * @public @memberof BiMapFindResult
         */
        explicit operator Iterator() const;

    private:
        /**
         * @if zh
         *
         * @brief 存储的迭代器
         *
         * @elseif en
         *
         * @brief Stored iterator
         *
         * @endif
         *
         * @private @memberof BiMapFindResult
         */
        Iterator iter_;

        /**
         * @if zh
         *
         * @brief 末尾标记标志
         * @details true表示查询未找到有效结果
         *
         * @elseif en
         *
         * @brief End flag
         * @details true indicates the query did not find a valid result
         *
         * @endif
         *
         * @private @memberof BiMapFindResult
         */
        bool endFlag_;
    };

    // BiMap

    /**
     * @if zh
     *
     * @class BiMap
     * @brief 双向映射容器
     * @details 支持Key-Value双向查找的关联容器。内部维护两个unordered_map,
     *        分别用于正向(Key→Value)和反向(Value→Key)查找,实现O(1)时间复杂度。
     *
     * @note 要求Key和Value类型都是可哈希的(满足std::hashable)
     * @note Key和Value必须互不相同,以确保双向映射的唯一性
     *
     * @par 示例:
     * @code
     * BiMap<int, std::string> map;
     * map.insert(1, "one");
     * map.insert(2, "two");
     *
     * // 通过Key查找Value
     * auto& value = map.at(1); // "one"
     *
     * // 通过Value查找Key
     * auto& key = map.at("two"); // 2
     * @endcode
     *
     * @elseif en
     *
     * @class BiMap
     * @brief Bidirectional map container
     * @details An associative container supporting bidirectional Key-Value lookup. Internally maintains
     *        two unordered_map instances for forward (Key→Value) and reverse (Value→Key) lookups,
     *        achieving O(1) time complexity.
     *
     * @note Requires Key and Value types to be hashable (satisfy std::hashable)
     * @note Key and Value must be distinct to ensure uniqueness of bidirectional mapping
     *
     * @par Example:
     * @code
     * BiMap<int, std::string> map;
     * map.insert(1, "one");
     * map.insert(2, "two");
     *
     * // Lookup Value by Key
     * auto& value = map.at(1); // "one"
     *
     * // Lookup Key by Value
     * auto& key = map.at("two"); // 2
     * @endcode
     *
     * @endif
     */
    template<typename K, typename V>
    class BiMap {
    public:
        /**
         * @if zh
         *
         * @brief 内容类型别名
         * @details 表示容器中存储的键值对类型
         *
         * @elseif en
         *
         * @brief Content type alias
         * @details Represents the key-value pair type stored in the container
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        using ContentType = std::pair<const K, V>;

        /**
         * @if zh
         *
         * @brief 默认构造函数
         * @details 构造空的双向映射容器
         *
         * @elseif en
         *
         * @brief Default constructor
         * @details Constructs an empty bidirectional map container
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        BiMap() = default;

        /**
         * @if zh
         *
         * @brief 初始化列表构造函数
         * @param list 键值对的初始化列表
         *
         * @elseif en
         *
         * @brief Initializer list constructor
         * @param list Initializer list of key-value pairs
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        BiMap(std::initializer_list<ContentType> list);

        /**
         * @if zh
         *
         * @brief 插入键值对
         * @param key 键
         * @param value 值
         * @throws std::invalid_argument 如果key或value已存在
         *
         * @elseif en
         *
         * @brief Insert a key-value pair
         * @param key The key
         * @param value The value
         * @throws std::invalid_argument if key or value already exists
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        void insert(const K &key, const V &value);

        /**
         * @if zh
         *
         * @brief 通过键或值访问元素
         * @tparam T 必须是键类型K或值类型V
         * @param keyOrValue 要查找的键或值
         * @return 对应的值或键的引用
         * @throws std::out_of_range 如果keyOrValue不存在
         *
         * @elseif en
         *
         * @brief Access element by key or value
         * @tparam T Must be key type K or value type V
         * @param keyOrValue Key or value to look up
         * @return Reference to corresponding value or key
         * @throws std::out_of_range if keyOrValue does not exist
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        template<IsBiMapKeyOrValue<K, V> T, typename Self>
        auto &at(this Self &self, const T &keyOrValue);

        /**
         * @if zh
         *
         * @brief 检查键或值是否存在
         * @tparam T 必须是键类型K或值类型V
         * @param keyOrValue 要检查的键或值
         * @return 存在返回true,否则返回false
         *
         * @elseif en
         *
         * @brief Check if key or value exists
         * @tparam T Must be key type K or value type V
         * @param keyOrValue Key or value to check
         * @return true if exists, false otherwise
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        template<IsBiMapKeyOrValue<K, V> T>
        bool contains(const T &keyOrValue) const;

        /**
         * @if zh
         *
         * @brief 通过键或值访问元素(下标运算符)
         * @tparam T 必须是键类型K或值类型V
         * @param keyOrValue 要查找的键或值
         * @return 对应的值或键的引用
         * @warning 如果不存在会自动插入默认值
         *
         * @elseif en
         *
         * @brief Access element by key or value (subscript operator)
         * @tparam T Must be key type K or value type V
         * @param keyOrValue Key or value to look up
         * @return Reference to corresponding value or key
         * @warning Will automatically insert default value if not exists
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        template<IsBiMapKeyOrValue<K, V> T, typename Self>
        auto &operator[](this Self &self, const T &keyOrValue);

        /**
         * @if zh
         *
         * @brief 查找键或值
         * @tparam T 必须是键类型K或值类型V
         * @param keyOrValue 要查找的键或值
         * @return BiMapFindResult结果包装对象
         *
         * @elseif en
         *
         * @brief Find key or value
         * @tparam T Must be key type K or value type V
         * @param keyOrValue Key or value to find
         * @return BiMapFindResult wrapper object
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        template<IsBiMapKeyOrValue<K, V> T, typename Self>
        auto find(this Self &self, const T &keyOrValue);

        /**
         * @if zh
         *
         * @brief 获取末尾迭代器
         * @return 表示末尾的静态BiMapEndTag对象
         *
         * @elseif en
         *
         * @brief Get end iterator
         * @return Static BiMapEndTag object representing end
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        static auto end();

        /**
         * @if zh
         *
         * @brief 获取容器中元素数量
         * @return 键值对的数量
         *
         * @elseif en
         *
         * @brief Get number of elements in container
         * @return Number of key-value pairs
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        [[nodiscard]] std::size_t size() const;

        /**
         * @if zh
         *
         * @brief 清空容器
         * @details 删除所有键值对
         *
         * @elseif en
         *
         * @brief Clear the container
         * @details Removes all key-value pairs
         *
         * @endif
         *
         * @public @memberof BiMap
         */
        void clear();

    private:
        /**
         * @if zh
         *
         * @brief 正向映射表
         * @details 存储键到值的映射,用于Key→Value查找
         *
         * @elseif en
         *
         * @brief Forward mapping table
         * @details Stores key-to-value mappings for Key→Value lookup
         *
         * @endif
         *
         * @private @memberof BiMap
         */
        std::unordered_map<K, V> forwardMap_;

        /**
         * @if zh
         *
         * @brief 反向映射表
         * @details 存储值到键的映射,用于Value→Key查找
         *
         * @elseif en
         *
         * @brief Backward mapping table
         * @details Stores value-to-key mappings for Value→Key lookup
         *
         * @endif
         *
         * @private @memberof BiMap
         */
        std::unordered_map<V, K> backwardMap_;
    };

}  // namespace stationeers

#include "bidirectional_map.inl"

#endif  // COMPILER_BIDIRECTIONAL_MAP_HPP
