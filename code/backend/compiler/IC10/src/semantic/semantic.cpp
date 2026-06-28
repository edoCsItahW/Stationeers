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
#include "ic10/semantic/semantic.hpp"
#include "common/exception/error.hpp"
#include "ic10/locals/local.hpp"
#include <format>
#include <sstream>
#include <ranges>


namespace stationeers::ic10 {

    std::string Symbol::toJSON() const {
        return std::format(R"({{ "name": "{}", "type": "{}" }})", name, enumToStr(type));
    }

    // SymbolTable::Entry

    bool SymbolTable::Entry::ready() const { return future.isReady(); }

    // SymbolTable

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
        else if (it->second.ready())
            return std::unexpected{Loc::msgFormat<MsgId::IEA2_1>(name)};

        // Future已决，通知所有等待者
        else
            it->second.promise.setValue({symbol});

        return {};
    }

    bool SymbolTable::contains(const std::string& name) const { return symbols_.contains(name); }

    std::string SymbolTable::toJSON() const {
        std::stringstream ss;

        ss << "[";

        for (const auto& entry : symbols_ | std::views::values)
            if (entry.ready())
                if (entry.future.get().has_value())
                    ss << entry.future.get().value()->toJSON() << ", ";


        std::string json = ss.str();

        if (json.size() > 2) json.erase(json.size() - 2);

        json += "]";

        return json;
    }

    void SymbolTable::failAllPending() {
        for (auto& [name, entry] : symbols_)
            if (!entry.ready())
                entry.promise.setException(
                    std::make_exception_ptr(Error(
                        UndefinedSymbolError{
                            Loc::msgFormat<MsgId::IEA3_1>(name), entry.pos,
                            endPos(entry.pos, name.size())
                        }
                    ))
                );
    }

}
