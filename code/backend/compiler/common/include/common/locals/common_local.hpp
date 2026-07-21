/**
 * @file common_local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:36
 * @if zh
 * @brief 公共模块本地化消息定义
 * @details 定义公共模块的诊断消息ID和消息映射模板,用于文件系统等公共功能的错误提示。
 *        支持中英双语消息定义。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Common module localization message definitions
 * @details Defines diagnostic message IDs and message mapping templates for common modules,
 *        used for error prompts in common functionality such as file system operations.
 *        Supports Chinese-English bilingual message definitions.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_COMMON_LOCAL_PRIVATE_HPP
#define COMPILER_COMMON_LOCAL_PRIVATE_HPP
#pragma once

#include "local.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @enum CommonMsgId
     * @brief 公共模块消息ID枚举
     * @details 定义公共模块(文件系统等)的诊断消息ID,遵循IE0格式命名规范。
     *
     * @elseif en
     *
     * @enum CommonMsgId
     * @brief Common module message ID enum
     * @details Defines diagnostic message IDs for common modules (file system, etc.),
     *        following IE0 format naming convention.
     *
     * @endif
     */
    enum class CommonMsgId : uint32_t {
        /** @if zh 文件打开失败 @else Could not open file @endif */
        CEF1_1,

        /** @if zh 文件读取失败 @else Failed to read file @endif */
        CEF2_1,

        /** @if zh 文件写入失败 @else Unable to write to file @endif */
        CEF3_1
    };

    /**
     * @if zh
     *
     * @brief 公共消息映射模板
     * @tparam T 语言类型
     * @tparam I 消息ID
     *
     * @elseif en
     *
     * @brief Common message mapping template
     * @tparam T Language type
     * @tparam I Message ID
     *
     * @endif
     */
    template<typename, CommonMsgId>
    struct CommonMsgMap;

    /**
     * @if zh
     * @brief 公共消息包类型
     * @elseif en
     * @brief Common message pack type
     * @endif
     */
    using CommonMsgPack = MsgPack<CommonMsgId, CommonMsgMap>;

    /**
     * @if zh
     * @brief 公共模块本地化管理器
     * @elseif en
     * @brief Common module localization manager
     * @endif
     */
    class CommonLocal : public Local<CommonMsgPack> {};

    /**
     * @if zh
     * @brief 公共消息ID类型别名
     * @elseif en
     * @brief Common message ID type alias
     * @endif
     */
    using CMsgId = CommonMsgId;

    /**
     * @if zh
     * @brief 公共本地化管理器类型别名
     * @elseif en
     * @brief Common localization manager type alias
     * @endif
     */
    using CLoc = CommonLocal;

    /**
     * @if zh
     *
     * @brief 公共模块本地化消息宏
     * @details 用于定义公共模块的本地化消息
     * @param type 语言类型
     * @param id 消息ID
     * @param _msg 消息文本
     *
     * @elseif en
     *
     * @brief Common module localization message macro
     * @details Used to define localization messages for common modules
     * @param type Language type
     * @param id Message ID
     * @param _msg Message text
     *
     * @endif
     */
#define COMMON_LOCAL_MSG(type, id, _msg)                                                           \
    template<>                                                                                     \
    struct CommonMsgMap<type, CommonMsgId::id> {                                                   \
        static constexpr auto msg = _msg##_fs;                                                     \
    };

}  // namespace stationeers

#endif  // COMPILER_COMMON_LOCAL_PRIVATE_HPP