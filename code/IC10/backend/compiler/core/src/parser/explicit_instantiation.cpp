// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file explicit_instantiation.cpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/01 15:01
 * @if zh
 * @brief 显式模板实例化与"只付一次"的实例化收拢单元
 * @details 本单元集中承担两类必须且只需实例化一次的重型代码：
 *          - 指令类模板的显式实例化（见下方 @c __IMP__ 系列宏），避免各 TU 各自实例化导致膨胀；
 *          - @ref ExecutableInstruction 与 @ref Statement 的复制/移动/赋值/析构及转换构造函数
 *            （在 ast.hpp 中只声明），使每个使用方 TU 退化为一次普通函数调用。
 * @note 实测数据：语句变体覆盖约 200 个指令节点，若其变体机械代码在使用方 TU 内联实例化，
 *       单个 TU 会多出约 60 秒编译时间与约 210 MiB 目标文件；收拢到本单元后，使用方 TU
 *       同样的移动+拷贝+容器操作由 62 秒/212 MiB 降到 14 秒/0.3 MiB。
 * @elseif en
 * @brief Explicit template instantiation and the "pay once" instantiation sink
 * @details This unit hosts the two kinds of heavy code that must be instantiated exactly once:
 *          - explicit instantiation of the instruction class templates (the @c __IMP__ macros
 *            below), so no other TU instantiates them again;
 *          - the copy/move constructors, assignments, destructor and converting constructors of
 *            @ref ExecutableInstruction and @ref Statement (only declared in ast.hpp), reducing
 *            every consumer TU to an ordinary function call.
 * @note Measured: the statement variant covers about 200 instruction nodes; instantiating its
 *       machinery inline in a consumer TU costs roughly 60 seconds of compile time and 210 MiB of
 *       object file, while with the members collected here the same move + copy + container
 *       workload drops from 62 s/212 MiB to 14 s/0.3 MiB.
 * @endif
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/pch/ast.hpp"

namespace stationeers::ic10 {

    // ExecutableInstruction（声明于ast.hpp）

    ExecutableInstruction::ExecutableInstruction(const ExecutableInstruction& other)
        : value_(other.value_) {}

    ExecutableInstruction::ExecutableInstruction(ExecutableInstruction&& other) noexcept(
        std::is_nothrow_move_constructible_v<Variant>
    )
        : value_(std::move(other.value_)) {}

    ExecutableInstruction& ExecutableInstruction::operator=(const ExecutableInstruction& other) {
        value_ = other.value_;

        return *this;
    }

    ExecutableInstruction& ExecutableInstruction::operator=(ExecutableInstruction&& other) noexcept(
        std::is_nothrow_move_assignable_v<Variant>
    ) {
        value_ = std::move(other.value_);

        return *this;
    }

    ExecutableInstruction::~ExecutableInstruction() = default;

    ExecutableInstruction::ExecutableInstruction(Variant&& value) noexcept(
        std::is_nothrow_move_constructible_v<Variant>
    )
        : value_(std::move(value)) {}

    ExecutableInstruction::ExecutableInstruction(ErrorNode&& node) noexcept
        : value_(std::in_place_type<ErrorNode>, std::move(node)) {}

    // Statement（声明于ast.hpp）

    Statement::Statement(const Statement& other) : value_(other.value_) {}

    Statement::Statement(Statement&& other) noexcept(std::is_nothrow_move_constructible_v<Variant>)
        : value_(std::move(other.value_)) {}

    Statement& Statement::operator=(const Statement& other) {
        value_ = other.value_;

        return *this;
    }

    Statement& Statement::operator=(Statement&& other) noexcept(
        std::is_nothrow_move_assignable_v<Variant>
    ) {
        value_ = std::move(other.value_);

        return *this;
    }

    Statement::~Statement() = default;

    Statement::Statement(ExecutableInstruction&& instruction)
        : value_(wide_cast<Variant>(std::move(instruction.raw()))) {}

    Statement::Statement(FirstStatement&& first) : value_(wide_cast<Variant>(std::move(first))) {}

    Statement::Statement(ErrorNode&& node) noexcept
        : value_(std::in_place_type<ErrorNode>, std::move(node)) {}

    // 显示模板实例化，避免实例膨胀导致编译爆内存

#ifdef _MSC_VER
    #define __IMP__(insType, lowerCase, ...) template struct insType<lowerCase, ##__VA_ARGS__>;
#else
    #define __IMP__(insType, lowerCase, ...)                                                       \
        template struct insType<lowerCase __VA_OPT__(, ) __VA_ARGS__>;
#endif

#define __0IMP__(lowerCase, ...) __IMP__(NullaryInstructionBase, lowerCase, __VA_ARGS__)

    __0IMP__("hcf")
    __0IMP__("yield")

#undef __0IMP__

#define __1IMP__(lowerCase, ...) __IMP__(UnaryInstructionBase, lowerCase, __VA_ARGS__)

    __1IMP__("peek", OperandType::REG_TARGET)
    __1IMP__("rand", OperandType::REG_TARGET)
    __1IMP__("pop", OperandType::REG_TARGET)
    __1IMP__("clr", OperandType::DEVICE_REF_STRICT)
    __1IMP__("sleep", OperandType::NUM_VALUE)
    __1IMP__("clrd", OperandType::HARDWARE_ID)
    __1IMP__("push", OperandType::NUM_VALUE)
    __1IMP__("jal", OperandType::JUMP_LINE)
    __1IMP__("jr", OperandType::JUMP_LINE)
    __1IMP__("j", OperandType::JUMP_LINE)

#undef __1IMP__

#define __2IMP__(lowerCase, ...) __IMP__(BinaryInstructionBase, lowerCase, __VA_ARGS__)

    __2IMP__("abs", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("acos", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("asin", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("atan", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("ceil", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("cos", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("exp", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("floor", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("log", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("round", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("sin", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("sqrt", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("tan", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("trunc", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("not", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("move", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("sgn", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("seqz", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("snez", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("sgez", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("sgtz", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("slez", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("sltz", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("snan", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("snanz", OperandType::REG_TARGET, OperandType::NUM_VALUE)
    __2IMP__("bdns", OperandType::DEVICE_REF, OperandType::JUMP_LINE)
    __2IMP__("bdnsal", OperandType::DEVICE_REF, OperandType::JUMP_LINE)
    __2IMP__("bdse", OperandType::DEVICE_REF, OperandType::JUMP_LINE)
    __2IMP__("bdseal", OperandType::DEVICE_REF, OperandType::JUMP_LINE)
    __2IMP__("brdns", OperandType::DEVICE_REF, OperandType::JUMP_LINE)
    __2IMP__("brdse", OperandType::DEVICE_REF, OperandType::JUMP_LINE)
    __2IMP__("sdns", OperandType::REG_TARGET, OperandType::DEVICE_REF)
    __2IMP__("sdse", OperandType::REG_TARGET, OperandType::DEVICE_REF)
    __2IMP__("poke", OperandType::ADDRESS, OperandType::NUM_VALUE)
    __2IMP__("beqz", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("beqzal", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bnez", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bnezal", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bgez", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bgezal", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bgtz", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bgtzal", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("blez", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("blezal", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bltz", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bltzal", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("bnan", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("breqz", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("brnez", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("brgez", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("brgtz", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("brlez", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("brltz", OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __2IMP__("brnan", OperandType::NUM_VALUE, OperandType::JUMP_LINE)

#undef __2IMP__

#define __3IMP__(lowerCase, ...) __IMP__(TernaryInstructionBase, lowerCase, __VA_ARGS__)

    __3IMP__("add", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("atan2", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("div", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("max", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("min", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("mod", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("mul", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("pow", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sub", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("rol", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("ror", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("and", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("nor", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("or", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sla", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sll", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sra", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("srl", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("xor", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sapz", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("snaz", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("seq", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sne", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sge", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sgt", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("sle", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("slt", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)
    __3IMP__("get", OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::ADDRESS)
    __3IMP__(
        "rmap", OperandType::REG_TARGET, OperandType::DEVICE_REF_STRICT, OperandType::REAGENT_HASH
    )
    __3IMP__("put", OperandType::DEVICE_REF, OperandType::ADDRESS, OperandType::NUM_VALUE)
    __3IMP__("l", OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::LOGIC_PROP)
    __3IMP__("s", OperandType::DEVICE_REF, OperandType::LOGIC_PROP, OperandType::NUM_VALUE)
    __3IMP__("sb", OperandType::DEVICE_HASH, OperandType::LOGIC_PROP, OperandType::NUM_VALUE)
    __3IMP__("bdnvl", OperandType::DEVICE_REF, OperandType::LOGIC_PROP, OperandType::JUMP_LINE)
    __3IMP__("bdnvs", OperandType::DEVICE_REF, OperandType::LOGIC_PROP, OperandType::JUMP_LINE)
    __3IMP__("beq", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("beqal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bne", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bneal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bge", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bgeal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bgt", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bgtal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("ble", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bleal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("blt", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bltal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bapz", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bapzal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bnaz", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("bnazal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("breq", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brne", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brge", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brgt", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brle", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brlt", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brapz", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)
    __3IMP__("brnaz", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

#undef __3IMP__

#define __4IMP__(lowerCase, ...) __IMP__(QuaternaryInstructionBase, lowerCase, __VA_ARGS__)

    __4IMP__(
        "clamp", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "lerp", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "ext", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "ins", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "sap", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "sna", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "select", OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "ss", OperandType::DEVICE_REF, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT_PROP,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "lb", OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::LOGIC_PROP,
        OperandType::AGG_MODE
    )
    __4IMP__(
        "sbn", OperandType::DEVICE_HASH, OperandType::NAME_HASH, OperandType::LOGIC_PROP,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "sbs", OperandType::DEVICE_HASH, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT_PROP,
        OperandType::NUM_VALUE
    )
    __4IMP__(
        "bap", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )
    __4IMP__(
        "bapal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )
    __4IMP__(
        "bna", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )
    __4IMP__(
        "bnaal", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )
    __4IMP__(
        "brap", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )
    __4IMP__(
        "brna", OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )
    __4IMP__(
        "ls", OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::SLOT_IDX,
        OperandType::LOGIC_SLOT_PROP
    )
    __4IMP__(
        "lr", OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::REAGENT_MODE,
        OperandType::NUM_VALUE
    )

#undef __4IMP__

#define __5IMP__(lowerCase, ...) __IMP__(QuinaryInstructionBase, lowerCase, __VA_ARGS__)

    __5IMP__(
        "lbn", OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::NAME_HASH,
        OperandType::LOGIC_PROP, OperandType::AGG_MODE
    )
    __5IMP__(
        "lbs", OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::SLOT_IDX,
        OperandType::LOGIC_SLOT_PROP, OperandType::AGG_MODE
    )

#undef __5IMP__

#define __6IMP__(lowerCase, ...) __IMP__(SenaryInstructionBase, lowerCase, __VA_ARGS__)

    __6IMP__(
        "lbns", OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::NAME_HASH,
        OperandType::SLOT_IDX, OperandType::LOGIC_SLOT_PROP, OperandType::AGG_MODE
    )

#undef __6IMP__

#undef __IMP__
}  // namespace stationeers::ic10
