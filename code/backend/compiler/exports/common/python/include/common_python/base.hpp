// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file base.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief Python导出基础工具
 * @details 提供pybind11导出的基础工具函数和命名空间别名,包括JSON转换、
 *        协程阻塞执行器和错误转换等。用于C++类与Python对象之间的桥接。
 * @note 依赖pybind11头文件
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Python export base utilities
 * @details Provides base utility functions and namespace aliases for pybind11 exports,
 *        including JSON conversion, coroutine blocking executor and error conversion.
 *        Used for bridging between C++ classes and Python objects.
 * @note Depends on pybind11 headers
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_BASE_HPP
#define EXPORT_PYTHON_BASE_HPP
#pragma once

#include "common/async/task.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace py = pybind11;

namespace st = stationeers;

namespace stationeers {

    /**
     * @if zh
     *
     * @brief JSON字符串转Python对象
     * @details 使用Python内置json模块将JSON字符串解析为Python对象(dict/list等)
     *
     * @param jsonStr JSON格式字符串
     * @return Python对象
     *
     * @elseif en
     *
     * @brief Convert JSON string to Python object
     * @details Uses Python built-in json module to parse JSON string into Python object(dict/list etc.)
     *
     * @param jsonStr JSON format string
     * @return Python object
     *
     * @endif
     */
    inline py::object jsonLoads(const std::string& jsonStr) {
        return py::module_::import("json").attr("loads")(jsonStr);
    }

    /**
     * @if zh
     *
     * @brief 阻塞执行协程任务
     * @details 释放GIL后阻塞等待协程任务完成,若协程抛出异常则重新抛出。
     *          用于将C++20协程(Task)转换为同步Python调用。
     *
     * @param task 协程任务
     *
     * @elseif en
     *
     * @brief Block execute coroutine task
     * @details Releases GIL then blocks until coroutine task completes,
     *          rethrows if coroutine threw exception.
     *          Used to convert C++20 coroutine(Task) into synchronous Python call.
     *
     * @param task Coroutine task
     *
     * @endif
     */
    inline void runCoroutine(Task<> task) {
        std::exception_ptr eptr;
        {
            py::gil_scoped_release release;
            auto result = task.getFuture().get();
            if (!result.has_value()) eptr = result.error();
        }
        if (eptr) std::rethrow_exception(eptr);
    }

}  // namespace stationeers

#endif  // EXPORT_PYTHON_BASE_HPP
