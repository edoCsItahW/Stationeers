// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file task.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:42
 * @if zh
 * @brief 协程Task封装
 * @details 提供协程的Task类型,用于协程的创建和管理。
 *        Task是协程的返回类型,内部关联Promise和Future,支持协程挂起和恢复。
 * @note 该文件包含Task相关类型声明,实现在task.inl中
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Coroutine Task encapsulation
 * @details Provides Task type for coroutines, used for coroutine creation and management.
 *        Task is the return type of coroutines, internally associated with Promise and Future,
 *        supporting coroutine suspension and resumption.
 * @note This file contains Task related type declarations, implementation is in task.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_TASK_HPP
#define COMPILER_TASK_HPP
#pragma once

#include "promise.hpp"

namespace stationeers {

    // 前向声明

    /**
     * @if zh
     *
     * @brief Task类型前向声明
     * @tparam T 协程返回值类型
     *
     * @elseif en
     *
     * @brief Task type forward declaration
     * @tparam T Coroutine return value type
     *
     * @endif
     */
    template<typename T>
    struct Task;

    /**
     * @if zh
     *
     * @class TaskPromiseBase
     * @brief Task Promise基类
     * @details 提供协程Promise的通用功能,包括初始挂起、最终挂起和异常处理
     *
     * @tparam T 协程返回值类型
     *
     * @elseif en
     *
     * @class TaskPromiseBase
     * @brief Task Promise base class
     * @details Provides common functionality for coroutine Promise, including initial suspension,
     *        final suspension and exception handling
     *
     * @tparam T Coroutine return value type
     *
     * @endif
     */
    template<typename T>
    struct TaskPromiseBase {
        /**
         * @if zh
         * @brief Promise对象
         * @elseif en
         * @brief Promise object
         * @endif
         */
        Promise<T> promise;

        /**
         * @if zh
         * @brief 协程状态弱引用
         * @details 由协程返回对象写入，供 Future::Awaiter::await_suspend 取得等待者状态。
         *          类型无关，因此 Task<void> 协程同样可被注册为等待者。
         *
         * @elseif en
         * @brief Coroutine state weak reference
         * @details Written by the coroutine return object and read by
         *          Future::Awaiter::await_suspend to obtain the waiter state. It is type-erased,
         *          so Task<void> coroutines can be registered as waiters as well.
         *
         * @endif
         */
        std::weak_ptr<CoroutineState> coro_state_weak_;

        /**
         * @if zh
         *
         * @brief 初始挂起
         * @return 初始挂起的awaiter
         *
         * @elseif en
         *
         * @brief Initial suspension
         * @return Awaiter for initial suspension
         *
         * @endif
         */
        auto initial_suspend() noexcept;

        /**
         * @if zh
         *
         * @brief 最终挂起
         * @return 最终挂起的awaiter
         *
         * @elseif en
         *
         * @brief Final suspension
         * @return Awaiter for final suspension
         *
         * @endif
         */
        auto final_suspend() noexcept;

        /**
         * @if zh
         * @brief 未处理异常
         * @elseif en
         * @brief Unhandled exception
         * @endif
         */
        void unhandled_exception() noexcept;
    };

    /**
     * @if zh
     *
     * @class TaskPromise
     * @brief Task Promise类(非void特化)
     * @details 处理非void返回值的协程Promise
     *
     * @tparam T 协程返回值类型
     *
     * @elseif en
     *
     * @class TaskPromise
     * @brief Task Promise class (non-void specialization)
     * @details Handles coroutine Promise with non-void return value
     *
     * @tparam T Coroutine return value type
     *
     * @endif
     */
    template<typename T>
    struct TaskPromise : TaskPromiseBase<T> {
        /**
         * @if zh
         *
         * @brief 获取返回对象
         * @return Task对象
         *
         * @elseif en
         *
         * @brief Get return object
         * @return Task object
         *
         * @endif
         */
        Task<T> get_return_object();

        /**
         * @if zh
         *
         * @brief 设置返回值
         * @param value 协程返回值
         *
         * @elseif en
         *
         * @brief Set return value
         * @param value Coroutine return value
         *
         * @endif
         */
        void return_value(T value);
    };

    /**
     * @if zh
     *
     * @class TaskBase
     * @brief Task基类
     * @details 提供Task的通用功能,包括协程句柄管理和协程等待
     *
     * @tparam T 协程返回值类型,默认为void
     *
     * @elseif en
     *
     * @class TaskBase
     * @brief Task base class
     * @details Provides common functionality for Task, including coroutine handle management and coroutine awaiting
     *
     * @tparam T Coroutine return value type, defaults to void
     *
     * @endif
     */
    template<typename T = void>
    struct TaskBase {
        /**
         * @if zh
         * @brief Promise类型别名
         * @elseif en
         * @brief Promise type alias
         * @endif
         */
        using promise_type = TaskPromise<T>;

    private:
        /**
         * @if zh
         * @brief 协程句柄
         * @elseif en
         * @brief Coroutine handle
         * @endif
         */
        std::coroutine_handle<promise_type> coro_;

    public:
        /**
         * @if zh
         * @brief 协程状态
         * @details 由协程返回对象持有：构造时创建并将弱引用写入协程promise，
         *          使该协程可作为Future的等待者被注册；最后一个强引用释放时销毁协程帧。
         *          置于基类，使 Task<void> 与 Task<T> 都具备等待者能力。
         *
         * @warning 若协程可能挂起后被恢复，其Task必须由调用方持有到恢复完成：
         *          丢弃Task（例如直接舍弃返回的临时对象）会让优化器认为该协程已结束并复用
         *          其帧内存储，恢复时读到被破坏的帧。实测GCC下丢弃后方恢复会段错误，
         *          而MSVC不复用故不显形，因此该缺陷只在部分编译器上暴露。
         *          仅在"协程不会挂起"或"挂起后永不恢复"时才可安全丢弃。
         *
         * @elseif en
         * @brief Coroutine state
         * @details Owned by the coroutine return object: created on construction and stored as a
         *          weak reference in the coroutine promise so the coroutine can register itself
         *          as a Future waiter; the coroutine frame is destroyed when the last strong
         *          reference is released. Living in the base class gives both Task<void> and
         *          Task<T> waiter capability.
         *
         * @warning When a coroutine may be resumed after suspension, its Task must be kept alive by
         *          the caller until resumption completes. Discarding the Task (e.g. dropping the
         *          returned temporary) lets the optimizer treat the coroutine as finished and reuse
         *          its frame storage, so a later resumption reads a corrupted frame. Measured as a
         *          segfault with GCC when a discarded coroutine is resumed afterwards; MSVC does not
         *          reuse and therefore hides the defect, so it only shows on some compilers.
         *          Discarding is safe only when the coroutine never suspends, or is never resumed.
         *
         * @endif
         */
        std::shared_ptr<CoroutineState> coro_state_;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 创建协程状态并写入promise的coro_state_weak_
         * @param handle 协程句柄
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates the coroutine state and writes it into the promise's coro_state_weak_
         * @param handle Coroutine handle
         *
         * @endif
         */
        TaskBase(std::coroutine_handle<promise_type> handle) noexcept;

        /**
         * @if zh
         *
         * @brief 移动构造函数
         * @param other 另一个TaskBase
         *
         * @elseif en
         *
         * @brief Move constructor
         * @param other Another TaskBase
         *
         * @endif
         */
        TaskBase(TaskBase&& other) noexcept;

        /**
         * @if zh
         *
         * @brief 获取Future
         * @return 关联的Future对象
         *
         * @elseif en
         *
         * @brief Get Future
         * @return Associated Future object
         *
         * @endif
         */
        [[nodiscard]] Future<T> getFuture() const;

        /**
         * @if zh
         *
         * @brief 移动赋值运算符
         * @param other 另一个TaskBase
         * @return 赋值后的引用
         *
         * @elseif en
         *
         * @brief Move assignment operator
         * @param other Another TaskBase
         * @return Reference after assignment
         *
         * @endif
         */
        TaskBase& operator=(TaskBase&& other) noexcept;

        /**
         * @if zh
         * @brief 析构函数
         * @elseif en
         * @brief Destructor
         * @endif
         */
        ~TaskBase();

        /**
         * @if zh
         *
         * @brief 协程await运算符
         * @return 用于协程等待的awaiter
         *
         * @elseif en
         *
         * @brief Coroutine await operator
         * @return Awaiter for coroutine awaiting
         *
         * @endif
         */
        auto operator co_await() const noexcept;
    };

    /**
     * @if zh
     *
     * @class Task
     * @brief Task类(非void特化)
     * @details 处理非void返回值的协程Task,管理协程状态生命周期
     *
     * @tparam T 协程返回值类型,默认为void
     *
     * @elseif en
     *
     * @class Task
     * @brief Task class (non-void specialization)
     * @details Handles coroutine Task with non-void return value, manages coroutine state lifecycle
     *
     * @tparam T Coroutine return value type, defaults to void
     *
     * @endif
     */
    template<typename T = void>
    struct Task : public TaskBase<T> {
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 仅转发给基类：协程状态由 TaskBase 创建并写入promise
         * @param h 协程句柄
         * @note 不能改为继承基类构造函数：该显式声明同时是 `Task{handle}` 形式
         *       类模板实参推导(CTAD)所需的推导指引，继承的构造函数不产生推导指引
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Only forwards to the base class: the coroutine state is created by TaskBase and
         *          written into the promise
         * @param h Coroutine handle
         * @note Do not replace this with an inherited base constructor: this explicit declaration
         *       also provides the deduction guide that `Task{handle}` (CTAD) relies on, and
         *       inherited constructors do not generate deduction guides
         *
         * @endif
         */
        Task(std::coroutine_handle<typename TaskBase<T>::promise_type> h);

        /**
         * @if zh
         * @brief 默认移动语义（协程句柄与协程状态均由基类移动）
         * @elseif en
         * @brief Default move semantics (handle and coroutine state are moved by the base class)
         * @endif
         */
        Task(Task&&)            = default;
        Task& operator=(Task&&) = default;

        /**
         * @if zh
         * @brief 默认析构函数
         * @elseif en
         * @brief Default destructor
         * @endif
         */
        ~Task() = default;

        /**
         * @if zh
         * @brief 禁用复制构造函数
         * @elseif en
         * @brief Disabled copy constructor
         * @endif
         */
        Task(const Task&)            = delete;

        /**
         * @if zh
         * @brief 禁用复制赋值运算符
         * @elseif en
         * @brief Disabled copy assignment operator
         * @endif
         */
        Task& operator=(const Task&) = delete;

        /**
         * @if zh
         *
         * @brief 分离协程
         * @details 将Task与协程分离,协程继续独立执行
         *
         * @elseif en
         *
         * @brief Detach coroutine
         * @details Detaches Task from coroutine, coroutine continues executing independently
         *
         * @endif
         */
        void detach() noexcept;
    };

    /**
     * @if zh
     *
     * @class Task<void>
     * @brief Task类(void特化)
     * @details 处理void返回值的协程Task
     *
     * @elseif en
     *
     * @class Task<void>
     * @brief Task class (void specialization)
     * @details Handles coroutine Task with void return value
     *
     * @endif
     */
    template<>
    struct Task<void> : public TaskBase<> {
        /**
         * @if zh
         * @brief 使用基类构造函数
         * @elseif en
         * @brief Use base class constructor
         * @endif
         */
        using TaskBase::TaskBase;

        /**
         * @if zh
         * @brief 默认移动语义
         * @elseif en
         * @brief Default move semantics
         * @endif
         */
        Task(Task&&)            = default;
        Task& operator=(Task&&) = default;

        /**
         * @if zh
         * @brief 禁用复制
         * @elseif en
         * @brief Disable copy
         * @endif
         */
        Task(const Task&)            = delete;
        Task& operator=(const Task&) = delete;
    };

    // 放在Task<void>后，此时Task定义完整

    /**
     * @if zh
     *
     * @class TaskPromise<void>
     * @brief Task Promise类(void特化)
     * @details 处理void返回值的协程Promise
     *
     * @elseif en
     *
     * @class TaskPromise<void>
     * @brief Task Promise class (void specialization)
     * @details Handles coroutine Promise with void return value
     *
     * @endif
     */
    template<>
    struct TaskPromise<void> : TaskPromiseBase<void> {
        /**
         * @if zh
         *
         * @brief 获取返回对象
         * @return Task对象
         *
         * @elseif en
         *
         * @brief Get return object
         * @return Task object
         *
         * @endif
         */
        Task<> get_return_object() noexcept;

        /**
         * @if zh
         * @brief 设置void返回值
         * @elseif en
         * @brief Set void return value
         * @endif
         */
        void return_void();
    };

}  // namespace stationeers

#include "task.inl"

#endif  // COMPILER_TASK_HPP
