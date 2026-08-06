// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file main.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/20
 * @if zh
 * @mainpage IC10 Node.js 绑定文档
 *
 * @section overview 概述
 *
 * IC10 Node.js 绑定提供了一套将 C++ 编译器核心功能导出到 **Node.js** 环境的适配器层。
 * 通过 **N-API** (Node.js Native API) 实现跨语言调用，使得 JavaScript/TypeScript 开发者
 * 能够在 Node.js 环境中使用 IC10 编译器的全部功能，包括词法分析、语法分析、语义分析、
 * 链接和增量编译。
 *
 * @section architecture 架构设计
 *
 * ```mermaid
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
 *         LKA[LinkerAdapter]
 *         Loca[IC10LocalAdapter]
 *         IncLA[IncLexerAdapter]
 *         IncPA[IncParserAdapter]
 *         IncCA[IncCompilerAdapter]
 *     end
 *
 *     subgraph C++核心层
 *         Lexer[Lexer]
 *         Parser[Parser]
 *         Analyser[Analyser]
 *         ST[SymbolTable]
 *         Linker[Linker]
 *         IncLexer[IncLexer]
 *         IncParser[IncParser]
 *         IncCompiler[IncCompiler]
 *     end
 *
 *     JS --> LA
 *     JS --> TA
 *     JS --> PA
 *     JS --> AA
 *     JS --> STA
 *     JS --> LKA
 *     JS --> Loca
 *     JS --> IncLA
 *     JS --> IncPA
 *     JS --> IncCA
 *
 *     LA --> Lexer
 *     TA --> Lexer
 *     PA --> Parser
 *     AA --> Analyser
 *     STA --> ST
 *     LKA --> Linker
 *     IncLA --> IncLexer
 *     IncPA --> IncParser
 *     IncCA --> IncCompiler
 *
 *     Parser --> Lexer
 *     Analyser --> Parser
 *     Analyser --> ST
 * ```
 *
 * ### 设计原则
 *
 * 1. **零拷贝设计**: 通过引用传递减少数据复制
 * 2. **类型安全**: 使用适配器模式进行类型转换
 * 3. **异步优先**: 长时间操作使用异步工作器避免阻塞事件循环
 * 4. **错误可追踪**: 错误信息包含位置和上下文
 *
 * @section adapter_list 适配器列表
 *
 * 绑定层共导出 **11 个适配器**，覆盖编译器全部功能：
 *
 * | 适配器 | 文件 | 说明 |
 * |:------|:-----|:-----|
 * | [LexerAdapter](@ref stationeers::ic10::LexerAdapter) | lexer_adapter.hpp | 词法分析器绑定 |
 * | [TokenAdapter](@ref stationeers::ic10::TokenAdapter) | token_adapter.hpp | Token 对象绑定 |
 * | [ParserAdapter](@ref stationeers::ic10::ParserAdapter) | parser_adapter.hpp | 语法分析器绑定 |
 * | [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) | ast_adapter.hpp | AST Program 节点绑定 |
 * | [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter) | analyser_adapter.hpp | 语义分析器绑定 |
 * | [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) | symbol_table_adapter.hpp | 符号表绑定 |
 * | [LinkerAdapter](@ref stationeers::ic10::LinkerAdapter) | linker_adapter.hpp | 链接器绑定 |
 * | [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) | local_adapter.hpp | 本地化功能绑定 |
 * | [IncLexerAdapter](@ref stationeers::ic10::IncLexerAdapter) | inc_lexer_adapter.hpp | 增量词法分析器绑定 |
 * | [IncParserAdapter](@ref stationeers::ic10::IncParserAdapter) | inc_parser_adapter.hpp | 增量语法分析器绑定 |
 * | [IncCompilerAdapter](@ref stationeers::ic10::IncCompilerAdapter) | inc_compiler_adapter.hpp | 增量编译器绑定 |
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
 * const program = Parser.parsing(tokens);
 * console.log('AST:', program.toJSON());
 *
 * // 3. 语义分析 (异步)
 * const analyser = new Analyser();
 * await analyser.visit(program);
 * console.log('符号表:', analyser.symbolTable.toJSON());
 * console.log('诊断:', analyser.diagnostics);
 * @endcode
 *
 * ### 错误处理
 *
 * @code{.javascript}
 * const { Lexer, Parser } = require('ic10-compiler');
 *
 * const source = 'move r0';  // 缺少操作数
 * const tokens = Lexer.tokenize(source);
 * const program = Parser.parsing(tokens);
 *
 * // 检查词法分析器诊断
 * const lexer = new Lexer(source);
 * const scanTokens = lexer.scan();
 * if (lexer.diagnostics.length > 0) {
 *     for (const diag of lexer.diagnostics) {
 *         console.log(`词法错误: ${diag.message}`);
 *     }
 * }
 *
 * // 检查语法分析器诊断
 * const parser = new Parser(tokens);
 * const ast = parser.parse();
 * if (parser.diagnostics.length > 0) {
 *     for (const diag of parser.diagnostics) {
 *         console.log(`语法错误: ${diag.message}`);
 *         console.log(`位置: 行 ${diag.start.line}, 列 ${diag.start.column}`);
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
 * // 或创建实例（支持复用和获取诊断）
 * const lexer = new Lexer('add r0 r1 r2', false);
 * const tokens = lexer.scan();
 * const diags = lexer.diagnostics;
 * @endcode
 *
 * **导出方法**:
 * | 方法/属性 | 类型 | 说明 | 返回值 |
 * |:----------|:-----|:-----|:-------|
 * | `tokenize(src)` | 静态方法 | 词法分析 | `Token[]` |
 * | `scan()` | 实例方法 | 扫描源代码返回全部 Token | `Token[]` |
 * | `diagnostics` | 访问器 | 获取诊断列表 | `Diagnostic[]` |
 *
 * @note `tokenize()` 是无状态的静态方法；`scan()` 是实例方法，扫描构造时传入的源代码。
 * 如需获取诊断信息，必须使用实例方式。
 *
 * ### TokenAdapter
 *
 * [TokenAdapter](@ref stationeers::ic10::TokenAdapter) 表示 IC10 词法标记。
 *
 * @code{.javascript}
 * const { Token, TokenType, TokenCategory } = require('ic10-compiler');
 *
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
 * **方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `toString()` | 字符串表示 | `string` |
 * | `toJSON()` | JSON 表示 | `string` |
 *
 * **导出枚举**:
 * | 枚举 | 说明 |
 * |:-----|:-----|
 * | `TokenType` | 词法标记类型(通过编译期反射自动同步 C++ 枚举) |
 * | `TokenCategory` | 词法标记类别 |
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
 * // 方式一: 静态方法
 * const program = Parser.parsing(tokens);
 *
 * // 方式二: 实例方法（支持获取诊断）
 * const parser = new Parser(tokens, false);  // 第二参数为 debug 模式
 * const program = parser.parse();
 * const diags = parser.diagnostics;
 *
 * console.log('语句数量:', program.statements.length);
 * console.log('AST:', program.toJSON());
 * @endcode
 *
 * **导出方法**:
 * | 方法/属性 | 类型 | 说明 | 返回值 |
 * |:----------|:-----|:-----|:-------|
 * | `parsing(tokens)` | 静态方法 | 解析 Token 序列 | `Program` |
 * | `parse()` | 实例方法 | 解析构造时传入的 Token 序列 | `Program` |
 * | `diagnostics` | 访问器 | 获取诊断列表 | `Diagnostic[]` |
 *
 * @note `parsing()` 和 `parse()` 均接收 **Token 数组**，不是源代码字符串。
 * 构造函数参数为 `(tokens, debug?)`。
 *
 * ### ProgramAdapter (AST)
 *
 * [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) 表示 IC10 程序的抽象语法树根节点。
 *
 * @code{.javascript}
 * const program = Parser.parsing(tokens);
 *
 * // 访问属性
 * console.log('Node name:', Program.nodeName);  // "Program" (静态属性)
 * console.log('Statements:', program.statements);  // 语句数组 (访问器)
 * console.log('End position:', program.end);  // 结束位置 (访问器)
 *
 * // 转换
 * console.log(program.toString());
 * console.log(program.toJSON());
 * @endcode
 *
 * **属性/方法**:
 * | 属性/方法 | 类型 | 说明 | 返回值 |
 * |:----------|:-----|:-----|:-------|
 * | `nodeName` | 静态访问器 | 获取节点名称 | `string` |
 * | `statements` | 实例访问器 | 获取语句列表 | `Statement[]` |
 * | `end` | 实例访问器 | 结束位置 | `Pos` |
 * | `toString()` | 实例方法 | 字符串表示 | `string` |
 * | `toJSON()` | 实例方法 | JSON 表示 | `string` |
 *
 * @note ProgramAdapter **没有** `start()` 方法。起始位置可通过第一个语句的位置获取。
 * `nodeName` 是静态访问器，通过 `Program.nodeName` 访问，不是实例方法。
 *
 * **导出枚举**:
 * | 枚举 | 说明 |
 * |:-----|:-----|
 * | `OperandType` | 操作数类型（AST JSON 序列化中使用） |
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
 * const program = Parser.parsing(tokens);
 *
 * // 方式一: 静态方法（异步）
 * await Analyser.analyse(program);
 *
 * // 方式二: 实例方法（异步，可获取结果）
 * const analyser = new Analyser();
 * await analyser.visit(program);
 *
 * // 获取分析结果
 * const symbolTable = analyser.symbolTable;
 * const diagnostics = analyser.diagnostics;
 *
 * if (diagnostics.length > 0) {
 *     console.log('发现诊断信息:');
 *     for (const diag of diagnostics) {
 *         console.log(`  - ${diag.message}`);
 *     }
 * }
 * @endcode
 *
 * **导出方法**:
 * | 方法/属性 | 类型 | 说明 | 返回值 |
 * |:----------|:-----|:-----|:-------|
 * | `analyse(program)` | 静态方法 | 异步语义分析 | `Promise<void>` |
 * | `visit(program)` | 实例方法 | 异步访问程序节点 | `Promise<void>` |
 * | `diagnostics` | 访问器 | 获取诊断列表 | `Diagnostic[]` |
 * | `symbolTable` | 访问器 | 获取符号表 | `SymbolTable` |
 *
 * @note `analyse()` 是静态方法，不保留分析结果；`visit()` 是实例方法，分析后可通过
 * `symbolTable` 和 `diagnostics` 访问器获取结果。两者均返回 Promise，必须使用 `await`。
 *
 * ### SymbolTableAdapter
 *
 * [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) 提供符号表的访问接口。
 *
 * @code{.javascript}
 * const analyser = new Analyser();
 * await analyser.visit(program);
 *
 * // 获取符号表
 * const symbolTable = analyser.symbolTable;
 *
 * // 转为 JSON
 * console.log(symbolTable.toJSON());
 * @endcode
 *
 * **方法**:
 * | 方法 | 说明 | 返回值 |
 * |:-----|:-----|:-------|
 * | `toJSON()` | JSON 表示 | `string` |
 *
 * ### LinkerAdapter
 *
 * [LinkerAdapter](@ref stationeers::ic10::LinkerAdapter) 将 C++ 链接器导出到 Node.js。
 * 支持多编译单元的符号合并和跨单元引用解析。
 *
 * @code{.javascript}
 * const { Lexer, Parser, Linker } = require('ic10-compiler');
 *
 * const linker = new Linker();
 *
 * // 方式一: 添加已编译的 Program 对象
 * const tokens1 = Lexer.tokenize(source1);
 * const program1 = Parser.parsing(tokens1);
 * linker.addUnit(program1);
 * linker.addUnit(program1, 'file1.ic');  // 带路径
 *
 * // 方式二: 添加源代码字符串（内部自动词法和语法分析）
 * linker.addUnit(source2);
 * linker.addUnit(source2, 'file2.ic');  // 带路径
 *
 * // 执行链接
 * const symbolTable = linker.link();
 *
 * // 获取诊断信息
 * const diagnostics = linker.diagnostics;
 *
 * // 获取编译单元信息
 * const units = linker.units;
 * for (const unit of units) {
 *     console.log(`Unit: ${unit.path}`);
 *     for (const diag of unit.diagnostics) {
 *         console.log(`  ${diag.message}`);
 *     }
 * }
 * @endcode
 *
 * **导出方法**:
 * | 方法/属性 | 类型 | 说明 | 返回值 |
 * |:----------|:-----|:-----|:-------|
 * | `addUnit(program, path?)` | 实例方法 | 添加 Program 对象 | `undefined` |
 * | `addUnit(source, path?)` | 实例方法 | 添加源代码字符串 | `undefined` |
 * | `link()` | 实例方法 | 执行链接 | `SymbolTable` |
 * | `diagnostics` | 访问器 | 获取所有诊断 | `Diagnostic[]` |
 * | `units` | 访问器 | 获取编译单元信息 | `UnitInfo[]` |
 *
 * **UnitInfo 结构**:
 * | 属性 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `path` | `string` | 编译单元路径 |
 * | `diagnostics` | `Diagnostic[]` | 该单元的诊断信息 |
 *
 * ### IC10LocalAdapter
 *
 * [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) 提供多语言本地化支持。
 *
 * @code{.javascript}
 * const { IC10Local } = require('ic10-compiler');
 *
 * // 设置语言（静态方法）
 * IC10Local.setLanguage('zh-hans');  // 简体中文
 * IC10Local.setLanguage('en-us');    // 英文
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `setLanguage(lang)` | 静态方法 | 设置当前语言 |
 *
 * **支持的语言**:
 * | 代码 | 语言 |
 * |:-----|:-----|
 * | `zh-hans` | 简体中文 |
 * | `en-us` | 美式英语 |
 *
 * @note IC10LocalAdapter **仅提供** `setLanguage()` 方法，不支持获取当前语言。
 * 默认语言为英文 (`en-us`)。
 *
 * ### IncLexerAdapter
 *
 * [IncLexerAdapter](@ref stationeers::ic10::IncLexerAdapter) 提供增量词法分析功能，
 * 仅重新分析修改的行，显著提升编辑场景下的性能。
 *
 * @code{.javascript}
 * const { IncLexer } = require('ic10-compiler');
 *
 * const lexer = new IncLexer();
 *
 * // 全量词法分析
 * const result1 = lexer.tokenizeFull(source);
 * console.log(`Tokens: ${result1.tokens.length}`);
 *
 * // 增量词法分析（仅处理变化部分）
 * const modifiedSource = source.replace('move', 'add');
 * const result2 = lexer.tokenizeInc(modifiedSource);
 * console.log(`增量: ${result2.incremental}`);
 * console.log(`重新词法分析的行: ${result2.relexedLines}`);
 *
 * // 检查缓存状态
 * if (lexer.hasCache()) {
 *     console.log('有缓存');
 * }
 *
 * // 清除缓存
 * lexer.clear();
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 类型 | 说明 | 返回值 |
 * |:-----|:-----|:-----|:-------|
 * | `tokenizeFull(src)` | 实例方法 | 全量词法分析 | `IncLexerResult` |
 * | `tokenizeInc(src)` | 实例方法 | 增量词法分析 | `IncLexerResult` |
 * | `hasCache()` | 实例方法 | 检查是否有缓存 | `boolean` |
 * | `clear()` | 实例方法 | 清除缓存 | `undefined` |
 *
 * **IncLexerResult 结构**:
 * | 属性 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `tokens` | `Token[]` | Token 序列 |
 * | `incremental` | `boolean` | 是否使用了增量分析 |
 * | `relexedLines` | `number` | 重新词法分析的行数 |
 *
 * ### IncParserAdapter
 *
 * [IncParserAdapter](@ref stationeers::ic10::IncParserAdapter) 提供增量语法分析功能，
 * 在语句级别进行缓存，仅重新解析修改的语句。
 *
 * @code{.javascript}
 * const { IncParser } = require('ic10-compiler');
 *
 * const parser = new IncParser();
 *
 * // 全量语法分析
 * const result1 = parser.parseFull(tokens);
 * console.log(`AST: ${result1.ast.toJSON()}`);
 *
 * // 增量语法分析
 * const result2 = parser.parseInc(newTokens);
 * console.log(`增量: ${result2.incremental}`);
 * console.log(`重新解析的语句: ${result2.reparsedStmts}`);
 *
 * // 清除缓存
 * parser.clear();
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 类型 | 说明 | 返回值 |
 * |:-----|:-----|:-----|:-------|
 * | `parseFull(tokens)` | 实例方法 | 全量语法分析 | `IncParserResult` |
 * | `parseInc(tokens)` | 实例方法 | 增量语法分析 | `IncParserResult` |
 * | `hasCache()` | 实例方法 | 检查是否有缓存 | `boolean` |
 * | `clear()` | 实例方法 | 清除缓存 | `undefined` |
 *
 * **IncParserResult 结构**:
 * | 属性 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `ast` | `Program` | AST 根节点 |
 * | `incremental` | `boolean` | 是否使用了增量分析 |
 * | `reparsedStmts` | `number` | 重新解析的语句数 |
 *
 * ### IncCompilerAdapter
 *
 * [IncCompilerAdapter](@ref stationeers::ic10::IncCompilerAdapter) 提供增量编译功能，
 * 整合增量词法分析和增量语法分析，提供一站式的增量编译体验。
 *
 * @code{.javascript}
 * const { IncCompiler } = require('ic10-compiler');
 *
 * const compiler = new IncCompiler();
 *
 * // 全量编译
 * const result1 = compiler.compileFull(source);
 * console.log(`Tokens: ${result1.tokens.length}`);
 * console.log(`AST: ${result1.ast.toJSON()}`);
 *
 * // 增量编译
 * const modifiedSource = source.replace('move', 'add');
 * const result2 = compiler.compileInc(modifiedSource);
 * console.log(`增量: ${result2.incremental}`);
 * console.log(`重新词法分析的行: ${result2.relexedLines}`);
 * console.log(`重新解析的语句: ${result2.reparsedStmts}`);
 *
 * // 检查缓存状态
 * if (compiler.hasCache()) {
 *     console.log('有缓存');
 * }
 *
 * // 清除缓存
 * compiler.clear();
 * @endcode
 *
 * **导出方法**:
 * | 方法 | 类型 | 说明 | 返回值 |
 * |:-----|:-----|:-----|:-------|
 * | `compileFull(src)` | 实例方法 | 全量编译 | `IncCompileResult` |
 * | `compileInc(src)` | 实例方法 | 增量编译 | `IncCompileResult` |
 * | `hasCache()` | 实例方法 | 检查是否有缓存 | `boolean` |
 * | `clear()` | 实例方法 | 清除缓存 | `undefined` |
 *
 * **IncCompileResult 结构**:
 * | 属性 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `tokens` | `Token[]` | Token 序列 |
 * | `ast` | `Program` | AST 根节点 |
 * | `incremental` | `boolean` | 是否使用了增量编译 |
 * | `relexedLines` | `number` | 重新词法分析的行数 |
 * | `reparsedStmts` | `number` | 重新解析的语句数 |
 *
 * @section type_conversion 类型转换
 *
 * C++ 与 JavaScript 之间的类型映射关系：
 *
 * | C++ 类型 | JavaScript 类型 | 说明 |
 * |:---------|:----------------|:-----|
 * | `int`, `float`, `double` | `number` | 数值类型 |
 * | `std::string` | `string` | 字符串 |
 * | `bool` | `boolean` | 布尔值 |
 * | `std::vector<T>` | `Array` | 数组 |
 * | `std::unordered_map<K,V>` | `Object` | 对象 |
 * | `std::optional<T>` | `T \| undefined` | 可选值 |
 * | `Pos` | `Pos` | 位置对象（适配器） |
 * | `Token` | `Token` | 词法标记（适配器） |
 * | `Program` | `Program` | AST 根节点（适配器） |
 * | `SymbolTable` | `SymbolTable` | 符号表（适配器） |
 * | `Diagnostic` | `Object` | 诊断信息（普通对象） |
 * | `Error` | `Error` | 错误对象（适配器） |
 *
 * **Diagnostic 对象结构**:
 * | 属性 | 类型 | 说明 |
 * |:-----|:-----|:-----|
 * | `level` | `string` | 严重级别 (`error`/`warning`/`info`) |
 * | `id` | `string` | 消息 ID (如 `IWL1`, `IPL1`) |
 * | `start` | `Pos` | 起始位置 |
 * | `end` | `Pos` | 结束位置 |
 * | `message` | `string` | 消息内容（根据当前语言） |
 *
 * @section async_workers 异步工作器
 *
 * 语义分析等长时间运行的操作使用 `TaskWorker` 在后台线程执行：
 *
 * ```mermaid
 * sequenceDiagram
 *     participant JS as JavaScript
 *     participant Worker as TaskWorker
 *     participant Core as C++ Core
 *
 *     JS->>Worker: 启动异步任务
 *     Worker->>Core: 在后台线程执行
 *     Core-->>Worker: 任务完成
 *     Worker-->>JS: Promise resolved
 * ```
 *
 * **优点**:
 * - 不阻塞 Node.js 主事件循环
 * - 支持协程调度（处理前向引用）
 * - 错误自动传播到 Promise
 *
 * @note `Analyser.analyse()` 和 `Analyser.visit()` 是仅有的异步方法，
 * 其他适配器方法均为同步调用。
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
 * @note 诊断信息（Diagnostic）不是错误，不会抛出异常。
 * 需通过 `diagnostics` 访问器主动检查。
 *
 * @section build 构建说明
 *
 * ### 前提条件
 *
 * - Node.js >= 14.x
 * - npm >= 6.x
 * - C++ 编译器 (g++/clang++/MSVC)
 * - CMake >= 3.15
 * - node-gyp (可选)
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
 *         │   └── ic10_node/
 *         │       ├── lexer/
 *         │       │   ├── lexer_adapter.hpp
 *         │       │   └── token_adapter.hpp
 *         │       ├── parser/
 *         │       │   ├── parser_adapter.hpp
 *         │       │   └── ast_adapter.hpp
 *         │       ├── semantic/
 *         │       │   ├── analyser_adapter.hpp
 *         │       │   └── symbol_table_adapter.hpp
 *         │       ├── link/
 *         │       │   └── linker_adapter.hpp
 *         │       ├── incremental/
 *         │       │   ├── inc_lexer_adapter.hpp
 *         │       │   ├── inc_parser_adapter.hpp
 *         │       │   └── inc_compiler_adapter.hpp
 *         │       ├── locals/
 *         │       │   └── local_adapter.hpp
 *         │       └── main.hpp      # 本文档
 *         ├── src/                  # 源文件
 *         │   ├── lexer/
 *         │   ├── parser/
 *         │   ├── semantic/
 *         │   ├── link/
 *         │   ├── incremental/
 *         │   ├── locals/
 *         │   └── build.cpp         # 模块入口
 *         ├── CMakeLists.txt        # CMake 配置
 *         ├── Doxyfile              # Doxygen 配置
 *         └── package.json
 * @endcode
 *
 * @section debugging 调试指南
 *
 * ### 启用调试模式
 *
 * Lexer 和 Parser 支持在构造时启用调试模式：
 *
 * @code{.javascript}
 * const { Lexer, Parser } = require('ic10-compiler');
 *
 * // 调试模式（第二参数为 true）
 * const lexer = new Lexer(source, true);
 * const tokens = lexer.scan();
 *
 * const parser = new Parser(tokens, true);
 * const program = parser.parse();
 * @endcode
 *
 * ### 常见问题
 *
 * | 问题 | 原因 | 解决方案 |
 * |:-----|:-----|:---------|
 * | 构建失败 | C++ 编译器版本过低 | 使用支持 C++20 的编译器 |
 * | 内存泄漏 | adapter 未正确释放 | 避免在循环中频繁创建实例 |
 * | 异步挂起 | 协程未正确恢复 | 确保 `await` 调用完整 |
 * | 诊断为空 | 使用了静态方法 | 改用实例方法获取诊断 |
 * | 增量不生效 | 缓存被清除 | 检查是否调用了 `clear()` |
 *
 * @section best_practices 最佳实践
 *
 * 1. **复用适配器**: 不要频繁创建/销毁适配器实例
 * 2. **批量处理**: 多次词法分析时复用 Lexer 实例
 * 3. **错误检查**: 始终检查 `diagnostics` 访问器
 * 4. **异步等待**: 使用 `await` 而非 Promise.then
 * 5. **增量优先**: 编辑器场景使用 IncCompiler 而非全量编译
 *
 * @code{.javascript}
 * // 推荐: 复用实例
 * const lexer = new Lexer(source);
 * const tokens = lexer.scan();
 * if (lexer.diagnostics.length > 0) {
 *     // 处理词法错误
 * }
 *
 * // 推荐: 使用增量编译器处理编辑场景
 * const compiler = new IncCompiler();
 * compiler.compileFull(initialSource);
 * // 后续编辑
 * compiler.compileInc(modifiedSource);
 *
 * // 推荐: 异步等待
 * const analyser = new Analyser();
 * await analyser.visit(program);
 * const diags = analyser.diagnostics;
 * @endcode
 *
 * @section dev_notes 开发者必知
 *
 * ### 适配器设计模式
 *
 * 所有适配器继承自 `node::ObjectWrap<T>`，通过 N-API 将 C++ 类暴露为 JavaScript 类。
 * 每个适配器遵循统一的设计模式：
 *
 * 1. **构造函数**: 从 JavaScript 参数创建 C++ 对象
 * 2. **init() 静态方法**: 注册类到 Node.js 模块
 * 3. **to() 静态方法**: C++ 对象 → JavaScript 对象
 * 4. **from() 静态方法**: JavaScript 对象 → C++ 对象
 * 5. **访问器/方法**: 暴露 C++ 对象的功能
 *
 * ### 诊断系统
 *
 * 诊断信息不是异常，不会中断编译流程。编译器会收集所有诊断信息并通过 `diagnostics`
 * 访问器暴露。诊断信息包含：
 * - **level**: 严重级别 (`error`/`warning`/`info`)
 * - **id**: 消息 ID（用于国际化）
 * - **start/end**: 源代码位置范围
 * - **message**: 本地化消息内容
 *
 * ### 同步与异步
 *
 * | 适配器 | 同步方法 | 异步方法 |
 * |:-------|:---------|:---------|
 * | Lexer | `tokenize()`, `scan()` | - |
 * | Parser | `parsing()`, `parse()` | - |
 * | Analyser | - | `analyse()`, `visit()` |
 * | Linker | `addUnit()`, `link()` | - |
 * | IncCompiler | `compileFull()`, `compileInc()` | - |
 *
 * 仅 AnalyserAdapter 的方法为异步（返回 Promise），因为语义分析需要处理前向引用，
 * 使用协程实现异步等待。
 *
 * ### 枚举导出
 *
 * C++ 枚举通过编译期反射自动导出到 JavaScript：
 * - `TokenType`: 词法标记类型（100+ 值）
 * - `TokenCategory`: 标记类别
 * - `OperandType`: AST 操作数类型
 *
 * 枚举值在 JavaScript 中以数值形式表示，与 C++ 枚举值一一对应。
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
 * - [LinkerAdapter](@ref stationeers::ic10::LinkerAdapter)
 * - [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter)
 * - [IncLexerAdapter](@ref stationeers::ic10::IncLexerAdapter)
 * - [IncParserAdapter](@ref stationeers::ic10::IncParserAdapter)
 * - [IncCompilerAdapter](@ref stationeers::ic10::IncCompilerAdapter)
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
 * developers can use all IC10 compiler features in the Node.js environment, including lexical
 * analysis, syntax analysis, semantic analysis, linking, and incremental compilation.
 *
 * @section architecture Architecture
 *
 * ```mermaid
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
 *         LKA[LinkerAdapter]
 *         Loca[IC10LocalAdapter]
 *         IncLA[IncLexerAdapter]
 *         IncPA[IncParserAdapter]
 *         IncCA[IncCompilerAdapter]
 *     end
 *
 *     subgraph CoreLayer
 *         Lexer[Lexer]
 *         Parser[Parser]
 *         Analyser[Analyser]
 *         ST[SymbolTable]
 *         Linker[Linker]
 *         IncLexer[IncLexer]
 *         IncParser[IncParser]
 *         IncCompiler[IncCompiler]
 *     end
 *
 *     JS --> LA
 *     JS --> TA
 *     JS --> PA
 *     JS --> AA
 *     JS --> STA
 *     JS --> LKA
 *     JS --> Loca
 *     JS --> IncLA
 *     JS --> IncPA
 *     JS --> IncCA
 *
 *     LA --> Lexer
 *     TA --> Lexer
 *     PA --> Parser
 *     AA --> Analyser
 *     STA --> ST
 *     LKA --> Linker
 *     IncLA --> IncLexer
 *     IncPA --> IncParser
 *     IncCA --> IncCompiler
 *
 *     Parser --> Lexer
 *     Analyser --> Parser
 *     Analyser --> ST
 * ```
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
 * The binding layer exports **11 adapters** covering all compiler functionality:
 *
 * | Adapter | File | Description |
 * |:--------|:-----|:------------|
 * | [LexerAdapter](@ref stationeers::ic10::LexerAdapter) | lexer_adapter.hpp | Lexer binding |
 * | [TokenAdapter](@ref stationeers::ic10::TokenAdapter) | token_adapter.hpp | Token object binding |
 * | [ParserAdapter](@ref stationeers::ic10::ParserAdapter) | parser_adapter.hpp | Parser binding |
 * | [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) | ast_adapter.hpp | AST Program node binding |
 * | [AnalyserAdapter](@ref stationeers::ic10::AnalyserAdapter) | analyser_adapter.hpp | Semantic analyser binding |
 * | [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) | symbol_table_adapter.hpp | Symbol table binding |
 * | [LinkerAdapter](@ref stationeers::ic10::LinkerAdapter) | linker_adapter.hpp | Linker binding |
 * | [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) | local_adapter.hpp | Localization binding |
 * | [IncLexerAdapter](@ref stationeers::ic10::IncLexerAdapter) | inc_lexer_adapter.hpp | Incremental lexer binding |
 * | [IncParserAdapter](@ref stationeers::ic10::IncParserAdapter) | inc_parser_adapter.hpp | Incremental parser binding |
 * | [IncCompilerAdapter](@ref stationeers::ic10::IncCompilerAdapter) | inc_compiler_adapter.hpp | Incremental compiler binding |
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
 * const program = Parser.parsing(tokens);
 * console.log('AST:', program.toJSON());
 *
 * // 3. Semantic analysis (async)
 * const analyser = new Analyser();
 * await analyser.visit(program);
 * console.log('Symbol table:', analyser.symbolTable.toJSON());
 * console.log('Diagnostics:', analyser.diagnostics);
 * @endcode
 *
 * ### Error Handling
 *
 * @code{.javascript}
 * const { Lexer, Parser } = require('ic10-compiler');
 *
 * const source = 'move r0';  // Missing operand
 * const tokens = Lexer.tokenize(source);
 * const program = Parser.parsing(tokens);
 *
 * // Check lexer diagnostics
 * const lexer = new Lexer(source);
 * const scanTokens = lexer.scan();
 * if (lexer.diagnostics.length > 0) {
 *     for (const diag of lexer.diagnostics) {
 *         console.log(`Lexical error: ${diag.message}`);
 *     }
 * }
 *
 * // Check parser diagnostics
 * const parser = new Parser(tokens);
 * const ast = parser.parse();
 * if (parser.diagnostics.length > 0) {
 *     for (const diag of parser.diagnostics) {
 *         console.log(`Syntax error: ${diag.message}`);
 *         console.log(`Position: Line ${diag.start.line}, Column ${diag.start.column}`);
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
 * // Or create instance (supports reuse and diagnostics)
 * const lexer = new Lexer('add r0 r1 r2', false);
 * const tokens = lexer.scan();
 * const diags = lexer.diagnostics;
 * @endcode
 *
 * **Exported Methods**:
 * | Method/Property | Type | Description | Return |
 * |:----------------|:-----|:------------|:-------|
 * | `tokenize(src)` | Static method | Lex analysis | `Token[]` |
 * | `scan()` | Instance method | Scan source code, return all tokens | `Token[]` |
 * | `diagnostics` | Accessor | Get diagnostic list | `Diagnostic[]` |
 *
 * @note `tokenize()` is a stateless static method; `scan()` is an instance method that
 * scans the source code passed to the constructor. To get diagnostics, use the instance approach.
 *
 * ### TokenAdapter
 *
 * [TokenAdapter](@ref stationeers::ic10::TokenAdapter) represents IC10 lexical token.
 *
 * @code{.javascript}
 * const { Token, TokenType, TokenCategory } = require('ic10-compiler');
 *
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
 * **Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `toString()` | String representation | `string` |
 * | `toJSON()` | JSON representation | `string` |
 *
 * **Exported Enums**:
 * | Enum | Description |
 * |:-----|:------------|
 * | `TokenType` | Token type (auto-synced with C++ enum via compile-time reflection) |
 * | `TokenCategory` | Token category |
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
 * // Method 1: Static method
 * const program = Parser.parsing(tokens);
 *
 * // Method 2: Instance method (supports diagnostics)
 * const parser = new Parser(tokens, false);  // Second param is debug mode
 * const program = parser.parse();
 * const diags = parser.diagnostics;
 *
 * console.log('Statement count:', program.statements.length);
 * console.log('AST:', program.toJSON());
 * @endcode
 *
 * **Exported Methods**:
 * | Method/Property | Type | Description | Return |
 * |:----------------|:-----|:------------|:-------|
 * | `parsing(tokens)` | Static method | Parse Token sequence | `Program` |
 * | `parse()` | Instance method | Parse Token sequence from constructor | `Program` |
 * | `diagnostics` | Accessor | Get diagnostic list | `Diagnostic[]` |
 *
 * @note Both `parsing()` and `parse()` accept **Token arrays**, not source code strings.
 * Constructor parameters: `(tokens, debug?)`.
 *
 * ### ProgramAdapter (AST)
 *
 * [ProgramAdapter](@ref stationeers::ic10::ProgramAdapter) represents IC10 program AST root node.
 *
 * @code{.javascript}
 * const program = Parser.parsing(tokens);
 *
 * // Access properties
 * console.log('Node name:', Program.nodeName);  // "Program" (static accessor)
 * console.log('Statements:', program.statements);  // Statement array (accessor)
 * console.log('End position:', program.end);  // End position (accessor)
 *
 * // Conversion
 * console.log(program.toString());
 * console.log(program.toJSON());
 * @endcode
 *
 * **Properties/Methods**:
 * | Property/Method | Type | Description | Return |
 * |:----------------|:-----|:------------|:-------|
 * | `nodeName` | Static accessor | Get node name | `string` |
 * | `statements` | Instance accessor | Get statement list | `Statement[]` |
 * | `end` | Instance accessor | End position | `Pos` |
 * | `toString()` | Instance method | String representation | `string` |
 * | `toJSON()` | Instance method | JSON representation | `string` |
 *
 * @note ProgramAdapter does **not** have a `start()` method. Start position can be
 * obtained from the first statement's position. `nodeName` is a static accessor,
 * accessed via `Program.nodeName`, not an instance method.
 *
 * **Exported Enums**:
 * | Enum | Description |
 * |:-----|:------------|
 * | `OperandType` | Operand type (used in AST JSON serialization) |
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
 * const program = Parser.parsing(tokens);
 *
 * // Method 1: Static method (async)
 * await Analyser.analyse(program);
 *
 * // Method 2: Instance method (async, can get results)
 * const analyser = new Analyser();
 * await analyser.visit(program);
 *
 * // Get analysis results
 * const symbolTable = analyser.symbolTable;
 * const diagnostics = analyser.diagnostics;
 *
 * if (diagnostics.length > 0) {
 *     console.log('Diagnostics found:');
 *     for (const diag of diagnostics) {
 *         console.log(`  - ${diag.message}`);
 *     }
 * }
 * @endcode
 *
 * **Exported Methods**:
 * | Method/Property | Type | Description | Return |
 * |:----------------|:-----|:------------|:-------|
 * | `analyse(program)` | Static method | Async semantic analysis | `Promise<void>` |
 * | `visit(program)` | Instance method | Async visit program node | `Promise<void>` |
 * | `diagnostics` | Accessor | Get diagnostic list | `Diagnostic[]` |
 * | `symbolTable` | Accessor | Get symbol table | `SymbolTable` |
 *
 * @note `analyse()` is a static method that doesn't retain results; `visit()` is an instance
 * method, after which results can be accessed via `symbolTable` and `diagnostics` accessors.
 * Both return Promises and must be awaited.
 *
 * ### SymbolTableAdapter
 *
 * [SymbolTableAdapter](@ref stationeers::ic10::SymbolTableAdapter) provides symbol table access interface.
 *
 * @code{.javascript}
 * const analyser = new Analyser();
 * await analyser.visit(program);
 *
 * // Get symbol table
 * const symbolTable = analyser.symbolTable;
 *
 * // Convert to JSON
 * console.log(symbolTable.toJSON());
 * @endcode
 *
 * **Methods**:
 * | Method | Description | Return |
 * |:-------|:------------|:-------|
 * | `toJSON()` | JSON representation | `string` |
 *
 * ### LinkerAdapter
 *
 * [LinkerAdapter](@ref stationeers::ic10::LinkerAdapter) exports C++ linker to Node.js.
 * Supports multi-unit symbol merging and cross-unit reference resolution.
 *
 * @code{.javascript}
 * const { Lexer, Parser, Linker } = require('ic10-compiler');
 *
 * const linker = new Linker();
 *
 * // Method 1: Add compiled Program object
 * const tokens1 = Lexer.tokenize(source1);
 * const program1 = Parser.parsing(tokens1);
 * linker.addUnit(program1);
 * linker.addUnit(program1, 'file1.ic');  // With path
 *
 * // Method 2: Add source code string (auto lexes and parses internally)
 * linker.addUnit(source2);
 * linker.addUnit(source2, 'file2.ic');  // With path
 *
 * // Perform linking
 * const symbolTable = linker.link();
 *
 * // Get diagnostics
 * const diagnostics = linker.diagnostics;
 *
 * // Get compilation units
 * const units = linker.units;
 * for (const unit of units) {
 *     console.log(`Unit: ${unit.path}`);
 *     for (const diag of unit.diagnostics) {
 *         console.log(`  ${diag.message}`);
 *     }
 * }
 * @endcode
 *
 * **Exported Methods**:
 * | Method/Property | Type | Description | Return |
 * |:----------------|:-----|:------------|:-------|
 * | `addUnit(program, path?)` | Instance method | Add Program object | `undefined` |
 * | `addUnit(source, path?)` | Instance method | Add source code string | `undefined` |
 * | `link()` | Instance method | Perform linking | `SymbolTable` |
 * | `diagnostics` | Accessor | Get all diagnostics | `Diagnostic[]` |
 * | `units` | Accessor | Get compilation units | `UnitInfo[]` |
 *
 * **UnitInfo Structure**:
 * | Property | Type | Description |
 * |:---------|:-----|:------------|
 * | `path` | `string` | Compilation unit path |
 * | `diagnostics` | `Diagnostic[]` | Diagnostics for this unit |
 *
 * ### IC10LocalAdapter
 *
 * [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter) provides multi-language localization support.
 *
 * @code{.javascript}
 * const { IC10Local } = require('ic10-compiler');
 *
 * // Set language (static method)
 * IC10Local.setLanguage('zh-hans');  // Simplified Chinese
 * IC10Local.setLanguage('en-us');    // English
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Type | Description |
 * |:-------|:-----|:------------|
 * | `setLanguage(lang)` | Static method | Set current language |
 *
 * **Supported Languages**:
 * | Code | Language |
 * |:-----|:---------|
 * | `zh-hans` | Simplified Chinese |
 * | `en-us` | American English |
 *
 * @note IC10LocalAdapter **only provides** `setLanguage()` method, does not support getting
 * current language. Default language is English (`en-us`).
 *
 * ### IncLexerAdapter
 *
 * [IncLexerAdapter](@ref stationeers::ic10::IncLexerAdapter) provides incremental lexical analysis,
 * only re-analyzing modified lines, significantly improving performance in editing scenarios.
 *
 * @code{.javascript}
 * const { IncLexer } = require('ic10-compiler');
 *
 * const lexer = new IncLexer();
 *
 * // Full lexical analysis
 * const result1 = lexer.tokenizeFull(source);
 * console.log(`Tokens: ${result1.tokens.length}`);
 *
 * // Incremental lexical analysis (only processes changed parts)
 * const modifiedSource = source.replace('move', 'add');
 * const result2 = lexer.tokenizeInc(modifiedSource);
 * console.log(`Incremental: ${result2.incremental}`);
 * console.log(`Re-lexed lines: ${result2.relexedLines}`);
 *
 * // Check cache status
 * if (lexer.hasCache()) {
 *     console.log('Has cache');
 * }
 *
 * // Clear cache
 * lexer.clear();
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Type | Description | Return |
 * |:-------|:-----|:------------|:-------|
 * | `tokenizeFull(src)` | Instance method | Full lexical analysis | `IncLexerResult` |
 * | `tokenizeInc(src)` | Instance method | Incremental lexical analysis | `IncLexerResult` |
 * | `hasCache()` | Instance method | Check if cache exists | `boolean` |
 * | `clear()` | Instance method | Clear cache | `undefined` |
 *
 * **IncLexerResult Structure**:
 * | Property | Type | Description |
 * |:---------|:-----|:------------|
 * | `tokens` | `Token[]` | Token sequence |
 * | `incremental` | `boolean` | Whether incremental analysis was used |
 * | `relexedLines` | `number` | Number of re-lexed lines |
 *
 * ### IncParserAdapter
 *
 * [IncParserAdapter](@ref stationeers::ic10::IncParserAdapter) provides incremental syntax analysis,
 * caching at statement level, only re-parsing modified statements.
 *
 * @code{.javascript}
 * const { IncParser } = require('ic10-compiler');
 *
 * const parser = new IncParser();
 *
 * // Full syntax analysis
 * const result1 = parser.parseFull(tokens);
 * console.log(`AST: ${result1.ast.toJSON()}`);
 *
 * // Incremental syntax analysis
 * const result2 = parser.parseInc(newTokens);
 * console.log(`Incremental: ${result2.incremental}`);
 * console.log(`Re-parsed statements: ${result2.reparsedStmts}`);
 *
 * // Clear cache
 * parser.clear();
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Type | Description | Return |
 * |:-------|:-----|:------------|:-------|
 * | `parseFull(tokens)` | Instance method | Full syntax analysis | `IncParserResult` |
 * | `parseInc(tokens)` | Instance method | Incremental syntax analysis | `IncParserResult` |
 * | `hasCache()` | Instance method | Check if cache exists | `boolean` |
 * | `clear()` | Instance method | Clear cache | `undefined` |
 *
 * **IncParserResult Structure**:
 * | Property | Type | Description |
 * |:---------|:-----|:------------|
 * | `ast` | `Program` | AST root node |
 * | `incremental` | `boolean` | Whether incremental analysis was used |
 * | `reparsedStmts` | `number` | Number of re-parsed statements |
 *
 * ### IncCompilerAdapter
 *
 * [IncCompilerAdapter](@ref stationeers::ic10::IncCompilerAdapter) provides incremental compilation,
 * integrating incremental lexer and incremental parser for a one-stop incremental compilation experience.
 *
 * @code{.javascript}
 * const { IncCompiler } = require('ic10-compiler');
 *
 * const compiler = new IncCompiler();
 *
 * // Full compilation
 * const result1 = compiler.compileFull(source);
 * console.log(`Tokens: ${result1.tokens.length}`);
 * console.log(`AST: ${result1.ast.toJSON()}`);
 *
 * // Incremental compilation
 * const modifiedSource = source.replace('move', 'add');
 * const result2 = compiler.compileInc(modifiedSource);
 * console.log(`Incremental: ${result2.incremental}`);
 * console.log(`Re-lexed lines: ${result2.relexedLines}`);
 * console.log(`Re-parsed statements: ${result2.reparsedStmts}`);
 *
 * // Check cache status
 * if (compiler.hasCache()) {
 *     console.log('Has cache');
 * }
 *
 * // Clear cache
 * compiler.clear();
 * @endcode
 *
 * **Exported Methods**:
 * | Method | Type | Description | Return |
 * |:-------|:-----|:------------|:-------|
 * | `compileFull(src)` | Instance method | Full compilation | `IncCompileResult` |
 * | `compileInc(src)` | Instance method | Incremental compilation | `IncCompileResult` |
 * | `hasCache()` | Instance method | Check if cache exists | `boolean` |
 * | `clear()` | Instance method | Clear cache | `undefined` |
 *
 * **IncCompileResult Structure**:
 * | Property | Type | Description |
 * |:---------|:-----|:------------|
 * | `tokens` | `Token[]` | Token sequence |
 * | `ast` | `Program` | AST root node |
 * | `incremental` | `boolean` | Whether incremental compilation was used |
 * | `relexedLines` | `number` | Number of re-lexed lines |
 * | `reparsedStmts` | `number` | Number of re-parsed statements |
 *
 * @section type_conversion Type Conversion
 *
 * Type mapping between C++ and JavaScript:
 *
 * | C++ Type | JavaScript Type | Description |
 * |:---------|:----------------|:------------|
 * | `int`, `float`, `double` | `number` | Numeric types |
 * | `std::string` | `string` | String |
 * | `bool` | `boolean` | Boolean |
 * | `std::vector<T>` | `Array` | Array |
 * | `std::unordered_map<K,V>` | `Object` | Object |
 * | `std::optional<T>` | `T \| undefined` | Optional value |
 * | `Pos` | `Pos` | Position object (adapter) |
 * | `Token` | `Token` | Lexical token (adapter) |
 * | `Program` | `Program` | AST root node (adapter) |
 * | `SymbolTable` | `SymbolTable` | Symbol table (adapter) |
 * | `Diagnostic` | `Object` | Diagnostic info (plain object) |
 * | `Error` | `Error` | Error object (adapter) |
 *
 * **Diagnostic Object Structure**:
 * | Property | Type | Description |
 * |:---------|:-----|:------------|
 * | `level` | `string` | Severity level (`error`/`warning`/`info`) |
 * | `id` | `string` | Message ID (e.g., `IWL1`, `IPL1`) |
 * | `start` | `Pos` | Start position |
 * | `end` | `Pos` | End position |
 * | `message` | `string` | Message content (based on current language) |
 *
 * @section async_workers Async Workers
 *
 * Long-running operations like semantic analysis use `TaskWorker` to execute in background threads:
 *
 * ```mermaid
 * sequenceDiagram
 *     participant JS as JavaScript
 *     participant Worker as TaskWorker
 *     participant Core as C++ Core
 *
 *     JS->>Worker: Start async task
 *     Worker->>Core: Execute in background thread
 *     Core-->>Worker: Task complete
 *     Worker-->>JS: Promise resolved
 * ```
 *
 * **Benefits**:
 * - Does not block Node.js main event loop
 * - Supports coroutine scheduling (handles forward references)
 * - Errors auto-propagate to Promise
 *
 * @note `Analyser.analyse()` and `Analyser.visit()` are the only async methods;
 * all other adapter methods are synchronous.
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
 * @note Diagnostics are not errors and don't throw exceptions.
 * Check via the `diagnostics` accessor proactively.
 *
 * @section build Build Instructions
 *
 * ### Prerequisites
 *
 * - Node.js >= 14.x
 * - npm >= 6.x
 * - C++ compiler (g++/clang++/MSVC)
 * - CMake >= 3.15
 * - node-gyp (optional)
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
 *         │   └── ic10_node/
 *         │       ├── lexer/
 *         │       │   ├── lexer_adapter.hpp
 *         │       │   └── token_adapter.hpp
 *         │       ├── parser/
 *         │       │   ├── parser_adapter.hpp
 *         │       │   └── ast_adapter.hpp
 *         │       ├── semantic/
 *         │       │   ├── analyser_adapter.hpp
 *         │       │   └── symbol_table_adapter.hpp
 *         │       ├── link/
 *         │       │   └── linker_adapter.hpp
 *         │       ├── incremental/
 *         │       │   ├── inc_lexer_adapter.hpp
 *         │       │   ├── inc_parser_adapter.hpp
 *         │       │   └── inc_compiler_adapter.hpp
 *         │       ├── locals/
 *         │       │   └── local_adapter.hpp
 *         │       └── main.hpp      # This documentation
 *         ├── src/                  # Source files
 *         │   ├── lexer/
 *         │   ├── parser/
 *         │   ├── semantic/
 *         │   ├── link/
 *         │   ├── incremental/
 *         │   ├── locals/
 *         │   └── build.cpp         # Module entry
 *         ├── CMakeLists.txt        # CMake configuration
 *         ├── Doxyfile              # Doxygen configuration
 *         └── package.json
 * @endcode
 *
 * @section debugging Debugging Guide
 *
 * ### Enable Debug Mode
 *
 * Lexer and Parser support debug mode at construction time:
 *
 * @code{.javascript}
 * const { Lexer, Parser } = require('ic10-compiler');
 *
 * // Debug mode (second parameter is true)
 * const lexer = new Lexer(source, true);
 * const tokens = lexer.scan();
 *
 * const parser = new Parser(tokens, true);
 * const program = parser.parse();
 * @endcode
 *
 * ### Common Issues
 *
 * | Issue | Cause | Solution |
 * |:------|:------|:---------|
 * | Build failed | C++ compiler version too low | Use a compiler supporting C++20 |
 * | Memory leak | Adapter not properly released | Avoid creating instances frequently in loops |
 * | Async hang | Coroutine not resumed | Ensure `await` is used correctly |
 * | Empty diagnostics | Used static method | Use instance method to get diagnostics |
 * | Incremental not working | Cache cleared | Check if `clear()` was called |
 *
 * @section best_practices Best Practices
 *
 * 1. **Reuse adapters**: Don't frequently create/destroy adapter instances
 * 2. **Batch processing**: Reuse Lexer instance for multiple lexical analyses
 * 3. **Error checking**: Always check the `diagnostics` accessor
 * 4. **Async waiting**: Use `await` instead of Promise.then
 * 5. **Incremental first**: Use IncCompiler for editor scenarios instead of full compilation
 *
 * @code{.javascript}
 * // Recommended: Reuse instance
 * const lexer = new Lexer(source);
 * const tokens = lexer.scan();
 * if (lexer.diagnostics.length > 0) {
 *     // Handle lexical errors
 * }
 *
 * // Recommended: Use incremental compiler for editing scenarios
 * const compiler = new IncCompiler();
 * compiler.compileFull(initialSource);
 * // Subsequent edits
 * compiler.compileInc(modifiedSource);
 *
 * // Recommended: Async wait
 * const analyser = new Analyser();
 * await analyser.visit(program);
 * const diags = analyser.diagnostics;
 * @endcode
 *
 * @section dev_notes Developer Notes
 *
 * ### Adapter Design Pattern
 *
 * All adapters inherit from `node::ObjectWrap<T>`, exposing C++ classes as JavaScript classes
 * via N-API. Each adapter follows a unified design pattern:
 *
 * 1. **Constructor**: Create C++ object from JavaScript arguments
 * 2. **init() static method**: Register class to Node.js module
 * 3. **to() static method**: C++ object → JavaScript object
 * 4. **from() static method**: JavaScript object → C++ object
 * 5. **Accessors/Methods**: Expose C++ object functionality
 *
 * ### Diagnostic System
 *
 * Diagnostics are not exceptions and don't interrupt compilation flow. The compiler collects
 * all diagnostics and exposes them via the `diagnostics` accessor. Diagnostics include:
 * - **level**: Severity (`error`/`warning`/`info`)
 * - **id**: Message ID (for internationalization)
 * - **start/end**: Source code position range
 * - **message**: Localized message content
 *
 * ### Sync vs Async
 *
 * | Adapter | Sync Methods | Async Methods |
 * |:--------|:-------------|:--------------|
 * | Lexer | `tokenize()`, `scan()` | - |
 * | Parser | `parsing()`, `parse()` | - |
 * | Analyser | - | `analyse()`, `visit()` |
 * | Linker | `addUnit()`, `link()` | - |
 * | IncCompiler | `compileFull()`, `compileInc()` | - |
 *
 * Only AnalyserAdapter methods are async (return Promise), because semantic analysis
 * requires handling forward references using coroutines.
 *
 * ### Enum Export
 *
 * C++ enums are automatically exported to JavaScript via compile-time reflection:
 * - `TokenType`: Lexical token types (100+ values)
 * - `TokenCategory`: Token categories
 * - `OperandType`: AST operand types
 *
 * Enum values are represented as numbers in JavaScript, corresponding to C++ enum values.
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
 * - [LinkerAdapter](@ref stationeers::ic10::LinkerAdapter)
 * - [IC10LocalAdapter](@ref stationeers::ic10::IC10LocalAdapter)
 * - [IncLexerAdapter](@ref stationeers::ic10::IncLexerAdapter)
 * - [IncParserAdapter](@ref stationeers::ic10::IncParserAdapter)
 * - [IncCompilerAdapter](@ref stationeers::ic10::IncCompilerAdapter)
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
