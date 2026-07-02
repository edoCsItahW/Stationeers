// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file pos_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_python/pos_adapter.hpp"
#include "common/utils/position.hpp"

namespace stationeers {

    void initPos(py::module_& m) {
        py::class_<Pos>(m, "Pos")
            /**
             * @if zh
             * @brief 默认构造函数,行号和列号从1开始,偏移从0开始
             * @elseif en
             * @brief Default constructor, line and column start from 1, offset from 0
             * @endif
             */
            .def(py::init<>())
            /// @if zh 行号(只读) @elseif en Line number(readonly) @endif
            .def_property_readonly("line", &Pos::line)
            /// @if zh 列号(只读) @elseif en Column number(readonly) @endif
            .def_property_readonly("column", &Pos::column)
            /// @if zh 偏移量(只读) @elseif en Offset(readonly) @endif
            .def_property_readonly("offset", &Pos::offset)
            /**
             * @if zh
             * @brief 换行,行号加1,列号重置为1,偏移加1
             * @elseif en
             * @brief Newline, line increments by 1, column resets to 1, offset increments by 1
             * @endif
             */
            .def("newline", &Pos::newline)
            /**
             * @if zh
             * @brief 前进一字符,列号和偏移各加1
             * @elseif en
             * @brief Advance one character, column and offset each increment by 1
             * @endif
             */
            .def("next", &Pos::next)
            /**
             * @if zh
             * @brief 移动到指定偏移位置
             * @param offset 目标偏移量
             * @elseif en
             * @brief Move to specified offset position
             * @param offset Target offset
             * @endif
             */
            .def("move", &Pos::move, py::arg("offset"))
            /// @if zh 字符串表示 @elseif en String representation @endif
            .def("__repr__", [](const Pos& self) {
                return "Pos(line=" + std::to_string(self.line())
                     + ", column=" + std::to_string(self.column())
                     + ", offset=" + std::to_string(self.offset()) + ")";
            });
    }

}  // namespace stationeers
