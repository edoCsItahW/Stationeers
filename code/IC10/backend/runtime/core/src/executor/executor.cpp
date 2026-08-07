// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file executor.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:55
 * @brief IC10 Executor implementation.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/executor/executor.hpp"
#include "ic10_runtime/value/value.hpp"
#include "ic10_runtime/locals/local.hpp"

namespace stationeers::ic10 {

    Executor::Executor(Context& ctx)
        : ctx_(ctx) {}

    bool Executor::execute() {
        auto stmt = ctx_.currentStatement();

        if (!stmt) return false;

        if (  //
            std::holds_alternative<LabelDef>(stmt->get())
            || std::holds_alternative<AliasDirective>(stmt->get())
            || std::holds_alternative<DefineDirective>(stmt->get())
            || std::holds_alternative<EnumDocComment>(stmt->get())
            || std::holds_alternative<DeviceDocComment>(stmt->get())
            || std::holds_alternative<ErrorNode>(stmt->get())
        ) {
            ctx_.advancePC();

            return true;
        }

        Flag flag = {false, false};

        std::visit(
            [&]<typename T>(T&& ins) {
                try {
                    executeIns(std::forward<T>(ins), flag);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << " - when execute ins: " << ins.toString() << std::endl;
                }
            },
            stmt->get()
        );

        if (!flag.jumped && !flag.halted && !flag.paused) ctx_.advancePC();

        return !flag.halted;
    }

    std::optional<double> Executor::operandValue(const std::shared_ptr<Symbol>& symbol) {
        switch (symbol->type.kind) {
            using enum BasicType;
            case INTEGER:
            case FLOAT:
                if (symbol->value) return std::stod(*symbol->value);
                break;
            case REGISTER: return ctx_.memory.getReg(*symbol->value);
            default:
                break;
        }

        switch (symbol->type.category) {
            using enum TypeCategory;
            case LABEL:
                if (symbol->value) return std::stoi(*symbol->value);
            case STR_CALL:
                if (symbol->value) return strValue(*symbol->value);
            case HASH_CALL:
                if (symbol->value) return hashValue(*symbol->value);
            case CONSTANT:
                if (const auto& it = CONSTANTS.find(symbol->name); it != CONSTANTS.end())
                    return it->second;
            default: return std::nullopt;
        }
    }

    // ========================================================================
    // 非指令类型 no-op
    // ========================================================================

    void Executor::executeIns(const LabelDef&, Flag&) {}

    void Executor::executeIns(const AliasDirective&, Flag&) {}

    void Executor::executeIns(const DefineDirective&, Flag&) {}

    void Executor::executeIns(const EnumDocComment&, Flag&) {}

    void Executor::executeIns(const DeviceDocComment&, Flag&) {}

    void Executor::executeIns(const ErrorNode&, Flag&) {}

}  // namespace stationeers::ic10