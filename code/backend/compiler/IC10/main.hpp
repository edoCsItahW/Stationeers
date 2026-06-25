// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file main.hpp
 * @author edocsitahw
 * @version 1.1
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
 * - **寄存器架构**：提供 8 个通用寄存器 (r0-r7)
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
 * | 寄存器 | `r0`, `r7` | r0 到 r7 |
 * | 设备引用 | `@Display`, `@Sensor` | @ 开头的设备名 |
 * | 字符串 | `"Hello"` | 双引号包围 |
 * | 标识符 | `main`, `_temp` | 字母或下划线开头 |
 * | 标签 | `loop:` | 标识符后跟冒号 |
 * | 符号 | `+`, `-`, `*`, `/` | 算术运算符 |
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
 * | 零元 | 0 | `yield`, `hcf`, `sleep` |
 * | 一元 | 1 | `snanz`, `peek`, `rand` |
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
 * | `OTHER` | 其他 | 宏调用 |
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
 * #include "ic10/lexer/lexer.hpp"
 * #include "ic10/parser/parser.hpp"
 * #include "ic10/semantic/analyser.hpp"
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
 * @section glossary 术语表
 *
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
 * - **Register architecture**: 8 general-purpose registers (r0-r7)
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
 * | Register | `r0`, `r7` | r0 to r7 |
 * | Device reference | `@Display`, `@Sensor` | Device name starting with @ |
 * | String | `"Hello"` | Enclosed in double quotes |
 * | Identifier | `main`, `_temp` | Letter or underscore prefix |
 * | Label | `loop:` | Identifier followed by colon |
 * | Symbol | `+`, `-`, `*`, `/` | Arithmetic operators |
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
 * | Nullary | 0 | `yield`, `hcf`, `sleep` |
 * | Unary | 1 | `snanz`, `peek`, `rand` |
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
 * | `OTHER` | Other | Macro calls |
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
 * #include "ic10/lexer/lexer.hpp"
 * #include "ic10/parser/parser.hpp"
 * #include "ic10/semantic/analyser.hpp"
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
 * - Email: 2207150234@st.sziit.edu.cn
 *
 * @endif
 */

#ifndef COMPILER_IC10_MAIN_HPP
#define COMPILER_IC10_MAIN_HPP

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
