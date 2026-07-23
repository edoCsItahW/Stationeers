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
 * @if zh
 * @brief 源代码位置信息管理
 * @details 提供源代码中令牌位置的管理功能,包括行号、列号和偏移量的追踪。
 *        支持位置的移动、换行等操作,用于编译器的错误定位和诊断信息输出。
 * @note 行号和列号从1开始,偏移量从0开始
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Source code position information management
 * @details Provides management functionality for token positions in source code, including tracking of
 *        line number, column number, and offset. Supports position movement, newline operations,
 *        used for compiler error location and diagnostic information output.
 * @note Line and column numbers start from 1, offset starts from 0
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_POSITION_HPP
#define COMPILER_POSITION_HPP
#pragma once

#include "common/utils/utf8string.hpp"
#include <string>
#include <type_traits>


namespace stationeers {

    /**
     * @if zh
     * @brief 计算 std::string 中 Unicode 字符的数量
     * @param str 待计算的字符串
     * @return Unicode 字符数量
     * @elseif en
     * @brief Count Unicode characters in a std::string
     * @param str String to count
     * @return Number of Unicode characters
     * @endif
     */
    inline std::size_t charLength(const std::string& str) {
        return static_cast<std::size_t>(utf::string_view(str.c_str()).length());
    }

    /**
     * @class Pos
     * @if zh
     *
     * @brief 令牌文本位置信息类
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
         * @brief 默认构造函数
         * @details 初始位置为第1行第1列，偏移量为0
         *
         * @elseif en
         *
         * @brief Default constructor
         * @details Initial position is line 1, column 1, offset 0
         *
         * @endif
         */
        Pos() = default;

        /**
         * @if zh
         *
         * @brief 带参数的构造函数
         * @param line 行号(从1开始)
         * @param column 列号(从1开始)
         * @param offset 偏移量(从0开始)
         *
         * @elseif en
         *
         * @brief Constructor with parameters
         * @param line Line number (starts from 1)
         * @param column Column number (starts from 1)
         * @param offset Offset (starts from 0)
         *
         * @endif
         */
        Pos(int line, int column, std::size_t offset);
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
         * @brief 移动到下一个字节位置
         * @details 传入当前字节值，若为 UTF-8 前导字节（非续字节）则列号加1，位移量加1
         * @param byte 当前字节值
         *
         * @elseif en
         *
         * @brief Move to next byte position
         * @details Takes current byte value; increments column by 1 only for UTF-8 leading bytes
         *          (non-continuation), always increments offset by 1
         * @param byte Current byte value
         *
         * @endif
         *
         * @public @memberof Pos
         */
        void next(unsigned char byte);

        /**
         * @if zh
         *
         * @brief 移动指定字符和字节距离
         * @details 列号加 charOffset，位移量加 byteOffset
         * @param charOffset 字符偏移量
         * @param byteOffset 字节偏移量
         *
         * @elseif en
         *
         * @brief Move by specified character and byte distances
         * @details Increment column by charOffset, offset by byteOffset
         * @param charOffset Character offset
         * @param byteOffset Byte offset
         *
         * @endif
         *
         * @public @memberof Pos
         */
        void move(std::size_t charOffset, std::size_t byteOffset);

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

    /**
     * @if zh
     *
     * @brief Pos类型检测特化
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Pos type detection specialization
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename>
    struct is_pos : std::false_type {};

    template<>
    struct is_pos<Pos> : std::true_type {};

    /**
     * @if zh
     *
     * @brief Pos类型检测变量模板
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Pos type detection variable template
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    inline constexpr bool is_pos_v = is_pos<T>::value;

    /**
     * @if zh
     *
     * @brief Pos约束概念
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Pos constraint concept
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    concept IsPos = is_pos_v<T>;

    /**
     * @if zh
     *
     * @brief 计算结束位置
     * @details 根据起始位置和字符/字节长度计算结束位置
     * @param pos 起始位置
     * @param charLength 字符长度（用于列号计算）
     * @param byteLength 字节长度（用于偏移量计算）
     * @return 结束位置
     *
     * @elseif en
     *
     * @brief Calculate end position
     * @details Calculates end position based on start position and char/byte lengths
     * @param pos Start position
     * @param charLength Character length (for column calculation)
     * @param byteLength Byte length (for offset calculation)
     * @return End position
     *
     * @endif
     */
    Pos endPos(const Pos &pos, std::size_t charLength, std::size_t byteLength);

    /**
     * @if zh
     *
     * @brief 根据字符串内容计算结束位置
     * @param pos 起始位置
     * @param lexeme 词素字符串
     * @return 结束位置
     *
     * @elseif en
     *
     * @brief Calculate end position from string content
     * @param pos Start position
     * @param lexeme Lexeme string
     * @return End position
     *
     * @endif
     */
    inline Pos endPos(const Pos &pos, const std::string& lexeme) {
        return endPos(pos, charLength(lexeme), lexeme.size());
    }

}  // namespace stationeers

#endif  // COMPILER_POSITION_HPP