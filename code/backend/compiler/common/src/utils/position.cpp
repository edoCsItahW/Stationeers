// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file position.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:14
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common/utils/position.hpp"
#include <cstddef>

namespace stationeers {

    int Pos::line() const { return line_; }

    int Pos::column() const { return column_; }

    std::size_t Pos::offset() const { return offset_; }

    void Pos::newline() {
        column_ = 1;
        ++line_;
        ++offset_;
    }

    void Pos::next() {
        ++column_;
        ++offset_;
    }

    void Pos::move(const std::size_t offset) {
        column_ += offset;
        offset_ += offset;
    }

    Pos endPos(const Pos& pos, const std::size_t length) {
        auto end = pos;

        end.move(length);

        return end;
    }

}  // namespace stationeers
