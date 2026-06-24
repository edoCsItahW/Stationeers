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
 * @mainpage IC10 Node.js 绑定文档
 *
 * @section overview 概述
 *
 * IC10 Node.js 绑定提供了一套将 C++ 编译器核心功能导出到 **Node.js** 环境的适配器层。
 * 通过 **N-API** (Node.js Native API) 实现跨语言调用，使得 JavaScript/TypeScript 开发者
 * 能够在 Node.js 环境中使用 IC10 编译器的全部功能。
 *
 * @section architecture 架构设计
 *
 * @code{.mermaid}
 * graph TB
 *     subgraph JavaScript层
 *         JS[JavaScript 代码]
 *     end
 *
 *     subgraph Node.js绑定层
 *         LA[LexerAdapter]
 *         TA[TokenAdapter]
 *         PA[ParserAdapter]
 *         AA[AnalyserAdapter]
 *         STA[SymbolTableAdapter]
 *         Loca[LocalAdapter]
 *     end
 *
 *     subgraph C++核心层
 *         Lexer[Lexer]
 *         Parser[Parser]
 *         Analyser[Analyser]
 *         ST[SymbolTable]
 *     end
 *
 *     JS --> LA
 *     JS --> TA
 *     JS --> PA
 *     JS --> AA
 *     JS --> STA
 *     JS --> Loca
 *
 *     LA --> Lexer
 *     TA --> Lexer
 *     PA --> Parser
 *     AA --> Analyser
 *     STA --> ST
 *
 *     Parser --> Lexer
 *     Analyser --> Parser
 *     Analyser --> ST
 * @endcode
 *
 * ### 设计原则
 *
 * 1. **零拷贝设计**: 通过引用传递减少数据复制
 * 2. **类型安全**: 使用适配器模式进行类型转换
 * 3. **异步优先**: 长时间操作使用异步工作器避免阻塞
 * 4. **错误可追踪**: 错误信息包含位置和上下文
 *
 * @section adapter_list 适配器列表
 *
 * | 适配器 | 文件 | 说明 |
 * |:------|:-----|:-----|
 * | [LexerAdapter](@ref stationeers::ic10::LexerAdapter) | lexer_adapter.hpp | 词法分析器绑定 |
 * | [TokenAdapter](@ref stationeers::ic10::TokenAdapter) | token_adapter.hpp | Token 对象绑定 |
 * | [ParserAdapter](@ref stationeers::ic10::ParserAdapter) | parser_adapter.hpp | 语法分析器绑定 |
 * | [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) | ast_adapter.hpp | AST Program 节点绑定 |
 * | [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter) | analyser_adapter.hpp | 语义分析器绑定 |
 * | [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) | symbol_table_adapter.hpp | 符号表绑定 |
 * | [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) | local_adapter.hpp | 本地化功能绑定 |
 *
 * @section usage 使用指南
 *
 * ### 安装
 *
 * @code{.bash}
 * npm install ic10-compiler
 * @endcode
 *
 * ### 基本用法
 *
 * @code{.javascript}
 * const {
 *     Lexer,
 *     Parser,
 *     Analyser,
 *     IC10Local
 * } = require('ic10-compiler');
 *
 * // 设置语言 (可选，默认英文)
 * IC10Local.setLanguage('zh-hans');
 *
 * // 源代码
 * const source = `
 *     alias disp d0
 *     main:
 *         move r0 42
 *         yield
 *         jal main
 * `;
 *
 * // 1. 词法分析
 * const tokens = Lexer.tokenize(source);
 * console.log(`生成了 ${tokens.length} 个 Token`);
 *
 * // 2. 语法分析
 * const program = Parser.parse(tokens);
 * console.log('AST:', program.toJSON());
 *
 * // 3. 语义分析
 * Analyser.analyse(program).then(() => {
 *     console.log('分析完成');
 *     console.log('符号表:', program.symbolTable.toJSON());
 * }).catch(err => {
 *     console.error('分析失败:', err);
 * });
 * @endcode
 *
 * ### 错误处理
 *
 * @code{.javascript}
 * const { Parser } = require('ic10-compiler');
 *
 * const source = 'move r0';  // 缺少操作数
 * const tokens = Lexer.tokenize(source);
 * const program = Parser.parse(tokens);
 *
 * // 检查是否有错误
 * if (program.errors && program.errors.length > 0) {
 *     for (const err of program.errors) {
 *         console.log(`错误: ${err.message}`);
 *         console.log(`位置: 行 ${err.pos.line}, 列 ${err.pos.column}`);
 *     }
 * }
 * @endcode
 *
 * @section adapter_detail 适配器详解
 *
 * ### LexerAdapter
 *
 * [LexerAdapter](@ref stationeers::ic10::LexerAdapter) 将 C++ 词法分析器导出到 Node.js。
 *
 * @code{.javascript}
 * const { Lexer } = require('ic10-compiler');
 *
 * // 静态方法调用
 * const tokens = Lexer.tokenize('add r0 r1 r2');
 *
 * // 或创建实例
 * const lexer = new Lexer('add r0 r1 r2', false);
 * const tokens = lexer.scan();
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `tokenize(src)` | 静态方法，词法分析 | `Token[]` |
 * | `scan()` | 扫描源代码 | `Token[]` |
 *
 * ### TokenAdapter
 *
 * [TokenAdapter](@ref stationeers::ic10::TokenAdapter) 表示 IC10 词法标记。
 *
 * @code{.javascript}
 * const token = new Token();
 * token.type = TokenType.KEYWORD_ADD;
 * token.lexeme = 'add';
 * token.pos = new Pos(1, 1, 0);
 *
 * console.log(token.toString());  // "Token{type: KEYWORD_ADD, lexeme: add}"
 * console.log(token.toJSON());     // JSON 格式
 * @endcode
 *
 * **属性**:
 * | 属性 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `type` | `TokenType` | 标记类型枚举 |
 * | `lexeme` | `string` | 原始词素文本 |
 * | `pos` | `Pos` | 位置信息 |
 * | `category` | `TokenCategory` | 标记类别 |
 *
 * ### ParserAdapter
 *
 * [ParserAdapter](@ref stationeers::ic10::ParserAdapter) 将 C++ 语法分析器导出到 Node.js。
 *
 * @code{.javascript}
 * const { Lexer, Parser } = require('ic10-compiler');
 *
 * // 词法分析
 * const tokens = Lexer.tokenize(source);
 *
 * // 语法分析
 * const program = Parser.parse(tokens);
 *
 * // 或使用静态方法
 * const program = Parser.parsing(tokens);
 *
 * console.log('语句数量:', program.statements.length);
 * console.log('AST:', program.toJSON());
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `parse(tokens)` | 解析 Token 序列 | `Program` |
 * | `parsing(tokens)` | 静态解析方法 | `Program` |
 *
 * ### ProgramAdapter (AST)
 *
 * [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) 表示 IC10 程序的抽象语法树根节点。
 *
 * @code{.javascript}
 * const program = Parser.parse(tokens);
 *
 * // 获取节点名称
 * console.log('Node name:', program.getNodeName());  // "Program"
 *
 * // 获取语句列表
 * const statements = program.getStatements();
 *
 * // 位置信息
 * console.log('Start:', program.start());
 * console.log('End:', program.end());
 *
 * // 转换为字符串/JSON
 * console.log(program.toString());
 * console.log(program.toJSON());
 * @endcode
 *
 * **方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `getNodeName()` | 获取节点名称 | `string` |
 * | `getStatements()` | 获取语句列表 | `Statement[]` |
 * | `start()` | 起始位置 | `Pos` |
 * | `end()` | 结束位置 | `Pos` |
 * | `toString()` | 字符串表示 | `string` |
 * | `toJSON()` | JSON 表示 | `string` |
 *
 * ### AnalyserAdapter
 *
 * [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter) 将 C++ 语义分析器导出到 Node.js。
 * 支持异步分析，利用协程处理前向引用。
 *
 * @code{.javascript}
 * const { Lexer, Parser, Analyser } = require('ic10-compiler');
 *
 * const tokens = Lexer.tokenize(source);
 * const program = Parser.parse(tokens);
 *
 * // 异步语义分析
 * await Analyser.analyse(program);
 *
 * // 获取分析结果
 * const symbolTable = program.getSymbolTable();
 * const errors = program.getErrors();
 *
 * if (errors.length > 0) {
 *     console.log('发现错误:');
 *     for (const err of errors) {
 *         console.log(`  - ${err}`);
 *     }
 * }
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `analyse(program)` | 异步语义分析 | `Promise<void>` |
 * | `getErrors()` | 获取错误列表 | `Error[]` |
 * | `getSymbolTable()` | 获取符号表 | `SymbolTable` |
 *
 * ### SymbolTableAdapter
 *
 * [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) 提供符号表的访问接口。
 *
 * @code{.javascript}
 * const symbolTable = program.getSymbolTable();
 *
 * // 转为 JSON
 * console.log(symbolTable.toJSON());
 *
 * // 遍历符号
 * for (const [name, entry] of symbolTable) {
 *     console.log(`${name}: ${entry.type}`);
 * }
 * @endcode
 *
 * **方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `toJSON()` | JSON 表示 | `string` |
 * | `[Symbol.iterator]` | 迭代器支持 | `Iterator` |
 *
 * ### IC10LocalAdapter
 *
 * [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) 提供多语言本地化支持。
 *
 * @code{.javascript}
 * const { IC10Local } = require('ic10-compiler');
 *
 * // 设置语言
 * IC10Local.setLanguage('zh-hans');  // 简体中文
 * IC10Local.setLanguage('en-us');    // 英文
 *
 * // 获取当前语言
 * const lang = IC10Local.getLanguage();
 * @endcode
 *
 * **支持的语言**:
 * | 代码 | 语言 |
 * |:-----|:-----|
 * | `zh-hans` | 简体中文 |
 * | `en-us` | 美式英语 |
 *
 * @section type_conversion 类型转换
 *
 * C++ 与 JavaScript 之间的类型映射关系：
 *
 * @code
 * C++ 类型                    JavaScript 类型
 * ──────────────────────────────────────────────
 * int                         number
 * float/double                number
 * std::string                string
 * bool                       boolean
 * std::vector<T>             Array
 * std::unordered_map<K,V>     Object
 * Pos                        Pos (adapter)
 * Token                      Token (adapter)
 * Program                    Program (adapter)
 * Error                      Error (adapter)
 * @endcode
 *
 * @section async_workers 异步工作器
 *
 * 长时间运行的操作使用 `TaskWorker` 在后台线程执行：
 *
 * @code{.mermaid}
 * sequenceDiagram
 *     participant JS as JavaScript
 *     participant Worker as TaskWorker
 *     participant Core as C++ Core
 *
 *     JS->>Worker: 启动异步任务
 *     Worker->>Core: 在后台线程执行
 *     Core-->>Worker: 任务完成
 *     Worker-->>JS: Promise resolved
 * @endcode
 *
 * **优点**:
 * - 不阻塞 Node.js 主事件循环
 * - 支持取消操作
 * - 错误自动传播到 Promise
 *
 * @section error_mapping 错误映射
 *
 * C++ 错误类型与 JavaScript Error 的映射：
 *
 * | C++ 错误类型 | JavaScript 错误类型 | 说明 |
 * |:------------|:------------------|:-----|
 * | `SyntaxError` | `SyntaxError` | 语法错误 |
 * | `TypeError` | `TypeError` | 类型错误 |
 * | `RedefinitionError` | `Error` | 重定义错误 |
 * | `UndefinedSymbolError` | `ReferenceError` | 未定义符号 |
 * | `RuntimeError` | `Error` | 运行时错误 |
 *
 * @section build 构建说明
 *
 * ### 前提条件
 *
 * - Node.js >= 14.x
 * - npm >= 6.x
 * - C++ 编译器 (g++/clang++/MSVC)
 * - node-gyp
 *
 * ### 构建步骤
 *
 * @code{.bash}
 * # 安装依赖
 * npm install
 *
 * # 构建原生模块
 * npm run build
 *
 * # 运行测试
 * npm test
 * @endcode
 *
 * ### 项目结构
 *
 * @code
 * exports/
 * └── IC10/
 *     └── node/
 *         ├── include/              # 头文件
 *         │   ├── main.hpp         # 本文档
 *         │   ├── lexer_adapter.hpp
 *         │   ├── token_adapter.hpp
 *         │   ├── parser_adapter.hpp
 *         │   ├── ast_adapter.hpp
 *         │   ├── analyser_adapter.hpp
 *         │   ├── symbol_table_adapter.hpp
 *         │   └── local_adapter.hpp
 *         ├── src/                # 源文件
 *         │   ├── lexer_adapter.cpp
 *         │   ├── token_adapter.cpp
 *         │   └── ...
 *         ├── binding.gyp         # node-gyp 配置
 *         └── package.json
 * @endcode
 *
 * @section debugging 调试指南
 *
 * ### 启用调试日志
 *
 * @code{.javascript}
 * const ic10 = require('ic10-compiler');
 * ic10.setDebug(true);
 * @endcode
 *
 * ### 常见问题
 *
 * | 问题 | 解决方案 |
 * |:-----|:-------|
 * | 编译失败 | 检查 C++ 编译器版本 |
 * | 内存泄漏 | 确保所有 adapter 被正确释放 |
 * | 异步挂起 | 检查协程是否正确恢复 |
 *
 * @section best_practices 最佳实践
 *
 * 1. **复用适配器**: 不要频繁创建/销毁适配器实例
 * 2. **批量处理**: 多次词法分析时复用 Lexer 实例
 * 3. **错误检查**: 始终检查返回值是否有错误
 * 4. **异步等待**: 使用 `await` 而非 Promise.then
 *
 * @code{.javascript}
 * // 推荐: 复用实例
 * const lexer = new Lexer(source);
 * for (const line of lines) {
 *     lexer.source = line;
 *     const tokens = lexer.scan();
 *     // 处理 tokens
 * }
 *
 * // 推荐: 检查错误
 * const program = Parser.parse(tokens);
 * if (program.errors && program.errors.length > 0) {
 *     // 处理错误
 * }
 *
 * // 推荐: 异步等待
 * await Analyser.analyse(program);
 * @endcode
 *
 * @section api_reference API 参考
 *
 * 完整 API 文档请参阅各适配器的 Doxygen 文档：
 * - [LexerAdapter](@ref stationeers::ic10::LexerAdapter)
 * - [TokenAdapter](@ref stationeers::ic10::TokenAdapter)
 * - [ParserAdapter](@ref stationeers::ic10::ParserAdapter)
 * - [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter)
 * - [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter)
 * - [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter)
 * - [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter)
 *
 * @section related_docs 相关文档
 *
 * - [@ref ic10_mainpage IC10 编译器核心文档]
 * - [@ref common_node_mainpage Common 模块 Node.js 绑定]
 * - [Node.js N-API 文档](https://nodejs.org/api/n-api.html)
 * - [node-gyp 文档](https://github.com/nodejs/node-gyp)
 *
 * @section license 许可证
 *
 * 本项目采用 **CC BY-NC-SA** 许可证，与 IC10 编译器核心保持一致。
 *
 * @section contact 联系方式
 *
 * 如有问题或建议，请联系作者：
 * - 邮箱: 2207150234@st.sziit.edu.cn
 *
 * @elseif en
 * @mainpage IC10 Node.js Bindings Documentation
 *
 * @section overview Overview
 *
 * IC10 Node.js bindings provide an adapter layer to export C++ compiler core functionality
 * to the **Node.js** environment. Through **N-API** (Node.js Native API), JavaScript/TypeScript
 * developers can use all IC10 compiler features in the Node.js environment.
 *
 * @section architecture Architecture
 *
 * @code{.mermaid}
 * graph TB
 *     subgraph JavaScriptLayer
 *         JS[JavaScript Code]
 *     end
 *
 *     subgraph NodeBindingLayer
 *         LA[LexerAdapter]
 *         TA[TokenAdapter]
 *         PA[ParserAdapter]
 *         AA[AnalyserAdapter]
 *         STA[SymbolTableAdapter]
 *         Loca[LocalAdapter]
 *     end
 *
 *     subgraph CoreLayer
 *         Lexer[Lexer]
 *         Parser[Parser]
 *         Analyser[Analyser]
 *         ST[SymbolTable]
 *     end
 *
 *     JS --> LA
 *     JS --> TA
 *     JS --> PA
 *     JS --> AA
 *     JS --> STA
 *     JS --> Loca
 *
 *     LA --> Lexer
 *     TA --> Lexer
 *     PA --> Parser
 *     AA --> Analyser
 *     STA --> ST
 *
 *     Parser --> Lexer
 *     Analyser --> Parser
 *     Analyser --> ST
 * @endcode
 *
 * ### Design Principles
 *
 * 1. **Zero-copy design**: Pass by reference to reduce data copying
 * 2. **Type safety**: Use adapter pattern for type conversion
 * 3. **Async-first**: Use async workers for long operations to avoid blocking
 * 4. **Traceable errors**: Error messages include position and context
 *
 * @section adapter_list Adapter List
 *
 * | Adapter | File | Description |
 * |:--------|:-----|:------------|
 * | [LexerAdapter](@ref stationeers::ic10::LexerAdapter) | lexer_adapter.hpp | Lexer binding |
 * | [TokenAdapter](@ref stationeers::ic10::TokenAdapter) | token_adapter.hpp | Token object binding |
 * | [ParserAdapter](@ref stationeers::ic10::ParserAdapter) | parser_adapter.hpp | Parser binding |
 * | [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) | ast_adapter.hpp | AST Program node binding |
 * | [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter) | analyser_adapter.hpp | Semantic analyser binding |
 * | [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) | symbol_table_adapter.hpp | Symbol table binding |
 * | [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) | local_adapter.hpp | Localization binding |
 *
 * @section usage Usage Guide
 *
 * ### Installation
 *
 * @code{.bash}
 * npm install ic10-compiler
 * @endcode
 *
 * ### Basic Usage
 *
 * @code{.javascript}
 * const {
 *     Lexer,
 *     Parser,
 *     Analyser,
 *     IC10Local
 * } = require('ic10-compiler');
 *
 * // Set language (optional, default English)
 * IC10Local.setLanguage('zh-hans');
 *
 * // Source code
 * const source = `
 *     alias disp d0
 *     main:
 *         move r0 42
 *         yield
 *         jal main
 * `;
 *
 * // 1. Lexical analysis
 * const tokens = Lexer.tokenize(source);
 * console.log(`Generated ${tokens.length} Tokens`);
 *
 * // 2. Syntax analysis
 * const program = Parser.parse(tokens);
 * console.log('AST:', program.toJSON());
 *
 * // 3. Semantic analysis
 * Analyser.analyse(program).then(() => {
 *     console.log('Analysis complete');
 *     console.log('Symbol table:', program.symbolTable.toJSON());
 * }).catch(err => {
 *     console.error('Analysis failed:', err);
 * });
 * @endcode
 *
 * ### Error Handling
 *
 * @code{.javascript}
 * const { Parser } = require('ic10-compiler');
 *
 * const source = 'move r0';  // Missing operand
 * const tokens = Lexer.tokenize(source);
 * const program = Parser.parse(tokens);
 *
 * // Check for errors
 * if (program.errors && program.errors.length > 0) {
 *     for (const err of program.errors) {
 *         console.log(`Error: ${err.message}`);
 *         console.log(`Position: Line ${err.pos.line}, Column ${err.pos.column}`);
 *     }
 * }
 * @endcode
 *
 * @section adapter_detail Adapter Details
 *
 * ### LexerAdapter
 *
 * [LexerAdapter](@ref stationeers::ic10::LexerAdapter) exports C++ lexer to Node.js.
 *
 * @code{.javascript}
 * const { Lexer } = require('ic10-compiler');
 *
 * // Static method call
 * const tokens = Lexer.tokenize('add r0 r1 r2');
 *
 * // Or create instance
 * const lexer = new Lexer('add r0 r1 r2', false);
 * const tokens = lexer.scan();
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `tokenize(src)` | Static method, lex analysis | `Token[]` |
 * | `scan()` | Scan source code | `Token[]` |
 *
 * ### TokenAdapter
 *
 * [TokenAdapter](@ref stationeers::ic10::TokenAdapter) represents IC10 lexical token.
 *
 * @code{.javascript}
 * const token = new Token();
 * token.type = TokenType.KEYWORD_ADD;
 * token.lexeme = 'add';
 * token.pos = new Pos(1, 1, 0);
 *
 * console.log(token.toString());  // "Token{type: KEYWORD_ADD, lexeme: add}"
 * console.log(token.toJSON());     // JSON format
 * @endcode
 *
 * **Properties**:
 * | Property | Type | Description |
 * |:---------|:-----|:------------|
 * | `type` | `TokenType` | Token type enum |
 * | `lexeme` | `string` | Raw lexeme text |
 * | `pos` | `Pos` | Position info |
 * | `category` | `TokenCategory` | Token category |
 *
 * ### ParserAdapter
 *
 * [ParserAdapter](@ref stationeers::ic10::ParserAdapter) exports C++ parser to Node.js.
 *
 * @code{.javascript}
 * const { Lexer, Parser } = require('ic10-compiler');
 *
 * // Lexical analysis
 * const tokens = Lexer.tokenize(source);
 *
 * // Syntax analysis
 * const program = Parser.parse(tokens);
 *
 * // Or use static method
 * const program = Parser.parsing(tokens);
 *
 * console.log('Statement count:', program.statements.length);
 * console.log('AST:', program.toJSON());
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `parse(tokens)` | Parse Token sequence | `Program` |
 * | `parsing(tokens)` | Static parse method | `Program` |
 *
 * ### ProgramAdapter (AST)
 *
 * [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) represents IC10 program AST root node.
 *
 * @code{.javascript}
 * const program = Parser.parse(tokens);
 *
 * // Get node name
 * console.log('Node name:', program.getNodeName());  // "Program"
 *
 * // Get statement list
 * const statements = program.getStatements();
 *
 * // Position info
 * console.log('Start:', program.start());
 * console.log('End:', program.end());
 *
 * // Convert to string/JSON
 * console.log(program.toString());
 * console.log(program.toJSON());
 * @endcode
 *
 * **Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `getNodeName()` | Get node name | `string` |
 * | `getStatements()` | Get statement list | `Statement[]` |
 * | `start()` | Start position | `Pos` |
 * | `end()` | End position | `Pos` |
 * | `toString()` | String representation | `string` |
 * | `toJSON()` | JSON representation | `string` |
 *
 * ### AnalyserAdapter
 *
 * [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter) exports C++ semantic analyser to Node.js.
 * Supports async analysis using coroutines for forward references.
 *
 * @code{.javascript}
 * const { Lexer, Parser, Analyser } = require('ic10-compiler');
 *
 * const tokens = Lexer.tokenize(source);
 * const program = Parser.parse(tokens);
 *
 * // Async semantic analysis
 * await Analyser.analyse(program);
 *
 * // Get analysis results
 * const symbolTable = program.getSymbolTable();
 * const errors = program.getErrors();
 *
 * if (errors.length > 0) {
 *     console.log('Errors found:');
 *     for (const err of errors) {
 *         console.log(`  - ${err}`);
 *     }
 * }
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `analyse(program)` | Async semantic analysis | `Promise<void>` |
 * | `getErrors()` | Get error list | `Error[]` |
 * | `getSymbolTable()` | Get symbol table | `SymbolTable` |
 *
 * ### SymbolTableAdapter
 *
 * [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) provides symbol table access interface.
 *
 * @code{.javascript}
 * const symbolTable = program.getSymbolTable();
 *
 * // Convert to JSON
 * console.log(symbolTable.toJSON());
 *
 * // Iterate symbols
 * for (const [name, entry] of symbolTable) {
 *     console.log(`${name}: ${entry.type}`);
 * }
 * @endcode
 *
 * **Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `toJSON()` | JSON representation | `string` |
 * | `[Symbol.iterator]` | Iterator support | `Iterator` |
 *
 * ### IC10LocalAdapter
 *
 * [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) provides multi-language localization support.
 *
 * @code{.javascript}
 * const { IC10Local } = require('ic10-compiler');
 *
 * // Set language
 * IC10Local.setLanguage('zh-hans');  // Simplified Chinese
 * IC10Local.setLanguage('en-us');    // English
 *
 * // Get current language
 * const lang = IC10Local.getLanguage();
 * @endcode
 *
 * **Supported Languages**:
 * | Code | Language |
 * |:-----|:---------|
 * | `zh-hans` | Simplified Chinese |
 * | `en-us` | American English |
 *
 * @section type_conversion Type Conversion
 *
 * Type mapping between C++ and JavaScript:
 *
 * @code
 * C++ Type                   JavaScript Type
 * ─────────────────────────────────────────────
 * int                         number
 * float/double                number
 * std::string                 string
 * bool                        boolean
 * std::vector<T>             Array
 * std::unordered_map<K,V>     Object
 * Pos                         Pos (adapter)
 * Token                       Token (adapter)
 * Program                     Program (adapter)
 * Error                       Error (adapter)
 * @endcode
 *
 * @section async_workers Async Workers
 *
 * Long-running operations use `TaskWorker` to execute in background threads:
 *
 * @code{.mermaid}
 * sequenceDiagram
 *     participant JS as JavaScript
 *     participant Worker as TaskWorker
 *     participant Core as C++ Core
 *
 *     JS->>Worker: Start async task
 *     Worker->>Core: Execute in background thread
 *     Core-->>Worker: Task complete
 *     Worker-->>JS: Promise resolved
 * @endcode
 *
 * **Benefits**:
 * - Does not block Node.js main event loop
 * - Supports cancellation
 * - Errors auto-propagate to Promise
 *
 * @section error_mapping Error Mapping
 *
 * C++ error types to JavaScript Error mapping:
 *
 * | C++ Error Type | JavaScript Error Type | Description |
 * |:--------------|:----------------------|:------------|
 * | `SyntaxError` | `SyntaxError` | Syntax errors |
 * | `TypeError` | `TypeError` | Type errors |
 * | `RedefinitionError` | `Error` | Redefinition errors |
 * | `UndefinedSymbolError` | `ReferenceError` | Undefined symbols |
 * | `RuntimeError` | `Error` | Runtime errors |
 *
 * @section build Build Instructions
 *
 * ### Prerequisites
 *
 * - Node.js >= 14.x
 * - npm >= 6.x
 * - C++ compiler (g++/clang++/MSVC)
 * - node-gyp
 *
 * ### Build Steps
 *
 * @code{.bash}
 * # Install dependencies
 * npm install
 *
 * # Build native module
 * npm run build
 *
 * # Run tests
 * npm test
 * @endcode
 *
 * ### Project Structure
 *
 * @code
 * exports/
 * └── IC10/
 *     └── node/
 *         ├── include/              # Header files
 *         │   ├── main.hpp         # This documentation
 *         │   ├── lexer_adapter.hpp
 *         │   ├── token_adapter.hpp
 *         │   ├── parser_adapter.hpp
 *         │   ├── ast_adapter.hpp
 *         │   ├── analyser_adapter.hpp
 *         │   ├── symbol_table_adapter.hpp
 *         │   └── local_adapter.hpp
 *         ├── src/                # Source files
 *         │   ├── lexer_adapter.cpp
 *         │   ├── token_adapter.cpp
 *         │   └── ...
 *         ├── binding.gyp         # node-gyp configuration
 *         └── package.json
 * @endcode
 *
 * @section debugging Debugging Guide
 *
 * ### Enable Debug Logs
 *
 * @code{.javascript}
 * const ic10 = require('ic10-compiler');
 * ic10.setDebug(true);
 * @endcode
 *
 * ### Common Issues
 *
 * | Issue | Solution |
 * |:------|:---------|
 * | Build failed | Check C++ compiler version |
 * | Memory leak | Ensure adapters are properly released |
 * | Async hang | Check if coroutines resume correctly |
 *
 * @section best_practices Best Practices
 *
 * 1. **Reuse adapters**: Don't frequently create/destroy adapter instances
 * 2. **Batch processing**: Reuse Lexer instance for multiple lexical analyses
 * 3. **Error checking**: Always check return values for errors
 * 4. **Async waiting**: Use `await` instead of Promise.then
 *
 * @code{.javascript}
 * // Recommended: Reuse instance
 * const lexer = new Lexer(source);
 * for (const line of lines) {
 *     lexer.source = line;
 *     const tokens = lexer.scan();
 *     // Process tokens
 * }
 *
 * // Recommended: Check errors
 * const program = Parser.parse(tokens);
 * if (program.errors && program.errors.length > 0) {
 *     // Handle errors
 * }
 *
 * // Recommended: Async wait
 * await Analyser.analyse(program);
 * @endcode
 *
 * @section api_reference API Reference
 *
 * For complete API documentation, see Doxygen docs for each adapter:
 * - [LexerAdapter](@ref stationeers::ic10::LexerAdapter)
 * - [TokenAdapter](@ref stationeers::ic10::TokenAdapter)
 * - [ParserAdapter](@ref stationeers::ic10::ParserAdapter)
 * - [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter)
 * - [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter)
 * - [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter)
 * - [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter)
 *
 * @section related_docs Related Documentation
 *
 * - [@ref ic10_mainpage IC10 Compiler Core Documentation]
 * - [@ref common_node_mainpage Common Module Node.js Bindings]
 * - [Node.js N-API Documentation](https://nodejs.org/api/n-api.html)
 * - [node-gyp Documentation](https://github.com/nodejs/node-gyp)
 *
 * @section license License
 *
 * This project uses **CC BY-NC-SA** license, consistent with IC10 compiler core.
 *
 * @section contact Contact
 *
 * For questions or suggestions, please contact the author:
 * - Email: 2207150234@st.sziit.edu.cn
 *
 * @endif
 */

#ifndef EXPORT_IC10_NODE_MAIN_HPP
#define EXPORT_IC10_NODE_MAIN_HPP

/**
 * @if zh
 *
 * @namespace stationeers::ic10
 * @brief IC10 Node.js 绑定命名空间
 * @details 包含所有 Node.js 绑定的适配器类,用于将 C++ 编译器核心导出到 Node.js 环境
 *
 * @elseif en
 *
 * @namespace stationeers::ic10
 * @brief IC10 Node.js bindings namespace
 * @details Contains all Node.js binding adapter classes for exporting C++ compiler core to Node.js
 *
 * @endif
 */
namespace stationeers::ic10 {
}

#endif  // EXPORT_IC10_NODE_MAIN_HPP
