// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semantic.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/semantic/semantic.hpp"
#include "common/exception/error.hpp"
#include "common/utils/json.hpp"
#include "ic10_compiler/locals/local.hpp"
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

namespace stationeers::ic10 {

    std::string Symbol::toJSON() const {
        return toJson<"name", "type", "category", "typeName", "value", "desc", "builtin">(
            name, static_cast<int>(type.kind), static_cast<int>(type.category),
            type.typeName ? type.typeName : std::nullopt, value ? value : std::nullopt,
            desc ? std::optional(call(*desc, [](const auto& d) { return d.toJSON(); }))
                 : std::nullopt,
            isBuiltin
        );
    }

    // SymbolTable::Entry

    bool SymbolTable::Entry::ready() const noexcept { return future.isReady(); }

    // SymbolTable

    SymbolTable::SymbolTable() {
        for (int i = 0; i < 6; ++i) {
            auto key = std::string("d") + std::to_string(i);
            builtinSymbols.emplace(key, Symbol{.name = key, .type = type_of<StaticDevice>});
        }
    }

    std::expected<void, std::string> SymbolTable::define(
        const std::string& name, const std::shared_ptr<Symbol>& symbol
    ) {
        // 未定义则直接添加
        if (auto it = symbols_.find(name); it == symbols_.end()) {
            Promise<std::shared_ptr<Symbol>> promise;

            promise.setValue({symbol});

            auto future = promise.getFuture();

            symbols_[name] = {std::move(promise), future};
        }
        // 已定义则报错
        else if (it->second.ready()) [[unlikely]]
            return std::unexpected{ICLoc::msgFormat<ICMsgId::IEA2_1>(name)};

        // Future已决，通知所有等待者
        else
            it->second.promise.setValue({symbol});

        return {};
    }

    bool SymbolTable::contains(const std::string& name) const noexcept {
        if (auto it = symbols_.find(name); it != symbols_.end()) return it->second.ready();
        return false;
    }

    std::string SymbolTable::toJSON() const {
        std::stringstream ss;

        ss << "{\"symbols\": {";

        bool first = true;

        for (const auto& [key, entry] : symbols_)
            if (entry.ready())
                if (entry.future.get().has_value()) {
                    if (!first) [[likely]]
                        ss << ", ";

                    ss << '\"' << key << "\": " << entry.future.get().value()->toJSON();

                    first = false;
                }


        ss << "}, \"builtinSymbols\": {";

        first = true;

        for (const auto& [key, symbol] : builtinSymbols) {
            if (!first) [[likely]]
                ss << ", ";

            ss << '\"' << key << "\": " << symbol.toJSON();

            first = false;
        }
        ss << "}}";

        return ss.str();
    }

    void SymbolTable::failAllPending() {
        // 失败一个条目会恢复其等待者，被恢复的协程会继续分析后续操作数并可能 resolve
        // 新的符号：既会向 symbols_ 插入（使遍历迭代器失效），也会产生新的待决条目。
        // 因此每轮先对待决条目做快照再统一失败，并循环扫尾直到不再产生新的待决条目。
        // 注意只挑选 PENDING：已失败条目即使再次扫描也不能重复 set（会抛异常）。
        for (bool progress = true; progress;) {
            progress = false;

            std::vector<std::pair<std::string, Entry>> pending;

            for (const auto& [name, entry] : symbols_)
                if (entry.future.is<Status::PENDING>()) pending.emplace_back(name, entry);

            for (auto& [name, entry] : pending) {
                progress = true;

                entry.promise.setException(
                    std::make_exception_ptr(Error(
                        UndefinedSymbolError{
                            ICLoc::msgFormat<ICMsgId::IEA3_1>(name), entry.pos,
                            endPos(entry.pos, name)
                        }
                    ))
                );
            }
        }
    }

}  // namespace stationeers::ic10
