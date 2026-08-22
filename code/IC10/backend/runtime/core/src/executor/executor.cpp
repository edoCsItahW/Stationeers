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
#include "ic10_runtime/locals/local.hpp"
#include "ic10_runtime/value/value.hpp"


namespace stationeers::ic10 {

    Executor::Executor(Context& ctx) noexcept
        : ctx_(ctx) {}

    void Executor::setReporter(DiagnosticReporter<IC10RuntimeMsgPack>* reporter) noexcept {
        reporter_ = reporter;
    }

    bool Executor::execute() {
        auto stmt = ctx_.currentStatement();

        if (!stmt) return false;

        Flag flag = {false, false};

        if (!std::visit(
                [&]<typename T>(T&& ins) -> bool {
                    using U = std::remove_cvref_t<T>;
                    try {
                        if constexpr (
                            std::is_same_v<U, LabelDef> || std::is_same_v<U, AliasDirective>
                            || std::is_same_v<U, DefineDirective>
                            || std::is_same_v<U, EnumDocComment>
                            || std::is_same_v<U, DeviceDocComment>
                        )
                            return false;

                        else if constexpr (std::is_same_v<U, ErrorNode>) {
                            reporter_->report<IRMsgId::IEC4>(
                                ctx_.cfg.allowErrorStatements ? DiagnosticLevel::Warning
                                                         : DiagnosticLevel::Error,
                                ins.start(), ins.end()
                            );

                            return false;
                        }

                        else {
                            executeIns(std::forward<T>(ins), flag);

                            return true;
                        }
                    } catch (const std::exception& e) {
                        reporter_->emplace<IRMsgId::IEE2_1>({RuntimeError(
                            IRLoc::msgFormat<IRMsgId::IEE2_1>(e.what()), ins.start(), ins.end()
                        )});

                        return true;
                    } catch (...) {
                        reporter_->emplace<IRMsgId::IEE3>(
                            {RuntimeError(IRLoc::msgStr<IRMsgId::IEE3>(), ins.start(), ins.end())}
                        );

                        return true;
                    }
                },
                stmt->get()
            )) {
            ctx_.advancePC();

            return true;
        }

        if (!flag.jumped && !flag.halted && !flag.paused) ctx_.advancePC();

        return !flag.halted && !flag.paused;
    }

    std::optional<double> Executor::operandValue(const std::shared_ptr<Symbol>& symbol) {
        if (!symbol || (symbol->type.category != TypeCategory::CONSTANT && !symbol->value))
            return std::nullopt;

        switch (symbol->type.kind) {
            using enum BasicType;
            case INTEGER:
                if (symbol->type.category == TypeCategory::LABEL) break;
                [[fallthrough]];
            case FLOAT: return std::stod(*symbol->value);
            case REGISTER: return ctx_.memory.getReg<double>(*symbol->value);
            default: break;
        }

        switch (symbol->type.category) {
            using enum TypeCategory;
            case LABEL:
                // 符号表里标签存的是行号；作为跳转目标时应在此统一转换为语句地址(pc)。
                if (const auto addr = ctx_.getAddr(std::stoi(*symbol->value)); addr)
                    return static_cast<double>(*addr);
                return std::nullopt;
            case STR_CALL: return strValue(*symbol->value);
            case HASH_CALL: return hashValue(*symbol->value);
            case CONSTANT:
                if (const auto& it = CONSTANTS.find(symbol->name); it != CONSTANTS.end())
                    return it->second;
                return std::nullopt;
            default: return std::nullopt;
        }
    }

    // ========================================================================
    // 非指令类型 no-op
    // ========================================================================

    void Executor::executeIns(const LabelDef&, Flag&) noexcept {}

    void Executor::executeIns(const AliasDirective&, Flag&) noexcept {}

    void Executor::executeIns(const DefineDirective&, Flag&) noexcept {}

    void Executor::executeIns(const EnumDocComment&, Flag&) noexcept {}

    void Executor::executeIns(const DeviceDocComment&, Flag&) noexcept {}

    void Executor::executeIns(const ErrorNode&, Flag&) noexcept {}

}  // namespace stationeers::ic10