// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:50
 * @if zh
 * @brief IC10抽象语法树顶层定义
 * @details 定义IC10程序的顶层AST结构,包括Program(程序)和Statement(语句)类型。
 *        程序由一系列语句组成,语句可以是可执行指令、标签定义或预处理指令。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 abstract syntax tree top-level definitions
 * @details Defines top-level AST structures for IC10 programs, including Program and Statement types.
 *        A program consists of a series of statements, which can be executable instructions,
 *        label definitions, or preprocessor directives.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef IC10_COMPILER_CORE_AST_HPP
#define IC10_COMPILER_CORE_AST_HPP
#pragma once

#include "ins_nullary.hpp"
#include "ins_unary.hpp"
#include "ins_binary.hpp"
#include "ins_ternary.hpp"
#include "ins_quaternary.hpp"
#include "ins_quinary.hpp"
#include "ins_senary.hpp"
#include "expand_node.hpp"
#include <type_traits>
#include <variant>
#include <vector>

namespace stationeers::ic10 {


    // AliasDirective（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class AliasDirective
     * @brief alias预处理指令节点
     * @details 表示IC10中的alias预处理指令,用于给设备或寄存器起别名
     *
     * @elseif en
     *
     * @class AliasDirective
     * @brief Alias directive node
     * @details Represents an alias preprocessor directive in IC10, used to alias devices or
     * registers
     *
     * @endif
     */
    struct AliasDirective : AST<AliasDirective> {
        static constexpr auto nodeName = "AliasDirective"_fs;

        static constexpr auto keyword = "alias"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::KEYWORD_ALIAS});

        AliasDef identifier;

        RegOrDev registerOrDevice;

        std::optional<TypeHint> typeHint;

        AliasDirective(Pos pos, Identifier id, RegOrDev regOrDev);

        AST_NODE_PRE_DEFINED_METHODS(AliasDirective)
    };

    // DefineDirective（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class DefineDirective
     * @brief define预处理指令节点
     * @details 表示IC10中的define预处理指令,用于定义常量。
     *          其操作数(operand)类型为NumberValue,接受数值字面量(Number)、关键字常量(Constant)、
     *          宏调用(MacroCall)或标识符(Identifier,用于引用其他已定义的常量),
     *          不再接受寄存器或设备等非数值类型。
     *
     * @elseif en
     *
     * @class DefineDirective
     * @brief Define directive node
     * @details Represents a define preprocessor directive in IC10, used to define constants.
     *          Its operand is of type NumberValue, accepting numeric literals (Number),
     *          keyword constants (Constant), macro calls (MacroCall), or identifiers
     *          (Identifier, to reference other defined constants),
     *          but no longer accepting non-numeric types such as registers or devices.
     *
     * @endif
     */
    struct DefineDirective : AST<DefineDirective> {
        static constexpr auto nodeName = "DefineDirective"_fs;

        static constexpr auto keyword = "define"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::KEYWORD_DEFINE});

        ConstDef identifier;

        ConstNum operand;

        std::optional<TypeHint> typeHint;

        DefineDirective(Pos pos, Identifier id, ConstNum op);

        AST_NODE_PRE_DEFINED_METHODS(DefineDirective)
    };

    // PreprocessorDirective

    /**
     * @if zh
     * @brief 预处理指令类型别名
     * @elseif en
     * @brief PreprocessorDirective type alias
     * @endif
     */
    using PreprocessorDirective = ShallowErrorable<AliasDirective, DefineDirective>;


    /**
     * @if zh
     *
     * @brief 语句首部类型别名
     * @details 表示一条语句在“尚未区分是否为指令”时的候选集合：标签定义、预处理指令或类型注解。
     *          与 @ref Statement 不同，这里保持为变体别名：候选项少、类型名短，直接使用变体语义更简洁。
     *
     * @elseif en
     *
     * @brief Statement head type alias
     * @details Candidate set for a statement before it is known to be an instruction: a label
     *          definition, a preprocessor directive or a type annotation. Unlike @ref Statement this
     *          stays a variant alias: few alternatives and short type names make plain variant
     *          semantics simpler.
     *
     * @endif
     */
    using FirstStatement = Errorable<LabelDef, PreprocessorDirective, TypeAnnotation>;

    namespace detail {

        /**
         * @if zh
         *
         * @brief 由类型列表取出对应的变体类型
         * @details 变体的类型参数需要以类型包形式给出，而展平/去重的结果一律是 @c type_list，
         *          本模板负责把这层包装去掉并还原成 @c std::variant。
         * @tparam List 类型列表(@c type_list)
         *
         * @elseif en
         *
         * @brief Obtain the variant type described by a type list
         * @details Variants need a template argument pack, while flattening/deduplicating yields a
         *          plain @c type_list, so this template unwraps it back into @c std::variant.
         * @tparam List The type list (@c type_list)
         *
         * @endif
         *
         * @public @memberof stationeers::ic10::detail
         */
        template<typename List>
        struct variant_of;

        template<typename... Ts>
        struct variant_of<type_list<Ts...>> {
            using type = std::variant<Ts...>;
        };

        /**
         * @if zh
         *
         * @brief 覆盖全部指令节点的元数变体列表
         * @details 指令变体与语句变体共用这份列表：整条链路只需展平并去重一次，
         *          并且新增一种元数变体时只需要改动这里一处。
         *
         * @elseif en
         *
         * @brief Per-arity variant list covering every instruction node
         * @details Shared by the instruction variant and the statement variant, so the chain is
         *          flattened and deduplicated once and adding an arity variant is a single edit.
         *
         * @endif
         *
         * @public @memberof stationeers::ic10::detail
         */
        using InstructionVariants = type_list<
            NullaryInstruction, UnaryInstruction, BinaryInstruction, TernaryInstruction,
            QuaternaryInstruction, QuinaryInstruction, SenaryInstruction
        >;

        /**
         * @if zh
         * @brief 可执行指令变体的扁平类型列表（含唯一的一个 @c ErrorNode）
         * @elseif en
         * @brief Flattened type list of the executable instruction variant (one @c ErrorNode)
         * @endif
         */
        using ExecutableInstructionTypes = typelist::flatten_with_unique_t<
            typelist::concat_t<InstructionVariants, type_list<ErrorNode>>
        >;

        /**
         * @if zh
         * @brief 语句变体的扁平类型列表（指令 + 标签 + 预处理指令 + 类型注解 + @c ErrorNode）
         * @elseif en
         * @brief Flattened type list of the statement variant
         *        (instructions + labels + preprocessor directives + type annotations + @c ErrorNode)
         * @endif
         */
        using StatementTypes = typelist::flatten_with_unique_t<typelist::concat_t<
            InstructionVariants,
            type_list<LabelDef, PreprocessorDirective, TypeAnnotation, ErrorNode>
        >>;

    }  // namespace detail

    /**
     * @if zh
     *
     * @class ExecutableInstruction
     * @brief 可执行指令句柄
     * @details 承载 @ref detail::ExecutableInstructionTypes 描述的扁平变体，但把复制构造、移动构造、
     *          复制赋值、移动赋值与析构**只声明在头文件中、定义在实现单元**（见
     *          explicit_instantiation.cpp）。
     *
     * @note 该变体覆盖全部指令节点（约 200 个候选分支）。若其变体机械代码在每个使用方
     *       TU 中各自实例化，单个 TU 将为此多付出约 60 秒编译时间与约 210 MiB 目标文件
     *       （实测 @c inc_parser.cpp.obj 的 233 MiB 中有 198 MiB 是 COFF 字符串表，即被实例化
     *       函数的符号名，而每个符号名都内嵌了这条 200 余候选的变体类型串）。外置后使用方 TU
     *       只产生一次函数调用：同样的移动+拷贝+容器操作由 62 秒/212 MiB 降到 14 秒/0.3 MiB。
     *
     * @warning 需要变体语义（@c std::visit、@c std::get、@c std::holds_alternative）时，
     *          必须先通过 @ref raw 取得内部变体。
     *
     * @par 使用示例:
     * @code
     * ExecutableInstruction ins;                       // 默认构造为第一个候选指令
     * std::visit([](const auto& node) { return node.toString(); }, ins.raw());
     * @endcode
     *
     * @elseif en
     *
     * @class ExecutableInstruction
     * @brief Handle for an executable instruction
     * @details Holds the flat variant described by @ref detail::ExecutableInstructionTypes, but the
     *          copy/move constructors, copy/move assignments and the destructor are **declared in
     *          the header and defined in an implementation unit** (see explicit_instantiation.cpp).
     *
     * @note The variant covers every instruction node (~200 alternatives). Instantiating its
     *       machinery inside each consumer TU costs about 60 seconds of compile time and 210 MiB of
     *       object file per TU (198 MiB of the measured 233 MiB @c inc_parser.cpp.obj is the COFF
     *       string table, i.e. the symbol names of instantiated functions, each embedding the
     *       200-alternative variant type). With the members out of line a consumer TU only emits a
     *       call: the same move + copy + container workload drops from 62 s/212 MiB to 14 s/0.3 MiB.
     *
     * @warning Use @ref raw to reach the underlying variant when variant semantics
     *          (@c std::visit, @c std::get, @c std::holds_alternative) are required.
     *
     * @par Example:
     * @code
     * ExecutableInstruction ins;                       // defaults to the first alternative
     * std::visit([](const auto& node) { return node.toString(); }, ins.raw());
     * @endcode
     *
     * @endif
     */
    class ExecutableInstruction {
    public:
        /**
         * @if zh
         * @brief 内部扁平变体类型
         * @elseif en
         * @brief Underlying flat variant type
         * @endif
         */
        using Variant = typename detail::variant_of<detail::ExecutableInstructionTypes>::type;

        /**
         * @if zh
         * @brief 默认构造为第一个候选指令
         * @elseif en
         * @brief Default-constructs the first alternative
         * @endif
         */
        ExecutableInstruction() = default;

        ExecutableInstruction(const ExecutableInstruction& other);

        ExecutableInstruction(ExecutableInstruction&& other) noexcept(
            std::is_nothrow_move_constructible_v<Variant>
        );

        ExecutableInstruction& operator=(const ExecutableInstruction& other);

        ExecutableInstruction& operator=(ExecutableInstruction&& other) noexcept(
            std::is_nothrow_move_assignable_v<Variant>
        );

        ~ExecutableInstruction();

        /**
         * @if zh
         *
         * @brief 接管一个变体值
         * @param value 待接管的变体（右值）
         *
         * @elseif en
         *
         * @brief Takes ownership of a variant value
         * @param value Variant to take over (rvalue)
         *
         * @endif
         */
        explicit ExecutableInstruction(Variant&& value) noexcept(
            std::is_nothrow_move_constructible_v<Variant>
        );

        /**
         * @if zh
         *
         * @brief 构造承载错误节点的指令（语法分析错误恢复用）
         * @param node 错误节点
         *
         * @elseif en
         *
         * @brief Builds an instruction carrying an error node (parser error recovery)
         * @param node The error node
         *
         * @endif
         */
        ExecutableInstruction(ErrorNode&& node) noexcept;

        /**
         * @if zh
         *
         * @brief 访问内部变体
         * @return 内部变体的引用
         *
         * @elseif en
         *
         * @brief Access the underlying variant
         * @return Reference to the underlying variant
         *
         * @endif
         */
        [[nodiscard]] Variant& raw() & noexcept { return value_; }

        /// @overload
        [[nodiscard]] const Variant& raw() const& noexcept { return value_; }

    private:
        /**
         * @if zh
         * @brief 内部变体
         * @elseif en
         * @brief Underlying variant
         * @endif
         */
        Variant value_;
    };

    /**
     * @if zh
     *
     * @class Statement
     * @brief 语句句柄
     * @details 语句可以是可执行指令、标签定义、预处理指令或类型注解。与
     *          @ref ExecutableInstruction 同样采取“特殊成员只在实现单元实例化”的策略，
     *          因为语句变体同样覆盖全部指令节点：任何一次语句的复制/移动/容器操作，
     *          若在头文件中就地实例化，都会让使用方 TU 生成数百个内嵌巨型类型名的符号。
     *
     * @note 由此得到的收益不限于编译期：@c Program 的复制、@c std::vector<Statement> 的扩容、
     *       增量编译的语句拼接等都退化为一次普通函数调用。
     *
     * @warning 需要变体语义时必须通过 @ref raw 取得内部变体。
     *
     * @elseif en
     *
     * @class Statement
     * @brief Handle for a statement
     * @details A statement is either an executable instruction, a label definition, a preprocessor
     *          directive or a type annotation. As with @ref ExecutableInstruction the special
     *          members are instantiated only in an implementation unit, because the statement
     *          variant likewise covers every instruction node: instantiating its machinery in the
     *          header would make each consumer TU emit hundreds of symbols embedding the huge type.
     *
     * @note The benefit is not limited to compile time: copying a @c Program, growing a
     *       @c std::vector<Statement> and splicing statements during incremental compilation all
     *       collapse into an ordinary function call.
     *
     * @warning Use @ref raw to reach the underlying variant when variant semantics are required.
     *
     * @endif
     */
    class Statement {
    public:
        /**
         * @if zh
         * @brief 内部扁平变体类型
         * @elseif en
         * @brief Underlying flat variant type
         * @endif
         */
        using Variant = typename detail::variant_of<detail::StatementTypes>::type;

        Statement() = default;

        Statement(const Statement& other);

        Statement(Statement&& other) noexcept(std::is_nothrow_move_constructible_v<Variant>);

        Statement& operator=(const Statement& other);

        Statement& operator=(Statement&& other) noexcept(std::is_nothrow_move_assignable_v<Variant>);

        ~Statement();

        /**
         * @if zh
         *
         * @brief 由可执行指令构造语句
         * @param instruction 可执行指令
         *
         * @elseif en
         *
         * @brief Builds a statement from an executable instruction
         * @param instruction The executable instruction
         *
         * @endif
         */
        Statement(ExecutableInstruction&& instruction);

        /**
         * @if zh
         *
         * @brief 由“语句首部”（标签/预处理指令/类型注解）构造语句
         * @param first 语句首部变体（@ref FirstStatement）
         *
         * @elseif en
         *
         * @brief Builds a statement from a statement head (label/directive/type annotation)
         * @param first Statement head variant (@ref FirstStatement)
         *
         * @endif
         */
        Statement(FirstStatement&& first);

        /**
         * @if zh
         *
         * @brief 构造承载错误节点的语句（语法分析错误恢复用）
         * @param node 错误节点
         *
         * @elseif en
         *
         * @brief Builds a statement carrying an error node (parser error recovery)
         * @param node The error node
         *
         * @endif
         */
        Statement(ErrorNode&& node) noexcept;

        /**
         * @if zh
         *
         * @brief 访问内部变体
         * @return 内部变体的引用
         *
         * @elseif en
         *
         * @brief Access the underlying variant
         * @return Reference to the underlying variant
         *
         * @endif
         */
        [[nodiscard]] Variant& raw() & noexcept { return value_; }

        /// @overload
        [[nodiscard]] const Variant& raw() const& noexcept { return value_; }

    private:
        /**
         * @if zh
         * @brief 内部变体
         * @elseif en
         * @brief Underlying variant
         * @endif
         */
        Variant value_;
    };

    // Program（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Program
     * @brief IC10程序节点
     * @details 表示完整的IC10程序,是AST的根节点
     *
     * @elseif en
     *
     * @class Program
     * @brief IC10 program node
     * @details Represents a complete IC10 program, the root node of AST
     *
     * @endif
     */
    struct Program : AST<Program> {
        /**
         * @if zh
         * @brief 节点名称
         * @elseif en
         * @brief Node name
         * @endif
         */
        static constexpr auto nodeName = "Program"_fs;

        /**
         * @if zh
         * @brief 程序语句列表
         * @elseif en
         * @brief Program statement list
         * @endif
         */
        std::vector<Statement> statements;

        /**
         * @if zh
         * @brief 使用基类构造函数
         * @elseif en
         * @brief Use base class constructor
         * @endif
         */
        using AST::AST;

        /**
         * @if zh
         * @brief 获取结束位置
         * @return 程序结束位置
         * @elseif en
         * @brief Get end position
         * @return Program end position
         * @endif
         */
        [[nodiscard]] Pos end() const override;

        /**
         * @if zh
         * @brief 转换为字符串表示
         * @return 程序的可读字符串表示
         * @elseif en
         * @brief Convert to string representation
         * @return Human-readable string representation of program
         * @endif
         */
        [[nodiscard]] std::string toString() const override;

        /**
         * @if zh
         * @brief 转换为JSON格式
         * @return 程序的JSON格式字符串
         * @elseif en
         * @brief Convert to JSON format
         * @return JSON format string of program
         * @endif
         */
        [[nodiscard]] std::string toJSON() const override;
    };

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_AST_HPP
