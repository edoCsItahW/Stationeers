// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file base.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 17:22
 * @if zh
 * @brief Node.js导出基础工具
 * @details 提供Node.js N-API导出的基础工具类和函数,包括类型转换、参数处理、
 *        异步任务工作器等。用于C++类与Node.js对象之间的桥接。
 * @note 依赖napi.h头文件
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Node.js export base utilities
 * @details Provides base utility classes and functions for Node.js N-API exports,
 *        including type conversion, parameter handling, async task workers, etc.
 *        Used for bridging between C++ classes and Node.js objects.
 * @note Depends on napi.h header
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_BASE_HPP
#define EXPORT_BASE_HPP
#pragma once

#include "common/async/task.hpp"

#include <napi.h>


namespace node = Napi;

namespace st = stationeers;


namespace stationeers {

    /**
     * @if zh
     *
     * @brief BaseOfValue概念
     * @details 检查类型T是否继承自node::Value
     *
     * @tparam T 待检查的类型
     *
     * @elseif en
     *
     * @brief BaseOfValue concept
     * @details Checks if type T inherits from node::Value
     *
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    concept BaseOfValue = std::is_base_of_v<node::Value, T>;

    /**
     * @if zh
     *
     * @brief Serializable概念
     * @details 检查类型T是否有toJSON方法返回可转换为std::string的值
     *
     * @tparam T 待检查的类型
     *
     * @elseif en
     *
     * @brief Serializable concept
     * @details Checks if type T has a toJSON method returning a value convertible to std::string
     *
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    concept Serializable = requires(T node) {
        { node.toJSON() } -> std::convertible_to<std::string>;
    };

    /**
     * @if zh
     *
     * @brief 类型转换函数模板
     * @details 将C++值转换为Node.js值
     *
     * @tparam T 值类型
     * @param env Node.js环境
     * @param value C++值
     * @return Node.js值
     *
     * @elseif en
     *
     * @brief Type conversion function template
     * @details Converts C++ value to Node.js value
     *
     * @tparam T Value type
     * @param env Node.js environment
     * @param value C++ value
     * @return Node.js value
     *
     * @endif
     */
    template<typename T>
    node::Value to(node::Env env, const T& value);

    /**
     * @if zh
     *
     * @brief variant类型转换函数
     * @details 将std::variant转换为Node.js值
     *
     * @tparam ...Ts variant的类型参数包
     * @param env Node.js环境
     * @param var variant值
     * @return Node.js值
     *
     * @elseif en
     *
     * @brief Variant type conversion function
     * @details Converts std::variant to Node.js value
     *
     * @tparam ...Ts Type parameter pack of variant
     * @param env Node.js environment
     * @param var Variant value
     * @return Node.js value
     *
     * @endif
     */
    template<typename... Ts>
    node::Value to(node::Env env, const std::variant<Ts...>& var);

    /**
     * @if zh
     *
     * @brief vector类型转换函数
     * @details 将std::vector转换为Node.js数组
     *
     * @tparam T vector元素类型
     * @param env Node.js环境
     * @param vec vector值
     * @return Node.js数组值
     *
     * @elseif en
     *
     * @brief Vector type conversion function
     * @details Converts std::vector to Node.js array
     *
     * @tparam T Vector element type
     * @param env Node.js environment
     * @param vec Vector value
     * @return Node.js array value
     *
     * @endif
     */
    template<typename T>
    node::Value to(node::Env env, const std::vector<T>& vec);

    /**
     * @if zh
     *
     * @class Arguments
     * @brief Node.js回调参数包装类
     * @details 封装Node.js回调函数的参数,提供类型安全的参数访问
     *
     * @elseif en
     *
     * @class Arguments
     * @brief Node.js callback argument wrapper class
     * @details Encapsulates callback function arguments, providing type-safe parameter access
     *
     * @endif
     */
    class Arguments {
    public:
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
        Arguments(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 下标运算符
         * @param index 参数索引
         * @return 参数值
         *
         * @elseif en
         *
         * @brief Subscript operator
         * @param index Parameter index
         * @return Parameter value
         *
         * @endif
         */
        const node::Value operator[](std::size_t index) const;

        /**
         * @if zh
         *
         * @brief 带类型检查的参数获取
         * @tparam T 期望的类型
         * @param index 参数索引
         * @return 类型转换后的参数值
         *
         * @elseif en
         *
         * @brief Get parameter with type check
         * @tparam T Expected type
         * @param index Parameter index
         * @return Type-converted parameter value
         *
         * @endif
         */
        template<BaseOfValue T>
        const T getWithCheck(std::size_t index) const;

        /**
         * @if zh
         *
         * @brief 获取参数
         * @param index 参数索引
         * @return 参数值
         *
         * @elseif en
         *
         * @brief Get parameter
         * @param index Parameter index
         * @return Parameter value
         *
         * @endif
         */
        const node::Value get(std::size_t index) const;

    private:
        /**
         * @if zh
         * @brief Node.js回调信息引用
         * @elseif en
         * @brief Node.js callback info reference
         * @endif
         */
        const node::CallbackInfo& info_;
    };

    /**
     * @if zh
     *
     * @brief Transformer概念
     * @details 检查类型T是否满足转换器要求:具有from和to静态方法
     *
     * @tparam T 待检查的类型
     *
     * @elseif en
     *
     * @brief Transformer concept
     * @details Checks if type T satisfies transformer requirements: has from and to static methods
     *
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    concept Transformer = std::is_void_v<T> || requires {
        requires !std::is_void_v<decltype(T::from(std::declval<const node::Object&>()))>;

        {
            T::to(
                std::declval<node::Env>(),
                std::declval<const std::invoke_result_t<decltype(T::from), const node::Object&>&>()
            )
        } -> std::convertible_to<node::Object>;
    };

    /**
     * @if zh
     *
     * @class TaskWorker
     * @brief 异步任务工作器模板
     * @details 基于N-API AsyncWorker的异步任务执行器,用于在后台线程执行协程任务
     *
     * @tparam T 任务返回值类型
     *
     * @elseif en
     *
     * @class TaskWorker
     * @brief Async task worker template
     * @details Async task executor based on N-API AsyncWorker, for executing coroutine tasks
     *          in background threads
     *
     * @tparam T Task return value type
     *
     * @endif
     */
    template<Transformer T>
    class TaskWorker : public node::AsyncWorker {
    public:
        /**
         * @if zh
         * @brief 任务工厂函数类型别名
         * @elseif en
         * @brief Task factory function type alias
         * @endif
         */
        using TaskFactory = std::function<Task<T>()>;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param deferred Node.js Promise延迟对象
         * @param factory 任务工厂函数
         *
         * @elseif en
         *
         * @brief Constructor
         * @param deferred Node.js Promise deferred object
         * @param factory Task factory function
         *
         * @endif
         */
        TaskWorker(node::Promise::Deferred deferred, TaskFactory factory);

        /**
         * @if zh
         *
         * @brief 执行任务
         * @details 在后台线程执行任务
         *
         * @elseif en
         *
         * @brief Execute task
         * @details Executes task in background thread
         *
         * @endif
         */
        void Execute() override;

        /**
         * @if zh
         *
         * @brief 任务成功回调
         * @details 任务成功完成后在主线程调用
         *
         * @elseif en
         *
         * @brief Task success callback
         * @details Called on main thread after task completes successfully
         *
         * @endif
         */
        void OnOK() override;

        /**
         * @if zh
         *
         * @brief 任务错误回调
         * @details 任务出错时在主线程调用
         * @param e N-API错误对象
         *
         * @elseif en
         *
         * @brief Task error callback
         * @details Called on main thread when task errors
         * @param e N-API error object
         *
         * @endif
         */
        void OnError(const Napi::Error& e) override;

    private:
        /**
         * @if zh
         * @brief Node.js Promise延迟对象
         * @elseif en
         * @brief Node.js Promise deferred object
         * @endif
         */
        node::Promise::Deferred deferred_;

        /**
         * @if zh
         * @brief 任务工厂函数
         * @elseif en
         * @brief Task factory function
         * @endif
         */
        TaskFactory factory_;

        /**
         * @if zh
         * @brief 任务结果值
         * @elseif en
         * @brief Task result value
         * @endif
         */
        T result_;

        /**
         * @if zh
         * @brief 执行标志
         * @elseif en
         * @brief Execution flag
         * @endif
         */
        bool flag_;
    };

    /**
     * @if zh
     *
     * @class TaskWorker<void>
     * @brief 异步任务工作器(void特化)
     * @details 无返回值的异步任务执行器特化版本
     *
     * @elseif en
     *
     * @class TaskWorker<void>
     * @brief Async task worker (void specialization)
     * @details Specialized version of async task executor with no return value
     *
     * @endif
     */
    template<>
    class TaskWorker<void> : public node::AsyncWorker {
    public:
        /**
         * @if zh
         * @brief 任务工厂函数类型别名
         * @elseif en
         * @brief Task factory function type alias
         * @endif
         */
        using TaskFactory = std::function<Task<>()>;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param deferred Node.js Promise延迟对象
         * @param factory 任务工厂函数
         *
         * @elseif en
         *
         * @brief Constructor
         * @param deferred Node.js Promise deferred object
         * @param factory Task factory function
         *
         * @endif
         */
        TaskWorker(node::Promise::Deferred deferred, TaskFactory factory);

        void Execute() override;

        void OnOK() override;

        void OnError(const Napi::Error& e) override;

    private:
        node::Promise::Deferred deferred_;

        TaskFactory factory_;

        bool flag_;
    };

}  // namespace stationeers

/**
 * @def EXPORT_D_ATTR_GETTER(attr)
 * @if zh
 *
 * @brief 声明属性getter方法
 * @details 用于在适配器类中声明指定属性的getter方法
 * @param attr 属性名(首字母大写)
 *
 * @elseif en
 *
 * @brief Declare attribute getter method
 * @details Used to declare the getter method of the specified attribute in the adapter class
 * @param attr Attribute name (capitalized)
 *
 * @endif
 */
#define EXPORT_D_ATTR_GETTER(attr) node::Value get##attr(const node::CallbackInfo& info);

/**
 * @def EXPORT_D_ATTR_SETTER(attr)
 * @if zh
 *
 * @brief 声明属性setter方法
 * @details 用于在适配器类中声明指定属性的setter方法
 * @param attr 属性名(首字母大写)
 *
 * @elseif en
 *
 * @brief Declare attribute setter method
 * @details Used to declare the setter method of the specified attribute in the adapter class
 * @param attr Attribute name (capitalized)
 *
 * @endif
 */
#define EXPORT_D_ATTR_SETTER(attr)                                                                 \
    void set##attr(const node::CallbackInfo& info, const node::Value& value);

/**
 * @def EXPORT_D_ATTR(attr)
 * @if zh
 *
 * @brief 声明属性的getter和setter方法
 * @details 同时声明指定属性的getter和setter方法
 * @param attr 属性名(首字母大写)
 *
 * @elseif en
 *
 * @brief Declare attribute getter and setter methods
 * @details Declares both getter and setter methods for the specified attribute
 * @param attr Attribute name (capitalized)
 *
 * @endif
 */
#define EXPORT_D_ATTR(attr)                                                                        \
    EXPORT_D_ATTR_GETTER(attr)                                                                     \
    EXPORT_D_ATTR_SETTER(attr)


/**
 * @def EXPORT_D_METHOD(name, result)
 * @if zh
 *
 * @brief 声明导出方法
 * @details 用于在适配器类中声明指定返回值类型的方法
 * @param name 方法名
 * @param result 返回值类型
 *
 * @elseif en
 *
 * @brief Declare export method
 * @details Used to declare a method with specified return type in the adapter class
 * @param name Method name
 * @param result Return value type
 *
 * @endif
 */
#define EXPORT_D_METHOD(name, result) result name(const node::CallbackInfo& info);

/**
 * @def EXPORT_D_METHOD_VALUE(name)
 * @if zh
 *
 * @brief 声明返回Value的导出方法
 * @details 用于声明返回node::Value类型的导出方法
 * @param name 方法名
 *
 * @elseif en
 *
 * @brief Declare export method returning Value
 * @details Used to declare an export method returning node::Value type
 * @param name Method name
 *
 * @endif
 */
#define EXPORT_D_METHOD_VALUE(name)   EXPORT_D_METHOD(name, node::Value)

/**
 * @def EXPORT_D_METHOD_VOID(name)
 * @if zh
 *
 * @brief 声明无返回值的导出方法
 * @details 用于声明返回void类型的导出方法
 * @param name 方法名
 *
 * @elseif en
 *
 * @brief Declare export method with no return value
 * @details Used to declare an export method returning void type
 * @param name Method name
 *
 * @endif
 */
#define EXPORT_D_METHOD_VOID(name)    EXPORT_D_METHOD(name, void)

// #define EXPORT_ATTR(attr)

#include "base.inl"

#endif  // EXPORT_BASE_HPP
