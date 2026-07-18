// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/07/16 10:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/semantic/types.hpp"


namespace stationeers::ic10 {

    void TypeTable::registerType(CustomType type) {
        std::visit([this]<typename T>(T&& t) {
            types_[t.name] = std::move(t);
        }, type);
    }

    const CustomType* TypeTable::find(const std::string& name) const {
        if (const auto res = types_.find(name); res != types_.end())
            return &res->second;

        return nullptr;
    }

}  // namespace stationeers::ic10
