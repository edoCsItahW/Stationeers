/**
 * @file instruction_dispatcher.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/28 16:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_INSTRUCTION_DISPATCHER_INL
#define STATIONEERS_INSTRUCTION_DISPATCHER_INL
#pragma once

namespace stationeers::ic10 {

    template<typename F>
    auto dispatch(F&& f, InstructionKeyword keyword) {
        // 使用索引序列来遍历所有可能的指令（InstructionKeyword）类型
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            using Variant = std::variant<decltype(f(instruction_map_t<Is>{}))...>;

            std::optional<Variant> result;
            bool matched = false;

            (
                [&] {
                    // 检查当前指令是否匹配当前关键字
                    if (keyword == to_instruction_keyword<Is>) {
                        result  = f(instruction_map_t<Is>{} /* 从特化映射中取出指令类型供回调 */);
                        matched = true;
                    }
                }(),
                ...);

            return matched ? result : std::optional<Variant>{};
        }(std::make_index_sequence<instruction_keyword_len>{});
    }

}  // namespace stationeers::ic10

#endif  // STATIONEERS_INSTRUCTION_DISPATCHER_INL
