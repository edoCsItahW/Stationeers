/**
 * @file value.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 10:34
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_VALUE_INL
#define IC10_RUNTIME_VALUE_INL
#pragma once

#include "common/exception/error.hpp"
#include "ic10_runtime/locals/local.hpp"

namespace stationeers::ic10 {

    template<typename T, IsVariant U>
        requires requires {
            { T::nodeName } -> is_fstring;
        }
    auto getValue(U& operand) -> std::conditional_t<std::is_const_v<U>, const T&, T&> {
        return std::visit(
            [&]<typename O, typename P = std::decay_t<O>>(O&& arg)
                -> std::conditional_t<std::is_const_v<U>, const T&, T&> {
                if constexpr (std::is_same_v<P, T>) return arg;

                std::tuple<std::string, Pos, Pos> args;

                if constexpr (requires {
                                  { arg.start() } -> std::same_as<Pos>;
                                  { arg.end() } -> std::same_as<Pos>;
                              })
                    args = {
                        IRLoc::msgFormat<IRMsgId::IEM2_1>(std::string(T::nodeName)), arg.start(),
                        arg.end()
                    };

                else
                    std::get<0>(args) = IRLoc::msgFormat<IRMsgId::IEM2_1>(std::string(T::nodeName));

                throw std::apply(
                    [&](auto&&... _args) {
                        return ValueError(std::forward<decltype(_args)>(_args)...);
                    },
                    args
                );
            },
            operand
        );
    }

    template<typename U, typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    U arithmeticTrans(T&& value) {
        if constexpr (std::is_same_v<std::decay_t<U>, std::decay_t<T>>)
            return std::forward<T>(value);

        else
            return static_cast<U>(std::forward<T>(value));
    }

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_VALUE_INL
