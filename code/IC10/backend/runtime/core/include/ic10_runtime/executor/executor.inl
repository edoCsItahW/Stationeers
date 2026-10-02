/**
 * @file executor.inl
 * @author edocsitahw
 * @version 1.2
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
                std::optional<double> value = std::nullopt;

                if constexpr (std::is_same_v<U, Identifier>) {
                    if (arg.value == "ra")
                        value = ctx_.memory.getReg<double>("ra");

                    else if (arg.value == "sp")
                        value = ctx_.memory.getSP();

                    else if (auto resolved = ctx_.resolve(arg.value); resolved)
                        value = operandValue(*resolved);

                    // 裸枚举成员名（Sum、Contents、Pressure 等）不是符号表条目，
                    // 语义阶段按枚举成员校验，这里据类型表求值
                    else
                        value = enumMember(arg.value);
                }

                // 寄存器：rr? 的动态编号需先读内层寄存器，故统一走文本解析
                else if constexpr (
                    std::is_same_v<U, DynamicRegister> || std::is_same_v<U, AddressRegister>
                    || std::is_same_v<U, StackPointerRegister>
                    || std::is_same_v<U, GeneralPurposeRegister>
                ) {
                    if (auto name = resolveRegisterText(arg.toString()); name)
                        value = readRegister(*name);
                }

                // 枚举常量：值来自类型表的枚举注解
                else if constexpr (std::is_same_v<U, Enum>)
                    value = enumValue(arg);

                else
                    value = directionValue(arg);

                if (!value && throwError && ctx_.cfg.strictEvaluation)
                    reporter_->errorWith<IRMsgId::IEM2_1>(arg.start(), arg.end(), arg.toString());

                // 宽松求值：求值失败时不上报诊断，以 0 顶替（调试器可关掉严苛求值以避开"无法求值"）
                if (!value && !ctx_.cfg.strictEvaluation) value = 0.0;

                return value;
            },
            std::forward<T>(op)
        );
    }

    template<IsVariant T>
    std::optional<std::string> Executor::targetRegister(T&& op, bool throwError) {
        return std::visit(
            [this, &throwError]<typename O, typename U = std::decay_t<O>>(O&& arg)
                -> std::optional<std::string> {
                std::optional<std::string> name = std::nullopt;

                // 别名：符号表里的 value 仍是寄存器文本（如 r3、rr0、ra）
                if constexpr (std::is_same_v<U, Identifier>) {
                    if (auto symbol = ctx_.resolve(arg.value); symbol) {
                        const auto& resolved = *symbol;

                        if (resolved->type.kind == BasicType::REGISTER && resolved->value)
                            name = resolveRegisterText(*resolved->value);
                    }
                }

                // 寄存器：rr? 的动态编号需先读内层寄存器，故统一走文本解析
                else if constexpr (
                    std::is_same_v<U, DynamicRegister> || std::is_same_v<U, AddressRegister>
                    || std::is_same_v<U, StackPointerRegister>
                    || std::is_same_v<U, GeneralPurposeRegister>
                )
                    name = resolveRegisterText(arg.toString());

                if (!name && throwError)
                    reporter_->errorWith<IRMsgId::IEM2_1>(arg.start(), arg.end(), arg.toString());

                return name;
            },
            std::forward<T>(op)
        );
    }

    template<IsVariant T>
    void Executor::assignRegister(T&& op, double value) {
        // 解析失败时已在 targetRegister 内上报，这里直接跳过写入
        if (auto name = targetRegister(std::forward<T>(op), false); name)
            writeRegister(*name, value);
    }

    template<IsVariant T>
    IDevice* Executor::deviceRef(T&& op) {
        return std::visit(
            [this]<typename O, typename U = std::decay_t<O>>(O&& arg) -> IDevice* {
                std::optional<std::string> text;

                // 设备别名：符号表里的 value 是设备文本（如 d0、db、dr0）
                if constexpr (std::is_same_v<U, Identifier>) {
                    if (auto symbol = ctx_.resolve(arg.value); symbol) {
                        const auto& resolved = *symbol;

                        if (resolved->type.kind == BasicType::DEVICE && resolved->value)
                            text = *resolved->value;
                    }
                }

                else if constexpr (std::is_same_v<U, StaticDevice> || std::is_same_v<U, DynamicDevice>)
                    text = arg.toString();

                if (text)
                    if (auto key = resolveDeviceKey(*text); key) return ctx_.manager.getDevice(*key);

                reporter_->errorWith<IRMsgId::IEM2_1>(arg.start(), arg.end(), arg.toString());

                return nullptr;
            },
            std::forward<T>(op)
        );
    }

    template<IsVariant T>
    std::optional<std::string> Executor::propName(T&& op, bool throwError) {
        return std::visit(
            [this, &throwError]<typename O, typename U = std::decay_t<O>>(O&& arg)
                -> std::optional<std::string> {
                std::optional<std::string> name = std::nullopt;

                if constexpr (std::is_same_v<U, Identifier>)
                    name = arg.value;

                // 数字形式是旧语法：属性名按十进制文本传递
                else if constexpr (
                    std::is_same_v<U, Integer> || std::is_same_v<U, Float>
                    || std::is_same_v<U, HexNumber> || std::is_same_v<U, BinaryNumber>
                ) {
                    if (auto num = numericText(arg.value); num) name = std::format("{}", *num);
                }

                if (!name && throwError)
                    reporter_->errorWith<IRMsgId::IEM2_1>(arg.start(), arg.end(), arg.toString());

                return name;
            },
            std::forward<T>(op)
        );
    }

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_EXECUTOR_INL
