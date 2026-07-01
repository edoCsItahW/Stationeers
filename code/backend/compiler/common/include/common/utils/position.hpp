// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file position.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:13
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_POSITION_HPP
#define COMPILER_POSITION_HPP
#pragma once

#include <type_traits>


namespace stationeers {

    /**
     * @class Pos
     * @if zh
     *
     * @brief 令牌文本间位置信息类
     * @details 用于记录令牌文本在源代码中的位置信息
     *
     * @note 该位置信息中行号和列号从1开始
     *
     *
     * @elseif en
     *
     * @brief Token text position information class
     * @details Used to record the position information of token text in the source code
     *
     * @note The line and column numbers in this position information start from 1
     *
     * @endif
     */
    class Pos {
    public:
        /**
         * @if zh
         *
         * @brief 私有属性line_的getter
         * @return 令牌起始行号(从1开始)
         *
         *
         * @elseif en
         *
         * @brief Getter of private property line_
         * @return Starting line number of the token(starts from 1)
         *
         *
         * @endif
         *
         * @public @memberof Pos
         */
        [[nodiscard]] int line() const;

        /**
         * @if zh
         *
         * @brief 私有属性column_的getter
         * @return 令牌起始列号(从1开始)
         *
         *
         * @elseif en
         *
         * @brief Getter of private property column_
         * @return Starting column number of the token(starts from 1)
         *
         *
         * @endif
         *
         * @public @memberof Pos
         */
        [[nodiscard]] int column() const;

        /**
         * @if zh
         *
         * @brief 私有属性offset_的getter
         * @return 令牌起始位置距离源代码开头的距离
         *
         *
         * @elseif en
         *
         * @brief Getter of private property offset_
         * @return Starting position of the token from the beginning of the source code
         *
         *
         * @endif
         *
         * @public @memberof Pos
         */
        [[nodiscard]] std::size_t offset() const;

        /**
         * @if zh
         *
         * @brief 换行并自动更新位置信息
         * @details 重置列号为1，行号和位移量加1
         *
         *
         * @elseif en
         *
         * @brief Move to next line and update position information
         * @details Reset column number to 1, line number and offset are incremented by 1
         *
         *
         * @endif
         *
         * @public @memberof Pos
         */
        void newline();

        /**
         * @if zh
         *
         * @brief 移动到下一个位置
         * @details 列号加1，位移量加1
         *
         *
         * @elseif en
         *
         * @brief Move to next position
         * @details Increment column number by 1, offset by 1
         *
         *
         * @endif
         *
         * @public @memberof Pos
         */
        void next();

        /**
         * @if zh
         *
         * @brief 移动指定距离
         * @details 列号和位移量都加上指定距离
         *
         * @param offset 指定距离
         *
         *
         * @elseif en
         *
         * @brief Move specified distance
         * @details Increment column number and offset by specified distance
         *
         * @param offset Specified distance
         *
         *
         * @endif
         *
         * @public @memberof Pos
         */
        void move(std::size_t offset);

    private:
        /**
         * @if zh
         *
         * @brief 行号
         * @details 令牌起始行号(从1开始)
         *
         *
         * @elseif en
         *
         * @brief Line number
         * @details Starting line number of the token(starts from 1)
         *
         *
         * @endif
         *
         * @private @memberof Pos
         */
        int line_ = 1;

        /**
         * @if zh
         *
         * @brief 列号
         * @details 令牌起始列号(从1开始)
         *
         *
         * @elseif en
         *
         * @brief Column number
         * @details Starting column number of the token(starts from 1)
         *
         *
         * @endif
         *
         * @private @memberof Pos
         */
        int column_ = 1;

        /**
         * @if zh
         *
         * @brief 位移量
         * @details 记录当前位置距离源代码开头的距离
         *
         *
         * @elseif en
         *
         * @brief Offset
         * @details Record the distance from the beginning of the source code to the current
         * position
         *
         *
         * @endif
         *
         * @private @memberof Pos
         */
        std::size_t offset_ = 0;
    };

    template<typename>
    struct is_pos : std::false_type {};

    template<>
    struct is_pos<Pos> : std::true_type {};

    template<typename T>
    inline constexpr bool is_pos_v = is_pos<T>::value;

    template<typename T>
    concept IsPos = is_pos_v<T>;

    Pos endPos(const Pos &pos, std::size_t length);

}  // namespace stationeers

#endif  // COMPILER_POSITION_HPP
