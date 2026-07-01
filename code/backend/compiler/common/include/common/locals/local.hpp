// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:40
 * @if zh
 * @brief 线程局部存储(TLS)管理工具
 * @details 提供线程局部存储的封装实现,支持线程独立的变量存储和访问。
 *        主要用于编译器中需要线程独立的全局状态管理。
 * @note 依赖于编译器特定的thread_local存储修饰符
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Thread Local Storage (TLS) management utilities
 * @details Provides encapsulated implementation of thread local storage, supporting thread-independent
 *        variable storage and access. Mainly used in compilers for thread-independent global state management.
 * @note Depends on compiler-specific thread_local storage modifier
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_COMMON_LOCAL_HPP
#define COMPILER_COMMON_LOCAL_HPP
#pragma once

#include "common/utils/common.hpp"
#include "common/utils/enum_to_str.hpp"
#include <array>
#include <functional>
#include <format>
#include <memory>
#include <string>

namespace stationeers {

    /**
     * @if zh
     *
     * @struct MsgPack
     * @brief 消息包模板结构
     * @details 将枚举类型和消息映射模板组合成一个消息包,用于多语言消息系统
     *
     * @tparam E 枚举类型(消息ID类型)
     * @tparam M 消息映射模板,接受(T, E)参数
     *
     * @elseif en
     *
     * @struct MsgPack
     * @brief Message pack template structure
     * @details Combines enum type and message mapping template into a message pack for multilingual message system
     *
     * @tparam E Enum type (message ID type)
     * @tparam M Message mapping template, accepts (T, E) parameters
     *
     * @endif
     */
    template<IsEnum E, template<typename, E> class M>
    struct MsgPack {
        /**
         * @if zh
         *
         * @brief 消息映射类型别名
         * @tparam T 消息类型
         * @tparam I 消息ID
         *
         * @elseif en
         *
         * @brief Message mapping type alias
         * @tparam T Message type
         * @tparam I Message ID
         *
         * @endif
         */
        template<typename T, E I>
        using msg_map_type = M<T, I>;

        /**
         * @if zh
         * @brief 消息ID类型别名
         * @elseif en
         * @brief Message ID type alias
         * @endif
         */
        using msg_id_type = E;
    };

    /**
     * @if zh
     *
     * @brief MsgPack类型检测特化
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief MsgPack type detection specialization
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename>
    struct is_msg_pack : std::false_type {};

    template<IsEnum E, template<typename, E> class M>
    struct is_msg_pack<MsgPack<E, M>> : std::true_type {};

    /**
     * @if zh
     *
     * @brief MsgPack类型检测变量模板
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief MsgPack type detection variable template
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    inline constexpr auto is_msg_pack_v = is_msg_pack<T>::value;

    /**
     * @if zh
     *
     * @brief MsgPack约束概念
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief MsgPack constraint concept
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    concept IsMsgPack = is_msg_pack_v<T>;

    /**
     * @if zh
     *
     * @brief 获取枚举类型的最大消息ID长度
     * @tparam E 枚举类型
     *
     * @elseif en
     *
     * @brief Get maximum message ID length for enum type
     * @tparam E Enum type
     *
     * @endif
     */
    template<IsEnum E>
    inline constexpr auto msg_id_len = enumMax<E>();

    /**
     * @if zh
     *
     * @brief 生成枚举类型的消息ID列表
     * @tparam E 枚举类型
     * @details 编译期生成从0到enumMax<E>()-1的枚举值数组
     *
     * @elseif en
     *
     * @brief Generate message ID list for enum type
     * @tparam E Enum type
     * @details Compile-time generates enum value array from 0 to enumMax<E>()-1
     *
     * @endif
     */
    template<IsEnum E>
    inline constexpr auto msg_id_list = []<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::array{static_cast<E>(Is)...};
    }(std::make_index_sequence<msg_id_len<E>>{});

    /**
     * @if zh
     *
     * @class LanguageBase
     * @brief 多语言消息基类
     * @details 提供多语言消息系统的抽象接口,具体语言实现需继承此类
     *
     * @tparam E 消息ID枚举类型
     *
     * @elseif en
     *
     * @class LanguageBase
     * @brief Multilingual message base class
     * @details Provides abstract interface for multilingual message system, concrete language implementations need to inherit this class
     *
     * @tparam E Message ID enum type
     *
     * @endif
     */
    template<IsEnum E>
    class LanguageBase {
    public:
        /**
         * @if zh
         *
         * @brief 获取指定ID的消息
         * @tparam I 消息ID
         * @return 消息字符串
         *
         * @elseif en
         *
         * @brief Get message by specified ID
         * @tparam I Message ID
         * @return Message string
         *
         * @endif
         */
        template<E I>
        [[nodiscard]] std::string_view getMsg() const;

        /**
         * @if zh
         * @brief 虚析构函数
         * @elseif en
         * @brief Virtual destructor
         * @endif
         */
        virtual ~LanguageBase() = default;

    protected:
        /**
         * @if zh
         *
         * @brief 获取消息实现(子类需实现)
         * @param id 消息ID
         * @return 消息字符串
         *
         * @elseif en
         *
         * @brief Get message implementation (subclass must implement)
         * @param id Message ID
         * @return Message string
         *
         * @endif
         */
        [[nodiscard]] virtual std::string_view getMsgImpl(E id) const = 0;
    };

    /**
     * @if zh
     *
     * @brief 从消息映射获取消息
     * @tparam T 语言类型
     * @tparam P 消息包类型
     * @tparam I 消息ID
     * @return 消息字符串
     *
     * @elseif en
     *
     * @brief Get message from message map
     * @tparam T Language type
     * @tparam P Message pack type
     * @tparam I Message ID
     * @return Message string
     *
     * @endif
     */
    template<typename T, IsMsgPack P, typename P::msg_id_type I>
    std::string_view getMsgFromMap();

    /**
     * @if zh
     *
     * @brief 获取枚举字符串参数个数
     * @details 解析枚举值对应的字符串,计算下划线后数字的位数作为参数个数
     * @tparam E 枚举类型
     * @tparam I 枚举值
     *
     * @elseif en
     *
     * @brief Get number of arguments from enum string
     * @details Parses enum value corresponding string, calculates digits after underscore as argument count
     * @tparam E Enum type
     * @tparam I Enum value
     *
     * @endif
     */
    template<IsEnum E, E I>
    inline constexpr int get_number_of_args = [] {
        constexpr std::string_view str = enumToStr(I);
        if constexpr (constexpr auto pos = str.rfind('_'); pos == std::string::npos)
            return 0;
        else {
            int result = 0;

            for (int i = pos + 1; i < str.size(); ++i)
                if (str[i] >= '0' && str[i] <= '9') result = result * 10 + (str[i] - '0');

            return result;
        }
    }();

    /**
     * @if zh
     *
     * @class Language
     * @brief 具体语言实现类
     * @details 基于消息包的具体语言消息实现,通过函数指针数组实现O(1)消息查找
     *
     * @tparam T 语言类型标记
     * @tparam P 消息包类型
     *
     * @elseif en
     *
     * @class Language
     * @brief Concrete language implementation class
     * @details Concrete language message implementation based on message pack, achieves O(1) message lookup through function pointer array
     *
     * @tparam T Language type tag
     * @tparam P Message pack type
     *
     * @endif
     */
    template<typename T, IsMsgPack P>
    class Language final : public LanguageBase<typename P::msg_id_type> {
        using E = P::msg_id_type;

        /**
         * @if zh
         * @brief 消息函数指针数组
         * @elseif en
         * @brief Message function pointer array
         * @endif
         */
        static constexpr auto msgFuncs = []<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array<std::string_view (*)(), sizeof...(Is)>{
                &getMsgFromMap<T, P, msg_id_list<E>[Is]>...
            };
        }(std::make_index_sequence<msg_id_len<E>>{});

        /**
         * @if zh
         *
         * @brief 查找消息ID对应的索引
         * @param id 消息ID
         * @return 对应的数组索引
         *
         * @elseif en
         *
         * @brief Find index corresponding to message ID
         * @param id Message ID
         * @return Corresponding array index
         *
         * @endif
         */
        static constexpr std::size_t findIndex(E id);

        /**
         * @if zh
         *
         * @brief 获取消息实现
         * @param id 消息ID
         * @return 消息字符串
         *
         * @elseif en
         *
         * @brief Get message implementation
         * @param id Message ID
         * @return Message string
         *
         * @endif
         */
        [[nodiscard]] std::string_view getMsgImpl(E id) const override;
    };

    /**
     * @if zh
     *
     * @brief 参数个数检查概念
     * @tparam E 枚举类型
     * @tparam I 枚举值
     * @tparam N 期望的参数个数
     *
     * @elseif en
     *
     * @brief Number of arguments check concept
     * @tparam E Enum type
     * @tparam I Enum value
     * @tparam N Expected number of arguments
     *
     * @endif
     */
    template<typename E, E I, int N>
    concept number_of_args_check = get_number_of_args<E, I> == N;

    /**
     * @if zh
     *
     * @class Local
     * @brief 多语言本地化管理器
     * @details 提供运行时多语言切换和消息获取功能,支持消息注册和格式化输出
     *
     * @tparam P 消息包类型
     *
     * @par 示例:
     * @code
     * // 定义消息包
     * enum class MsgId { Hello, Bye };
     *
     * template<typename T, MsgId I>
     * struct MsgMap {
     *     static std::string get() {
     *         if constexpr (std::is_same_v<T, Chinese>) return "你好";
     *         else return "Hello";
     *     }
     * };
     *
     * using MsgPackType = MsgPack<MsgId, MsgMap>;
     * using LocalMgr = Local<MsgPackType>;
     *
     * // 注册和使用
     * LocalMgr::registerLanguage<Chinese>("zh");
     * LocalMgr::setLanguage("zh");
     * auto msg = LocalMgr::msgStr<MsgId::Hello>();
     * @endcode
     *
     * @elseif en
     *
     * @class Local
     * @brief Multilingual localization manager
     * @details Provides runtime multilingual switching and message retrieval, supports message registration and formatted output
     *
     * @tparam P Message pack type
     *
     * @par Example:
     * @code
     * // Define message pack
     * enum class MsgId { Hello, Bye };
     *
     * template<typename T, MsgId I>
     * struct MsgMap {
     *     static std::string get() {
     *         if constexpr (std::is_same_v<T, Chinese>) return "你好";
     *         else return "Hello";
     *     }
     * };
     *
     * using MsgPackType = MsgPack<MsgId, MsgMap>;
     * using LocalMgr = Local<MsgPackType>;
     *
     * // Register and use
     * LocalMgr::registerLanguage<Chinese>("zh");
     * LocalMgr::setLanguage("zh");
     * auto msg = LocalMgr::msgStr<MsgId::Hello>();
     * @endcode
     *
     * @endif
     */
    template<IsMsgPack P>
    class Local {
    public:
        /**
         * @if zh
         * @brief 消息ID类型别名
         * @elseif en
         * @brief Message ID type alias
         * @endif
         */
        using E = P::msg_id_type;

        /**
         * @if zh
         *
         * @brief 注册语言
         * @tparam T 语言类型
         * @param name 语言名称标识
         *
         * @elseif en
         *
         * @brief Register language
         * @tparam T Language type
         * @param name Language name identifier
         *
         * @endif
         */
        template<typename T>
        static void registerLanguage(const std::string& name);

        /**
         * @if zh
         *
         * @brief 设置当前语言
         * @param name 语言名称标识
         *
         * @elseif en
         *
         * @brief Set current language
         * @param name Language name identifier
         *
         * @endif
         */
        static void setLanguage(const std::string& name);

        /**
         * @if zh
         *
         * @brief 获取无参数消息
         * @tparam I 消息ID
         * @return 消息字符串
         *
         * @elseif en
         *
         * @brief Get message without arguments
         * @tparam I Message ID
         * @return Message string
         *
         * @endif
         */
        template<E I, std::size_t N = 0>
            requires number_of_args_check<E, I, N>
        [[nodiscard]] static auto msg()
// resolve MSVC C2244 error
#ifndef _MSC_VER
            ;
#else
        {
            return language_->template getMsg<I>();
        }
#endif

        /**
         * @if zh
         *
         * @brief 获取无参数消息(std::string版本)
         * @tparam I 消息ID
         * @return 消息字符串
         *
         * @elseif en
         *
         * @brief Get message without arguments (std::string version)
         * @tparam I Message ID
         * @return Message string
         *
         * @endif
         */
        template<E I>
            requires number_of_args_check<E, I, 0>
        [[nodiscard]] static std::string msgStr()
// resolve MSVC C2244 error
#ifndef _MSC_VER
            ;
#else
        {
            return std::string(msg<I, 0>());
        }
#endif

        /**
         * @if zh
         *
         * @brief 获取格式化消息
         * @tparam I 消息ID
         * @tparam Args 参数类型包
         * @param args 格式化参数
         * @return 格式化后的字符串
         *
         * @elseif en
         *
         * @brief Get formatted message
         * @tparam I Message ID
         * @tparam Args Argument type pack
         * @param args Format arguments
         * @return Formatted string
         *
         * @endif
         */
        template<E I, typename... Args>
            requires number_of_args_check<E, I, sizeof...(Args)>
        [[nodiscard]] static auto msgFormat(Args&&... args)
// resolve MSVC C2244 error
#ifndef _MSC_VER
            ;
#else
        {
            return std::apply(
                [&](const auto&... unpacked) {
                    return std::vformat(
                        msg<I, sizeof...(Args)>(), std::make_format_args(unpacked...)
                    );
                },
                std::tuple<Args...>(std::forward<Args>(args)...)
            );
        }
#endif

    private:
        /**
         * @if zh
         *
         * @brief 语言创建器类型别名
         *
         * @elseif en
         *
         * @brief Language creator type alias
         *
         * @endif
         */
        using Creator = std::function<std::unique_ptr<LanguageBase<E>>()>;

        /**
         * @if zh
         * @brief 语言注册表
         * @elseif en
         * @brief Language registry
         * @endif
         */
        inline static std::unordered_map<std::string, Creator> registry_ = {};

        /**
         * @if zh
         * @brief 当前语言实例
         * @elseif en
         * @brief Current language instance
         * @endif
         */
        inline static std::unique_ptr<LanguageBase<E>> language_ = nullptr;
    };

}  // namespace stationeers

#include "local.inl"

#endif  // COMPILER_COMMON_LOCAL_HPP
