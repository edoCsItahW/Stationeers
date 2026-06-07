// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:55
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COMMON_LOCAL_HPP
#define COMPILER_COMMON_LOCAL_HPP
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <array>

#include "common/utils/enum_to_str.hpp"
#include "common/utils/common.hpp"


namespace stationeers {

    template<IsEnum E, template<typename, E> class M>
    struct MsgPack {
        template<typename T, E I>
        using msg_map_type = M<T, I>;

        using msg_id_type = E;
    };

    template<typename>
    struct is_msg_pack : std::false_type {};

    template<IsEnum E, template<typename, E> class M>
    struct is_msg_pack<MsgPack<E, M>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_msg_pack_v = is_msg_pack<T>::value;

    template<typename T>
    concept IsMsgPack = is_msg_pack_v<T>;

    template<IsEnum E>
    inline constexpr auto msg_id_len = enumMax<E>();

    template<IsEnum E>
    inline constexpr auto msg_id_list = []<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::array{static_cast<E>(Is)...};
    }(std::make_index_sequence<msg_id_len<E>>{});

    template<IsEnum E>
    class LanguageBase {
    public:
        template<E I>
        [[nodiscard]] std::string_view getMsg() const;

        virtual ~LanguageBase() = default;

    protected:
        [[nodiscard]] virtual std::string_view getMsgImpl(E id) const = 0;
    };

    template<typename T, IsMsgPack P, typename P::msg_id_type I>
    std::string_view getMsgFromMap();

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

    template<typename T, IsMsgPack P>
    class Language final : public LanguageBase<typename P::msg_id_type> {
        using E = P::msg_id_type;

        static constexpr auto msgFuncs = []<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array<std::string_view (*)(), sizeof...(Is)>{
                &getMsgFromMap<T, P, msg_id_list<E>[Is]>...
            };
        }(std::make_index_sequence<msg_id_len<E>>{});

        static constexpr std::size_t findIndex(E id);

        [[nodiscard]] std::string_view getMsgImpl(E id) const override;
    };

    template<typename E, E I, int N>
    concept number_of_args_check = get_number_of_args<E, I> == N;

    template<IsMsgPack P>
    class Local {
    public:
        using E = P::msg_id_type;

        template<typename T>
        static void registerLanguage(const std::string& name);

        static void setLanguage(const std::string& name);

        template<E I, std::size_t N = 0>
            requires number_of_args_check<E, I, N>
        [[nodiscard]] static auto msg();

        template<E I>
            requires number_of_args_check<E, I, 0>
        [[nodiscard]] static std::string msgStr();

        template<E I, typename... Args>
            requires number_of_args_check<E, I, sizeof...(Args)>
        [[nodiscard]] static auto msgFormat(Args&&... args);

    private:
        using Creator = std::function<std::unique_ptr<LanguageBase<E>>()>;

        inline static std::unordered_map<std::string, Creator> registry_ = {};

        inline static std::unique_ptr<LanguageBase<E>> language_ = nullptr;
    };

}  // namespace stationeers

#include "local.inl"

#endif  // COMPILER_COMMON_LOCAL_HPP
