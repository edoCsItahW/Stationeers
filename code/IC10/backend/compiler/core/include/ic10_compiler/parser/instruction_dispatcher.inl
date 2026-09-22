/**
 * @file instruction_dispatcher.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/28 16:21
 * @if zh
 * @brief 指令关键字到 AST 节点的运行时分派实现
 * @details 实现 @ref dispatch：以 @c std::index_sequence 在编译期展开全部 @ref InstructionKeyword 取值，运行时只进入命中的那一支，并将回调构造出的指令节点就地写入 @p Result 的对应分支。
 *          全部取值均不匹配时返回空的 @c std::optional，而非抛出异常。
 *
 * @note 该展开同时充当穷尽性校验：新增指令关键字而未提供对应的 @c InstructionMapper 特化时，此处会直接编译失败。
 * @see instruction_dispatcher.hpp 分派接口声明
 * @see instructions.hpp 指令节点定义
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Runtime dispatch from instruction keyword to AST node
 * @details Implements @ref dispatch: a @c std::index_sequence expands every @ref InstructionKeyword value at compile time; at runtime only the matching branch is entered and the node built by the callback is written in place into the matching branch of @p Result.
 *          When no value matches it returns an empty @c std::optional rather than throwing.
 *
 * @note The expansion doubles as an exhaustiveness check: adding an instruction keyword without a matching @c InstructionMapper specialization fails to compile right here.
 * @see instruction_dispatcher.hpp dispatch interface
 * @see instructions.hpp instruction node definitions
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_INSTRUCTION_DISPATCHER_INL
#define IC10_COMPILER_CORE_INSTRUCTION_DISPATCHER_INL
#pragma once

namespace stationeers::ic10 {

    template<typename Result, typename F>
    std::optional<Result> dispatch(F&& f, InstructionKeyword keyword) {
        std::optional<Result> result;

        // 使用索引序列来遍历所有可能的指令（InstructionKeyword）类型
        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            (
                [&] {
                    // 检查当前指令是否匹配当前关键字
                    if (keyword == to_instruction_keyword<Is>) {
                        using Instruction = instruction_map_t<Is>;  // 从特化映射中取出指令类型

                        result.emplace(
                            std::in_place_type<Instruction>, f(Instruction{} /* 供回调萃取 */)
                        );
                    }
                }(),
                ...);
        }(std::make_index_sequence<instruction_keyword_len>{});

        return result;
    }

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_INSTRUCTION_DISPATCHER_INL
