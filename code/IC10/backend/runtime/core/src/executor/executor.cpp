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
#include <algorithm>
#include <ranges>
#include <cctype>
#include <array>
#include <string_view>


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
                            || std::is_same_v<U, EnumAnnotation>
                            || std::is_same_v<U, DeviceAnnotation>
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
                // Statement 是语句变体的句柄，取内部变体后才能分派
                stmt->get().raw()
            )) {
            ctx_.advancePC();

            return true;
        }

        if (!flag.jumped && !flag.halted && !flag.paused) ctx_.advancePC();

        return !flag.halted && !flag.paused;
    }

    namespace {

        /**
         * @if zh
         * @brief 从宏调用文本里取出实参
         * @details 符号表把宏常量按源码文本存放（`HASH("Test")`），因此这里剥掉 `名称(` 与结尾的
         *          `)`，再去掉字符串两端的引号。注意：词法阶段写入的空白转义不会在这里还原。
         * @param text 宏调用文本
         * @return 实参文本
         *
         * @else
         * @brief Extract the argument from a macro call text
         * @details The symbol table stores macro constants as source text (`HASH("Test")`), so this
         *          strips `NAME(`, the trailing `)` and the surrounding quotes. Note: whitespace
         *          escapes written by the lexer are not decoded here.
         * @param text Macro call text
         * @return The argument text
         *
         * @endif
         * */
        std::string macroArgument(const std::string& text) {
            const auto open  = text.find('(');
            const auto close = text.rfind(')');

            const auto inner = open == std::string::npos || close <= open
                ? std::string_view{text}
                : std::string_view{text}.substr(open + 1, close - open - 1);

            if (inner.size() >= 2 && inner.front() == '"' && inner.back() == '"')
                return std::string{inner.substr(1, inner.size() - 2)};

            return std::string{inner};
        }

    }  // namespace

    std::optional<double> Executor::operandValue(const std::shared_ptr<Symbol>& symbol) {
        if (!symbol || (symbol->type.category != TypeCategory::CONSTANT && !symbol->value))
            return std::nullopt;

        // 宏常量（`define H HASH("x")` / `define S STR("x")`）必须先于按 kind 的数值分支：
        // 它们的 kind 是数值类型，但存的文本是宏调用（`HASH("x")`），numericText 解不出来
        if (symbol->value)
            switch (symbol->type.category) {
                using enum TypeCategory;
                case HASH_CALL: return hashValue(macroArgument(*symbol->value));
                case STR_CALL: return strValue(macroArgument(*symbol->value));
                default: break;
            }

        switch (symbol->type.kind) {
            using enum BasicType;
            case INTEGER:
                if (symbol->type.category == TypeCategory::LABEL) break;
                [[fallthrough]];
            case FLOAT: return numericText(*symbol->value);
            case REGISTER:
                // 别名指向的仍是寄存器文本（可能是动态寄存器），需再解析一层
                if (symbol->value)
                    if (auto name = resolveRegisterText(*symbol->value); name)
                        return readRegister(*name);

                return std::nullopt;
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
    // 操作数解析辅助
    // ========================================================================

    double Executor::readRegister(const std::string& name) {
        // sp 是独立于寄存器表的栈指针
        if (name == "sp") return ctx_.memory.getSP();

        return ctx_.memory.getReg<double>(name);
    }

    void Executor::writeRegister(const std::string& name, double value) {
        if (name == "sp") {
            ctx_.memory.setSP(value);

            return;
        }

        ctx_.memory.setReg(name, value);
    }

    std::optional<std::string> Executor::resolveRegisterText(const std::string& text) {
        if (text == "ra") return text;

        if (text == "sp") return text;

        if (text.size() > 1 && text.front() == 'r') {
            std::string_view rest = std::string_view(text).substr(1);

            // r?：直接寄存器
            if (std::ranges::all_of(rest, [](unsigned char c) { return std::isdigit(c) != 0; }))
                return text;

            // r<reg>：动态寄存器，内层寄存器的值即目标寄存器编号
            if (auto inner = resolveRegisterText(std::string(rest)); inner) {
                auto index = readRegister(*inner);

                if (auto idx = static_cast<int64_t>(index);
                    static_cast<double>(idx) == index && idx >= 0 && idx <= 17)
                    return std::format("r{}", idx);
            }
        }

        return std::nullopt;
    }

    std::optional<std::string> Executor::resolveDeviceKey(const std::string& text) {
        if (text == "db") return text;

        if (text.size() < 2 || text.front() != 'd') return std::nullopt;

        std::string_view rest = std::string_view(text).substr(1);

        // d? / d?:?：静态端口（带引脚时端口名整体作为键）
        if (std::isdigit(static_cast<unsigned char>(rest.front())) != 0) return text;

        // d<reg>：动态端口，寄存器内容即端口编号（-1 为自身引用）
        if (auto reg = resolveRegisterText(std::string(rest)); reg) {
            auto port = readRegister(*reg);

            if (port == -1.0) return std::string("db");

            if (auto idx = static_cast<int64_t>(port);
                static_cast<double>(idx) == port && idx >= 0 && idx <= 5)
                return std::format("d{}", idx);
        }

        return std::nullopt;
    }

    std::optional<double> Executor::enumValue(const Enum& node) {
        const auto* name  = std::get_if<Identifier>(&node.name);
        const auto* value = std::get_if<Identifier>(&node.value);
        if (!name || !value) return std::nullopt;

        return enumMemberOf(name->value, value->value);
    }

    std::optional<double> Executor::enumMember(const std::string& name) {
        // 与语义阶段的操作数检查保持一致：这些位置的裸成员名分别属于下列枚举
        // （LogicType / LogicSlotType / BatchMode / ReagentMode）
        static constexpr std::array<std::string_view, 4> kOperandEnums{
            "LogicType", "LogicSlotType", "BatchMode", "ReagentMode"
        };

        for (auto enumName : kOperandEnums)
            if (auto value = enumMemberOf(std::string(enumName), name); value) return value;

        return std::nullopt;
    }

    std::optional<double> Executor::enumMemberOf(
        const std::string& enumName, const std::string& member
    ) {
        const CustomType* type = ctx_.types.find(enumName);
        if (!type) return std::nullopt;

        const auto* enumType = std::get_if<EnumAnnotation>(type);
        if (!enumType) return std::nullopt;

        for (const auto& item : enumType->values)
            if (item.name == member) return numericText(item.value);

        return std::nullopt;
    }

    // ========================================================================
    // 非指令类型 no-op
    // ========================================================================

    void Executor::executeIns(const LabelDef&, Flag&) noexcept {}

    void Executor::executeIns(const AliasDirective&, Flag&) noexcept {}

    void Executor::executeIns(const DefineDirective&, Flag&) noexcept {}

    void Executor::executeIns(const EnumAnnotation&, Flag&) noexcept {}

    void Executor::executeIns(const DeviceAnnotation&, Flag&) noexcept {}

    void Executor::executeIns(const ErrorNode&, Flag&) noexcept {}

}  // namespace stationeers::ic10