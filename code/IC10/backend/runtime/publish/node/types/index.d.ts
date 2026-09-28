// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file index.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc IC10 运行时 Node.js API 类型定义（入口）。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 */

/**
 * @summary IC10 运行时 Node.js API
 *
 * @desc 本模块提供 IC10 运行时的 Node.js 原生绑定 API 类型定义。IC10 是一种用于 Stationeers
 *       游戏的汇编式编程语言，本模块负责**执行**它：输入是 `@ic10/compiler` 产出的 AST、符号表
 *       与类型表，输出是可逐 tick 推进的执行引擎，以及运行期诊断。
 *
 * API 分为三层：
 * 1. **执行层**：{@link Engine}（tick / 单步 / 执行到停机）与 {@link Context}（PC、内存、管理器）
 * 2. **设备与内存**：{@link Device}、{@link Manager}、{@link Memory}
 * 3. **工具与设置**：`hashValue` / `strValue` / `constantValue` / `EPSILON_TIMES_8` 与
 *    `IC10RuntimeLocal`
 *
 * @note 程序必须先编译：解析、语义分析与类型表来自 `@ic10/compiler`。执行期的错误不会抛出，
 *       而是记录在 `engine.diagnostics` 中。
 *
 * @example
 * ```typescript
 * import * as ic10c from '@ic10/compiler';
 * import * as ic10r from '@ic10/runtime';
 *
 * const tokens = ic10c.Lexer.tokenize('alias led d0\ns led Setting 1\nhcf\n');
 * const program = new ic10c.Parser(tokens).parse();
 * const analyser = new ic10c.Analyser();
 * await analyser.visit(program);
 *
 * const engine = new ic10r.Engine(program, analyser.symbolTable, undefined, analyser.typeTable);
 *
 * engine.context.manager.setExternalDevice('d0', undefined as never);
 * const led = engine.context.manager.getDevice('d0');
 *
 * engine.runFull();
 * console.log(led.readLogic('Setting'));   // 1
 * ```
 *
 * @see https://www.npmjs.com/package/@ic10/compiler 编译器绑定（词法、语法、语义、链接与增量编译）
 *
 * @public
 */
export * from "./config";
export * from "./context";
export * from "./device";
export * from "./engine";
export * from "./manager";
export * from "./memory";
export * from "./value";
export * from "./locale";
