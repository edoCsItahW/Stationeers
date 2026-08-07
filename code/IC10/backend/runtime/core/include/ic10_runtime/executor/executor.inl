/**
 * @file executor.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 14:16
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_EXECUTOR_INL
#define IC10_RUNTIME_EXECUTOR_INL
#pragma once

namespace stationeers::ic10 {

    template<IsVariant T>
    std::optional<double> Executor::operandValue(T&& op, bool throwError) {
        return std::visit(
            [this, &throwError]<typename O, typename U = std::decay_t<O>>(O&& arg)
                -> std::optional<double> {
                std::optional<double> value;

                if constexpr (std::is_same_v<U, Identifier>) {
                    if (auto resolved = ctx_.resolve(arg.value); resolved)
                        value = operandValue(*resolved);
                }

                else if constexpr (std::is_same_v<U, Register>)
                    value = ctx_.memory.getReg(arg.value);

                else
                    value = directionValue(arg);

                if (!value && throwError)
                    throw ValueError(
                        IRLoc::msgFormat<IRMsgId::IEM2_1>(arg.toString()), arg.start(), arg.end()
                    );

                return value;
            },
            std::forward<T>(op)
        );
    }

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_EXECUTOR_INL
