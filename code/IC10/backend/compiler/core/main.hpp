// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file main.hpp
 * @author edocsitahw
 * @version 2.0
 * @date 2026/06/24
 * @if zh
 * @mainpage IC10 编译器项目文档
 *
 * @section project_overview 项目概述
 *
 * IC10 是一个面向 **Stationeers** 游戏内 IC10 芯片的**编译器**项目。
 * 它将类汇编语言的 IC10 源代码编译为游戏可执行的字节码格式。
 *
 * @section what_is_ic10 什么是 IC10？
 *
 * IC10 是 Stationeers 游戏中的集成电路(Integrated Circuit)芯片规格。
 * 玩家可以通过编写 IC10 程序来控制游戏中的各种设备和系统，实现自动化。
 *
 * IC10 程序具有以下特点：
 * - **类汇编语言**：使用简短的指令名称，如 `add`, `move`, `yield` 等
 * - **寄存器架构**：提供 16 个通用寄存器 (r0-r15)
 * - **设备交互**：通过 `@Device` 语法访问和控制游戏中的设备
 * - **有限资源**：受限于游戏引擎，支持的操作和精度有限
 *
 * @par 示例 IC10 程序:
 * @code
 * # 这是一个简单的 IC10 程序示例
 * alias myDisplay d0
 *
 * main:
 *     move r0 0          # 将 r0 设置为 0
 *     move r1 1          # 将 r1 设置为 1
 *     add r2 r0 r1       # r2 = r0 + r1
 *     s myDisplay r3 r2  # 设置显示
 *     yield              # 让出执行权
 *     jal main           # 跳转到 main 标签
 * @endcode
 *
 * @section architecture 系统架构
 *
 * IC10 编译器采用经典的**三阶段编译流程**：
 *
 * ```mermaid
 * graph LR
 *     A[源代码] --> B[词法分析<br/>Lexer]
 *     B --> C[Token序列]
 *     C --> D[语法分析<br/>Parser]
 *     D --> E[AST<br/>抽象语法树]
 *     E --> F[语义分析<br/>Analyser]
 *     F --> G[符号表<br/>SymbolTable]
 *     F --> H[错误报告<br/>ErrorList]
 *     G --> I{分析完成?}
 *     I -->|是| J[编译产物]
 *     I -->|否| F
 * ```
 *
 * ### 1. 词法分析 (Lexical Analysis)
 *
 * 词法分析器([Lexer](@ref stationeers::ic10::Lexer))负责将源代码字符串分解为词法标记(Token)序列。
 *
 * **职责**：
 * - 识别源代码中的各种词法单元
 * - 过滤空白字符和注释
 * - 报告词法错误(如非法字符)
 * - 记录每个 Token 的位置信息(行号、列号、偏移量)
 *
 * **支持的语言元素**：
 * | 类别 | 示例 | 说明 |
 * |:-----|:-----|:-----|
 * | 指令关键字 | `add`, `move`, `yield` | 操作的助记符 |
 * | 整数 | `42`, `-7` | 十进制整数 |
 * | 浮点数 | `3.14`, `-2.5` | 带小数点的数 |
 * | 十六进制 | `0xFF`, `0X1A` | 以 0x/0X 开头 |
 * | 二进制 | `0b1010`, `0B0011` | 以 0b/0B 开头 |
 * | 寄存器 | `r0`, `r15` | r0 到 r15 |
 * | 设备引用 | `@Display`, `@Sensor` | @ 开头的设备名 |
 * | 字符串 | `"Hello"` | 双引号包围 |
 * | 标识符 | `main`, `_temp` | 字母或下划线开头 |
 * | 标签 | `loop:` | 标识符后跟冒号 |
 * | 符号 | `(`, `)`, `:` | 括号和冒号 |
 *
 * **输出**：[Token](@ref stationeers::ic10::Token) 序列
 *
 * ### 2. 语法分析 (Syntax Analysis)
 *
 * 语法分析器([Parser](@ref stationeers::ic10::Parser))将 Token 序列解析为**抽象语法树(AST)**。
 *
 * **特点**：
 * - 采用**递归下降**解析算法
 * - 支持**错误恢复**：遇到语法错误时跳过当前 Token 继续解析
 * - 生成的 AST 包含错误节点(ErrorNode)以便继续分析
 *
 * **AST 节点类型层次**：
 * ```mermaid
 * classDiagram
 *     class AST~T~ {
 *         <<template>>
 *         +Pos position
 *         +toString() string
 *         +toJSON() string
 *     }
 *     class Program {
 *         +vector~Statement~ statements
 *     }
 *     class ErrorNode {
 *         +Token token
 *         +string message
 *     }
 *     class LabelDef {
 *         +Identifier identifier
 *     }
 *     class NullaryInstruction {
 *         +tuple args
 *     }
 *     class UnaryInstruction {
 *         +O1 operand1
 *     }
 *     class BinaryInstruction {
 *         +O1 operand1
 *         +O2 operand2
 *     }
 *     class TernaryInstruction {
 *         +O1 operand1
 *         +O2 operand2
 *         +O3 operand3
 *     }
 *     class QuaternaryInstruction
 *     class QuinaryInstruction
 *     class SenaryInstruction
 *
 *     AST <|-- Program
 *     AST <|-- ErrorNode
 *     AST <|-- LabelDef
 *     AST <|-- NullaryInstruction
 *     AST <|-- UnaryInstruction
 *     AST <|-- BinaryInstruction
 *     AST <|-- TernaryInstruction
 *     AST <|-- QuaternaryInstruction
 *     AST <|-- QuinaryInstruction
 *     AST <|-- SenaryInstruction
 * ```
 *
 * **指令分类**：
 * | 类型 | 操作数个数 | 示例指令 |
 * |:-----|:----------|:---------|
 * | 零元 | 0 | `yield`, `hcf` |
 * | 一元 | 1 | `snanz`, `peek`, `rand`, `sleep` |
 * | 二元 | 2 | `move`, `abs`, `acos` |
 * | 三元 | 3 | `add`, `sub`, `mul`, `div`, `sap` |
 * | 四元 | 4 | `lerp`, `ext`, `ins` |
 * | 五元 | 5 | `lbn`, `lbs` |
 * | 六元 | 6 | `lbns` |
 *
 * **预处理指令**：
 * - `alias name device` - 为设备定义别名
 * - `define name value` - 定义常量
 *
 * **输出**：[Program](@ref stationeers::ic10::Program) AST 节点
 *
 * ### 3. 语义分析 (Semantic Analysis)
 *
 * 语义分析器([Analyser](@ref stationeers::ic10::Analyser))对 AST 进行**类型检查**和**符号解析**。
 *
 * **职责**：
 * - **符号解析**：解析标识符和引用的实际位置
 * - **类型检查**：验证操作数和指令的类型兼容性
 * - **重定义检测**：检测重复定义的符号
 * - **前向引用处理**：支持在定义之前引用标签
 *
 * **符号表**([SymbolTable](@ref stationeers::ic10::SymbolTable))：
 * - 存储所有已定义符号的信息
 * - 使用 **Future/Promise 模式**处理前向引用
 * - 支持异步符号解析
 *
 * **类型系统**([Type](@ref stationeers::ic10::Type))：
 * | 类型 | 说明 | 示例 |
 * |:-----|:-----|:-----|
 * | `INTEGER` | 整数 | `42`, `0xFF` |
 * | `FLOAT` | 浮点数 | `3.14` |
 * | `STRING` | 字符串 | `"text"` |
 * | `REGISTER` | 寄存器 | `r0` |
 * | `DEVICE` | 设备 | `@Display` |
 * | `ENUM` | 枚举 | 自定义枚举值 |
 * | `UNKNOWN` | 未知/错误 | - |
 *
 * **错误处理**：
 * - [SyntaxError](@ref stationeers::ic10::SyntaxError) - 语法错误
 * - [TypeError](@ref stationeers::ic10::TypeError) - 类型错误
 * - [RedefinitionError](@ref stationeers::ic10::RedefinitionError) - 重定义错误
 * - [UndefinedSymbolError](@ref stationeers::ic10::UndefinedSymbolError) - 未定义符号错误
 * - [RuntimeError](@ref stationeers::ic10::RuntimeError) - 运行时错误
 *
 * **输出**：
 * - 更新后的 AST
 * - 符号表
 * - 错误列表
 *
 * @section code_structure 代码结构
 *
 * @code
 * ic10/
 * ├── include/
 * │   └── ic10/
 * │       ├── main.hpp          # 本文档(项目主文档)
 * │       ├── lexer/             # 词法分析
 * │       │   ├── lexer.hpp      # 词法分析器
 * │       │   └── token.hpp      # Token 定义
 * │       ├── parser/            # 语法分析
 * │       │   ├── parser.hpp      # 语法分析器
 * │       │   ├── doc_parser.hpp  # 文档注释解析器
 * │       │   └── ast/            # AST 定义
 * │       │       ├── ast.hpp              # Program 节点
 * │       │       ├── ast_node.hpp         # 基础节点和 ErrorNode
 * │       │       ├── ast_nullary_ins.hpp  # 零元指令
 * │       │       ├── ast_unary_ins.hpp    # 一元指令
 * │       │       ├── ast_binary_ins.hpp   # 二元指令
 * │       │       ├── ast_ternary_ins.hpp # 三元指令
 * │       │       ├── ast_quaternary_ins.hpp# 四元指令
 * │       │       ├── ast_quinary_ins.hpp  # 五元指令
 * │       │       └── ast_senary_ins.hpp   # 六元指令
 * │       ├── semantic/           # 语义分析
 * │       │   ├── semantic.hpp     # 基础类型和符号表
 * │       │   └── analyser.hpp    # 语义分析器
 * │       ├── linker/            # 链接器
 * │       │   └── linker.hpp      # 跨编译单元符号链接
 * │       ├── incremental/       # 增量编译
 * │       │   ├── inc_lexer.hpp   # 增量词法分析
 * │       │   └── inc_parser.hpp  # 增量语法分析
 * │       ├── locals/             # 本地化
 * │       │   ├── local.hpp       # 本地化管理器
 * │       │   └── languages/      # 语言定义
 * │       │       ├── zh_hans.hpp  # 简体中文
 * │       │       └── en_us.hpp    # 美式英语
 * │       └── pch/               # 预编译头
 * │           └── ast.hpp
 * │
 * exports/                        # Node.js 绑定
 * └── IC10/
 *     └── node/
 *         ├── lexer_adapter.hpp
 *         ├── parser_adapter.hpp
 *         ├── ast_adapter.hpp
 *         ├── analyser_adapter.hpp
 *         ├── symbol_table_adapter.hpp
 *         └── local_adapter.hpp
 *
 * common/                         # 公共模块
 * ├── include/
 * │   └── common/
 * │       ├── utils/              # 工具函数
 * │       │   ├── position.hpp    # 位置信息
 * │       │   ├── common.hpp      # 通用工具
 * │       │   ├── fstr.hpp        # 格式化字符串
 * │       │   ├── type_list.hpp   # 类型列表
 * │       │   ├── type_map.hpp    # 类型映射
 * │       │   └── bidirectional_map.hpp
 * │       ├── exception/         # 异常处理
 * │       │   ├── error.hpp       # 错误类型
 * │       │   └── debug.hpp       # 调试工具
 * │       ├── async/              # 异步协程
 * │       │   ├── task.hpp        # Task 封装
 * │       │   ├── future.hpp      # Future
 * │       │   ├── promise.hpp     # Promise
 * │       │   ├── coroutine_state.hpp
 * │       │   └── shared_state.hpp
 * │       └── locals/             # 公共本地化
 * │
 * exports/
 *     └── common/
 *         └── node/              # Node.js 绑定
 *             ├── base.hpp       # 基础工具
 *             ├── pos_adapter.hpp
 *             └── error_adapter.hpp
 * @endcode
 *
 * @section key_design 设计要点
 *
 * ### 模板元编程
 *
 * 项目大量使用 C++ 模板元编程技术，主要用于：
 *
 * 1. **类型安全的 AST 节点**
 *    - 使用 CRTP (Curiously Recurring Template Pattern) 模式
 *    - 每个节点类型继承自 `AST<T>` 模板
 *    - 编译期确定节点名称和类型
 *
 * 2. **指令模板自动生成**
 *    - 使用宏 `DEFINE_*_INSTRUCTION` 自动生成指令类型
 *    - TypeMap 在编译期建立 TokenType 到指令类型的映射
 *    - 减少代码重复，提高类型安全
 *
 * 3. **类型列表** ([type_list.hpp](@ref stationeers::type_list))
 *    - 实现编译期类型容器
 *    - 支持类型拼接、过滤、去重等操作
 *    - 用于 Errorable 类型和 variant 操作
 *
 * ### 错误处理
 *
 * 项目采用**错误包容**的设计理念：
 * - 语法/语义错误不会导致整个编译失败
 * - 错误以 ErrorNode 形式嵌入 AST
 * - 允许后续阶段继续分析有效部分
 * - 最终生成包含错误的报告
 *
 * ### 协程与异步
 *
 * 语义分析使用 C++20 协程实现异步处理：
 * - [Task](@ref stationeers::Task) - 协程任务封装
 * - [Future/Promise](@ref stationeers::Future) - 异步值
 * - 支持前向引用的异步解析
 * - 避免阻塞等待
 *
 * ### 跨语言绑定
 *
 * 通过 **Node.js N-API** 实现跨语言调用：
 * - C++ 核心逻辑独立于平台
 * - 适配器模式桥接 C++ 和 JavaScript
 * - 支持 Node.js 环境中的 IC10 编译
 *
 * @section usage 使用指南
 *
 * ### C++ 嵌入使用
 *
 * @code{.cpp}
 * #include "ic10_compiler/lexer/lexer.hpp"
 * #include "ic10_compiler/parser/parser.hpp"
 * #include "ic10_compiler/semantic/analyser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 1. 源代码
 * std::string_view source = R"(
 *     alias disp d0
 *     main:
 *         move r0 42
 *         yield
 *         jal main
 * )";
 *
 * // 2. 词法分析
 * auto tokens = Lexer::tokenize(source);
 *
 * // 3. 语法分析
 * Program program = Parser::parsing(tokens);
 *
 * // 4. 语义分析(协程)
 * co_await Analyser::analyse(program);
 *
 * // 5. 检查错误
 * auto errors = analyser.getErrors();
 * if (!errors.empty()) {
 *     for (const auto& err : errors) {
 *         std::cout << err.toString() << std::endl;
 *     }
 * }
 * @endcode
 *
 * ### Node.js 使用
 *
 * @code{.javascript}
 * const ic10 = require('ic10-compiler');
 *
 * const source = `
 *     alias disp d0
 *     main:
 *         move r0 42
 *         yield
 *         jal main
 * `;
 *
 * // 编译
 * const result = ic10.compile(source);
 *
 * if (result.errors.length > 0) {
 *     console.log('Errors:', result.errors);
 * } else {
 *     console.log('Compiled successfully');
 *     console.log('AST:', result.ast);
 * }
 * @endcode
 *
 * @section extension 扩展指南
 *
 * ### 添加新指令
 *
 * 1. 在对应的指令头文件中添加指令定义
 * 2. 使用 `DEFINE_*_INSTRUCTION` 宏注册
 * 3. 在 TokenType 枚举中添加关键字
 *
 * @code{.cpp}
 * // ast_ternary_ins.hpp
 * DEFINE_TERNARY_INSTRUCTION(UPPER, Pascal, UPPER, ic10::TernaryInstructionBase,
 *     RegisterOrDevice, Operand, Operand)
 * @endcode
 *
 * ### 添加新 AST 节点
 *
 * 1. 在 [ast_node.hpp](@ref stationeers::ic10::ast_node) 中定义节点结构
 * 2. 在对应的 .inl 文件中实现方法
 * 3. 如果需要，在 Parser 中添加解析逻辑
 *
 * ### 添加新错误类型
 *
 * 在 [error.hpp](@ref stationeers::Error) 中：
 * @code{.cpp}
 * struct NewError : ErrorBase<NewError> {
 *     static constexpr auto id = MsgId::NEW_ERROR;
 *     // ...
 * };
 * @endcode
 *
 * ### 添加新语言支持
 *
 * 1. 在 `locals/languages/` 下创建新的语言文件
 * 2. 定义语言类型和消息映射
 * 3. 使用 `LOCAL_MSG_*` 宏注册消息
 *
 * @section dependencies 依赖关系
 *
 * ```mermaid
 * graph TD
 *     UserCode[用户代码] --> Lexer
 *     Lexer --> Token[Token序列]
 *     Token --> Parser
 *     Parser --> AST[AST树]
 *     AST --> Analyser
 *     Analyser --> SymbolTable
 *     Analyser --> ErrorList
 *     SymbolTable --> FuturePromise[Future/Promise]
 *     Analyser -.-> FuturePromise
 *
 *     Lexer --> Pos[Pos位置]
 *     Parser --> Pos
 *     Analyser --> Pos
 *
 *     Analyser --> Type[Type系统]
 *     Type --> TypeList[TypeList]
 *
 *     Task --> CoroutineState
 *     FuturePromise --> SharedState
 * ```
 *
 * @section subpages 专题文档

 * 以下是 IC10 编译器各核心模块的深入专题文档：
 *
 * @subsection lexer_page 词法分析器 (Lexer)
 * @subpage ic10_lexer_page
 * 深入探讨词法分析的状态机实现、Token 类型体系、注释处理机制。
 *
 * @subsection parser_page 语法分析器 (Parser)
 * @subpage ic10_parser_page
 * 详细介绍递归下降解析算法、错误恢复策略、预处理指令解析。
 *
 * @subsection ast_page 抽象语法树 (AST)
 * @subpage ic10_ast_page
 * 剖析 AST 节点设计、模板元编程实现、指令类型系统。
 *
 * @subsection semantic_page 语义分析 (Semantic)
 * @subpage ic10_semantic_page
 * 详解符号表管理、类型检查、前向引用处理的 Future/Promise 模式。
 *
 * @subsection linker_page 链接器 (Linker)
 * @subpage ic10_linker_page
 * 介绍跨编译单元的符号合并、诊断信息聚合、链接策略。
 *
 * @subsection incremental_page 增量编译 (Incremental)
 * @subpage ic10_incremental_page
 * 深入分析增量词法/语法分析的缓存策略和性能优化。
 *
 * @subsection locals_page 本地化 (Locals)
 * @subpage ic10_locals_page
 * 说明多语言诊断消息系统、消息 ID 体系、本地化扩展机制。
 *
 * @section glossary 术语表

 * | 术语 | 英文 | 说明 |
 * |:-----|:-----|:-----|
 * | 词法分析 | Lexical Analysis | 将源代码分解为 Token 序列 |
 * | 语法分析 | Syntax Analysis | 将 Token 序列解析为 AST |
 * | 语义分析 | Semantic Analysis | 验证含义，检查类型和引用 |
 * | 抽象语法树 | AST | 程序的树形表示 |
 * | 词法标记 | Token | 词法分析的最小单元 |
 * | 符号表 | Symbol Table | 存储变量/函数等符号信息 |
 * | 前向引用 | Forward Reference | 在定义前引用 |
 * | CRTP | CRTP | 奇异递归模板模式 |
 * | variant | variant | 类型联合体 |
 * | 协程 | Coroutine | 可暂停/恢复的函数 |
 *
 * @section license 许可证
 *
 * 本项目采用 **CC BY-NC-SA** (Creative Commons Attribution-NonCommercial-ShareAlike) 许可证。
 *
 * - **BY**: 必须给出适当的署名
 * - **NC**: 不得将本作品用于商业目的
 * - **SA**: 以相同方式共享
 *
 * @section contact 联系方式
 *
 * 如有问题或建议，请联系作者：
 * - 邮箱: 2207150234@st.sziit.edu.cn
 *
 * @elseif en
 * @mainpage IC10 Compiler Project Documentation
 *
 * @section project_overview Project Overview
 *
 * IC10 is a **compiler** project for the IC10 chip in **Stationeers** game.
 * It compiles assembly-like IC10 source code into executable bytecode for the game.
 *
 * @section what_is_ic10 What is IC10?
 *
 * IC10 is the Integrated Circuit chip specification in Stationeers.
 * Players can write IC10 programs to control various devices and systems in the game,
 * enabling automation.
 *
 * IC10 programs have these characteristics:
 * - **Assembly-like language**: Short instruction mnemonics like `add`, `move`, `yield`
 * - **Register architecture**: 16 general-purpose registers (r0-r15)
 * - **Device interaction**: Access devices via `@Device` syntax
 * - **Limited resources**: Limited operations and precision due to game engine
 *
 * @par Example IC10 Program:
 * @code
 * # This is a simple IC10 program example
 * alias myDisplay d0
 *
 * main:
 *     move r0 0          # Set r0 to 0
 *     move r1 1          # Set r1 to 1
 *     add r2 r0 r1       # r2 = r0 + r1
 *     s myDisplay r3 r2   # Set display
 *     yield              # Yield execution
 *     jal main           # Jump to main label
 * @endcode
 *
 * @section architecture System Architecture
 *
 * IC10 compiler uses the classic **three-phase compilation process**:
 *
 * ```mermaid
 * graph LR
 *     A[Source Code] --> B[Lexical Analysis<br/>Lexer]
 *     B --> C[Token Sequence]
 *     C --> D[Syntax Analysis<br/>Parser]
 *     D --> E[AST<br/>Abstract Syntax Tree]
 *     E --> F[Semantic Analysis<br/>Analyser]
 *     F --> G[SymbolTable<br/>SymbolTable]
 *     F --> H[ErrorReport<br/>ErrorList]
 *     G --> I{Analysis Complete?}
 *     I -->|Yes| J[Compiled Output]
 *     I -->|No| F
 * ```
 *
 * ### 1. Lexical Analysis
 *
 * The [Lexer](@ref stationeers::ic10::Lexer) decomposes source code string into a sequence of lexical tokens.
 *
 * **Responsibilities**:
 * - Recognize various lexical units in source code
 * - Filter whitespace and comments
 * - Report lexical errors (invalid characters)
 * - Record position info (line, column, offset) for each Token
 *
 * **Supported Language Elements**:
 * | Category | Example | Description |
 * |:---------|:--------|:------------|
 * | Instruction keyword | `add`, `move`, `yield` | Operation mnemonics |
 * | Integer | `42`, `-7` | Decimal integer |
 * | Float | `3.14`, `-2.5` | Number with decimal point |
 * | Hexadecimal | `0xFF`, `0X1A` | Prefix 0x/0X |
 * | Binary | `0b1010`, `0B0011` | Prefix 0b/0B |
 * | Register | `r0`, `r15` | r0 to r15 |
 * | Device reference | `@Display`, `@Sensor` | Device name starting with @ |
 * | String | `"Hello"` | Enclosed in double quotes |
 * | Identifier | `main`, `_temp` | Letter or underscore prefix |
 * | Label | `loop:` | Identifier followed by colon |
 * | Symbol | `(`, `)`, `:` | Parentheses and colon |
 *
 * **Output**: Sequence of [Token](@ref stationeers::ic10::Token)
 *
 * ### 2. Syntax Analysis
 *
 * The [Parser](@ref stationeers::ic10::Parser) parses Token sequence into **Abstract Syntax Tree (AST)**.
 *
 * **Characteristics**:
 * - Uses **recursive descent** parsing algorithm
 * - Supports **error recovery**: skips current Token and continues on syntax error
 * - Generated AST contains ErrorNode for continued analysis
 *
 * **AST Node Type Hierarchy**:
 * ```mermaid
 * classDiagram
 *     class AST~T~ {
 *         <<template>>
 *         +Pos position
 *         +toString() string
 *         +toJSON() string
 *     }
 *     class Program {
 *         +vector~Statement~ statements
 *     }
 *     class ErrorNode {
 *         +Token token
 *         +string message
 *     }
 *     class LabelDef {
 *         +Identifier identifier
 *     }
 *     class NullaryInstruction {
 *         +tuple args
 *     }
 *     class UnaryInstruction {
 *         +O1 operand1
 *     }
 *     class BinaryInstruction {
 *         +O1 operand1
 *         +O2 operand2
 *     }
 *     class TernaryInstruction {
 *         +O1 operand1
 *         +O2 operand2
 *         +O3 operand3
 *     }
 *     class QuaternaryInstruction
 *     class QuinaryInstruction
 *     class SenaryInstruction
 *
 *     AST <|-- Program
 *     AST <|-- ErrorNode
 *     AST <|-- LabelDef
 *     AST <|-- NullaryInstruction
 *     AST <|-- UnaryInstruction
 *     AST <|-- BinaryInstruction
 *     AST <|-- TernaryInstruction
 *     AST <|-- QuaternaryInstruction
 *     AST <|-- QuinaryInstruction
 *     AST <|-- SenaryInstruction
 * ```
 *
 * **Instruction Classification**:
 * | Type | Operand Count | Example Instructions |
 * |:-----|:--------------|:---------------------|
 * | Nullary | 0 | `yield`, `hcf` |
 * | Unary | 1 | `snanz`, `peek`, `rand`, `sleep` |
 * | Binary | 2 | `move`, `abs`, `acos` |
 * | Ternary | 3 | `add`, `sub`, `mul`, `div`, `sap` |
 * | Quaternary | 4 | `lerp`, `ext`, `ins` |
 * | Quinary | 5 | `lbn`, `lbs` |
 * | Senary | 6 | `lbns` |
 *
 * **Preprocessor Directives**:
 * - `alias name device` - Define alias for device
 * - `define name value` - Define constant
 *
 * **Output**: [Program](@ref stationeers::ic10::Program) AST node
 *
 * ### 3. Semantic Analysis
 *
 * The [Analyser](@ref stationeers::ic10::Analyser) performs **type checking** and **symbol resolution** on AST.
 *
 * **Responsibilities**:
 * - **Symbol Resolution**: Resolve actual positions of identifiers and references
 * - **Type Checking**: Verify operand and instruction type compatibility
 * - **Redefinition Detection**: Detect duplicate symbol definitions
 * - **Forward Reference Handling**: Support referencing labels before definition
 *
 * **SymbolTable**([SymbolTable](@ref stationeers::ic10::SymbolTable)):
 * - Stores information for all defined symbols
 * - Uses **Future/Promise pattern** for forward reference handling
 * - Supports asynchronous symbol resolution
 *
 * **Type System**([Type](@ref stationeers::ic10::Type)):
 * | Type | Description | Example |
 * |:-----|:------------|:--------|
 * | `INTEGER` | Integer | `42`, `0xFF` |
 * | `FLOAT` | Floating point | `3.14` |
 * | `STRING` | String | `"text"` |
 * | `REGISTER` | Register | `r0` |
 * | `DEVICE` | Device | `@Display` |
 * | `ENUM` | Enum | Custom enum values |
 * | `UNKNOWN` | Unknown/Error | - |
 *
 * **Error Handling**:
 * - [SyntaxError](@ref stationeers::ic10::SyntaxError) - Syntax errors
 * - [TypeError](@ref stationeers::ic10::TypeError) - Type errors
 * - [RedefinitionError](@ref stationeers::ic10::RedefinitionError) - Redefinition errors
 * - [UndefinedSymbolError](@ref stationeers::ic10::UndefinedSymbolError) - Undefined symbol errors
 * - [RuntimeError](@ref stationeers::ic10::RuntimeError) - Runtime errors
 *
 * **Output**:
 * - Updated AST
 * - Symbol table
 * - Error list
 *
 * @section code_structure Code Structure
 *
 * @code
 * ic10/
 * ├── include/
 * │   └── ic10/
 * │       ├── main.hpp          # This documentation (project main documentation)
 * │       ├── lexer/             # Lexical analysis
 * │       │   ├── lexer.hpp      # Lexer
 * │       │   └── token.hpp      # Token definitions
 * │       ├── parser/            # Syntax analysis
 * │       │   ├── parser.hpp      # Parser
 * │       │   ├── doc_parser.hpp  # Doc comment parser
 * │       │   └── ast/            # AST definitions
 * │       │       ├── ast.hpp              # Program node
 * │       │       ├── ast_node.hpp         # Base nodes and ErrorNode
 * │       │       ├── ast_nullary_ins.hpp  # Nullary instructions
 * │       │       ├── ast_unary_ins.hpp    # Unary instructions
 * │       │       ├── ast_binary_ins.hpp   # Binary instructions
 * │       │       ├── ast_ternary_ins.hpp # Ternary instructions
 * │       │       ├── ast_quaternary_ins.hpp# Quaternary instructions
 * │       │       ├── ast_quinary_ins.hpp  # Quinary instructions
 * │       │       └── ast_senary_ins.hpp   # Senary instructions
 * │       ├── semantic/           # Semantic analysis
 * │       │   ├── semantic.hpp     # Base types and symbol table
 * │       │   └── analyser.hpp    # Semantic analyser
 * │       ├── linker/            # Linker
 * │       │   └── linker.hpp      # Cross-unit symbol linking
 * │       ├── incremental/       # Incremental compilation
 * │       │   ├── inc_lexer.hpp   # Incremental lexer
 * │       │   └── inc_parser.hpp  # Incremental parser
 * │       ├── locals/             # Localization
 * │       │   ├── local.hpp       # Localization manager
 * │       │   └── languages/      # Language definitions
 * │       │       ├── zh_hans.hpp  # Simplified Chinese
 * │       │       └── en_us.hpp    # American English
 * │       └── pch/               # Precompiled headers
 * │           └── ast.hpp
 * │
 * exports/                        # Node.js bindings
 * └── IC10/
 *     └── node/
 *         ├── lexer_adapter.hpp
 *         ├── parser_adapter.hpp
 *         ├── ast_adapter.hpp
 *         ├── analyser_adapter.hpp
 *         ├── symbol_table_adapter.hpp
 *         └── local_adapter.hpp
 *
 * common/                         # Common module
 * ├── include/
 * │   └── common/
 * │       ├── utils/              # Utility functions
 * │       │   ├── position.hpp    # Position information
 * │       │   ├── common.hpp      # Common utilities
 * │       │   ├── fstr.hpp        # Formatted string
 * │       │   ├── type_list.hpp   # Type list
 * │       │   ├── type_map.hpp    # Type map
 * │       │   └── bidirectional_map.hpp
 * │       ├── exception/         # Exception handling
 * │       │   ├── error.hpp       # Error types
 * │       │   └── debug.hpp       # Debug utilities
 * │       ├── async/              # Async coroutines
 * │       │   ├── task.hpp        # Task wrapper
 * │       │   ├── future.hpp      # Future
 * │       │   ├── promise.hpp     # Promise
 * │       │   ├── coroutine_state.hpp
 * │       │   └── shared_state.hpp
 * │       └── locals/             # Common localization
 * │
 * exports/
 *     └── common/
 *         └── node/              # Node.js bindings
 *             ├── base.hpp       # Base utilities
 *             ├── pos_adapter.hpp
 *             └── error_adapter.hpp
 * @endcode
 *
 * @section key_design Key Design Points
 *
 * ### Template Metaprogramming
 *
 * The project extensively uses C++ template metaprogramming techniques, mainly for:
 *
 * 1. **Type-safe AST Nodes**
 *    - Uses CRTP (Curiously Recurring Template Pattern)
 *    - Each node type inherits from `AST<T>` template
 *    - Node name and type determined at compile time
 *
 * 2. **Automatic Instruction Template Generation**
 *    - Uses `DEFINE_*_INSTRUCTION` macros to auto-generate instruction types
 *    - TypeMap establishes TokenType to instruction type mapping at compile time
 *    - Reduces code duplication, improves type safety
 *
 * 3. **Type List** ([type_list.hpp](@ref stationeers::type_list))
 *    - Implements compile-time type container
 *    - Supports type concatenation, filtering, deduplication
 *    - Used for Errorable types and variant operations
 *
 * ### Error Handling
 *
 * The project adopts an **error-tolerant** design philosophy:
 * - Syntax/semantic errors don't cause entire compilation to fail
 * - Errors are embedded in AST as ErrorNode
 * - Allows subsequent phases to continue analyzing valid parts
 * - Generates error report at the end
 *
 * ### Coroutines and Async
 *
 * Semantic analysis uses C++20 coroutines for async processing:
 * - [Task](@ref stationeers::Task) - Coroutine task wrapper
 * - [Future/Promise](@ref stationeers::Future) - Async values
 * - Supports async resolution of forward references
 * - Avoids blocking waits
 *
 * ### Cross-language Bindings
 *
 * Cross-language calls via **Node.js N-API**:
 * - C++ core logic platform-independent
 * - Adapter pattern bridges C++ and JavaScript
 * - Supports IC10 compilation in Node.js environment
 *
 * @section usage Usage Guide
 *
 * ### C++ Embedding Usage
 *
 * @code{.cpp}
 * #include "ic10_compiler/lexer/lexer.hpp"
 * #include "ic10_compiler/parser/parser.hpp"
 * #include "ic10_compiler/semantic/analyser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 1. Source code
 * std::string_view source = R"(
 *     alias disp d0
 *     main:
 *         move r0 42
 *         yield
 *         jal main
 * )";
 *
 * // 2. Lexical analysis
 * auto tokens = Lexer::tokenize(source);
 *
 * // 3. Syntax analysis
 * Program program = Parser::parsing(tokens);
 *
 * // 4. Semantic analysis (coroutine)
 * co_await Analyser::analyse(program);
 *
 * // 5. Check errors
 * auto errors = analyser.getErrors();
 * if (!errors.empty()) {
 *     for (const auto& err : errors) {
 *         std::cout << err.toString() << std::endl;
 *     }
 * }
 * @endcode
 *
 * ### Node.js Usage
 *
 * @code{.javascript}
 * const ic10 = require('ic10-compiler');
 *
 * const source = `
 *     alias disp d0
 *     main:
 *         move r0 42
 *         yield
 *         jal main
 * `;
 *
 * // Compile
 * const result = ic10.compile(source);
 *
 * if (result.errors.length > 0) {
 *     console.log('Errors:', result.errors);
 * } else {
 *     console.log('Compiled successfully');
 *     console.log('AST:', result.ast);
 * }
 * @endcode
 *
 * @section extension Extension Guide
 *
 * ### Adding New Instructions
 *
 * 1. Add instruction definition in corresponding instruction header file
 * 2. Register using `DEFINE_*_INSTRUCTION` macro
 * 3. Add keyword in TokenType enum
 *
 * @code{.cpp}
 * // ast_ternary_ins.hpp
 * DEFINE_TERNARY_INSTRUCTION(UPPER, Pascal, UPPER, ic10::TernaryInstructionBase,
 *     RegisterOrDevice, Operand, Operand)
 * @endcode
 *
 * ### Adding New AST Nodes
 *
 * 1. Define node structure in [ast_node.hpp](@ref stationeers::ic10::ast_node)
 * 2. Implement methods in corresponding .inl file
 * 3. Add parsing logic in Parser if needed
 *
 * ### Adding New Error Types
 *
 * In [error.hpp](@ref stationeers::Error):
 * @code{.cpp}
 * struct NewError : ErrorBase<NewError> {
 *     static constexpr auto id = MsgId::NEW_ERROR;
 *     // ...
 * };
 * @endcode
 *
 * ### Adding New Language Support
 *
 * 1. Create new language file under `locals/languages/`
 * 2. Define language type and message mappings
 * 3. Register messages using `LOCAL_MSG_*` macros
 *
 * @section dependencies Dependencies
 *
 * ```mermaid
 * graph TD
 *     UserCode[User Code] --> Lexer
 *     Lexer --> Token[Token Sequence]
 *     Token --> Parser
 *     Parser --> AST[AST Tree]
 *     AST --> Analyser
 *     Analyser --> SymbolTable
 *     Analyser --> ErrorList
 *     SymbolTable --> FuturePromise[Future/Promise]
 *     Analyser -.-> FuturePromise
 *
 *     Lexer --> Pos[Pos Position]
 *     Parser --> Pos
 *     Analyser --> Pos
 *
 *     Analyser --> Type[Type System]
 *     Type --> TypeList[TypeList]
 *
 *     Task --> CoroutineState
 *     FuturePromise --> SharedState
 * ```
 *
 * @section subpages Subpages
 *
 * The following are in-depth topic documents for each core module of the IC10 compiler:
 *
 * @subsection lexer_page Lexer
 * @subpage ic10_lexer_page
 * Deep dive into state machine implementation, token type system, and comment handling.
 *
 * @subsection parser_page Parser
 * @subpage ic10_parser_page
 * Detailed introduction to recursive descent parsing, error recovery, and preprocessor directives.
 *
 * @subsection ast_page AST
 * @subpage ic10_ast_page
 * Analysis of AST node design, template metaprogramming, and instruction type system.
 *
 * @subsection semantic_page Semantic Analysis
 * @subpage ic10_semantic_page
 * Detailed explanation of symbol table management, type checking, and Future/Promise pattern for forward references.
 *
 * @subsection linker_page Linker
 * @subpage ic10_linker_page
 * Introduction to cross-unit symbol merging, diagnostic aggregation, and linking strategies.
 *
 * @subsection incremental_page Incremental Compilation
 * @subpage ic10_incremental_page
 * Deep analysis of caching strategies and performance optimization for incremental lexer/parser.
 *
 * @subsection locals_page Localization
 * @subpage ic10_locals_page
 * Description of multi-language diagnostic message system, message ID system, and localization extension mechanism.
 *
 * @section glossary Glossary
 *
 * | Term | Description |
 * |:-----|:------------|
 * | Lexical Analysis | Decomposing source code into Token sequence |
 * | Syntax Analysis | Parsing Token sequence into AST |
 * | Semantic Analysis | Verifying meaning, checking types and references |
 * | AST | Abstract Syntax Tree - tree representation of program |
 * | Token | Smallest unit of lexical analysis |
 * | Symbol Table | Storage for variable/function symbol information |
 * | Forward Reference | Referencing before definition |
 * | CRTP | Curiously Recurring Template Pattern |
 * | variant | Type union |
 * | Coroutine | Function that can pause/resume |
 *
 * @section license License
 *
 * This project uses **CC BY-NC-SA** (Creative Commons Attribution-NonCommercial-ShareAlike) license.
 *
 * - **BY**: Must give appropriate credit
 * - **NC**: Cannot use for commercial purposes
 * - **SA**: Must share under same license
 *
 * @section contact Contact
 *
 * For questions or suggestions, please contact the author:
 * - Email: edocsitahw@qq.com
 *
 * @endif
 */

#ifndef COMPILER_IC10_MAIN_HPP
#define COMPILER_IC10_MAIN_HPP

/**
 * @page ic10_lexer_page Lexer / 词法分析器
 * @tableofcontents
 *
 * @section lexer_intro Introduction / 概述
 *
 * @if zh
 *
 * 词法分析器是 IC10 编译器的**第一阶段**，负责将源代码字符串分解为结构化的词法标记(Token)序列。
 * 本分析器采用**状态机**方式实现，通过字符级别的状态转换来识别不同类型的词法单元。
 *
 * @elseif en
 *
 * The Lexer is the **first phase** of the IC10 compiler, responsible for breaking source code strings into structured token sequences.
 * This analyzer is implemented using a **state machine** approach, recognizing different lexical units through character-level state transitions.
 *
 * @endif
 *
 * @section lexer_architecture Architecture / 架构设计
 *
 * @subsection lexer_state_machine State Machine Implementation / 状态机实现
 *
 * @if zh
 *
 * Lexer 内部通过 `next()` 方法驱动状态机，根据当前字符类型选择不同的提取策略。
 * 核心逻辑位于 `lexer.cpp` 的 `next()` 方法中，采用单字符前瞻的贪心算法：
 *
 * @elseif en
 *
 * The Lexer internally drives the state machine through the `next()` method, selecting different extraction strategies based on the current character type.
 * The core logic is located in the `next()` method of `lexer.cpp`, using a greedy algorithm with single-character lookahead:
 *
 * @endif
 *
 * ```mermaid
 * graph TD
 *     A[Start / 开始] --> B{Char Type? / 字符类型?}
 *     B -->|Newline| C[Return NEWLINE / 返回 NEWLINE]
 *     B -->|$| D[Extract Hex / 提取十六进制数]
 *     B -->|%| E[Extract Binary / 提取二进制数]
 *     B -->|Digit| F[Extract Number / 提取数字]
 *     B -->|"'"| G[Extract String / 提取字符串]
 *     B -->|#| H[Extract # Comment / 提取#注释]
 *     B -->|//| I[Extract // Comment / 提取//注释]
 *     B -->|Symbol| J[Extract Symbol / 提取符号]
 *     B -->|d+digit| K[Extract Device / 提取设备]
 *     B -->|r+digit| L[Extract Register / 提取寄存器]
 *     B -->|Other| M[Extract Identifier / 提取标识符]
 *     C --> N[End / 结束]
 *     D --> N
 *     E --> N
 *     F --> N
 *     G --> N
 *     H --> N
 *     I --> N
 *     J --> N
 *     K --> N
 *     L --> N
 *     M --> N
 * ```
 *
 * @if zh
 *
 * **状态机核心代码片段**：
 *
 * ```cpp
 * Token Lexer::next() {
 *     skip();
 *     const auto start = pos_;
 *     if (!inScope()) return {TokenType::END, start, "", TokenCategory::END};
 *
 *     const auto c = current();
 *     if (*c == '\n') { pos_.newline(); return {TokenType::NEWLINE, start, "\\n", TokenCategory::WHITESPACE}; }
 *     if (*c == '$') return extractHexNumber();
 *     if (*c == '%') return extractBinaryNumber();
 *     if (isAsciiDigit(*c)) return extractNumber();
 *     if (*c == '"') return extractString();
 *     if (*c == '#') return extractHashComment();
 *     if (*c == '/' && peek().value_or(' ') == '/') return extractSlashComment();
 *     if (SYMBOL_MAP.contains(std::string(1, *c))) return extractSymbol();
 *     if (*c == 'd' && isAsciiDigit(peek().value_or(' ')) && isAsciiSpace(peek(2).value_or(' ')))
 *         return extractDevice();
 *     if (*c == 'r' && isAsciiDigit(peek().value_or(' '))) return extractRegister();
 *     return extractIdentifier();
 * }
 * ```
 *
 * **关键设计要点**：
 * - 使用 `peek()` 方法进行字符前瞻，不消费字符
 * - `skip()` 方法跳过空白字符但保留换行符
 * - 设备识别需要额外检查第三个字符是否为空白，避免误识别标识符
 * - 寄存器识别使用双字符检查 (`r` + 数字)
 *
 * @elseif en
 *
 * **State Machine Core Code Snippet**:
 *
 * ```cpp
 * Token Lexer::next() {
 *     skip();
 *     const auto start = pos_;
 *     if (!inScope()) return {TokenType::END, start, "", TokenCategory::END};
 *
 *     const auto c = current();
 *     if (*c == '\n') { pos_.newline(); return {TokenType::NEWLINE, start, "\\n", TokenCategory::WHITESPACE}; }
 *     if (*c == '$') return extractHexNumber();
 *     if (*c == '%') return extractBinaryNumber();
 *     if (isAsciiDigit(*c)) return extractNumber();
 *     if (*c == '"') return extractString();
 *     if (*c == '#') return extractHashComment();
 *     if (*c == '/' && peek().value_or(' ') == '/') return extractSlashComment();
 *     if (SYMBOL_MAP.contains(std::string(1, *c))) return extractSymbol();
 *     if (*c == 'd' && isAsciiDigit(peek().value_or(' ')) && isAsciiSpace(peek(2).value_or(' ')))
 *         return extractDevice();
 *     if (*c == 'r' && isAsciiDigit(peek().value_or(' '))) return extractRegister();
 *     return extractIdentifier();
 * }
 * ```
 *
 * **Key Design Points**:
 * - Uses `peek()` for character lookahead without consuming
 * - `skip()` method skips whitespace but preserves newlines
 * - Device identification requires an additional check that the third character is whitespace
 * - Register identification uses two-character check (`r` + digit)
 *
 * @endif
 *
 * @subsection lexer_token_system Token Type System / Token 类型体系
 *
 * @if zh
 *
 * Token 分为多个类别，通过 [TokenCategory](@ref stationeers::ic10::TokenCategory) 枚举管理。
 * TokenType 枚举包含超过 100 个值，涵盖所有指令关键字、字面量、符号等。
 *
 * @elseif en
 *
 * Tokens are categorized through the [TokenCategory](@ref stationeers::ic10::TokenCategory) enum.
 * The TokenType enum contains over 100 values covering all instruction keywords, literals, symbols, etc.
 *
 * @endif
 *
 * | Category / 类别 | Types / 包含类型 | Description / 说明 |
 * |:---------------|:----------------|:------------------|
 * | KEYWORD | Instruction keywords | `add`, `move`, `yield`, etc. / 指令关键字 |
 * | LITERAL | Integer, float, string | Numeric and string literals / 字面量 |
 * | REGISTER | `r0`-`r15` | General-purpose registers / 寄存器 |
 * | DEVICE | `d0`-`d5` | Device references / 设备 |
 * | IDENTIFIER | Variable and label names | User-defined names / 标识符 |
 * | SYMBOL | `(`, `)`, `:` | Punctuation / 符号 |
 * | COMMENT | `#` and `//` comments | Ignored during parsing / 注释 |
 * | WHITESPACE | Newline character | Line separator / 空白 |
 * | END | End-of-file marker | Termination signal / 结束标记 |
 *
 * @subsection lexer_bimap Bi-directional Mapping / 双向映射
 *
 * @if zh
 *
 * Token 和字符串之间的双向映射通过 `BiMap<TokenType, std::string>` 实现：
 *
 * - `KEYWORD_MAP`: 指令关键字映射 (TokenType ↔ 字符串)
 * - `SYMBOL_MAP`: 符号映射
 *
 * 这种设计允许快速的正向查找（字符串→TokenType）和反向查找（TokenType→字符串）。
 *
 * @elseif en
 *
 * Bi-directional mapping between tokens and strings is implemented via `BiMap<TokenType, std::string>`:
 *
 * - `KEYWORD_MAP`: Instruction keyword mapping (TokenType ↔ string)
 * - `SYMBOL_MAP`: Symbol mapping
 *
 * This design enables fast forward lookup (string→TokenType) and reverse lookup (TokenType→string).
 *
 * @endif
 *
 * @section lexer_features Core Features / 核心特性
 *
 * @subsection lexer_hex_binary Multi-base Number Support / 多进制数字支持
 *
 * @if zh
 *
 * Lexer 支持四种数字格式，每种格式有独特的前缀标识：
 *
 * **十六进制**：以 `$` 开头，支持 0-9 和 A-F（大小写不敏感）
 * ```cpp
 * // 示例: $FF, $1A, $abcdef
 * Token extractHexNumber() {
 *     pos_.next(); // 跳过 $
 *     while (inScope() && isxdigit(*current())) pos_.next();
 *     // ...
 * }
 * ```
 *
 * **二进制**：以 `%` 开头，仅支持 0 和 1
 *
 * **十进制整数**：普通数字，支持负数
 *
 * **浮点数**：带小数点的数字，支持科学计数法
 *
 * @elseif en
 *
 * The Lexer supports four number formats, each with a unique prefix:
 *
 * **Hexadecimal**: Starts with `$`, supports 0-9 and A-F (case-insensitive)
 * ```cpp
 * // Examples: $FF, $1A, $abcdef
 * Token extractHexNumber() {
 *     pos_.next(); // skip $
 *     while (inScope() && isxdigit(*current())) pos_.next();
 *     // ...
 * }
 * ```
 *
 * **Binary**: Starts with `%`, supports only 0 and 1
 *
 * **Decimal Integer**: Plain numbers, supports negative numbers
 *
 * **Floating Point**: Numbers with decimal point, supports scientific notation
 *
 * @endif
 *
 * @subsection lexer_device_register Device and Register Recognition / 设备与寄存器识别
 *
 * @if zh
 *
 * 设备和寄存器的识别逻辑比较特殊，需要注意边界条件：
 *
 * **设备识别规则**：`d` + 数字(0-5) + 空白字符
 * - 必须检查第三个字符是否为空白，防止误识别如 `d0foo` 这样的标识符
 * - 设备编号范围：0-5（IC10 芯片限制）
 *
 * **寄存器识别规则**：`r` + 数字(0-15)
 * - 寄存器编号范围：0-15（IC10 芯片提供 16 个寄存器）
 * - 支持两位数编号如 `r10`, `r15`
 *
 * **常见陷阱**：
 * - `d6` 会被识别为设备，但会触发 IWL3 错误（设备超出范围）
 * - `r16` 会被识别为寄存器，但会触发 IWL2 错误（寄存器超出范围）
 * - `d0foo` 会被识别为标识符（因为第三个字符不是空白）
 *
 * @elseif en
 *
 * Device and register recognition logic is special, with specific boundary conditions:
 *
 * **Device Recognition**: `d` + digit(0-5) + whitespace
 * - Must check that the third character is whitespace to prevent misidentifying identifiers like `d0foo`
 * - Device number range: 0-5 (IC10 chip limitation)
 *
 * **Register Recognition**: `r` + digit(0-15)
 * - Register number range: 0-15 (IC10 chip provides 16 registers)
 * - Supports two-digit numbers like `r10`, `r15`
 *
 * **Common Pitfalls**:
 * - `d6` is recognized as a device but triggers IWL3 error (device out of range)
 * - `r16` is recognized as a register but triggers IWL2 error (register out of range)
 * - `d0foo` is recognized as an identifier (third character is not whitespace)
 *
 * @endif
 *
 * @subsection lexer_comment Comment Handling / 注释处理
 *
 * @if zh
 *
 * 支持两种注释格式，注释会被完全过滤，不会生成 Token：
 *
 * - **# 注释**：从 `#` 到行尾，常用于 IC10 原生语法
 * - **\// 注释**：从 `//` 到行尾，兼容 C++ 风格注释
 *
 * 注释提取逻辑会跳过所有字符直到换行符或文件结束，不影响后续 Token 的位置计算。
 *
 * @elseif en
 *
 * Two comment formats are supported, comments are completely filtered out and do not generate tokens:
 *
 * - **# Comment**: From `#` to end of line, commonly used in IC10 native syntax
 * - **\// Comment**: From `//` to end of line, C++ style comment compatibility
 *
 * Comment extraction skips all characters until newline or end of file, without affecting position calculation for subsequent tokens.
 *
 * @endif
 *
 * @subsection lexer_position Position Tracking / 位置追踪
 *
 * @if zh
 *
 * Lexer 使用 `Pos` 类精确追踪每个 Token 的位置信息：
 *
 * - **offset**: 字节偏移量（从文件开头）
 * - **line**: 行号（从 1 开始）
 * - **column**: 列号（从 1 开始）
 *
 * 位置信息对诊断错误至关重要，允许编译器精确定位错误位置。
 *
 * @elseif en
 *
 * The Lexer uses the `Pos` class to precisely track position information for each token:
 *
 * - **offset**: Byte offset (from start of file)
 * - **line**: Line number (1-based)
 * - **column**: Column number (1-based)
 *
 * Position information is crucial for error diagnostics, allowing precise error location reporting.
 *
 * @endif
 *
 * @section lexer_api API Reference / API 接口
 *
 * @if zh
 *
 * @code{.cpp}
 * #include "ic10_compiler/lexer/lexer.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 静态方法调用（推荐）
 * auto tokens = Lexer::tokenize(source);
 *
 * // 实例方法调用（支持调试模式）
 * Lexer lexer(source, debug_mode);
 * auto tokens = lexer.scan();
 *
 * // 遍历 Token 序列
 * for (const auto& token : tokens) {
 *     std::cout << token->toString() << std::endl;
 * }
 * @endcode
 *
 * **核心方法**：
 * | 方法 | 说明 |
 * |:-----|:-----|
 * | `tokenize(src, debug)` | 静态方法，执行词法分析 |
 * | `scan()` | 实例方法，扫描所有 Token |
 * | `next()` | 内部方法，获取下一个 Token |
 * | `current()` | 获取当前字符 |
 * | `peek(n)` | 前瞻第 n 个字符 |
 *
 * @elseif en
 *
 * @code{.cpp}
 * #include "ic10_compiler/lexer/lexer.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // Static method call (recommended)
 * auto tokens = Lexer::tokenize(source);
 *
 * // Instance method call (supports debug mode)
 * Lexer lexer(source, debug_mode);
 * auto tokens = lexer.scan();
 *
 * // Iterate through token sequence
 * for (const auto& token : tokens) {
 *     std::cout << token->toString() << std::endl;
 * }
 * @endcode
 *
 * **Core Methods**:
 * | Method | Description |
 * |:-------|:------------|
 * | `tokenize(src, debug)` | Static method, perform lexical analysis |
 * | `scan()` | Instance method, scan all tokens |
 * | `next()` | Internal method, get next token |
 * | `current()` | Get current character |
 * | `peek(n)` | Look ahead n characters |
 *
 * @endif
 *
 * @section lexer_diagnostics Diagnostics / 诊断信息
 *
 * @if zh
 *
 * Lexer 在遇到非法字符或格式错误时会生成诊断信息：
 *
 * | 消息 ID | 说明 | 触发条件 |
 * |:--------|:-----|:---------|
 * | IWL1 | 非法字符 | 遇到不在 SYMBOLS 中的字符 |
 * | IWL2 | 寄存器超出范围 | 寄存器编号 > 15 |
 * | IWL3 | 设备超出范围 | 设备编号 > 5 |
 * | IWL4 | 字符串未闭合 | 字符串缺少结束引号 |
 *
 * **诊断信息格式**：`[行:列] 错误类型: 消息内容`
 *
 * @elseif en
 *
 * The Lexer generates diagnostics for invalid characters or format errors:
 *
 * | Message ID | Description | Trigger Condition |
 * |:-----------|:------------|:------------------|
 * | IWL1 | Invalid character | Character not in SYMBOLS |
 * | IWL2 | Register out of range | Register number > 15 |
 * | IWL3 | Device out of range | Device number > 5 |
 * | IWL4 | Unclosed string | Missing closing quote |
 *
 * **Diagnostic Format**: `[line:column] Error Type: Message`
 *
 * @endif
 *
 * @section lexer_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 * **新增 Token 类型流程**：
 * 1. 在 `token.hpp` 的 `TokenType` 枚举中添加新类型
 * 2. 在 `token.cpp` 的 `KEYWORD_MAP` 或 `SYMBOL_MAP` 中添加映射
 * 3. 在 `lexer.cpp` 的 `next()` 方法中添加识别逻辑（如果需要）
 * 4. 在相应的语言文件中添加诊断消息
 *
 * **性能优化建议**：
 * - 使用 `std::string_view` 避免不必要的字符串拷贝
 * - 预分配 Token 向量容量减少内存分配
 * - 对于高频调用，考虑复用 Lexer 实例
 *
 * **测试要点**：
 * - 边界条件测试（寄存器/设备范围）
 * - 特殊字符测试（空白、换行、制表符）
 * - 混合格式测试（注释与代码混合）
 * - 错误恢复测试（非法字符后的解析）
 *
 * @elseif en
 *
 * **Adding New Token Type**:
 * 1. Add new type to `TokenType` enum in `token.hpp`
 * 2. Add mapping in `KEYWORD_MAP` or `SYMBOL_MAP` in `token.cpp`
 * 3. Add recognition logic in `next()` method in `lexer.cpp` (if needed)
 * 4. Add diagnostic message in corresponding language files
 *
 * **Performance Tips**:
 * - Use `std::string_view` to avoid unnecessary string copies
 * - Pre-allocate token vector capacity to reduce memory allocation
 * - Consider reusing Lexer instances for high-frequency calls
 *
 * **Testing Checklist**:
 * - Boundary condition tests (register/device ranges)
 * - Special character tests (whitespace, newline, tab)
 * - Mixed format tests (comments mixed with code)
 * - Error recovery tests (parsing after invalid characters)
 *
 * @endif
 *
 * @see [Lexer](@ref stationeers::ic10::Lexer), [Token](@ref stationeers::ic10::Token), [TokenType](@ref stationeers::ic10::TokenType), [Pos](@ref stationeers::Pos)
 */

/**
 * @page ic10_parser_page Parser / 语法分析器
 * @tableofcontents
 *
 * @section parser_intro Introduction / 概述
 *
 * @if zh
 *
 * 语法分析器接收 Token 序列，通过**递归下降**算法构建抽象语法树(AST)。
 * Parser 采用错误包容策略，遇到语法错误时生成 ErrorNode 继续解析，确保即使存在错误也能尽可能多地获取有用信息。
 *
 * @elseif en
 *
 * The Parser takes a token sequence and builds an Abstract Syntax Tree (AST) through a **recursive descent** algorithm.
 * It employs an error-tolerant strategy, generating ErrorNode when syntax errors are encountered to continue parsing and extract as much useful information as possible.
 *
 * @endif
 *
 * @section parser_architecture Architecture / 架构设计
 *
 * @subsection parser_recursive_descent Recursive Descent Parsing / 递归下降解析
 *
 * @if zh
 *
 * Parser 将语法规则分解为一系列递归函数，每个函数对应一个语法产生式：
 *
 * **语法规则示例**：
 * ```
 * program      → statement*
 * statement    → label_def | instruction | preprocessor | NEWLINE
 * label_def    → IDENTIFIER ':'
 * instruction  → nullary | unary | binary | ternary | quaternary | quinary | senary
 * nullary      → KEYWORD_YIELD | KEYWORD_HCF
 * unary        → KEYWORD_J operand | KEYWORD_SLEEP operand
 * binary       → KEYWORD_MOVE operand operand
 * ...
 * ```
 *
 * @elseif en
 *
 * The Parser decomposes grammar rules into a series of recursive functions, each corresponding to a grammar production:
 *
 * **Grammar Rules Example**:
 * ```
 * program      → statement*
 * statement    → label_def | instruction | preprocessor | NEWLINE
 * label_def    → IDENTIFIER ':'
 * instruction  → nullary | unary | binary | ternary | quaternary | quinary | senary
 * nullary      → KEYWORD_YIELD | KEYWORD_HCF
 * unary        → KEYWORD_J operand | KEYWORD_SLEEP operand
 * binary       → KEYWORD_MOVE operand operand
 * ...
 * ```
 *
 * @endif
 *
 * ```mermaid
 * graph TD
 *     A[parse] --> B[parseProgram]
 *     B --> C[parseStatement]
 *     C --> D{Statement Type? / 语句类型?}
 *     D -->|Label| E[parseLabelDef]
 *     D -->|Instruction| F[parseInstruction]
 *     D -->|Preprocessor| G[parsePreprocessor]
 *     F --> H{Operand Count? / 操作数个数?}
 *     H -->|0| I[parseNullary]
 *     H -->|1| J[parseUnary]
 *     H -->|2| K[parseBinary]
 *     H -->|3| L[parseTernary]
 *     H -->|4| M[parseQuaternary]
 *     H -->|5| N[parseQuinary]
 *     H -->|6| O[parseSenary]
 * ```
 *
 * @if zh
 *
 * **核心解析函数结构**：
 *
 * ```cpp
 * class Parser {
 * public:
 *     static Program parsing(const std::vector<std::shared_ptr<Token>>& tokens);
 *
 * private:
 *     Program parseProgram();
 *     std::shared_ptr<Statement> parseStatement();
 *     std::shared_ptr<Statement> parseLabelDef();
 *     std::shared_ptr<Instruction> parseInstruction();
 *     std::shared_ptr<Instruction> parseNullary();
 *     std::shared_ptr<Instruction> parseUnary();
 *     // ... 其他操作数解析函数
 *     std::shared_ptr<Operand> parseOperand();
 *     void parsePreprocessor();
 *
 *     // 辅助方法
 *     std::shared_ptr<Token> peek();
 *     std::shared_ptr<Token> consume();
 *     bool match(TokenType type);
 *     void expect(TokenType type, const std::string& msg);
 * };
 * ```
 *
 * **关键设计要点**：
 * - 使用 `peek()` 前瞻下一个 Token 而不消费
 * - 使用 `consume()` 消费并返回当前 Token
 * - 使用 `match()` 检查当前 Token 类型并消费
 * - 使用 `expect()` 期望特定 Token 类型，否则报错
 *
 * @elseif en
 *
 * **Core Parser Function Structure**:
 *
 * ```cpp
 * class Parser {
 * public:
 *     static Program parsing(const std::vector<std::shared_ptr<Token>>& tokens);
 *
 * private:
 *     Program parseProgram();
 *     std::shared_ptr<Statement> parseStatement();
 *     std::shared_ptr<Statement> parseLabelDef();
 *     std::shared_ptr<Instruction> parseInstruction();
 *     std::shared_ptr<Instruction> parseNullary();
 *     std::shared_ptr<Instruction> parseUnary();
 *     // ... other operand parsing functions
 *     std::shared_ptr<Operand> parseOperand();
 *     void parsePreprocessor();
 *
 *     // Helper methods
 *     std::shared_ptr<Token> peek();
 *     std::shared_ptr<Token> consume();
 *     bool match(TokenType type);
 *     void expect(TokenType type, const std::string& msg);
 * };
 * ```
 *
 * **Key Design Points**:
 * - `peek()` looks ahead at the next token without consuming
 * - `consume()` consumes and returns the current token
 * - `match()` checks current token type and consumes
 * - `expect()` expects a specific token type, reports error otherwise
 *
 * @endif
 *
 * @subsection parser_error_recovery Error Recovery / 错误恢复策略
 *
 * @if zh
 *
 * Parser 采用**语句级错误恢复**策略，确保单个错误不会导致整个解析失败：
 *
 * **错误恢复流程**：
 * 1. 检测到语法错误时，创建 ErrorNode 记录错误位置和消息
 * 2. 跳过当前 Token，继续尝试解析
 * 3. 在语句边界（NEWLINE 或 END）处恢复同步
 * 4. 收集所有错误，不中断编译流程
 *
 * **错误节点设计**：
 * ```cpp
 * struct ErrorNode : AST<ErrorNode> {
 *     std::shared_ptr<Token> token;
 *     std::string message;
 * };
 * ```
 *
 * ErrorNode 会被插入到 AST 中的错误位置，语义分析阶段可以继续处理有效部分。
 *
 * **诊断消息**：
 * - IPL1: 期望的 Token 未找到
 * - IPL2: 缺少操作数
 * - IPL3: 多余的操作数
 * - IPL4: 无效的指令组合
 *
 * @elseif en
 *
 * The Parser employs a **statement-level error recovery** strategy, ensuring single errors don't cause complete parsing failure:
 *
 * **Error Recovery Flow**:
 * 1. When a syntax error is detected, create ErrorNode with error position and message
 * 2. Skip the current token and continue parsing
 * 3. Resynchronize at statement boundaries (NEWLINE or END)
 * 4. Collect all errors without interrupting the compilation flow
 *
 * **Error Node Design**:
 * ```cpp
 * struct ErrorNode : AST<ErrorNode> {
 *     std::shared_ptr<Token> token;
 *     std::string message;
 * };
 * ```
 *
 * ErrorNode is inserted at the error location in the AST, allowing semantic analysis to continue processing valid parts.
 *
 * **Diagnostic Messages**:
 * - IPL1: Expected token not found
 * - IPL2: Missing operand
 * - IPL3: Extra operand
 * - IPL4: Invalid instruction combination
 *
 * @endif
 *
 * @section parser_features Core Features / 核心特性
 *
 * @subsection parser_instruction_dispatch Instruction Dispatch / 指令分发机制
 *
 * @if zh
 *
 * Parser 使用 **TypeMap** 在编译期建立 TokenType 到指令类型的映射，实现零运行时开销的指令分发：
 *
 * ```cpp
 * // 指令映射表定义
 * using InstructionMap = TypeMap<TokenType, std::tuple<
 *     std::pair<TokenType::KEYWORD_ADD, AddInstruction>,
 *     std::pair<TokenType::KEYWORD_MOVE, MoveInstruction>,
 *     // ... 所有指令
 * >>;
 *
 * // 在解析时获取指令类型
 * TokenType tokenType = currentToken->type;
 * using InsType = InstructionMap::Get<tokenType>;
 * auto ins = std::make_shared<InsType>(pos, args...);
 * ```
 *
 * **设计优势**：
 * - 编译期类型安全，运行时无分支开销
 * - 新增指令只需添加映射，无需修改解析逻辑
 * - 支持模板元编程实现的类型检查
 *
 * @elseif en
 *
 * The Parser uses **TypeMap** to establish TokenType-to-instruction-type mapping at compile time, achieving zero runtime overhead instruction dispatch:
 *
 * ```cpp
 * // Instruction mapping table definition
 * using InstructionMap = TypeMap<TokenType, std::tuple<
 *     std::pair<TokenType::KEYWORD_ADD, AddInstruction>,
 *     std::pair<TokenType::KEYWORD_MOVE, MoveInstruction>,
 *     // ... all instructions
 * >>;
 *
 * // Get instruction type during parsing
 * TokenType tokenType = currentToken->type;
 * using InsType = InstructionMap::Get<tokenType>;
 * auto ins = std::make_shared<InsType>(pos, args...);
 * ```
 *
 * **Design Advantages**:
 * - Compile-time type safety with no runtime branch overhead
 * - New instructions only require adding mappings, no parsing logic changes
 * - Supports type checking via template metaprogramming
 *
 * @endif
 *
 * @subsection parser_preprocessor Preprocessor Directives / 预处理指令
 *
 * @if zh
 *
 * Parser 支持两种预处理指令，它们在解析阶段被处理但不会生成 AST 节点：
 *
 * **alias 指令**：为设备定义别名
 * ```
 * alias myDisplay d0      # myDisplay 是 d0 的别名
 * ```
 * 处理逻辑：将别名存储到符号表，后续引用时自动替换为实际设备
 *
 * **define 指令**：定义常量
 * ```
 * define PI 3.14159      # PI 是常量 3.14159
 * ```
 * 处理逻辑：将常量值存储，后续引用时直接替换为字面量
 *
 * **预处理指令的特殊处理**：
 * - 预处理指令在词法分析之后、语法分析之前处理
 * - 别名和常量定义会影响后续的符号解析
 * - 预处理错误会生成专门的诊断信息
 *
 * @elseif en
 *
 * The Parser supports two preprocessor directives that are processed during parsing but don't generate AST nodes:
 *
 * **alias Directive**: Define device alias
 * ```
 * alias myDisplay d0      # myDisplay is an alias for d0
 * ```
 * Processing logic: Store alias in symbol table, automatically replace with actual device on subsequent references
 *
 * **define Directive**: Define constant
 * ```
 * define PI 3.14159      # PI is constant 3.14159
 * ```
 * Processing logic: Store constant value, directly replace with literal on subsequent references
 *
 * **Special Handling for Preprocessor Directives**:
 * - Processed after lexical analysis but before syntax analysis
 * - Alias and constant definitions affect subsequent symbol resolution
 * - Preprocessor errors generate dedicated diagnostic messages
 *
 * @endif
 *
 * @subsection parser_doc_comment Doc Comment Parsing / 文档注释解析
 *
 * @if zh
 *
 * Parser 集成了 [DocParser](@ref stationeers::ic10::DocParser)，用于解析 `@device` 和 `@enum` 标签，提取设备和枚举的元数据：
 *
 * **支持的标签**：
 * | 标签 | 说明 | 示例 |
 * |:-----|:-----|:-----|
 * | `@device` | 设备定义开始 | `@device MyDevice` |
 * | `@end-device` | 设备定义结束 | `@end-device` |
 * | `@enum` | 枚举定义开始 | `@enum MyEnum` |
 * | `@end-enum` | 枚举定义结束 | `@end-enum` |
 * | `@name` | 名称 | `@name "显示设备"` |
 * | `@desc` | 描述 | `@desc "用于显示文本"` |
 * | `@value` | 枚举值 | `@value ON 1` |
 * | `@slot` | 设备槽位 | `@slot input 0` |
 * | `@connect` | 连接信息 | `@connect d0` |
 *
 * **解析流程**：
 * 1. 在遇到 HEX_COMMENT 或 SLASH_COMMENT 时检查是否为文档注释
 * 2. 如果是文档注释，调用 DocParser 解析标签
 * 3. 解析结果存储到 DeviceDocComment 或 EnumDocComment 结构中
 * 4. 语义分析阶段使用这些元数据进行类型检查
 *
 * @elseif en
 *
 * The Parser integrates [DocParser](@ref stationeers::ic10::DocParser) to parse `@device` and `@enum` tags, extracting device and enum metadata:
 *
 * **Supported Tags**:
 * | Tag | Description | Example |
 * |:----|:------------|:--------|
 * | `@device` | Start device definition | `@device MyDevice` |
 * | `@end-device` | End device definition | `@end-device` |
 * | `@enum` | Start enum definition | `@enum MyEnum` |
 * | `@end-enum` | End enum definition | `@end-enum` |
 * | `@name` | Name | `@name "Display Device"` |
 * | `@desc` | Description | `@desc "Used for text display"` |
 * | `@value` | Enum value | `@value ON 1` |
 * | `@slot` | Device slot | `@slot input 0` |
 * | `@connect` | Connection info | `@connect d0` |
 *
 * **Parsing Flow**:
 * 1. Check if comment is a doc comment when encountering HEX_COMMENT or SLASH_COMMENT
 * 2. If doc comment, call DocParser to parse tags
 * 3. Store results in DeviceDocComment or EnumDocComment structures
 * 4. Semantic analysis uses this metadata for type checking
 *
 * @endif
 *
 * @subsection parser_operand_parsing Operand Parsing / 操作数解析
 *
 * @if zh
 *
 * 操作数解析是 Parser 的核心逻辑之一，支持多种操作数类型：
 *
 * **操作数类型**：
 * | 类型 | 说明 | 示例 |
 * |:-----|:-----|:-----|
 * | 寄存器 | 直接引用 | `r0`, `r15` |
 * | 设备 | 直接引用或别名 | `d0`, `myDisplay` |
 * | 字面量 | 数值或字符串 | `42`, `$FF`, `"text"` |
 * | 标识符 | 标签或常量引用 | `main`, `PI` |
 *
 * **操作数解析逻辑**：
 * ```cpp
 * std::shared_ptr<Operand> Parser::parseOperand() {
 *     auto token = peek();
 *     switch (token->type) {
 *         case TokenType::REGISTER:
 *             return std::make_shared<RegisterOperand>(consume());
 *         case TokenType::DEVICE:
 *             return std::make_shared<DeviceOperand>(consume());
 *         case TokenType::IDENTIFIER:
 *             return std::make_shared<IdentifierOperand>(consume());
 *         case TokenType::INTEGER_LITERAL:
 *         case TokenType::FLOAT_LITERAL:
 *             return std::make_shared<LiteralOperand>(consume());
 *         case TokenType::STRING_LITERAL:
 *             return std::make_shared<StringOperand>(consume());
 *         default:
 *             // 错误处理
 *             break;
 *     }
 * }
 * ```
 *
 * **操作数验证**：
 * - 每个指令对操作数类型有特定要求
 * - 例如 `add` 指令要求三个寄存器操作数
 * - 类型不匹配会在语义分析阶段报错
 *
 * @elseif en
 *
 * Operand parsing is one of the core logic components of the Parser, supporting multiple operand types:
 *
 * **Operand Types**:
 * | Type | Description | Example |
 * |:-----|:------------|:--------|
 * | Register | Direct reference | `r0`, `r15` |
 * | Device | Direct reference or alias | `d0`, `myDisplay` |
 * | Literal | Numeric or string value | `42`, `$FF`, `"text"` |
 * | Identifier | Label or constant reference | `main`, `PI` |
 *
 * **Operand Parsing Logic**:
 * ```cpp
 * std::shared_ptr<Operand> Parser::parseOperand() {
 *     auto token = peek();
 *     switch (token->type) {
 *         case TokenType::REGISTER:
 *             return std::make_shared<RegisterOperand>(consume());
 *         case TokenType::DEVICE:
 *             return std::make_shared<DeviceOperand>(consume());
 *         case TokenType::IDENTIFIER:
 *             return std::make_shared<IdentifierOperand>(consume());
 *         case TokenType::INTEGER_LITERAL:
 *         case TokenType::FLOAT_LITERAL:
 *             return std::make_shared<LiteralOperand>(consume());
 *         case TokenType::STRING_LITERAL:
 *             return std::make_shared<StringOperand>(consume());
 *         default:
 *             // Error handling
 *             break;
 *     }
 * }
 * ```
 *
 * **Operand Validation**:
 * - Each instruction has specific requirements for operand types
 * - For example, `add` requires three register operands
 * - Type mismatches are reported during semantic analysis
 *
 * @endif
 *
 * @section parser_api API Reference / API 接口
 *
 * @if zh
 *
 * @code{.cpp}
 * #include "ic10_compiler/parser/parser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 解析 Token 序列
 * Program program = Parser::parsing(tokens);
 *
 * // 遍历 AST
 * for (const auto& stmt : program.statements) {
 *     std::cout << stmt->toString() << std::endl;
 * }
 *
 * // 检查错误
 * if (!program.errors.empty()) {
 *     for (const auto& err : program.errors) {
 *         std::cerr << err->message << std::endl;
 *     }
 * }
 * @endcode
 *
 * **核心方法**：
 * | 方法 | 说明 |
 * |:-----|:-----|
 * | `parsing(tokens)` | 静态方法，解析 Token 序列 |
 * | `parseProgram()` | 解析程序根节点 |
 * | `parseStatement()` | 解析单个语句 |
 * | `parseInstruction()` | 解析指令 |
 * | `parseOperand()` | 解析操作数 |
 *
 * @elseif en
 *
 * @code{.cpp}
 * #include "ic10_compiler/parser/parser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // Parse token sequence
 * Program program = Parser::parsing(tokens);
 *
 * // Traverse AST
 * for (const auto& stmt : program.statements) {
 *     std::cout << stmt->toString() << std::endl;
 * }
 *
 * // Check errors
 * if (!program.errors.empty()) {
 *     for (const auto& err : program.errors) {
 *         std::cerr << err->message << std::endl;
 *     }
 * }
 * @endcode
 *
 * **Core Methods**:
 * | Method | Description |
 * |:-------|:------------|
 * | `parsing(tokens)` | Static method, parse token sequence |
 * | `parseProgram()` | Parse program root node |
 * | `parseStatement()` | Parse single statement |
 * | `parseInstruction()` | Parse instruction |
 * | `parseOperand()` | Parse operand |
 *
 * @endif
 *
 * @section parser_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 * **新增指令流程**：
 * 1. 在 `token.hpp` 的 `TokenType` 枚举中添加指令关键字
 * 2. 在 `token.cpp` 的 `KEYWORD_MAP` 中添加映射
 * 3. 在对应的 AST 指令头文件中定义指令类型（使用 DEFINE_*_INSTRUCTION 宏）
 * 4. 在 `parser.cpp` 中添加指令分发逻辑
 * 5. 在语义分析器中添加类型检查规则
 *
 * **语法扩展指南**：
 * - 如果需要添加新的语法结构，首先修改语法规则
 * - 然后添加对应的解析函数
 * - 最后添加 AST 节点类型
 *
 * **测试要点**：
 * - 边界条件测试（操作数个数、类型）
 * - 错误恢复测试（非法指令、缺少操作数）
 * - 预处理指令测试（别名、常量定义）
 * - 文档注释测试（@device、@enum 标签）
 * - 性能测试（大文件解析速度）
 *
 * **调试技巧**：
 * - 使用 `program.toJSON()` 输出 AST 结构
 * - 使用 `debug` 模式查看解析过程
 * - 在关键位置添加断点检查 Token 序列
 *
 * @elseif en
 *
 * **Adding New Instruction**:
 * 1. Add instruction keyword to `TokenType` enum in `token.hpp`
 * 2. Add mapping in `KEYWORD_MAP` in `token.cpp`
 * 3. Define instruction type in corresponding AST instruction header (using DEFINE_*_INSTRUCTION macro)
 * 4. Add instruction dispatch logic in `parser.cpp`
 * 5. Add type checking rules in semantic analyser
 *
 * **Syntax Extension Guide**:
 * - If adding new syntax structures, first modify grammar rules
 * - Then add corresponding parsing functions
 * - Finally add AST node types
 *
 * **Testing Checklist**:
 * - Boundary condition tests (operand count, types)
 * - Error recovery tests (invalid instructions, missing operands)
 * - Preprocessor directive tests (aliases, constant definitions)
 * - Doc comment tests (@device, @enum tags)
 * - Performance tests (large file parsing speed)
 *
 * **Debugging Tips**:
 * - Use `program.toJSON()` to output AST structure
 * - Use `debug` mode to see parsing process
 * - Add breakpoints at key locations to check token sequence
 *
 * @endif
 *
 * @see [Parser](@ref stationeers::ic10::Parser), [Program](@ref stationeers::ic10::Program), [DocParser](@ref stationeers::ic10::DocParser), [Instruction](@ref stationeers::ic10::Instruction)
 */

/**
 * @page ic10_ast_page AST / 抽象语法树
 * @tableofcontents
 *
 * @section ast_intro Introduction / 概述
 *
 * @if zh
 *
 * AST (Abstract Syntax Tree) 是源代码的树形表示，每个节点代表一个语法结构。
 * IC10 的 AST 设计大量使用 C++ 模板元编程，实现类型安全的节点体系。
 * 这种设计确保了在编译期就能捕获类型错误，避免运行时错误。
 *
 * @elseif en
 *
 * AST (Abstract Syntax Tree) is a tree representation of source code, where each node represents a syntactic structure.
 * IC10's AST design heavily utilizes C++ template metaprogramming to achieve a type-safe node system.
 * This design ensures type errors are caught at compile time, preventing runtime errors.
 *
 * @endif
 *
 * @section ast_architecture Architecture / 架构设计
 *
 * @subsection ast_base_class Base Node Design / 基础节点设计
 *
 * @if zh
 *
 * 所有 AST 节点继承自 `AST<T>` 模板，使用 **CRTP (Curiously Recurring Template Pattern)** 模式：
 *
 * **CRTP 模式的优势**：
 * - 静态多态，避免虚函数开销
 * - 在基类中访问派生类成员
 * - 编译期类型检查
 *
 * **基础模板定义**：
 *
 * @elseif en
 *
 * All AST nodes inherit from the `AST<T>` template, using the **CRTP (Curiously Recurring Template Pattern)**:
 *
 * **CRTP Pattern Advantages**:
 * - Static polymorphism, avoiding virtual function overhead
 * - Access derived class members in base class
 * - Compile-time type checking
 *
 * **Base Template Definition**:
 *
 * @endif
 *
 * @code{.cpp}
 * template<typename Derived>
 * struct AST {
 *     Pos pos;
 *
 *     std::string toString() const {
 *         return static_cast<const Derived*>(this)->toStringImpl();
 *     }
 *
 *     std::string toJSON() const {
 *         return static_cast<const Derived*>(this)->toJSONImpl();
 *     }
 *
 *     std::shared_ptr<AST> clone() const {
 *         return static_cast<const Derived*>(this)->cloneImpl();
 *     }
 * };
 *
 * struct Program : AST<Program> {
 *     std::vector<std::shared_ptr<Statement>> statements;
 *     std::vector<std::shared_ptr<ErrorNode>> errors;
 *     // ...
 * };
 * @endcode
 *
 * @subsection ast_instruction_hierarchy Instruction Hierarchy / 指令节点层次
 *
 * @if zh
 *
 * 指令按操作数个数分类，使用宏自动生成类型定义。这种设计使得新增指令只需调用宏即可，无需手动编写大量重复代码：
 *
 * **宏展开原理**：
 *
 * ```cpp
 * // DEFINE_TERNARY_INSTRUCTION 宏展开示例
 * template<ic10::OperandType O1, ic10::OperandType O2, ic10::OperandType O3>
 * struct TernaryInstruction : Instruction {
 *     OperandWrapper<O1> op1;
 *     OperandWrapper<O2> op2;
 *     OperandWrapper<O3> op3;
 *
 *     explicit TernaryInstruction(Pos pos,
 *         std::shared_ptr<Operand> o1,
 *         std::shared_ptr<Operand> o2,
 *         std::shared_ptr<Operand> o3)
 *         : Instruction(pos), op1(o1), op2(o2), op3(o3) {}
 *
 *     std::string toString() const override;
 *     std::string toJSON() const override;
 * };
 * ```
 *
 * **OperandWrapper 的作用**：
 * - 编译期验证操作数类型
 * - 存储操作数的类型信息
 * - 提供类型安全的访问接口
 *
 * @elseif en
 *
 * Instructions are categorized by operand count, with types automatically generated using macros. This design allows new instructions to be added simply by calling a macro, eliminating the need for manual repetitive code:
 *
 * **Macro Expansion Principle**:
 *
 * ```cpp
 * // DEFINE_TERNARY_INSTRUCTION macro expansion example
 * template<ic10::OperandType O1, ic10::OperandType O2, ic10::OperandType O3>
 * struct TernaryInstruction : Instruction {
 *     OperandWrapper<O1> op1;
 *     OperandWrapper<O2> op2;
 *     OperandWrapper<O3> op3;
 *
 *     explicit TernaryInstruction(Pos pos,
 *         std::shared_ptr<Operand> o1,
 *         std::shared_ptr<Operand> o2,
 *         std::shared_ptr<Operand> o3)
 *         : Instruction(pos), op1(o1), op2(o2), op3(o3) {}
 *
 *     std::string toString() const override;
 *     std::string toJSON() const override;
 * };
 * ```
 *
 * **OperandWrapper Purpose**:
 * - Compile-time operand type validation
 * - Store operand type information
 * - Provide type-safe access interface
 *
 * @endif
 *
 * | Macro / 宏 | Operands / 操作数 | Generated Type / 生成类型 | Header / 头文件 |
 * |:-----------|:------------------|:-------------------------|:----------------|
 * | DEFINE_NULLARY_INSTRUCTION | 0 | NullaryInstruction | ast_nullary_ins.hpp |
 * | DEFINE_UNARY_INSTRUCTION | 1 | UnaryInstruction<O1> | ast_unary_ins.hpp |
 * | DEFINE_BINARY_INSTRUCTION | 2 | BinaryInstruction<O1,O2> | ast_binary_ins.hpp |
 * | DEFINE_TERNARY_INSTRUCTION | 3 | TernaryInstruction<O1,O2,O3> | ast_ternary_ins.hpp |
 * | DEFINE_QUATERNARY_INSTRUCTION | 4 | QuaternaryInstruction<O1-O4> | ast_quaternary_ins.hpp |
 * | DEFINE_QUINARY_INSTRUCTION | 5 | QuinaryInstruction<O1-O5> | ast_quinary_ins.hpp |
 * | DEFINE_SENARY_INSTRUCTION | 6 | SenaryInstruction<O1-O6> | ast_senary_ins.hpp |
 *
 * @subsection ast_type_system Operand Type System / 操作数类型系统
 *
 * @if zh
 *
 * 操作数类型通过 `OperandType` 枚举定义，每个操作数类型对应特定的验证规则：
 *
 * **操作数类型验证规则**：
 * - `REG_IDENT`: 必须是 `r0`-`r15` 格式
 * - `REG_NUM`: 必须是 0-15 的整数
 * - `DEVICE_IDENT`: 必须是 `d0`-`d5` 或 `@DeviceName` 格式
 * - `INT_LITERAL`: 必须是整数（支持十进制、十六进制、二进制）
 * - `FLOAT_LITERAL`: 必须是浮点数
 * - `STRING_LITERAL`: 必须是带引号的字符串
 * - `IDENTIFIER`: 用户定义的名称（标签、常量、别名）
 *
 * **操作数类型与语义分析的关系**：
 * 在语义分析阶段，Analyser 会根据指令定义的操作数类型检查实际操作数是否匹配。
 *
 * @elseif en
 *
 * Operand types are defined through the `OperandType` enum, with specific validation rules for each type:
 *
 * **Operand Type Validation Rules**:
 * - `REG_IDENT`: Must be in `r0`-`r15` format
 * - `REG_NUM`: Must be integer 0-15
 * - `DEVICE_IDENT`: Must be in `d0`-`d5` or `@DeviceName` format
 * - `INT_LITERAL`: Must be integer (decimal, hexadecimal, binary)
 * - `FLOAT_LITERAL`: Must be floating-point number
 * - `STRING_LITERAL`: Must be quoted string
 * - `IDENTIFIER`: User-defined name (label, constant, alias)
 *
 * **Relationship with Semantic Analysis**:
 * During semantic analysis, the Analyser checks whether actual operands match the operand types defined by the instruction.
 *
 * @endif
 *
 * | Type / 类型 | Description / 说明 | Example / 示例 | Validation / 验证规则 |
 * |:-----------|:------------------|:----------------|:-------------------|
 * | REG_IDENT | Register identifier | `r0`, `r15` | Must match `r[0-15]` |
 * | REG_NUM | Register number | `0`, `15` | Must be 0-15 |
 * | DEVICE_IDENT | Device identifier | `d0`, `@Display` | `d[0-5]` or `@Name` |
 * | INT_LITERAL | Integer literal | `42`, `$FF` | Valid integer format |
 * | FLOAT_LITERAL | Float literal | `3.14` | Valid float format |
 * | STRING_LITERAL | String literal | `"Hello"` | Quoted string |
 * | IDENTIFIER | Identifier | `main`, `myVar` | User-defined name |
 *
 * @section ast_features Core Features / 核心特性
 *
 * @subsection ast_type_safety Type Safety / 类型安全
 *
 * @if zh
 *
 * 每个指令类型在编译期确定操作数类型，不匹配会导致编译错误：
 *
 * ```cpp
 * // add 指令要求: 目标寄存器标识符, 源寄存器编号, 源寄存器编号
 * DEFINE_TERNARY_INSTRUCTION(add, Add, ADD,
 *     ic10::OperandType::REG_TARGET,      // 目标寄存器: r0, r1, ...
 *     ic10::OperandType::NUM_VALUE,        // 源寄存器1: 0, 1, ...
 *     ic10::OperandType::NUM_VALUE         // 源寄存器2: 0, 1, ...
 * )
 * ```
 *
 * **类型安全的优势**：
 * - 编译期捕获操作数类型错误
 * - 运行时无需类型检查
 * - 代码自动完成和静态分析支持
 *
 ** 常见错误模式**：
 * - 试图将设备作为寄存器操作数使用
 * - 将字符串传递给需要数字的指令
 * - 使用超出范围的寄存器编号
 *
 * @elseif en
 *
 * Each instruction type determines operand types at compile time, type mismatches cause compilation errors:
 *
 * ```cpp
 * // add instruction requires: target register identifier, source register number, source register number
 * DEFINE_TERNARY_INSTRUCTION(add, Add, ADD,
 *     ic10::OperandType::REG_TARGET,      // Target: r0, r1, ...
 *     ic10::OperandType::NUM_VALUE,        // Source 1: 0, 1, ...
 *     ic10::OperandType::NUM_VALUE         // Source 2: 0, 1, ...
 * )
 * ```
 *
 * **Type Safety Advantages**:
 * - Compile-time operand type error detection
 * - No runtime type checking needed
 * - Code completion and static analysis support
 *
 * **Common Error Patterns**:
 * - Using device as register operand
 * - Passing string to instruction expecting number
 * - Using register number out of range
 *
 * @endif
 *
 * @subsection ast_error_node Error Node / 错误节点
 *
 * @if zh
 *
 * ErrorNode 在语法错误时创建，包含错误位置和消息，允许后续阶段继续分析：
 *
 * ```cpp
 * struct ErrorNode : AST<ErrorNode> {
 *     std::shared_ptr<Token> token;
 *     std::string message;
 *     DiagnosticSeverity severity;
 * };
 * ```
 *
 ** 错误节点的设计考虑**：
 * - **位置信息**：包含错误发生的确切行号和列号
 * - **错误消息**：描述错误的详细信息
 * - **严重程度**：区分 Error、Warning、Info
 * - **插入位置**：保持 AST 结构完整，便于后续遍历
 *
 * **错误处理流程**：
 * 1. Parser 检测到错误 → 创建 ErrorNode
 * 2. ErrorNode 插入到 AST 中的错误位置
 * 3. Semantic Analyser 跳过 ErrorNode 继续分析
 * 4. 最终收集所有错误报告给用户
 *
 * @elseif en
 *
 * ErrorNode is created when syntax errors are encountered, containing error position and message, allowing subsequent phases to continue analysis:
 *
 * ```cpp
 * struct ErrorNode : AST<ErrorNode> {
 *     std::shared_ptr<Token> token;
 *     std::string message;
 *     DiagnosticSeverity severity;
 * };
 * ```
 *
 * **Error Node Design Considerations**:
 * - **Position Information**: Exact line and column of error
 * - **Error Message**: Detailed error description
 * - **Severity**: Error, Warning, Info distinction
 * - **Insertion Point**: Maintain AST structure integrity for traversal
 *
 * **Error Handling Flow**:
 * 1. Parser detects error → creates ErrorNode
 * 2. ErrorNode inserted at error location in AST
 * 3. Semantic Analyser skips ErrorNode and continues
 * 4. All errors collected and reported to user
 *
 * @endif
 *
 * @subsection ast_serialization Serialization / 序列化支持
 *
 * @if zh
 *
 * 所有节点支持 `toString()` 和 `toJSON()` 方法，便于调试和跨语言传递：
 *
 * **toString()**: 生成人类可读的文本表示
 * ```cpp
 * // 示例输出
 * add r0 1 2
 * j label
 * ```
 *
 * **toJSON()**: 生成 JSON 格式，便于跨语言处理
 * ```cpp
 * // 示例输出
 * {
 *   "type": "AddInstruction",
 *   "pos": {"line": 1, "column": 1},
 *   "operands": [
 *     {"type": "Register", "value": "r0"},
 *     {"type": "Literal", "value": 1},
 *     {"type": "Literal", "value": 2}
 *   ]
 * }
 * ```
 *
 ** 序列化的应用场景**：
 * - 调试输出
 * - 编辑器语法高亮
 * - 跨进程通信
 * - 测试断言
 *
 * @elseif en
 *
 * All nodes support `toString()` and `toJSON()` methods for debugging and cross-language communication:
 *
 * **toString()**: Generates human-readable text representation
 * ```cpp
 * // Example output
 * add r0 1 2
 * j label
 * ```
 *
 * **toJSON()**: Generates JSON format for cross-language processing
 * ```cpp
 * // Example output
 * {
 *   "type": "AddInstruction",
 *   "pos": {"line": 1, "column": 1},
 *   "operands": [
 *     {"type": "Register", "value": "r0"},
 *     {"type": "Literal", "value": 1},
 *     {"type": "Literal", "value": 2}
 *   ]
 * }
 * ```
 *
 * **Serialization Use Cases**:
 * - Debug output
 * - Editor syntax highlighting
 * - Cross-process communication
 * - Test assertions
 *
 * @endif
 *
 * @subsection ast_cloning Cloning / 节点克隆
 *
 * @if zh
 *
 * AST 节点支持克隆操作，用于增量编译时的节点复用：
 *
 * ```cpp
 * std::shared_ptr<AST> cloned = node->clone();
 * ```
 *
 ** 克隆的应用场景**：
 * - 增量编译：未修改的节点直接复用
 * - 代码转换：在不修改原节点的情况下生成新节点
 * - 缓存策略：缓存已解析的节点
 *
 * @elseif en
 *
 * AST nodes support cloning, used for node reuse in incremental compilation:
 *
 * ```cpp
 * std::shared_ptr<AST> cloned = node->clone();
 * ```
 *
 * **Cloning Use Cases**:
 * - Incremental compilation: Reuse unchanged nodes
 * - Code transformation: Generate new nodes without modifying originals
 * - Caching strategy: Cache parsed nodes
 *
 * @endif
 *
 * @section ast_node_types Node Types / 节点类型列表
 *
 * @if zh
 *
 ** 节点层次结构**：
 *
 * ```mermaid
 * graph TD
 *     A[AST] --> B[Statement]
 *     B --> C[LabelDef]
 *     B --> D[Instruction]
 *     D --> E[NullaryInstruction]
 *     D --> F[UnaryInstruction]
 *     D --> G[BinaryInstruction]
 *     D --> H[TernaryInstruction]
 *     D --> I[QuaternaryInstruction]
 *     D --> J[QuinaryInstruction]
 *     D --> K[SenaryInstruction]
 *     B --> L[ErrorNode]
 *     A --> M[Operand]
 *     M --> N[RegisterOperand]
 *     M --> O[DeviceOperand]
 *     M --> P[LiteralOperand]
 *     M --> Q[StringOperand]
 *     M --> R[IdentifierOperand]
 * ```
 *
 * @elseif en
 *
 * **Node Hierarchy**:
 *
 * ```mermaid
 * graph TD
 *     A[AST] --> B[Statement]
 *     B --> C[LabelDef]
 *     B --> D[Instruction]
 *     D --> E[NullaryInstruction]
 *     D --> F[UnaryInstruction]
 *     D --> G[BinaryInstruction]
 *     D --> H[TernaryInstruction]
 *     D --> I[QuaternaryInstruction]
 *     D --> J[QuinaryInstruction]
 *     D --> K[SenaryInstruction]
 *     B --> L[ErrorNode]
 *     A --> M[Operand]
 *     M --> N[RegisterOperand]
 *     M --> O[DeviceOperand]
 *     M --> P[LiteralOperand]
 *     M --> Q[StringOperand]
 *     M --> R[IdentifierOperand]
 * ```
 *
 * @endif
 *
 * | Node Type / 节点类型 | Header / 头文件 | Description / 说明 |
 * |:-------------------|:----------------|:------------------|
 * | Program | ast.hpp | 程序根节点 |
 * | Statement | ast.hpp | 语句基类 |
 * | ErrorNode | ast_node.hpp | 错误节点 |
 * | LabelDef | ast_node.hpp | 标签定义 |
 * | Instruction | ast.hpp | 指令基类 |
 * | NullaryInstruction | ast_nullary_ins.hpp | 零元指令 |
 * | UnaryInstruction | ast_unary_ins.hpp | 一元指令 |
 * | BinaryInstruction | ast_binary_ins.hpp | 二元指令 |
 * | TernaryInstruction | ast_ternary_ins.hpp | 三元指令 |
 * | QuaternaryInstruction | ast_quaternary_ins.hpp | 四元指令 |
 * | QuinaryInstruction | ast_quinary_ins.hpp | 五元指令 |
 * | SenaryInstruction | ast_senary_ins.hpp | 六元指令 |
 * | Operand | ast.hpp | 操作数基类 |
 * | RegisterOperand | ast_node.hpp | 寄存器操作数 |
 * | DeviceOperand | ast_node.hpp | 设备操作数 |
 * | LiteralOperand | ast_node.hpp | 字面量操作数 |
 * | StringOperand | ast_node.hpp | 字符串操作数 |
 * | IdentifierOperand | ast_node.hpp | 标识符操作数 |
 *
 * @section ast_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 ** 新增指令类型流程**：
 * 1. 在对应的 AST 指令头文件中使用 DEFINE_*_INSTRUCTION 宏定义指令
 * 2. 在 `token.hpp` 中添加对应的 TokenType
 * 3. 在 `token.cpp` 中添加关键字映射
 * 4. 在 `parser.cpp` 中添加指令分发逻辑
 * 5. 在语义分析器中添加类型检查规则
 *
 ** 模板元编程注意事项**：
 * - 操作数类型在编译期确定，运行时无法更改
 * - 宏展开可能导致复杂的错误消息，需要仔细检查模板参数
 * - 使用 `static_assert` 确保操作数类型组合有效
 *
 ** 测试要点**：
 * - 指令类型检查（操作数个数、类型）
 * - 序列化/反序列化测试
 * - 节点克隆测试
 * - 错误节点处理测试
 * - 大型 AST 性能测试
 *
 * @elseif en
 *
 * **Adding New Instruction Type**:
 * 1. Use DEFINE_*_INSTRUCTION macro in corresponding AST instruction header
 * 2. Add corresponding TokenType in `token.hpp`
 * 3. Add keyword mapping in `token.cpp`
 * 4. Add instruction dispatch logic in `parser.cpp`
 * 5. Add type checking rules in semantic analyser
 *
 * **Template Metaprogramming Notes**:
 * - Operand types are determined at compile time, cannot be changed at runtime
 * - Macro expansion may produce complex error messages, verify template parameters carefully
 * - Use `static_assert` to ensure valid operand type combinations
 *
 * **Testing Checklist**:
 * - Instruction type checking (operand count, types)
 * - Serialization/deserialization tests
 * - Node cloning tests
 * - Error node handling tests
 * - Large AST performance tests
 *
 * @endif
 *
 * @see [AST](@ref stationeers::ic10::AST), [Program](@ref stationeers::ic10::Program), [Instruction](@ref stationeers::ic10::Instruction), [Operand](@ref stationeers::ic10::Operand)
 */

/**
 * @page ic10_semantic_page Semantic Analysis / 语义分析
 * @tableofcontents
 *
 * @section semantic_intro Introduction / 概述
 *
 * @if zh
 *
 * 语义分析器对 AST 进行**类型检查**和**符号解析**，确保程序的语义正确性。
 * 使用 C++20 协程和 Future/Promise 模式处理前向引用，支持标签在定义之前被引用。
 *
 * 语义分析是编译流程的关键阶段，它将语法正确但语义可能错误的程序转化为语义正确的程序。
 *
 * @elseif en
 *
 * The Semantic Analyser performs **type checking** and **symbol resolution** on the AST, ensuring semantic correctness of the program.
 * It uses C++20 coroutines and Future/Promise pattern to handle forward references, allowing labels to be referenced before definition.
 *
 * Semantic analysis is a critical stage in the compilation process, transforming syntactically correct but potentially semantically incorrect programs into semantically correct ones.
 *
 * @endif
 *
 * @section semantic_architecture Architecture / 架构设计
 *
 * @subsection semantic_symbol_table Symbol Table Design / 符号表设计
 *
 * @if zh
 *
 * [SymbolTable](@ref stationeers::ic10::SymbolTable) 使用 **Future/Promise** 模式支持前向引用，这是一个异步符号解析机制：
 *
 * **设计目标**：
 * - 支持标签在定义之前被引用
 * - 异步解析，不阻塞编译流程
 * - 统一的符号管理接口
 *
 * **核心数据结构**：
 *
 * ```cpp
 * struct SymbolEntry {
 *     std::shared_ptr<Symbol> symbol;  // 已解析的符号（可能为空）
 *     std::optional<std::promise<std::shared_ptr<Symbol>>> promise;
 *     std::future<std::shared_ptr<Symbol>> future;
 *     std::vector<Pos> references;     // 所有引用位置
 * };
 *
 * struct SymbolTable {
 *     std::unordered_map<std::string, SymbolEntry> entries;
 *
 *     // 定义符号
 *     void define(const std::string& name, std::shared_ptr<Symbol> symbol);
 *
 *     // 解析符号（支持前向引用）
 *     SymbolEntry& resolve(const std::string& name, const Pos& pos);
 *
 *     // 检查符号是否已定义
 *     bool contains(const std::string& name) const;
 * };
 * ```
 *
 ** Future/Promise 模式工作原理**：
 * - 当符号首次被引用时，如果尚未定义，创建一个未决的 Future
 * - 后续引用同一个符号时，返回同一个 Future
 * - 当符号被定义时，Promise 设置值，所有等待该 Future 的协程被唤醒
 * - 如果编译完成时符号仍未定义，生成未定义符号错误
 *
 * @elseif en
 *
 * [SymbolTable](@ref stationeers::ic10::SymbolTable) uses the **Future/Promise** pattern to support forward references, an asynchronous symbol resolution mechanism:
 *
 * **Design Goals**:
 * - Support label references before definition
 * - Asynchronous resolution without blocking compilation
 * - Unified symbol management interface
 *
 * **Core Data Structure**:
 *
 * ```cpp
 * struct SymbolEntry {
 *     std::shared_ptr<Symbol> symbol;  // Resolved symbol (may be null)
 *     std::optional<std::promise<std::shared_ptr<Symbol>>> promise;
 *     std::future<std::shared_ptr<Symbol>> future;
 *     std::vector<Pos> references;     // All reference positions
 * };
 *
 * struct SymbolTable {
 *     std::unordered_map<std::string, SymbolEntry> entries;
 *
 *     // Define symbol
 *     void define(const std::string& name, std::shared_ptr<Symbol> symbol);
 *
 *     // Resolve symbol (supports forward reference)
 *     SymbolEntry& resolve(const std::string& name, const Pos& pos);
 *
 *     // Check if symbol exists
 *     bool contains(const std::string& name) const;
 * };
 * ```
 *
 * **Future/Promise Pattern Working Principle**:
 * - When a symbol is first referenced and not yet defined, create a pending Future
 * - Subsequent references to the same symbol return the same Future
 * - When the symbol is defined, Promise sets the value and all waiting coroutines are resumed
 * - If symbol remains undefined at compilation completion, generate undefined symbol error
 *
 * @endif
 *
 * ```mermaid
 * sequenceDiagram
 *     participant Parser as Parser / 语法分析器
 *     participant ST as SymbolTable / 符号表
 *     participant Future as Future
 *     participant Promise as Promise
 *
 *     Parser->>ST: resolve("label", pos)
 *     alt Symbol defined / 符号已定义
 *         ST-->>Parser: Return resolved symbol / 返回已解析的符号
 *     else Symbol undefined / 符号未定义
 *         ST-->>Parser: Return pending Future / 返回未决的 Future
 *         Parser->>Parser: Continue parsing / 继续解析
 *         Parser->>ST: define("label", symbol)
 *         ST->>Promise: set_value(symbol)
 *         Future-->>Parser: Resolution complete / 解析完成
 *     end
 * ```
 *
 * @subsection semantic_type_system Type System / 类型系统
 *
 * @if zh
 *
 * 类型由 [BasicType](@ref stationeers::ic10::BasicType) 和 [TypeCategory](@ref stationeers::ic10::TypeCategory) 组成，形成二维类型系统：
 *
 ** BasicType**: 表示基本数据类型
 * - `UNKNOWN`: 未知类型（解析错误或尚未确定）
 * - `INTEGER`: 整数类型
 * - `FLOAT`: 浮点类型
 * - `STRING`: 字符串类型
 * - `REGISTER`: 寄存器引用
 * - `DEVICE`: 设备引用
 * - `ENUM`: 枚举类型（自定义枚举值）
 *
 ** TypeCategory**: 表示类型的分类
 * - `CONSTANT`: 常量（值在编译期确定）
 * - `VARIABLE`: 变量（值在运行时确定）
 *
 ** Type 结构体定义**：
 *
 * ```cpp
 * struct Type {
 *     BasicType basicType;
 *     TypeCategory category;
 *
 *     bool operator==(const Type& other) const = default;
 *     bool operator!=(const Type& other) const = default;
 * };
 * ```
 *
 ** 类型推导规则**：
 * - 字面量 → CONSTANT（INTEGER/FLOAT/STRING）
 * - 寄存器引用 → VARIABLE（REGISTER）
 * - 设备引用 → VARIABLE（DEVICE）
 * - 枚举值 → CONSTANT（ENUM）
 *
 * @elseif en
 *
 * Types consist of [BasicType](@ref stationeers::ic10::BasicType) and [TypeCategory](@ref stationeers::ic10::TypeCategory), forming a two-dimensional type system:
 *
 * **BasicType**: Represents basic data types
 * - `UNKNOWN`: Unknown type (parse error or undetermined)
 * - `INTEGER`: Integer type
 * - `FLOAT`: Floating-point type
 * - `STRING`: String type
 * - `REGISTER`: Register reference
 * - `DEVICE`: Device reference
 * - `ENUM`: Enum type (custom enum values)
 *
 * **TypeCategory**: Represents type classification
 * - `CONSTANT`: Constant (value determined at compile time)
 * - `VARIABLE`: Variable (value determined at runtime)
 *
 * **Type Structure Definition**:
 *
 * ```cpp
 * struct Type {
 *     BasicType basicType;
 *     TypeCategory category;
 *
 *     bool operator==(const Type& other) const = default;
 *     bool operator!=(const Type& other) const = default;
 * };
 * ```
 *
 * **Type Inference Rules**:
 * - Literal → CONSTANT (INTEGER/FLOAT/STRING)
 * - Register reference → VARIABLE (REGISTER)
 * - Device reference → VARIABLE (DEVICE)
 * - Enum value → CONSTANT (ENUM)
 *
 * @endif
 *
 * | BasicType / 基本类型 | Description / 说明 | TypeCategory / 分类 | Example / 示例 |
 * |:-------------------|:------------------|:-------------------|:--------------|
 * | UNKNOWN | Unknown type | - | Parse error |
 * | INTEGER | Integer | CONSTANT | `42`, `$FF` |
 * | FLOAT | Floating-point | CONSTANT | `3.14` |
 * | STRING | String | CONSTANT | `"Hello"` |
 * | REGISTER | Register reference | VARIABLE | `r0`, `r15` |
 * | DEVICE | Device reference | VARIABLE | `d0`, `@Display` |
 * | ENUM | Enum value | CONSTANT | Custom enum |
 *
 * @section semantic_features Core Features / 核心特性
 *
 * @subsection semantic_forward_reference Forward Reference Handling / 前向引用处理
 *
 * @if zh
 *
 * 标签可以在定义之前被引用，通过协程异步等待符号定义：
 *
 * ```cpp
 * // 引用尚未定义的符号
 * auto& entry = symbolTable.resolve("target", pos);
 * auto symbol = co_await entry.future;
 *
 * // 使用符号
 * if (symbol) {
 *     // 符号已定义，正常使用
 * } else {
 *     // 符号未定义，生成错误
 *     reporter.report<UndefinedSymbolError>(pos, "target");
 * }
 * ```
 *
 ** 协程的优势**：
 * - 异步等待，不阻塞其他编译流程
 * - 代码简洁，类似同步代码
 * - 支持复杂的依赖关系
 *
 ** 前向引用场景**：
 * - 跳转指令引用尚未定义的标签
 * - 常量引用尚未定义的宏
 * - 设备别名引用尚未定义的设备
 *
 * @elseif en
 *
 * Labels can be referenced before definition, with coroutines asynchronously waiting for symbol definition:
 *
 * ```cpp
 * // Reference undefined symbol
 * auto& entry = symbolTable.resolve("target", pos);
 * auto symbol = co_await entry.future;
 *
 * // Use symbol
 * if (symbol) {
 *     // Symbol defined, use normally
 * } else {
 *     // Symbol undefined, generate error
 *     reporter.report<UndefinedSymbolError>(pos, "target");
 * }
 * ```
 *
 * **Coroutine Advantages**:
 * - Asynchronous waiting without blocking other compilation flows
 * - Clean code, similar to synchronous code
 * - Supports complex dependency relationships
 *
 * **Forward Reference Scenarios**:
 * - Jump instruction referencing undefined label
 * - Constant referencing undefined macro
 * - Device alias referencing undefined device
 *
 * @endif
 *
 * @subsection semantic_type_checking Type Checking / 类型检查
 *
 * @if zh
 *
 * Analyser 验证操作数类型与指令要求是否匹配。类型检查分为两个阶段：
 *
 ** 阶段一：操作数类型验证**
 * - 检查操作数是否符合指令定义的类型要求
 * - 例如 `add` 指令要求三个寄存器操作数
 *
 ** 阶段二：值范围验证**
 * - 检查寄存器编号是否在有效范围内（0-15）
 * - 检查设备编号是否在有效范围内（0-5）
 * - 检查枚举值是否在定义范围内
 *
 ** 类型检查示例**：
 *
 * ```cpp
 * // 检查操作数类型
 * void checkOperandTypes(const Instruction& ins) {
 *     const auto& expectedTypes = ins.getExpectedOperandTypes();
 *     const auto& operands = ins.getOperands();
 *
 *     for (size_t i = 0; i < operands.size(); ++i) {
 *         auto actualType = inferType(operands[i]);
 *         if (actualType != expectedTypes[i]) {
 *             reporter.report<TypeMismatchError>(
 *                 operands[i]->pos,
 *                 expectedTypes[i],
 *                 actualType
 *             );
 *         }
 *     }
 * }
 * ```
 *
 ** 常见类型错误**：
 * - 将设备传递给需要寄存器的指令
 * - 将字符串传递给需要数字的指令
 * - 使用超出范围的寄存器编号
 * - 使用未定义的枚举值
 *
 * @elseif en
 *
 * The Analyser verifies whether operand types match instruction requirements. Type checking has two phases:
 *
 * **Phase 1: Operand Type Validation**
 * - Check if operands match the type requirements defined by the instruction
 * - For example, `add` requires three register operands
 *
 * **Phase 2: Value Range Validation**
 * - Check if register number is within valid range (0-15)
 * - Check if device number is within valid range (0-5)
 * - Check if enum value is within defined range
 *
 * **Type Checking Example**:
 *
 * ```cpp
 * // Check operand types
 * void checkOperandTypes(const Instruction& ins) {
 *     const auto& expectedTypes = ins.getExpectedOperandTypes();
 *     const auto& operands = ins.getOperands();
 *
 *     for (size_t i = 0; i < operands.size(); ++i) {
 *         auto actualType = inferType(operands[i]);
 *         if (actualType != expectedTypes[i]) {
 *             reporter.report<TypeMismatchError>(
 *                 operands[i]->pos,
 *                 expectedTypes[i],
 *                 actualType
 *             );
 *         }
 *     }
 * }
 * ```
 *
 * **Common Type Errors**:
 * - Passing device to instruction expecting register
 * - Passing string to instruction expecting number
 * - Using register number out of range
 * - Using undefined enum value
 *
 * @endif
 *
 * @subsection semantic_redefinition Redefinition Detection / 重定义检测
 *
 * @if zh
 *
 * 检测重复定义的符号，生成 RedefinitionError：
 *
 * ```cpp
 * void defineSymbol(const std::string& name, std::shared_ptr<Symbol> symbol) {
 *     if (symbolTable.contains(name)) {
 *         auto& existing = symbolTable.resolve(name, {});
 *         reporter.report<RedefinitionError>(
 *             symbol->pos,
 *             name,
 *             existing.symbol->pos  // 原定义位置
 *         );
 *         return;
 *     }
 *     symbolTable.define(name, symbol);
 * }
 * ```
 *
 ** 重定义检测场景**：
 * - 标签重复定义
 * - 常量重复定义
 * - 设备别名重复定义
 *
 ** 重定义错误格式**：
 * - 显示新定义位置和原定义位置
 * - 帮助用户快速定位冲突
 *
 * @elseif en
 *
 * Detect redefined symbols and generate RedefinitionError:
 *
 * ```cpp
 * void defineSymbol(const std::string& name, std::shared_ptr<Symbol> symbol) {
 *     if (symbolTable.contains(name)) {
 *         auto& existing = symbolTable.resolve(name, {});
 *         reporter.report<RedefinitionError>(
 *             symbol->pos,
 *             name,
 *             existing.symbol->pos  // Original definition position
 *         );
 *         return;
 *     }
 *     symbolTable.define(name, symbol);
 * }
 * ```
 *
 * **Redefinition Detection Scenarios**:
 * - Label redefinition
 * - Constant redefinition
 * - Device alias redefinition
 *
 * **Redefinition Error Format**:
 * - Shows new definition position and original definition position
 * - Helps users quickly locate conflicts
 *
 * @endif
 *
 * @subsection semantic_device_validation Device Validation / 设备验证
 *
 * @if zh
 *
 * 语义分析器验证设备引用的正确性：
 *
 ** 设备存在性检查**：
 * - 检查设备别名是否已定义
 * - 检查设备编号是否有效（0-5）
 *
 ** 设备槽位检查**：
 * - 检查设备槽位编号是否有效
 * - 检查槽位方向是否匹配（输入/输出）
 *
 ** 设备连接检查**：
 * - 检查设备是否已连接到物理设备
 * - 检查连接是否符合设备规格
 *
 * @elseif en
 *
 * The Semantic Analyser validates device references:
 *
 * **Device Existence Check**:
 * - Check if device alias is defined
 * - Check if device number is valid (0-5)
 *
 * **Device Slot Check**:
 * - Check if device slot number is valid
 * - Check if slot direction matches (input/output)
 *
 * **Device Connection Check**:
 * - Check if device is connected to physical device
 * - Check if connection matches device specifications
 *
 * @endif
 *
 * @subsection semantic_doc_comment Doc Comment Integration / 文档注释集成
 *
 * @if zh
 *
 * 语义分析器使用 DocParser 提取的元数据进行类型检查：
 *
 ** 设备元数据使用**：
 * - 根据 `@device` 标签验证设备引用
 * - 根据 `@slot` 标签验证槽位访问
 * - 根据 `@connect` 标签验证连接关系
 *
 ** 枚举元数据使用**：
 * - 根据 `@enum` 标签验证枚举值
 * - 根据 `@value` 标签验证枚举范围
 *
 ** 文档注释与语义分析的关系**：
 * - 文档注释提供类型信息
 * - 语义分析使用这些信息进行验证
 * - 验证结果可以生成文档注释中的错误提示
 *
 * @elseif en
 *
 * The Semantic Analyser uses metadata extracted by DocParser for type checking:
 *
 * **Device Metadata Usage**:
 * - Validate device references based on `@device` tags
 * - Validate slot access based on `@slot` tags
 * - Validate connection relationships based on `@connect` tags
 *
 * **Enum Metadata Usage**:
 * - Validate enum values based on `@enum` tags
 * - Validate enum ranges based on `@value` tags
 *
 * **Relationship Between Doc Comments and Semantic Analysis**:
 * - Doc comments provide type information
 * - Semantic analysis uses this information for validation
 * - Validation results can generate error hints in doc comments
 *
 * @endif
 *
 * @section semantic_api API Reference / API 接口
 *
 * @if zh
 *
 * @code{.cpp}
 * #include "ic10_compiler/semantic/analyser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 创建分析器
 * Analyser analyser;
 *
 * // 协程语义分析
 * co_await analyser.analyse(program);
 *
 * // 获取诊断信息
 * auto diagnostics = analyser.diagnostics;
 *
 * // 获取符号表
 * auto& symbolTable = analyser.getSymbolTable();
 *
 * // 检查符号
 * if (symbolTable.contains("myLabel")) {
 *     auto symbol = symbolTable.resolve("myLabel", {}).symbol;
 * }
 * @endcode
 *
 ** 核心方法**：
 * | 方法 | 说明 |
 * |:-----|:-----|
 * | `analyse(program)` | 协程方法，执行语义分析 |
 * | `getSymbolTable()` | 获取符号表引用 |
 * | `inferType(operand)` | 推断操作数类型 |
 * | `checkInstruction(ins)` | 检查单个指令 |
 *
 ** 诊断消息**：
 * | 消息 ID | 说明 |
 * |:--------|:-----|
 * | IEA1 | 未定义符号 |
 * | IEA2 | 符号重定义 |
 * | IEA3 | 类型不匹配 |
 * | IEA4 | 寄存器超出范围 |
 * | IEA5 | 设备超出范围 |
 * | IEA6 | 枚举值无效 |
 *
 * @elseif en
 *
 * @code{.cpp}
 * #include "ic10_compiler/semantic/analyser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // Create analyser
 * Analyser analyser;
 *
 * // Coroutine semantic analysis
 * co_await analyser.analyse(program);
 *
 * // Get diagnostics
 * auto diagnostics = analyser.diagnostics;
 *
 * // Get symbol table
 * auto& symbolTable = analyser.getSymbolTable();
 *
 * // Check symbol
 * if (symbolTable.contains("myLabel")) {
 *     auto symbol = symbolTable.resolve("myLabel", {}).symbol;
 * }
 * @endcode
 *
 * **Core Methods**:
 * | Method | Description |
 * |:-------|:------------|
 * | `analyse(program)` | Coroutine method, perform semantic analysis |
 * | `getSymbolTable()` | Get symbol table reference |
 * | `inferType(operand)` | Infer operand type |
 * | `checkInstruction(ins)` | Check single instruction |
 *
 * **Diagnostic Messages**:
 * | Message ID | Description |
 * |:-----------|:------------|
 * | IEA1 | Undefined symbol |
 * | IEA2 | Symbol redefinition |
 * | IEA3 | Type mismatch |
 * | IEA4 | Register out of range |
 * | IEA5 | Device out of range |
 * | IEA6 | Invalid enum value |
 *
 * @endif
 *
 * @section semantic_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 ** 新增类型检查规则流程**：
 * 1. 在 `semantic.hpp` 中添加新的错误类型
 * 2. 在 `analyser.cpp` 中添加检查逻辑
 * 3. 在 `local.hpp` 中添加消息 ID
 * 4. 在语言文件中添加翻译
 *
 ** 协程使用注意事项**：
 * - 语义分析器是协程，调用者需要使用 `co_await`
 * - Future/Promise 模式需要正确处理生命周期
 * - 未定义符号的错误需要在编译结束时统一报告
 *
 ** 测试要点**：
 * - 前向引用测试（定义前引用）
 * - 重定义测试（重复定义）
 * - 类型检查测试（操作数类型不匹配）
 * - 设备验证测试（设备别名、槽位）
 * - 枚举验证测试（枚举值范围）
 * - 大型程序性能测试
 *
 ** 调试技巧**：
 * - 使用 `symbolTable.toString()` 输出符号表内容
 * - 在关键位置添加断点检查符号状态
 * - 使用协程调试工具跟踪异步流程
 *
 * @elseif en
 *
 * **Adding New Type Checking Rule**:
 * 1. Add new error type in `semantic.hpp`
 * 2. Add checking logic in `analyser.cpp`
 * 3. Add message ID in `local.hpp`
 * 4. Add translation in language files
 *
 * **Coroutine Usage Notes**:
 * - Semantic analyser is a coroutine, caller needs `co_await`
 * - Future/Promise pattern requires proper lifecycle management
 * - Undefined symbol errors should be reported at compilation end
 *
 * **Testing Checklist**:
 * - Forward reference tests (reference before definition)
 * - Redefinition tests (duplicate definitions)
 * - Type checking tests (operand type mismatch)
 * - Device validation tests (device aliases, slots)
 * - Enum validation tests (enum value ranges)
 * - Large program performance tests
 *
 * **Debugging Tips**:
 * - Use `symbolTable.toString()` to output symbol table contents
 * - Add breakpoints at key locations to check symbol state
 * - Use coroutine debugging tools to trace asynchronous flow
 *
 * @endif
 *
 * @see [Analyser](@ref stationeers::ic10::Analyser), [SymbolTable](@ref stationeers::ic10::SymbolTable), [Type](@ref stationeers::ic10::Type), [BasicType](@ref stationeers::ic10::BasicType)
 */

/**
 * @page ic10_linker_page Linker / 链接器
 * @tableofcontents
 *
 * @section linker_intro Introduction / 概述
 *
 * @if zh
 *
 * 链接器负责合并多个编译单元的符号表，解决跨单元的符号引用问题。
 * 在模块化编译场景中，Linker 确保所有符号都能被正确解析，实现编译单元之间的协作。
 *
 * 链接器是编译器的最后阶段，它将多个独立编译的单元整合成一个完整的可执行程序。
 *
 * @elseif en
 *
 * The Linker merges symbol tables from multiple compilation units and resolves cross-unit symbol references.
 * In modular compilation scenarios, the Linker ensures all symbols are properly resolved, enabling collaboration between compilation units.
 *
 * The Linker is the final stage of the compiler, integrating multiple independently compiled units into a complete executable program.
 *
 * @endif
 *
 * @section linker_architecture Architecture / 架构设计
 *
 * @subsection linker_symbol_merge Symbol Merge Strategy / 符号合并策略
 *
 * @if zh
 *
 * Linker 将多个编译单元的 SymbolTable 合并为全局符号表。合并过程遵循以下规则：
 *
 ** 符号合并规则**：
 * - 如果符号在多个单元中定义，报告重定义错误
 * - 如果符号只在一个单元中定义，其他单元引用它，解析为该定义
 * - 如果符号在所有单元中都未定义，报告未定义符号错误
 *
 ** 合并算法**：
 *
 * ```cpp
 * struct Linker {
 *     std::vector<CompilationUnit> units;
 *
 *     LinkResult link() {
 *         SymbolTable globalTable;
 *         std::vector<Diagnostic> diagnostics;
 *
 *         // 第一遍：收集所有符号定义
 *         for (const auto& unit : units) {
 *             for (const auto& [name, entry] : unit.symbolTable.entries) {
 *                 if (globalTable.contains(name)) {
 *                     diagnostics.push_back(RedefinitionError{
 *                         entry.symbol->pos,
 *                         name,
 *                         globalTable.resolve(name, {}).symbol->pos
 *                     });
 *                 } else {
 *                     globalTable.define(name, entry.symbol);
 *                 }
 *             }
 *         }
 *
 *         // 第二遍：解析跨单元引用
 *         for (const auto& unit : units) {
 *             for (const auto& [name, entry] : unit.symbolTable.entries) {
 *                 for (const auto& refPos : entry.references) {
 *                     if (!globalTable.contains(name)) {
 *                         diagnostics.push_back(UndefinedSymbolError{refPos, name});
 *                     }
 *                 }
 *             }
 *         }
 *
 *         return {std::move(globalTable), std::move(diagnostics)};
 *     }
 * };
 * ```
 *
 ** 两阶段合并的优势**：
 * - 第一阶段确保没有重复定义
 * - 第二阶段确保所有引用都能解析
 * - 诊断信息完整，包含所有错误位置
 *
 * @elseif en
 *
 * The Linker merges SymbolTables from multiple compilation units into a global symbol table. The merging process follows these rules:
 *
 * **Symbol Merge Rules**:
 * - If a symbol is defined in multiple units, report redefinition error
 * - If a symbol is defined in only one unit and referenced by others, resolve to that definition
 * - If a symbol is undefined in all units, report undefined symbol error
 *
 * **Merge Algorithm**:
 *
 * ```cpp
 * struct Linker {
 *     std::vector<CompilationUnit> units;
 *
 *     LinkResult link() {
 *         SymbolTable globalTable;
 *         std::vector<Diagnostic> diagnostics;
 *
 *         // Pass 1: Collect all symbol definitions
 *         for (const auto& unit : units) {
 *             for (const auto& [name, entry] : unit.symbolTable.entries) {
 *                 if (globalTable.contains(name)) {
 *                     diagnostics.push_back(RedefinitionError{
 *                         entry.symbol->pos,
 *                         name,
 *                         globalTable.resolve(name, {}).symbol->pos
 *                     });
 *                 } else {
 *                     globalTable.define(name, entry.symbol);
 *                 }
 *             }
 *         }
 *
 *         // Pass 2: Resolve cross-unit references
 *         for (const auto& unit : units) {
 *             for (const auto& [name, entry] : unit.symbolTable.entries) {
 *                 for (const auto& refPos : entry.references) {
 *                     if (!globalTable.contains(name)) {
 *                         diagnostics.push_back(UndefinedSymbolError{refPos, name});
 *                     }
 *                 }
 *             }
 *         }
 *
 *         return {std::move(globalTable), std::move(diagnostics)};
 *     }
 * };
 * ```
 *
 * **Two-Phase Merge Advantages**:
 * - First phase ensures no duplicate definitions
 * - Second phase ensures all references are resolved
 * - Complete diagnostic information with all error locations
 *
 * @endif
 *
 * ```mermaid
 * graph TD
 *     A[Unit 1 / 编译单元1] --> B[Symbol Table 1 / 符号表1]
 *     C[Unit 2 / 编译单元2] --> D[Symbol Table 2 / 符号表2]
 *     E[Unit 3 / 编译单元3] --> F[Symbol Table 3 / 符号表3]
 *     B --> G[Linker / 链接器]
 *     D --> G
 *     F --> G
 *     G --> H[Global Symbol Table / 全局符号表]
 *     G --> I[Diagnostic Aggregation / 诊断信息聚合]
 * ```
 *
 * @subsection linker_diagnostic_aggregation Diagnostic Aggregation / 诊断信息聚合
 *
 * @if zh
 *
 * Linker 收集所有编译单元的诊断信息，按严重程度排序：
 *
 ** 诊断信息分类**：
 * - **Error**: 致命错误，无法继续编译或链接
 * - **Warning**: 警告，不影响编译但可能导致运行时问题
 * - **Info**: 信息性提示，帮助用户了解编译过程
 *
 ** 聚合策略**：
 * - 收集所有单元的诊断信息
 * - 按严重程度排序（Error > Warning > Info）
 * - 按位置排序（文件内按行号排序）
 * - 去重相同错误
 *
 ** 诊断信息格式**：
 * ```
 * [文件名:行号:列号] 严重程度: 消息内容
 * ```
 *
 * @elseif en
 *
 * The Linker collects diagnostic information from all compilation units and sorts by severity:
 *
 * **Diagnostic Categories**:
 * - **Error**: Fatal error, compilation or linking cannot continue
 * - **Warning**: Warning, does not affect compilation but may cause runtime issues
 * - **Info**: Informational message, helps users understand the compilation process
 *
 * **Aggregation Strategy**:
 * - Collect diagnostics from all units
 * - Sort by severity (Error > Warning > Info)
 * - Sort by position (by line number within file)
 * - Deduplicate identical errors
 *
 * **Diagnostic Format**:
 * ```
 * [filename:line:column] Severity: Message
 * ```
 *
 * @endif
 *
 * @section linker_features Core Features / 核心特性
 *
 * @subsection linker_cross_unit_resolution Cross-Unit Symbol Resolution / 跨单元符号解析
 *
 * @if zh
 *
 * Linker 解决不同编译单元之间的符号引用，这是链接器的核心功能：
 *
 ** 跨单元引用场景**：
 * - 单元 A 定义标签，单元 B 跳转到该标签
 * - 单元 A 定义常量，单元 B 使用该常量
 * - 单元 A 定义设备别名，单元 B 使用该别名
 *
 ** 解析流程**：
 *
 * ```cpp
 * // 单元 A 定义符号
 * symbolTable.define("shared_label", symbol);
 *
 * // 单元 B 引用符号（编译时未解析）
 * auto& entry = symbolTable.resolve("shared_label", pos);
 * // entry.future 未决
 *
 * // Linker 合并后
 * // globalTable 包含 "shared_label" 的定义
 * // 所有对 "shared_label" 的引用都指向单元 A 的定义
 * ```
 *
 ** 跨单元解析的挑战**：
 * - 需要追踪每个符号的所有引用位置
 * - 需要处理循环引用（虽然 IC10 中不太常见）
 * - 需要生成准确的错误信息
 *
 * @elseif en
 *
 * The Linker resolves symbol references between different compilation units, the core function of the linker:
 *
 * **Cross-Unit Reference Scenarios**:
 * - Unit A defines a label, Unit B jumps to it
 * - Unit A defines a constant, Unit B uses it
 * - Unit A defines a device alias, Unit B uses it
 *
 * **Resolution Flow**:
 *
 * ```cpp
 * // Unit A defines symbol
 * symbolTable.define("shared_label", symbol);
 *
 * // Unit B references symbol (unresolved during compilation)
 * auto& entry = symbolTable.resolve("shared_label", pos);
 * // entry.future is pending
 *
 * // After Linker merge
 * // globalTable contains "shared_label" definition
 * // All references to "shared_label" point to Unit A's definition
 * ```
 *
 * **Cross-Unit Resolution Challenges**:
 * - Need to track all reference positions for each symbol
 * - Need to handle circular references (though rare in IC10)
 * - Need to generate accurate error messages
 *
 * @endif
 *
 * @subsection linker_symbol_visibility Symbol Visibility Control / 符号可见性控制
 *
 * @if zh
 *
 * Linker 支持符号可见性控制，区分公共符号和私有符号：
 *
 ** 公共符号（Public）**：
 * - 可被其他编译单元引用
 * - 默认可见性
 * - 用于共享的标签、常量、设备别名
 *
 ** 私有符号（Private）**：
 * - 仅在当前编译单元可见
 * - 使用 `static` 或特定语法标记
 * - 用于内部实现细节
 *
 ** 可见性控制的实现**：
 *
 * ```cpp
 * struct Symbol {
 *     std::string name;
 *     Type type;
 *     Pos pos;
 *     bool isPublic = true;  // 默认公共
 * };
 *
 * // 链接时检查可见性
 * void checkVisibility(const Symbol& symbol, const Pos& refPos) {
 *     if (!symbol.isPublic) {
 *         reporter.report<PrivateSymbolError>(refPos, symbol.name);
 *     }
 * }
 * ```
 *
 ** 可见性控制的意义**：
 * - 防止意外的符号冲突
 * - 控制接口暴露范围
 * - 支持模块化编程
 *
 * @elseif en
 *
 * The Linker supports symbol visibility control, distinguishing between public and private symbols:
 *
 * **Public Symbols**:
 * - Accessible by other compilation units
 * - Default visibility
 * - Used for shared labels, constants, device aliases
 *
 * **Private Symbols**:
 * - Visible only within the current compilation unit
 * - Marked with `static` or specific syntax
 * - Used for internal implementation details
 *
 * **Visibility Control Implementation**:
 *
 * ```cpp
 * struct Symbol {
 *     std::string name;
 *     Type type;
 *     Pos pos;
 *     bool isPublic = true;  // Default public
 * };
 *
 * // Check visibility during linking
 * void checkVisibility(const Symbol& symbol, const Pos& refPos) {
 *     if (!symbol.isPublic) {
 *         reporter.report<PrivateSymbolError>(refPos, symbol.name);
 *     }
 * }
 * ```
 *
 * **Visibility Control Significance**:
 * - Prevent accidental symbol conflicts
 * - Control interface exposure scope
 * - Support modular programming
 *
 * @endif
 *
 * @subsection linker_unit_dependency Unit Dependency Management / 单元依赖管理
 *
 * @if zh
 *
 * Linker 管理编译单元之间的依赖关系：
 *
 ** 依赖关系类型**：
 * - **符号依赖**：单元 A 使用单元 B 定义的符号
 * - **设备依赖**：单元 A 使用单元 B 定义的设备别名
 * - **常量依赖**：单元 A 使用单元 B 定义的常量
 *
 ** 依赖解析顺序**：
 * - 无循环依赖：按依赖顺序链接
 * - 有循环依赖：同时链接所有相关单元
 *
 ** 依赖检测**：
 *
 * ```cpp
 * // 构建依赖图
 * std::map<std::string, std::vector<std::string>> dependencyGraph;
 *
 * for (const auto& unit : units) {
 *     for (const auto& [name, entry] : unit.symbolTable.entries) {
 *         for (const auto& refPos : entry.references) {
 *             // 找到引用所在单元
 *             auto refUnit = findUnitByPosition(refPos);
 *             dependencyGraph[unit.name].push_back(refUnit.name);
 *         }
 *     }
 * }
 *
 * // 检测循环依赖
 * if (hasCycle(dependencyGraph)) {
 *     diagnostics.push_back(CircularDependencyError{dependencyGraph});
 * }
 * ```
 *
 ** 循环依赖处理**：
 * - 报告循环依赖错误
 * - 建议用户重构代码
 * - 提供循环依赖路径
 *
 * @elseif en
 *
 * The Linker manages dependencies between compilation units:
 *
 * **Dependency Types**:
 * - **Symbol Dependency**: Unit A uses symbols defined by Unit B
 * - **Device Dependency**: Unit A uses device aliases defined by Unit B
 * - **Constant Dependency**: Unit A uses constants defined by Unit B
 *
 * **Dependency Resolution Order**:
 * - No circular dependency: Link in dependency order
 * - With circular dependency: Link all related units simultaneously
 *
 * **Dependency Detection**:
 *
 * ```cpp
 * // Build dependency graph
 * std::map<std::string, std::vector<std::string>> dependencyGraph;
 *
 * for (const auto& unit : units) {
 *     for (const auto& [name, entry] : unit.symbolTable.entries) {
 *         for (const auto& refPos : entry.references) {
 *             // Find unit containing the reference
 *             auto refUnit = findUnitByPosition(refPos);
 *             dependencyGraph[unit.name].push_back(refUnit.name);
 *         }
 *     }
 * }
 *
 * // Detect circular dependencies
 * if (hasCycle(dependencyGraph)) {
 *     diagnostics.push_back(CircularDependencyError{dependencyGraph});
 * }
 * ```
 *
 * **Circular Dependency Handling**:
 * - Report circular dependency error
 * - Suggest users refactor code
 * - Provide circular dependency path
 *
 * @endif
 *
 * @section linker_api API Reference / API 接口
 *
 * @if zh
 *
 * @code{.cpp}
 * #include "ic10_compiler/linker/linker.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 创建链接器
 * Linker linker;
 *
 * // 添加编译单元
 * linker.addUnit(unit1);
 * linker.addUnit(unit2);
 *
 * // 执行链接
 * auto result = linker.link();
 *
 * // 获取全局符号表和诊断信息
 * auto globalSymbols = result.symbolTable;
 * auto diagnostics = result.diagnostics;
 *
 * // 检查链接结果
 * if (result.success) {
 *     // 链接成功
 * } else {
 *     // 链接失败，处理错误
 * }
 * @endcode
 *
 ** 核心方法**：
 * | 方法 | 说明 |
 * |:-----|:-----|
 * | `addUnit(unit)` | 添加编译单元 |
 * | `link()` | 执行链接，返回链接结果 |
 * | `getGlobalSymbolTable()` | 获取全局符号表 |
 * | `getDiagnostics()` | 获取所有诊断信息 |
 *
 ** 链接结果结构**：
 * ```cpp
 * struct LinkResult {
 *     SymbolTable symbolTable;    // 全局符号表
 *     std::vector<Diagnostic> diagnostics;  // 诊断信息
 *     bool success;              // 链接是否成功
 * };
 * ```
 *
 ** 诊断消息**：
 * | 消息 ID | 说明 |
 * |:--------|:-----|
 * | ILL1 | 跨单元符号未定义 |
 * | ILL2 | 跨单元符号重定义 |
 * | ILL3 | 私有符号被外部引用 |
 * | ILL4 | 循环依赖 |
 *
 * @elseif en
 *
 * @code{.cpp}
 * #include "ic10_compiler/linker/linker.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // Create linker
 * Linker linker;
 *
 * // Add compilation units
 * linker.addUnit(unit1);
 * linker.addUnit(unit2);
 *
 * // Perform linking
 * auto result = linker.link();
 *
 * // Get global symbol table and diagnostics
 * auto globalSymbols = result.symbolTable;
 * auto diagnostics = result.diagnostics;
 *
 * // Check link result
 * if (result.success) {
 *     // Link succeeded
 * } else {
 *     // Link failed, handle errors
 * }
 * @endcode
 *
 * **Core Methods**:
 * | Method | Description |
 * |:-------|:------------|
 * | `addUnit(unit)` | Add compilation unit |
 * | `link()` | Perform linking, return result |
 * | `getGlobalSymbolTable()` | Get global symbol table |
 * | `getDiagnostics()` | Get all diagnostics |
 *
 * **Link Result Structure**:
 * ```cpp
 * struct LinkResult {
 *     SymbolTable symbolTable;    // Global symbol table
 *     std::vector<Diagnostic> diagnostics;  // Diagnostics
 *     bool success;              // Whether linking succeeded
 * };
 * ```
 *
 * **Diagnostic Messages**:
 * | Message ID | Description |
 * |:-----------|:------------|
 * | ILL1 | Undefined cross-unit symbol |
 * | ILL2 | Cross-unit symbol redefinition |
 * | ILL3 | Private symbol referenced externally |
 * | ILL4 | Circular dependency |
 *
 * @endif
 *
 * @section linker_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 ** 新增链接规则流程**：
 * 1. 在 `linker.hpp` 中添加新的错误类型
 * 2. 在 `linker.cpp` 中添加链接逻辑
 * 3. 在 `local.hpp` 中添加消息 ID
 * 4. 在语言文件中添加翻译
 *
 ** 编译单元设计注意事项**：
 * - 每个编译单元应该独立可编译
 * - 避免循环依赖
 * - 明确符号的可见性
 *
 ** 测试要点**：
 * - 跨单元符号引用测试
 * - 符号重定义测试
 * - 符号可见性测试
 * - 循环依赖测试
 * - 大型项目链接性能测试
 *
 ** 调试技巧**：
 * - 使用 `linker.dump()` 输出链接过程
 * - 检查每个单元的符号表内容
 * - 查看全局符号表的合并结果
 *
 * @elseif en
 *
 * **Adding New Linking Rule**:
 * 1. Add new error type in `linker.hpp`
 * 2. Add linking logic in `linker.cpp`
 * 3. Add message ID in `local.hpp`
 * 4. Add translation in language files
 *
 * **Compilation Unit Design Notes**:
 * - Each compilation unit should be independently compilable
 * - Avoid circular dependencies
 * - Explicitly define symbol visibility
 *
 * **Testing Checklist**:
 * - Cross-unit symbol reference tests
 * - Symbol redefinition tests
 * - Symbol visibility tests
 * - Circular dependency tests
 * - Large project linking performance tests
 *
 * **Debugging Tips**:
 * - Use `linker.dump()` to output linking process
 * - Check symbol table contents of each unit
 * - View global symbol table merge results
 *
 * @endif
 *
 * @see [Linker](@ref stationeers::ic10::Linker)
 */

/**
 * @page ic10_incremental_page Incremental Compilation / 增量编译
 * @tableofcontents
 *
 * @section incremental_intro Introduction / 概述
 *
 * @if zh
 *
 * 增量编译模块通过缓存机制，仅重新处理发生变化的代码部分，显著提升编辑场景下的编译性能。
 * 在代码编辑器中，用户每次修改代码时，增量编译可以避免重新编译整个文件，只处理变化的部分。
 *
 * @elseif en
 *
 * The incremental compilation module uses caching mechanisms to only reprocess changed code portions, significantly improving compilation performance in editing scenarios.
 * In code editors, when users modify code, incremental compilation avoids recompiling the entire file and only processes changed parts.
 *
 * @endif
 *
 * @section incremental_architecture Architecture / 架构设计
 *
 * @subsection incremental_lexer Incremental Lexer / 增量词法分析
 *
 * @if zh
 *
 * [IncLexer](@ref stationeers::ic10::IncLexer) 使用行级缓存，仅重新分析修改的行：
 *
 ** 缓存结构**：
 *
 * ```cpp
 * struct IncLexer {
 *     std::vector<std::vector<std::shared_ptr<Token>>> lineCache;
 *     std::string lastSource;
 *
 *     IncLexerResult tokenizeInc(std::string_view newSource) {
 *         // 1. 检测变化的行
 *         auto changedLines = detectChangedLines(lastSource, newSource);
 *
 *         // 2. 重新词法分析变化的行
 *         for (auto lineNum : changedLines) {
 *             lineCache[lineNum] = lexLine(getLine(newSource, lineNum));
 *         }
 *
 *         // 3. 合并缓存结果
 *         lastSource = std::string(newSource);
 *         return mergeCache();
 *     }
 * };
 * ```
 *
 ** 行级缓存的优势**：
 * - 大多数编辑只影响少数几行
 * - 缓存命中率高
 * - 位置信息保持准确
 *
 ** 差异检测算法**：
 * - 使用基于行的哈希比较
 * - 检测新增、删除、修改的行
 * - 处理行号偏移
 *
 * @elseif en
 *
 * [IncLexer](@ref stationeers::ic10::IncLexer) uses line-level caching, only re-analyzing modified lines:
 *
 * **Cache Structure**:
 *
 * ```cpp
 * struct IncLexer {
 *     std::vector<std::vector<std::shared_ptr<Token>>> lineCache;
 *     std::string lastSource;
 *
 *     IncLexerResult tokenizeInc(std::string_view newSource) {
 *         // 1. Detect changed lines
 *         auto changedLines = detectChangedLines(lastSource, newSource);
 *
 *         // 2. Re-lex changed lines
 *         for (auto lineNum : changedLines) {
 *             lineCache[lineNum] = lexLine(getLine(newSource, lineNum));
 *         }
 *
 *         // 3. Merge cached results
 *         lastSource = std::string(newSource);
 *         return mergeCache();
 *     }
 * };
 * ```
 *
 * **Line-Level Cache Advantages**:
 * - Most edits only affect a few lines
 * - High cache hit rate
 * - Position information remains accurate
 *
 * **Diff Detection Algorithm**:
 * - Uses line-based hash comparison
 * - Detects added, deleted, modified lines
 * - Handles line number offsets
 *
 * @endif
 *
 * ```mermaid
 * graph TD
 *     A[New Source / 新源码] --> B[Diff Detection / 差异检测]
 *     B --> C{Line Changed? / 行是否变化?}
 *     C -->|Yes / 是| D[Re-lex / 重新词法分析]
 *     C -->|No / 否| E[Use Cache / 使用缓存]
 *     D --> F[Update Cache / 更新缓存]
 *     E --> G[Merge Results / 合并结果]
 *     F --> G
 *     G --> H[Token Sequence / Token序列]
 * ```
 *
 * @subsection incremental_parser Incremental Parser / 增量语法分析
 *
 * @if zh
 *
 * [IncParser](@ref stationeers::ic10::IncParser) 在语句级别进行缓存，保持 AST 节点的连续性：
 *
 ** 缓存策略**：
 *
 * ```cpp
 * struct IncParser {
 *     std::vector<std::shared_ptr<Statement>> statementCache;
 *     SymbolTable symbolTableCache;
 *
 *     IncParseResult parseInc(const std::vector<std::shared_ptr<Token>>& tokens) {
 *         // 1. 检测变化的语句边界
 *         auto changedStmts = detectChangedStatements(tokens);
 *
 *         // 2. 重新解析变化的语句
 *         for (auto stmtIdx : changedStmts) {
 *             statementCache[stmtIdx] = parseStatementAt(tokens, stmtIdx);
 *         }
 *
 *         // 3. 增量更新符号表
 *         updateSymbolTable(changedStmts);
 *
 *         return {statementCache, symbolTableCache};
 *     }
 * };
 * ```
 *
 ** 语句级缓存的优势**：
 * - 未修改的语句直接复用原有 AST 节点
 * - 修改的语句重新解析，生成新节点
 * - 保持符号表和诊断信息的增量更新
 * - 支持细粒度的增量更新
 *
 ** 语句边界检测**：
 * - 基于 NEWLINE Token 确定语句边界
 * - 检测新增、删除、修改的语句
 * - 处理语句依赖关系
 *
 * @elseif en
 *
 * [IncParser](@ref stationeers::ic10::IncParser) caches at the statement level, maintaining AST node continuity:
 *
 * **Cache Strategy**:
 *
 * ```cpp
 * struct IncParser {
 *     std::vector<std::shared_ptr<Statement>> statementCache;
 *     SymbolTable symbolTableCache;
 *
 *     IncParseResult parseInc(const std::vector<std::shared_ptr<Token>>& tokens) {
 *         // 1. Detect changed statement boundaries
 *         auto changedStmts = detectChangedStatements(tokens);
 *
 *         // 2. Re-parse changed statements
 *         for (auto stmtIdx : changedStmts) {
 *             statementCache[stmtIdx] = parseStatementAt(tokens, stmtIdx);
 *         }
 *
 *         // 3. Incrementally update symbol table
 *         updateSymbolTable(changedStmts);
 *
 *         return {statementCache, symbolTableCache};
 *     }
 * };
 * ```
 *
 * **Statement-Level Cache Advantages**:
 * - Unmodified statements reuse existing AST nodes
 * - Modified statements are re-parsed, generating new nodes
 * - Incremental updates to symbol table and diagnostics
 * - Fine-grained incremental updates
 *
 * **Statement Boundary Detection**:
 * - Determines statement boundaries based on NEWLINE tokens
 * - Detects added, deleted, modified statements
 * - Handles statement dependencies
 *
 * @endif
 *
 * @section incremental_features Core Features / 核心特性
 *
 * @subsection incremental_diff_algorithm Diff Algorithm / 差异算法
 *
 * @if zh
 *
 * 增量模块使用高效的差异算法检测源码变化：
 *
 ** 基于行的比较**：
 * - 将源码按行分割
 * - 计算每行的哈希值
 * - 比较哈希值检测变化
 *
 ** 最小化重新处理范围**：
 * - 只重新处理变化的行/语句
 * - 避免级联重新处理
 * - 保持位置信息的准确性
 *
 ** 位置信息维护**：
 * - 跟踪行号偏移
 * - 维护 Token 的位置信息
 * - 确保诊断信息的准确性
 *
 ** 差异算法复杂度**：
 * - 时间复杂度：O(n)，n 为行数
 * - 空间复杂度：O(n)，缓存所有行的 Token
 *
 * @elseif en
 *
 * The incremental module uses efficient diff algorithms to detect source code changes:
 *
 * **Line-Based Comparison**:
 * - Split source code by lines
 * - Calculate hash value for each line
 * - Compare hashes to detect changes
 *
 * **Minimize Reprocessing Scope**:
 * - Only reprocess changed lines/statements
 * - Avoid cascading reprocessing
 * - Maintain position information accuracy
 *
 * **Position Information Maintenance**:
 * - Track line number offsets
 * - Maintain token position information
 * - Ensure diagnostic accuracy
 *
 * **Diff Algorithm Complexity**:
 * - Time Complexity: O(n), where n is the number of lines
 * - Space Complexity: O(n), caching tokens for all lines
 *
 * @endif
 *
 * @subsection incremental_performance Performance Optimization / 性能优化
 *
 * @if zh
 *
 * 增量编译的性能优势：
 *
 ** 减少重复计算**：
 * - 未修改的代码不需要重新编译
 * - 缓存结果直接复用
 * - 只计算变化的部分
 *
 ** 保持编辑器响应性**：
 * - 编译时间与修改量成正比
 * - 大文件编辑也能快速响应
 * - 支持实时语法高亮和错误提示
 *
 ** 支持实时反馈**：
 * - 每输入一个字符都可以触发增量编译
 * - 实时显示语法错误
 * - 即时语法高亮更新
 *
 ** 性能对比**：
 * | 场景 | 全量编译 | 增量编译 |
 * |:-----|:---------|:---------|
 * | 首次编译 | O(n) | O(n) |
 * | 修改一行 | O(n) | O(1) |
 * | 修改多行 | O(n) | O(k), k为修改行数 |
 *
 * @elseif en
 *
 * Performance advantages of incremental compilation:
 *
 * **Reduce Redundant Computation**:
 * - Unmodified code doesn't need recompilation
 * - Cache results are directly reused
 * - Only compute changed parts
 *
 * **Maintain Editor Responsiveness**:
 * - Compilation time proportional to changes
 * - Fast response even for large files
 * - Supports real-time syntax highlighting and error hints
 *
 * **Support Real-Time Feedback**:
 * - Incremental compilation can be triggered on every character input
 * - Real-time syntax error display
 * - Instant syntax highlighting updates
 *
 * **Performance Comparison**:
 * | Scenario | Full Compilation | Incremental Compilation |
 * |:---------|:----------------|:------------------------|
 * | First compile | O(n) | O(n) |
 * | Modify one line | O(n) | O(1) |
 * | Modify multiple lines | O(n) | O(k), k = number of modified lines |
 *
 * @endif
 *
 * @subsection incremental_cache_invalidation Cache Invalidation / 缓存失效
 *
 * @if zh
 *
 * 增量编译需要正确处理缓存失效：
 *
 ** 失效触发条件**：
 * - 源码内容变化
 * - 编译选项变化
 * - 依赖文件变化
 * - 缓存版本不匹配
 *
 ** 失效策略**：
 *
 * ```cpp
 * void invalidateCache() {
 *     lineCache.clear();
 *     statementCache.clear();
 *     symbolTableCache.clear();
 *     lastSource.clear();
 * }
 * ```
 *
 ** 部分失效**：
 * - 只失效受影响的缓存部分
 * - 避免完全失效导致性能下降
 * - 智能判断失效范围
 *
 ** 缓存一致性**：
 * - 确保缓存与源码同步
 * - 检测缓存不一致并自动修复
 * - 提供手动失效接口
 *
 * @elseif en
 *
 * Incremental compilation requires proper cache invalidation handling:
 *
 * **Invalidation Triggers**:
 * - Source code content changes
 * - Compilation option changes
 * - Dependency file changes
 * - Cache version mismatch
 *
 * **Invalidation Strategy**:
 *
 * ```cpp
 * void invalidateCache() {
 *     lineCache.clear();
 *     statementCache.clear();
 *     symbolTableCache.clear();
 *     lastSource.clear();
 * }
 * ```
 *
 * **Partial Invalidation**:
 * - Only invalidate affected cache portions
 * - Avoid full invalidation causing performance degradation
 * - Smart determination of invalidation scope
 *
 * **Cache Consistency**:
 * - Ensure cache synchronization with source code
 * - Detect cache inconsistency and auto-fix
 * - Provide manual invalidation interface
 *
 * @endif
 *
 * @section incremental_api API Reference / API 接口
 *
 * @if zh
 *
 * @code{.cpp}
 * #include "ic10_compiler/incremental/inc_lexer.hpp"
 * #include "ic10_compiler/incremental/inc_parser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 增量词法分析
 * IncLexer incLexer;
 * auto result = incLexer.tokenizeInc(newSource);
 *
 * // 获取增量统计
 * std::cout << "Changed lines: " << result.changedLines.size() << std::endl;
 * std::cout << "Total tokens: " << result.tokens.size() << std::endl;
 *
 * // 增量语法分析
 * IncParser incParser;
 * auto ast = incParser.parseInc(tokens);
 *
 * // 失效缓存
 * incLexer.invalidateCache();
 * incParser.invalidateCache();
 * @endcode
 *
 ** 核心方法**：
 * | 方法 | 说明 |
 * |:-----|:-----|
 * | `tokenizeInc(source)` | 增量词法分析 |
 * | `parseInc(tokens)` | 增量语法分析 |
 * | `invalidateCache()` | 失效缓存 |
 * | `getCacheStats()` | 获取缓存统计 |
 *
 ** 增量结果结构**：
 * ```cpp
 * struct IncLexerResult {
 *     std::vector<std::shared_ptr<Token>> tokens;
 *     std::vector<size_t> changedLines;
 *     size_t cachedTokens;
 *     size_t newTokens;
 * };
 * ```
 *
 * @elseif en
 *
 * @code{.cpp}
 * #include "ic10_compiler/incremental/inc_lexer.hpp"
 * #include "ic10_compiler/incremental/inc_parser.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // Incremental lexer
 * IncLexer incLexer;
 * auto result = incLexer.tokenizeInc(newSource);
 *
 * // Get incremental statistics
 * std::cout << "Changed lines: " << result.changedLines.size() << std::endl;
 * std::cout << "Total tokens: " << result.tokens.size() << std::endl;
 *
 * // Incremental parser
 * IncParser incParser;
 * auto ast = incParser.parseInc(tokens);
 *
 * // Invalidate cache
 * incLexer.invalidateCache();
 * incParser.invalidateCache();
 * @endcode
 *
 * **Core Methods**:
 * | Method | Description |
 * |:-------|:------------|
 * | `tokenizeInc(source)` | Incremental lexer |
 * | `parseInc(tokens)` | Incremental parser |
 * | `invalidateCache()` | Invalidate cache |
 * | `getCacheStats()` | Get cache statistics |
 *
 * **Incremental Result Structure**:
 * ```cpp
 * struct IncLexerResult {
 *     std::vector<std::shared_ptr<Token>> tokens;
 *     std::vector<size_t> changedLines;
 *     size_t cachedTokens;
 *     size_t newTokens;
 * };
 * ```
 *
 * @endif
 *
 * @section incremental_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 ** 缓存设计注意事项**：
 * - 缓存粒度要合适，过粗导致浪费，过细导致复杂
 * - 位置信息必须准确，否则诊断信息错误
 * - 失效策略要完善，避免缓存不一致
 *
 ** 性能优化建议**：
 * - 使用高效的哈希算法
 * - 预分配缓存容量
 * - 使用 move 语义避免拷贝
 *
 ** 测试要点**：
 * - 单行修改测试
 * - 多行修改测试
 * - 删除行测试
 * - 插入行测试
 * - 缓存失效测试
 * - 性能基准测试
 *
 ** 调试技巧**：
 * - 使用 `getCacheStats()` 查看缓存状态
 * - 打印变化的行号列表
 * - 对比全量编译和增量编译的结果
 *
 * @elseif en
 *
 * **Cache Design Notes**:
 * - Cache granularity must be appropriate - too coarse causes waste, too fine causes complexity
 * - Position information must be accurate, otherwise diagnostic messages are wrong
 * - Invalidation strategy must be complete to avoid cache inconsistency
 *
 * **Performance Optimization Tips**:
 * - Use efficient hash algorithms
 * - Pre-allocate cache capacity
 * - Use move semantics to avoid copies
 *
 * **Testing Checklist**:
 * - Single line modification tests
 * - Multi-line modification tests
 * - Line deletion tests
 * - Line insertion tests
 * - Cache invalidation tests
 * - Performance benchmark tests
 *
 * **Debugging Tips**:
 * - Use `getCacheStats()` to view cache status
 * - Print changed line number lists
 * - Compare full compilation vs incremental compilation results
 *
 * @endif
 *
 * @see [IncLexer](@ref stationeers::ic10::IncLexer), [IncParser](@ref stationeers::ic10::IncParser)
 */

/**
 * @page ic10_locals_page Localization / 本地化
 * @tableofcontents
 *
 * @section locals_intro Introduction / 概述
 *
 * @if zh
 *
 * 本地化模块提供多语言诊断消息支持，使编译器能够输出用户熟悉语言的错误信息。
 * 通过统一的消息 ID 体系和语言文件结构，实现编译错误消息的多语言支持。
 *
 * @elseif en
 *
 * The localization module provides multi-language diagnostic message support, enabling the compiler to output error messages in the user's familiar language.
 * Through a unified message ID system and language file structure, multi-language support for compilation error messages is achieved.
 *
 * @endif
 *
 * @section locals_architecture Architecture / 架构设计
 *
 * @subsection locals_message_id Message ID System / 消息 ID 体系
 *
 * @if zh
 *
 * 每个诊断消息有唯一的 ID，格式为 `I` + 模块前缀 + 编号：
 *
 ** 消息 ID 格式**：
 * - `I`: IC10 的缩写前缀
 * - **模块前缀**: 两位字母，表示消息所属模块
 * - **编号**: 数字，同一模块内唯一
 *
 ** 模块前缀定义**：
 *
 * @elseif en
 *
 * Each diagnostic message has a unique ID, formatted as `I` + module prefix + number:
 *
 * **Message ID Format**:
 * - `I`: IC10 abbreviation prefix
 * - **Module Prefix**: Two letters indicating the module
 * - **Number**: Numeric identifier, unique within the module
 *
 * **Module Prefix Definitions**:
 *
 * @endif
 *
 * | Prefix / 前缀 | Module / 模块 | Description / 说明 | Examples / 示例 |
 * |:-------------|:-------------|:------------------|:---------------|
 * | WL | Lexer | Lexical analysis / 词法分析 | IWL1, IWL2, IWL3, IWL4 |
 * | PL | Parser | Syntax analysis / 语法分析 | IPL1, IPL2, IPL3, IPL4 |
 * | EA | Semantic | Semantic analysis / 语义分析 | IEA1, IEA2, IEA3 |
 * | LL | Linker | Linker / 链接器 | ILL1, ILL2, ILL3 |
 *
 * @if zh
 *
 ** 消息 ID 分配规则**：
 * - 按模块分配，避免重复
 * - 预留扩展空间
 * - 按功能分类编号
 *
 ** 消息严重程度**：
 * - **Error** (E): 致命错误，无法继续编译
 * - **Warning** (W): 警告，不影响编译但可能有问题
 * - **Info** (I): 信息性提示
 *
 * @elseif en
 *
 * **Message ID Allocation Rules**:
 * - Allocated by module to avoid duplication
 * - Reserved extension space
 * - Numbered by function category
 *
 * **Message Severity**:
 * - **Error** (E): Fatal error, compilation cannot continue
 * - **Warning** (W): Warning, doesn't affect compilation but may indicate issues
 * - **Info** (I): Informational message
 *
 * @endif
 *
 * @subsection locals_language_files Language File Structure / 语言文件结构
 *
 * @if zh
 *
 * 每种语言对应一个头文件，使用宏注册消息。语言文件位于 `locals/languages/` 目录下：
 *
 ** 语言文件命名规范**：
 * - `zh_hans.hpp`: 中文(简体)
 * - `en_us.hpp`: 英文(美国)
 * - 命名格式：`语言代码_地区代码.hpp`
 *
 ** 消息注册宏**：
 *
 * ```cpp
 * // 宏定义
 * #define IC10_LOCAL_MSG_ZH_HANS(id, msg) \
 *     template<> const char* IC10MsgPack<MsgId::id>::msg = msg;
 *
 * // 使用示例
 * IC10_LOCAL_MSG_ZH_HANS(IWL1, "非法字符 '{}' - 词法分析器")
 * IC10_LOCAL_MSG_ZH_HANS(IWL2, "寄存器超出范围(r0-r15)")
 * IC10_LOCAL_MSG_ZH_HANS(IWL3, "设备超出范围(d0-d5)")
 * IC10_LOCAL_MSG_ZH_HANS(IWL4, "字符串未闭合")
 * ```
 *
 ** 宏展开原理**：
 * - 使用模板特化实现类型安全的消息查找
 * - 编译期检查消息 ID 是否存在
 * - 零运行时开销
 *
 ** 语言文件结构**：
 *
 * ```
 * locals/
 * ├── local.hpp              # 本地化核心接口
 * ├── msg_id.hpp             # 消息 ID 枚举定义
 * └── languages/
 *     ├── zh_hans.hpp        # 中文(简体)消息
 *     ├── en_us.hpp          # 英文(美国)消息
 *     └── <lang_code>_<region>.hpp  # 其他语言
 * ```
 *
 * @elseif en
 *
 * Each language corresponds to a header file, using macros to register messages. Language files are located in the `locals/languages/` directory:
 *
 * **Language File Naming Convention**:
 * - `zh_hans.hpp`: Chinese (Simplified)
 * - `en_us.hpp`: English (US)
 * - Format: `language_code_region_code.hpp`
 *
 * **Message Registration Macro**:
 *
 * ```cpp
 * // Macro definition
 * #define IC10_LOCAL_MSG_ZH_HANS(id, msg) \
 *     template<> const char* IC10MsgPack<MsgId::id>::msg = msg;
 *
 * // Usage example
 * IC10_LOCAL_MSG_ZH_HANS(IWL1, "Invalid character '{}' - lexical")
 * IC10_LOCAL_MSG_ZH_HANS(IWL2, "Register out of range (r0-r15)")
 * IC10_LOCAL_MSG_ZH_HANS(IWL3, "Device out of range (d0-d5)")
 * IC10_LOCAL_MSG_ZH_HANS(IWL4, "Unclosed string")
 * ```
 *
 * **Macro Expansion Principle**:
 * - Uses template specialization for type-safe message lookup
 * - Compile-time check for message ID existence
 * - Zero runtime overhead
 *
 * **Language File Structure**:
 *
 * ```
 * locals/
 * ├── local.hpp              # Core localization interface
 * ├── msg_id.hpp             # Message ID enum definition
 * └── languages/
 *     ├── zh_hans.hpp        # Chinese (Simplified) messages
 *     ├── en_us.hpp          # English (US) messages
 *     └── <lang_code>_<region>.hpp  # Other languages
 * ```
 *
 * @endif
 *
 * @section locals_features Core Features / 核心特性
 *
 * @subsection locals_dynamic_switch Dynamic Language Switching / 动态语言切换
 *
 * @if zh
 *
 * 运行时可切换语言，无需重新编译：
 *
 ** 语言切换接口**：
 *
 * ```cpp
 * // 设置语言
 * IC10Local::setLanguage("zh-hans");  // 切换到简体中文
 * IC10Local::setLanguage("en-us");    // 切换到英文
 *
 * // 获取当前语言
 * auto lang = IC10Local::getLanguage();  // 返回 "zh-hans" 或 "en-us"
 * ```
 *
 ** 语言代码格式**：
 * - 使用 BCP 47 语言标签格式
 * - `zh-hans`: 中文(简体)
 * - `en-us`: 英文(美国)
 * - 格式：`语言代码-地区代码`
 *
 ** 语言切换的实现**：
 * - 使用线程本地存储或全局状态
 * - 消息查找时根据当前语言选择对应翻译
 * - 支持热切换，无需重启程序
 *
 ** 默认语言**：
 * - 如果未设置语言，使用系统默认语言
 * - 如果系统语言不支持，回退到英文
 *
 * @elseif en
 *
 * Language can be switched at runtime without recompilation:
 *
 * **Language Switching Interface**:
 *
 * ```cpp
 * // Set language
 * IC10Local::setLanguage("zh-hans");  // Switch to Simplified Chinese
 * IC10Local::setLanguage("en-us");    // Switch to English
 *
 * // Get current language
 * auto lang = IC10Local::getLanguage();  // Returns "zh-hans" or "en-us"
 * ```
 *
 * **Language Code Format**:
 * - Uses BCP 47 language tag format
 * - `zh-hans`: Chinese (Simplified)
 * - `en-us`: English (US)
 * - Format: `language_code-region_code`
 *
 * **Language Switching Implementation**:
 * - Uses thread-local storage or global state
 * - Message lookup selects corresponding translation based on current language
 * - Supports hot switching without program restart
 *
 * **Default Language**:
 * - If no language is set, use system default language
 * - If system language is not supported, fall back to English
 *
 * @endif
 *
 * @subsection locals_parameterized_messages Parameterized Messages / 参数化消息
 *
 * @if zh
 *
 * 消息支持占位符，运行时填充具体值：
 *
 ** 占位符格式**：
 * - `{}`: 位置占位符，按顺序填充
 * - 支持多个占位符
 * - 支持字符串、数字等类型
 *
 ** 使用示例**：
 *
 * ```cpp
 * // 消息定义
 * IC10_LOCAL_MSG_ZH_HANS(IEA1_2, "期望 '{}'，但发现 '{}'")
 *
 * // 使用
 * reporter.report<SemanticError>(pos, expected, found);
 * // 输出: "期望 'add'，但发现 'ad'"
 * ```
 *
 ** 占位符替换机制**：
 *
 * ```cpp
 * std::string formatMessage(const char* msg, std::initializer_list<std::string> args) {
 *     std::string result = msg;
 *     size_t idx = 0;
 *     for (const auto& arg : args) {
 *         size_t pos = result.find("{}");
 *         if (pos != std::string::npos) {
 *             result.replace(pos, 2, arg);
 *         }
 *         idx++;
 *     }
 *     return result;
 * }
 * ```
 *
 ** 参数类型支持**：
 * - 字符串：直接替换
 * - 数字：自动转换为字符串
 * - 枚举值：转换为对应的字符串表示
 * - 位置信息：转换为 "行:列" 格式
 *
 ** 参数化消息的优势**：
 * - 支持复杂的错误消息
 * - 消息格式统一
 * - 翻译更灵活
 *
 * @elseif en
 *
 * Messages support placeholders that are filled with specific values at runtime:
 *
 * **Placeholder Format**:
 * - `{}`: Position placeholder, filled in order
 * - Supports multiple placeholders
 * - Supports strings, numbers, and other types
 *
 * **Usage Example**:
 *
 * ```cpp
 * // Message definition
 * IC10_LOCAL_MSG_EN_US(IEA1_2, "'{}' expected, but found '{}'")
 *
 * // Usage
 * reporter.report<SemanticError>(pos, expected, found);
 * // Output: "'add' expected, but found 'ad'"
 * ```
 *
 * **Placeholder Replacement Mechanism**:
 *
 * ```cpp
 * std::string formatMessage(const char* msg, std::initializer_list<std::string> args) {
 *     std::string result = msg;
 *     size_t idx = 0;
 *     for (const auto& arg : args) {
 *         size_t pos = result.find("{}");
 *         if (pos != std::string::npos) {
 *             result.replace(pos, 2, arg);
 *         }
 *         idx++;
 *     }
 *     return result;
 * }
 * ```
 *
 * **Supported Parameter Types**:
 * - String: Direct replacement
 * - Number: Auto-converted to string
 * - Enum value: Converted to corresponding string representation
 * - Position: Converted to "line:column" format
 *
 * **Parameterized Message Advantages**:
 * - Supports complex error messages
 * - Consistent message format
 * - More flexible translation
 *
 * @endif
 *
 * @subsection locals_message_fallback Message Fallback / 消息回退
 *
 * @if zh
 *
 * 当某个语言的消息不存在时，系统会自动回退到英文：
 *
 ** 回退机制**：
 *
 * ```cpp
 * std::string getMessage(MsgId id) {
 *     auto lang = IC10Local::getLanguage();
 *     auto msg = getMessageByLanguage(id, lang);
 *     if (msg.empty()) {
 *         // 回退到英文
 *         msg = getMessageByLanguage(id, "en-us");
 *     }
 *     return msg;
 * }
 * ```
 *
 ** 回退顺序**：
 * 1. 当前语言的翻译
 * 2. 英文翻译（默认）
 * 3. 消息 ID（最后回退）
 *
 ** 回退的意义**：
 * - 确保即使翻译不完整也能显示消息
 * - 帮助开发者调试
 * - 支持渐进式翻译
 *
 ** 翻译完整性检查**：
 * - 提供工具检查翻译完整性
 * - 报告缺失的翻译
 * - 支持翻译进度统计
 *
 * @elseif en
 *
 * When a message doesn't exist in a language, the system automatically falls back to English:
 *
 * **Fallback Mechanism**:
 *
 * ```cpp
 * std::string getMessage(MsgId id) {
 *     auto lang = IC10Local::getLanguage();
 *     auto msg = getMessageByLanguage(id, lang);
 *     if (msg.empty()) {
 *         // Fall back to English
 *         msg = getMessageByLanguage(id, "en-us");
 *     }
 *     return msg;
 * }
 * ```
 *
 * **Fallback Order**:
 * 1. Current language translation
 * 2. English translation (default)
 * 3. Message ID (final fallback)
 *
 * **Fallback Significance**:
 * - Ensures messages are displayed even when translations are incomplete
 * - Helps developers debug
 * - Supports progressive translation
 *
 * **Translation Completeness Check**:
 * - Provides tools to check translation completeness
 * - Reports missing translations
 * - Supports translation progress statistics
 *
 * @endif
 *
 * @section locals_extension Extending New Languages / 扩展新语言
 *
 * @if zh
 *
 ** 添加新语言支持的步骤**：
 *
 * 1. **创建语言文件**：在 `locals/languages/` 下创建新语言文件
 *    ```
 *    touch locals/languages/ja_jp.hpp
 *    ```
 *
 * 2. **定义语言宏**：在语言文件中定义消息注册宏
 *    ```cpp
 *    #define IC10_LOCAL_MSG_JA_JP(id, msg) \
 *        template<> const char* IC10MsgPack<MsgId::id>::msg_ja_jp = msg;
 *    ```
 *
 * 3. **实现语言切换支持**：在 `IC10Local::setLanguage()` 中添加新语言的处理
 *    ```cpp
 *    void setLanguage(const std::string& lang) {
 *        if (lang == "ja-jp") {
 *            currentLang = Language::JA_JP;
 *        } else if (...) {
 *            // ...
 *        }
 *    }
 *    ```
 *
 * 4. **添加翻译**：在新语言文件中为所有消息 ID 添加翻译
 *    ```cpp
 *    IC10_LOCAL_MSG_JA_JP(IWL1, "不正な文字 '{}' - 字句解析")
 *    IC10_LOCAL_MSG_JA_JP(IWL2, "レジスタ範囲外(r0-r15)")
 *    // ... 所有消息 ID
 *    ```
 *
 ** 翻译注意事项**：
 * - 保持消息格式一致（占位符位置）
 * - 注意文化差异
 * - 使用专业术语
 * - 保持消息简洁明了
 *
 ** 翻译工具**：
 * - 提供翻译模板生成工具
 * - 支持翻译进度跟踪
 * - 支持批量翻译导入
 *
 * @elseif en
 *
 * **Steps to Add New Language Support**:
 *
 * 1. **Create Language File**: Create a new language file under `locals/languages/`
 *    ```
 *    touch locals/languages/ja_jp.hpp
 *    ```
 *
 * 2. **Define Language Macro**: Define message registration macro in the language file
 *    ```cpp
 *    #define IC10_LOCAL_MSG_JA_JP(id, msg) \
 *        template<> const char* IC10MsgPack<MsgId::id>::msg_ja_jp = msg;
 *    ```
 *
 * 3. **Implement Language Switching**: Add new language handling in `IC10Local::setLanguage()`
 *    ```cpp
 *    void setLanguage(const std::string& lang) {
 *        if (lang == "ja-jp") {
 *            currentLang = Language::JA_JP;
 *        } else if (...) {
 *            // ...
 *        }
 *    }
 *    ```
 *
 * 4. **Add Translations**: Add translations for all message IDs in the new language file
 *    ```cpp
 *    IC10_LOCAL_MSG_JA_JP(IWL1, "Invalid character '{}' - lexical")
 *    IC10_LOCAL_MSG_JA_JP(IWL2, "Register out of range (r0-r15)")
 *    // ... all message IDs
 *    ```
 *
 * **Translation Notes**:
 * - Maintain consistent message format (placeholder positions)
 * - Be aware of cultural differences
 * - Use professional terminology
 * - Keep messages concise and clear
 *
 * **Translation Tools**:
 * - Provide translation template generation tools
 * - Support translation progress tracking
 * - Support batch translation import
 *
 * @endif
 *
 * @section locals_api API Reference / API 接口
 *
 * @if zh
 *
 * @code{.cpp}
 * #include "ic10_compiler/locals/local.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // 设置语言
 * IC10Local::setLanguage("zh-hans");
 *
 * // 获取当前语言
 * auto lang = IC10Local::getLanguage();
 *
 * // 获取消息
 * auto msg = IC10Local::getMessage(MsgId::IWL1);
 *
 * // 参数化消息
 * auto formattedMsg = IC10Local::formatMessage(MsgId::IEA1_2, "add", "ad");
 * // 输出: "期望 'add'，但发现 'ad'"
 * @endcode
 *
 ** 核心方法**：
 * | 方法 | 说明 |
 * |:-----|:-----|
 * | `setLanguage(lang)` | 设置当前语言 |
 * | `getLanguage()` | 获取当前语言代码 |
 * | `getMessage(id)` | 获取消息文本 |
 * | `formatMessage(id, args...)` | 获取参数化消息 |
 * | `getAllLanguages()` | 获取支持的语言列表 |
 *
 ** 支持的语言**：
 * | 语言代码 | 语言名称 | 文件 |
 * |:---------|:---------|:-----|
 * | zh-hans | 中文(简体) | zh_hans.hpp |
 * | en-us | 英文(美国) | en_us.hpp |
 *
 ** 消息 ID 示例**：
 * | 消息 ID | 中文消息 | 英文消息 |
 * |:--------|:---------|:---------|
 * | IWL1 | 非法字符 '{}' | Invalid character '{}' |
 * | IWL2 | 寄存器超出范围(r0-r15) | Register out of range (r0-r15) |
 * | IWL3 | 设备超出范围(d0-d5) | Device out of range (d0-d5) |
 * | IPL1 | 期望的 Token 未找到 | Expected token not found |
 * | IEA1 | 未定义符号 '{}' | Undefined symbol '{}' |
 *
 * @elseif en
 *
 * @code{.cpp}
 * #include "ic10_compiler/locals/local.hpp"
 *
 * using namespace stationeers::ic10;
 *
 * // Set language
 * IC10Local::setLanguage("zh-hans");
 *
 * // Get current language
 * auto lang = IC10Local::getLanguage();
 *
 * // Get message
 * auto msg = IC10Local::getMessage(MsgId::IWL1);
 *
 * // Parameterized message
 * auto formattedMsg = IC10Local::formatMessage(MsgId::IEA1_2, "add", "ad");
 * // Output: "'add' expected, but found 'ad'"
 * @endcode
 *
 * **Core Methods**:
 * | Method | Description |
 * |:-------|:------------|
 * | `setLanguage(lang)` | Set current language |
 * | `getLanguage()` | Get current language code |
 * | `getMessage(id)` | Get message text |
 * | `formatMessage(id, args...)` | Get parameterized message |
 * | `getAllLanguages()` | Get list of supported languages |
 *
 * **Supported Languages**:
 * | Language Code | Language Name | File |
 * |:-------------|:-------------|:-----|
 * | zh-hans | Chinese (Simplified) | zh_hans.hpp |
 * | en-us | English (US) | en_us.hpp |
 *
 * **Message ID Examples**:
 * | Message ID | Chinese Message | English Message |
 * |:-----------|:---------------|:----------------|
 * | IWL1 | 非法字符 '{}' | Invalid character '{}' |
 * | IWL2 | 寄存器超出范围(r0-r15) | Register out of range (r0-r15) |
 * | IWL3 | 设备超出范围(d0-d5) | Device out of range (d0-d5) |
 * | IPL1 | 期望的 Token 未找到 | Expected token not found |
 * | IEA1 | 未定义符号 '{}' | Undefined symbol '{}' |
 *
 * @endif
 *
 * @section locals_developer Developer Notes / 开发者必读
 *
 * @if zh
 *
 ** 新增消息流程**：
 * 1. 在 `msg_id.hpp` 中添加消息 ID
 * 2. 在所有语言文件中添加翻译
 * 3. 在报告错误的代码中使用新消息 ID
 *
 ** 翻译注意事项**：
 * - 保持占位符数量和顺序一致
 * - 使用正确的语言代码
 * - 注意专业术语的翻译
 * - 测试翻译效果
 *
 ** 消息 ID 命名规范**：
 * - 使用模块前缀 + 编号
 * - 编号按功能分类
 * - 避免重复使用编号
 *
 ** 测试要点**：
 * - 消息查找测试
 * - 语言切换测试
 * - 参数化消息测试
 * - 消息回退测试
 * - 翻译完整性测试
 *
 ** 调试技巧**：
 * - 使用 `getMessage()` 直接获取消息
 * - 打印当前语言设置
 * - 检查翻译文件是否正确包含
 *
 * @elseif en
 *
 * **Adding New Message**:
 * 1. Add message ID in `msg_id.hpp`
 * 2. Add translations in all language files
 * 3. Use the new message ID in error reporting code
 *
 * **Translation Notes**:
 * - Maintain consistent placeholder count and order
 * - Use correct language codes
 * - Pay attention to professional terminology translation
 * - Test translation effects
 *
 * **Message ID Naming Convention**:
 * - Use module prefix + number
 * - Number by function category
 * - Avoid duplicate numbers
 *
 * **Testing Checklist**:
 * - Message lookup tests
 * - Language switching tests
 * - Parameterized message tests
 * - Message fallback tests
 * - Translation completeness tests
 *
 * **Debugging Tips**:
 * - Use `getMessage()` to directly retrieve messages
 * - Print current language settings
 * - Check if translation files are properly included
 *
 * @endif
 *
 * @see [IC10Local](@ref stationeers::ic10::IC10Local), [IC10MsgPack](@ref stationeers::ic10::IC10MsgPack)
 */

/**
 * @if zh
 *
 * @namespace stationeers::ic10
 * @brief IC10 编译器命名空间
 * @details 包含 IC10 编译器的所有组件：词法分析、语法分析、语义分析
 *
 * @elseif en
 *
 * @namespace stationeers::ic10
 * @brief IC10 compiler namespace
 * @details Contains all components of the IC10 compiler: lexical analysis,
 *         syntax analysis, semantic analysis
 *
 * @endif
 */
namespace stationeers::ic10 {
}

#endif  // COMPILER_IC10_MAIN_HPP
