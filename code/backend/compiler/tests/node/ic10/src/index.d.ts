/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */

/**
 * @file index.d.ts
 * @summary IC10 编译器 Node.js API 类型定义
 *
 * @desc 本模块提供 IC10 编译器的 Node.js 原生绑定 API 类型定义。
 * IC10 是一种用于 Stationeers 游戏的汇编式编程语言，用于控制游戏中的计算机和设备。
 *
 * API 分为两个主要层次：
 * 1. **运行时绑定**：通过 `ObjectWrap` 包装的 C++ 类（如 {@link IC10Local}、{@link Pos}、{@link Token}、{@link Lexer}、{@link Parser}、{@link Program}、{@link Analyser} 等）
 * 2. **数据序列化**：通过 `toJSON()` 序列化的 AST 节点对象（如各种 {@link ASTNode} 子接口）
 *
 * AST 节点（如指令、表达式）通过 `toJSON()` 方法序列化为普通的 JavaScript 对象，
 * 可直接用于静态分析、代码转换或调试可视化。
 *
 * @example
 * ```typescript
 * import * as ic10 from 'ic10-node-api';
 *
 * // 词法分析
 * const tokens = ic10.Lexer.tokenize('move r0 r1');
 *
 * // 语法分析
 * const parser = new ic10.Parser(tokens);
 * const program = parser.parse();
 *
 * // 访问 AST
 * const json = JSON.parse(program.toJSON());
 * console.log(json.statements[0].type); // "moveInstruction"
 * ```
 *
 * @public
 */
declare module "ic10-node-api" {

    /**
     * @summary 本地化设置类
     *
     * @desc 提供 IC10 编译器的本地化支持，允许设置错误消息和诊断信息的语言。
     * 
     * @public
     */
    export class IC10Local {
        /**
         * @summary 设置编译器的语言环境
         *
         * @param language - 语言代码（如 'en'、'zh' 等）
         *
         * @example
         * ```typescript
         * ic10.IC10Local.setLanguage('zh');
         * ```
         */
        static setLanguage(language: string): void;
    }

    /**
     * @summary 位置信息类
     *
     * @desc 表示源代码中的位置，通过行号和列号定位。
     * 词法分析器使用此类记录每个 Token 的起始位置。
     *
     * @note
     * 行号和列号的起始值取决于具体实现，通常从 0 开始。
     *
     * @public
     */
    export class Pos {
        /**
         * @summary 行号
         * @desc 源代码中的行索引（从 0 或 1 开始）
         */
        line: number;

        /**
         * @summary 列号
         * @desc 当前行中的字符位置（从 0 或 1 开始）
         */
        column: number;

        /**
         * @summary 相对于源代码起始位置的字节偏移量
         */
        offset: number;

        /**
         * @summary 移动到下一行
         *
         * @desc 将位置推进到下一行的起始处（列号归零，行号加一）。
         * 通常在解析换行符时调用。
         */
        newline(): void;

        /**
         * @summary 移动到下一个字符
         *
         * @desc 将列号加一，表示读取了一个字符。
         * 通常在解析普通字符时调用。
         */
        next(): void;

        /**
         * @summary 移动到指定偏移位置
         *
         * @param index - 目标偏移量（相对于源代码起始位置）
         *
         * @desc 将位置直接设置到指定的字节偏移处。
         * 用于回溯或跳转到特定位置。
         */
        move(index: number): void;
    }

    /**
     * @summary 词法单元类
     *
     * @desc 表示 IC10 源代码中的最小语义单元。
     * 词法单元包括关键字（如 `move`、`add`）、标识符、数字、运算符等。
     *
     * Token 是词法分析的结果，作为语法分析器的输入。
     *
     * @public
     */
    export class Token {
        /**
         * @summary 构造函数
         *
         * @param type - Token 类型（对应 ic10::TokenType 枚举）
         * @param pos - 位置对象，记录该 Token 在源代码中的位置
         * @param lexeme - 词素，即源代码中的原始字符串
         * @param category - Token 类别（对应 ic10::TokenCategory 枚举，可选）
         *
         * @example
         * ```typescript
         * const pos = new ic10.Pos();
         * pos.line = 0;
         * pos.column = 0;
         * const token = new ic10.Token(1, pos, 'move', 0);
         * ```
         */
        constructor(type: number, pos: Pos, lexeme?: string, category?: number);

        /**
         * @summary Token 类型
         * @desc 对应 ic10::TokenType 枚举值，表示 Token 的语法类别
         */
        type: number;

        /**
         * @summary 位置信息
         * @desc 该 Token 在源代码中的起始位置
         */
        pos: Pos;

        /**
         * @summary 词素
         * @desc 源代码中匹配该 Token 的原始字符串
         */
        lexeme: string;

        /**
         * @summary Token 类别
         * @desc 对应 ic10::TokenCategory 枚举值，提供更细粒度的分类
         */
        category: number;

        /**
         * @summary 返回可读字符串表示
         *
         * @returns 格式化的字符串，如 `Token{type=1, lexeme="move"}`
         *
         * @desc 用于调试目的，返回人类可读的 Token 描述。
         */
        toString(): string;

        /**
         * @summary 返回 JSON 字符串表示
         *
         * @returns JSON 格式的 Token 表示
         *
         * @desc 返回包含 type、pos、lexeme、category 的 JSON 对象字符串。
         */
        toJSON(): string;
    }

    /**
     * @summary 词法分析器类
     *
     * @desc 将 IC10 源代码字符串分解为 Token 序列。
     * 支持调试模式，可保留注释 Token。
     *
     * @public
     */
    export class Lexer {
        /**
         * @summary 构造函数
         *
         * @param source - 待分析的 IC10 源代码字符串
         * @param debug - 是否保留注释（可选，默认为 false）
         *
         * @desc 创建词法分析器实例，但不立即执行分析。
         * 调用 {@link scan} 方法执行实际的词法分析。
         *
         * @example
         * ```typescript
         * const lexer = new ic10.Lexer('move r0 r1', true);
         * ```
         */
        constructor(source: string, debug?: boolean);

        /**
         * @summary 静态方法：直接对源代码进行词法分析
         *
         * @param source - IC10 源代码字符串
         * @returns Token 数组
         *
         * @desc 便捷方法，无需创建 Lexer 实例即可获取 Token 序列。
         *
         * @example
         * ```typescript
         * const tokens = ic10.Lexer.tokenize('add r0 r1 r2');
         * ```
         */
        static tokenize(source: string): Token[];

        /**
         * @summary 实例方法：对构造时传入的源代码进行词法分析
         *
         * @returns Token 数组
         *
         * @desc 执行实际的词法分析，将源代码分解为 Token 序列。
         * 如果构造时设置了 debug=true，注释也会作为 Token 保留。
         *
         * @example
         * ```typescript
         * const lexer = new ic10.Lexer('move r0 r1');
         * const tokens = lexer.scan();
         * ```
         */
        scan(): Token[];
    }

    // =========================================================================
    // AST 节点类型定义
    // AST 节点通过 toJSON() 序列化为普通 JavaScript 对象
    // 每个节点都包含 type 和 position 字段
    // =========================================================================

    /**
     * @summary AST 节点在源代码中的位置信息
     *
     * @desc 描述 AST 节点在原始源代码中的位置，用于错误报告、调试和代码导航。
     * 位置信息包括行号和列号，从 0 或 1 开始取决于具体实现。
     *
     * @public
     */
    export interface Position {
        /**
         * @summary 行号
         * @desc 源代码中的行索引（从 0 或 1 开始）
         */
        line: number;

        /**
         * @summary 列号
         * @desc 当前行中的字符位置（从 0 或 1 开始）
         */
        column: number;
    }

    /**
     * @summary 所有 AST 节点的基础接口
     *
     * @desc 所有 IC10 AST 节点都继承自此接口，提供公共的 `type` 和 `position` 字段。
     * - `type`: 节点类型标识符，用于区分不同种类的 AST 节点
     * - `position`: 节点在源代码中的位置
     *
     * @note
     * 此接口仅用于类型推断，不应在运行时直接实例化。
     *
     * @public
     */
    export interface ASTNode {
        /**
         * @summary 节点类型
         * @desc 标识 AST 节点的类型，如 `"Integer"`、`"Identifier"`、`"addInstruction"` 等。
         * 可用于在运行时进行类型守卫和节点分类。
         *
         * @example
         * ```typescript
         * function processNode(node: ASTNode) {
         *     if (node.type === 'addInstruction') {
         *         // 处理加法指令
         *     }
         * }
         * ```
         */
        type: string;

        /**
         * @summary 节点位置
         * @desc 节点在源代码中的起始位置
         */
        position: Position;
    }

    // -------------------------------------------------------------------------
    // 字面量 / 叶子节点
    // -------------------------------------------------------------------------

    /**
     * @summary 字面量/叶子节点概述
     *
     * @desc 字面量节点是 AST 中的最小语义单元，表示不可再分的基本值。
     * 这些节点包括：
     * - **数值字面量**：{@link IntegerNode}、{@link FloatNode}、{@link HexNumberNode}、{@link BinaryNumberNode}
     * - **标识符**：{@link IdentifierNode}
     * - **字符串字面量**：{@link StringNode}
     * - **寄存器引用**：{@link RegisterNode}
     * - **设备引用**：{@link DeviceNode}
     * - **常量关键字**：{@link ConstantNode}
     * - **宏调用**：{@link HashCallNode}、{@link StrCallNode}
     * - **错误节点**：{@link ErrorNode}
     *
     * @see {@link NumberNode} - 数值字面量的联合类型
     * @see {@link OperandNode} - 操作数类型的联合类型
     */

    /**
     * @summary 整数字面量节点
     *
     * @desc 表示 IC10 程序中的十进制整数值。
     * 例如 `42`、`-17`、`0` 等。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Integer",
     *   "position": { "line": 0, "column": 5 },
     *   "value": 42
     * }
     * ```
     *
     * @public
     */
    export interface IntegerNode extends ASTNode {
        type: "Integer";
        /**
         * @summary 整数值
         * @desc 十进制整数，可正可负
         */
        value: number;
    }

    /**
     * @summary 浮点数字面量节点
     *
     * @desc 表示 IC10 程序中的十进制浮点数值。
     * 例如 `3.14`、`-0.5`、`.5` 等。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Float",
     *   "position": { "line": 0, "column": 5 },
     *   "value": 3.14
     * }
     * ```
     *
     * @public
     */
    export interface FloatNode extends ASTNode {
        type: "Float";
        /**
         * @summary 浮点数值
         * @desc 十进制浮点数
         */
        value: number;
    }

    /**
     * @summary 十六进制数字面量节点
     *
     * @desc 表示 IC10 程序中的十六进制整数值。
     * 以 `0x` 或 `0X` 前缀开头。
     * 例如 `0xFF`、`0X1A` 等。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "HexNumber",
     *   "position": { "line": 0, "column": 5 },
     *   "value": "0xFF"
     * }
     * ```
     *
     * @public
     */
    export interface HexNumberNode extends ASTNode {
        type: "HexNumber";
        /**
         * @summary 十六进制字符串
         * @desc 包含前缀的十六进制字符串，如 "0xFF"
         */
        value: string;
    }

    /**
     * @summary 二进制数字面量节点
     *
     * @desc 表示 IC10 程序中的二进制整数值。
     * 以 `0b` 或 `0B` 前缀开头。
     * 例如 `0b1010`、`0B11` 等。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "BinaryNumber",
     *   "position": { "line": 0, "column": 5 },
     *   "value": "0b1010"
     * }
     * ```
     *
     * @public
     */
    export interface BinaryNumberNode extends ASTNode {
        type: "BinaryNumber";
        /**
         * @summary 二进制字符串
         * @desc 包含前缀的二进制字符串，如 "0b1010"
         */
        value: string;
    }

    /**
     * @summary 标识符节点
     *
     * @desc 表示 IC10 程序中的标识符，如变量名、标签名、别名等。
     * 标识符必须以字母或下划线开头，后续字符可以是字母、数字或下划线。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Identifier",
     *   "position": { "line": 0, "column": 0 },
     *   "value": "myVariable"
     * }
     * ```
     *
     * @public
     */
    export interface IdentifierNode extends ASTNode {
        type: "Identifier";
        /**
         * @summary 标识符名称
         * @desc 标识符的字符串名称
         */
        value: string;
    }

    /**
     * @summary 字符串字面量节点
     *
     * @desc 表示 IC10 程序中的字符串字面量。
     * 字符串由双引号包围，支持转义字符。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "String",
     *   "position": { "line": 0, "column": 0 },
     *   "value": "\"Hello, World!\""
     * }
     * ```
     *
     * @public
     */
    export interface StringNode extends ASTNode {
        type: "String";
        /**
         * @summary 字符串内容
         * @desc 包括引号的完整字符串值
         */
        value: string;
    }

    /**
     * @summary 寄存器引用节点
     *
     * @desc 表示 IC10 程序中对寄存器的引用。
     * 寄存器是 IC10 CPU 的内部存储单元，名称格式为 `r0` 到 `r15`（共16个寄存器）。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Register",
     *   "position": { "line": 0, "column": 5 },
     *   "value": "r0"
     * }
     * ```
     *
     * @public
     */
    export interface RegisterNode extends ASTNode {
        type: "Register";
        /**
         * @summary 寄存器名称
         * @desc 格式为 "r0" 到 "r15"
         */
        value: string;
    }

    /**
     * @summary 设备引用节点
     *
     * @desc 表示 IC10 程序中对设备的引用。
     * 设备是通过 `alias` 指令定义的逻辑设备名称。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Device",
     *   "position": { "line": 0, "column": 6 },
     *   "value": "console1"
     * }
     * ```
     *
     * @public
     */
    export interface DeviceNode extends ASTNode {
        type: "Device";
        /**
         * @summary 设备名称
         * @desc 通过 alias 指令定义的设备别名
         */
        value: string;
    }

    /**
     * @summary 常量关键字节点
     *
     * @desc 表示 IC10 程序中的预定义常量关键字。
     * 常用常量包括：`nan`（非数字）、`pi`（圆周率）、`e`（自然常数）等。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Constant",
     *   "position": { "line": 0, "column": 5 },
     *   "keyword": "pi"
     * }
     * ```
     *
     * @public
     */
    export interface ConstantNode extends ASTNode {
        type: "Constant";
        /**
         * @summary 常量关键字
         * @desc 如 "nan"、"pi"、"e" 等
         */
        keyword: string;
    }

    /**
     * @summary HASH 宏调用节点
     *
     * @desc 表示 IC10 程序中的 HASH 宏调用。
     * HASH 宏用于计算字符串的哈希值，格式为 `HASH("string")`。
     * 返回值为给定字符串的 32 位无符号整数哈希。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "HashCall",
     *   "position": { "line": 0, "column": 0 },
     *   "value": {
     *     "type": "String",
     *     "position": { "line": 0, "column": 6 },
     *     "value": "\"Example\""
     *   }
     * }
     * ```
     *
     * @public
     */
    export interface HashCallNode extends ASTNode {
        type: "HashCall";
        /**
         * @summary 参数字符串
         * @desc 要计算哈希的字符串参数
         */
        value: StringNode;
    }

    /**
     * @summary STR 宏调用节点
     *
     * @desc 表示 IC10 程序中的 STR 宏调用。
     * STR 宏用于获取字符串的长度，格式为 `STR("string")`。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "StrCall",
     *   "position": { "line": 0, "column": 0 },
     *   "value": {
     *     "type": "String",
     *     "position": { "line": 0, "column": 4 },
     *     "value": "\"Example\""
     *   }
     * }
     * ```
     *
     * @public
     */
    export interface StrCallNode extends ASTNode {
        type: "StrCall";
        /**
         * @summary 参数字符串
         * @desc 要获取长度的字符串参数
         */
        value: StringNode;
    }

    /**
     * @summary 错误节点
     *
     * @desc 表示词法分析或语法分析过程中产生的错误。
     * 当无法识别有效的 Token 或无法解析正确的语法结构时，会生成错误节点。
     * 错误节点会被嵌入到正常的 AST 结构中，作为 {@link Errorable} 联合类型的一部分。
     *
     * @note
     * 错误节点的存在允许编译器继续处理其余代码，同时报告已发生的错误。
     *
     * @example
     * ```typescript
     * // JSON 表示
     * {
     *   "type": "Error",
     *   "position": { "line": 0, "column": 0 },
     *   "token": { ... },
     *   "message": "Unexpected token"
     * }
     * ```
     *
     * @public
     */
    export interface ErrorNode extends ASTNode {
        type: "Error";
        /**
         * @summary 导致错误的 Token
         * @desc 触发错误的源 Token，用于定位错误位置
         */
        token: Token;
        /**
         * @summary 错误消息
         * @desc 描述错误原因的文本
         */
        message: string;
    }

    // -------------------------------------------------------------------------
    // 类型别名（联合类型）
    // -------------------------------------------------------------------------

    /**
     * @summary 类型别名概述
     *
     * @desc 以下是 IC10 AST 中常用的类型别名，它们通常是多种具体节点类型的联合。
     * 使用类型别名可以简化复杂类型签名，提高代码可读性。
     *
     * 重要类型别名说明：
     * - {@link NumberNode}：所有数值字面量的联合
     * - {@link OperandNode}：指令操作数可能的所有类型
     * - {@link RegisterOrIdentifierNode}：寄存器或标识符（用于可读写的位置）
     */

    /**
     * @summary 数值字面量联合类型
     *
     * @desc 表示所有可能的数值字面量类型，包括十进制整数、浮点数、十六进制数和二进制数。
     * 也可能包含错误节点（当数值解析失败时）。
     *
     * @example
     * ```typescript
     * function processNumber(node: NumberNode) {
     *     if (node.type === 'Integer') {
     *         console.log('Integer:', node.value);
     *     } else if (node.type === 'Float') {
     *         console.log('Float:', node.value);
     *     } else if (node.type === 'HexNumber') {
     *         console.log('Hex:', node.value);
     *     } else if (node.type === 'BinaryNumber') {
     *         console.log('Binary:', node.value);
     *     }
     * }
     * ```
     *
     * @public
     */
    export type NumberNode =
        | IntegerNode
        | FloatNode
        | HexNumberNode
        | BinaryNumberNode
        | ErrorNode;

    /**
     * @summary 宏调用联合类型
     *
     * @desc 表示宏调用的可能类型，包括 HASH 宏和 STR 宏。
     *
     * @public
     */
    export type MacroCallNode = HashCallNode | StrCallNode | ErrorNode;

    /**
     * @summary 寄存器或标识符联合类型
     *
     * @desc 表示可以作为寄存器或标识符的位置。
     * 在 IC10 中，某些位置既可以是具体的寄存器（如 `r0`），也可以是变量名（标识符）。
     * 此类型用于区分可写位置和只读操作数。
     *
     * @public
     */
    export type RegisterOrIdentifierNode = RegisterNode | IdentifierNode | ErrorNode;

    /**
     * @summary 设备引用联合类型
     *
     * @desc 表示设备引用的可能形式：
     * - 直接的设备名称（如 `console1`）
     * - 寄存器或标识符（用于间接引用）
     * - 数值（用于立即数地址）
     *
     * @public
     */
    export type DeviceReferenceNode =
        | DeviceNode
        | RegisterOrIdentifierNode
        | NumberNode;

    /**
     * @summary 寄存器或设备联合类型
     *
     * @desc 表示可以绑定到 alias 的目标类型。
     * alias 指令可以将设备绑定到寄存器，或将设备绑定到另一个设备别名。
     *
     * @example
     * ```typescript
     * // alias myReg r0 - 将寄存器绑定到别名
     * // alias console1 console0 - 将设备绑定到别名
     * ```
     *
     * @public
     */
    export type RegisterOrDeviceNode = RegisterOrIdentifierNode | DeviceNode;

    /**
     * @summary 试剂模式联合类型
     *
     * @desc 用于库存管理指令中的试剂筛选模式。
     * 可以是标识符（如 `"ic2"`）或数值。
     *
     * @public
     */
    export type ReagentModeNode = IdentifierNode | NumberNode;

    /**
     * @summary 批处理模式联合类型
     *
     * @desc 用于库存管理指令中的批处理控制。
     * 可以是标识符或数值。
     *
     * @public
     */
    export type BatchModeNode = IdentifierNode | NumberNode;

    /**
     * @summary 逻辑槽类型联合类型
     *
     * @desc 用于指定设备逻辑槽的类型。
     * 可以是标识符（如 `"logic"`）或数值编码。
     *
     * @public
     */
    export type LogicSlotTypeNode = IdentifierNode | NumberNode;

    /**
     * @summary 槽索引类型
     *
     * @desc 用于指定设备槽的索引位置。
     * 通常为数值类型。
     *
     * @public
     */
    export type SlotIndexNode = NumberNode;

    /**
     * @summary 逻辑类型联合类型
     *
     * @desc 用于指定逻辑类型（如数字、逻辑、字符等）。
     * 可以是标识符或数值编码。
     *
     * @public
     */
    export type LogicTypeNode = IdentifierNode | NumberNode;

    /**
     * @summary 操作数联合类型
     *
     * @desc 表示指令中操作数可能的所有类型。
     * 操作数是指令的参数，可以是：
     * - 寄存器（r0-r15）
     * - 设备引用
     * - 数值（整数、浮点、十六进制、二进制）
     * - 标识符
     * - 常量关键字
     * - 宏调用（HASH、STR）
     *
     * @example
     * ```typescript
     * function processOperand(op: OperandNode) {
     *     switch (op.type) {
     *         case 'Register':
     *             return `Register ${op.value}`;
     *         case 'Integer':
     *         case 'Float':
     *         case 'HexNumber':
     *         case 'BinaryNumber':
     *             return `Number ${op.value}`;
     *         case 'Identifier':
     *             return `Variable ${op.value}`;
     *         case 'Device':
     *             return `Device ${op.value}`;
     *         case 'Constant':
     *             return `Constant ${op.keyword}`;
     *         case 'HashCall':
     *             return `Hash of ${op.value.value}`;
     *         case 'StrCall':
     *             return `Length of ${op.value.value}`;
     *     }
     * }
     * ```
     *
     * @see {@link RegisterOrIdentifierNode} - 可写位置的类型
     * @see {@link DeviceReferenceNode} - 设备引用类型
     *
     * @public
     */
    export type OperandNode =
        | RegisterNode
        | DeviceNode
        | NumberNode
        | IdentifierNode
        | ConstantNode
        | MacroCallNode;

    // -------------------------------------------------------------------------
    // 预处理指令节点
    // -------------------------------------------------------------------------

    /**
     * @summary 预处理指令概述
     *
     * @desc 预处理指令在程序编译前处理，用于定义别名、常量等。
     * IC10 支持的预处理指令包括：
     * - {@link AliasDirectiveNode}：`alias` - 为设备或寄存器定义别名
     * - {@link DefineDirectiveNode}：`define` - 定义常量
     *
     * 预处理指令必须在程序主体之前声明。
     *
     * @see {@link PreprocessorDirectiveNode} - 预处理指令联合类型
     */

    /**
     * @summary alias 预处理指令节点
     *
     * @desc 表示 IC10 程序中的 `alias` 预处理指令。
     * alias 指令用于为设备引用或寄存器定义别名，使代码更易读。
     *
     * 语法：`alias <identifier> <registerOrDevice>`
     *
     * @example
     * ```typescript
     * // alias console1 console0
     * // 含义：将 console0 设备绑定到别名 console1
     *
     * // JSON 表示
     * {
     *   "type": "AliasDirective",
     *   "position": { "line": 0, "column": 0 },
     *   "identifier": { "type": "Identifier", "value": "console1", ... },
     *   "registerOrDevice": { "type": "Identifier", "value": "console0", ... }
     * }
     * ```
     *
     * @public
     */
    export interface AliasDirectiveNode extends ASTNode {
        type: "AliasDirective";
        /**
         * @summary 别名标识符
         * @desc 别名的名称，用于在后续代码中引用
         */
        identifier: IdentifierNode;
        /**
         * @summary 被绑定的寄存器或设备
         * @desc 实际的目标，可以是寄存器或设备
         */
        registerOrDevice: RegisterOrDeviceNode;
    }

    /**
     * @summary define 预处理指令节点
     *
     * @desc 表示 IC10 程序中的 `define` 预处理指令。
     * define 指令用于定义常量，可以在代码中像使用字面量一样使用常量名。
     *
     * 语法：`define <identifier> <operand>`
     *
     * @example
     * ```typescript
     * // define MAX_VAL 100
     * // 含义：定义常量 MAX_VAL，值为 100
     *
     * // JSON 表示
     * {
     *   "type": "DefineDirective",
     *   "position": { "line": 0, "column": 0 },
     *   "identifier": { "type": "Identifier", "value": "MAX_VAL", ... },
     *   "number": { "type": "Integer", "value": 100, ... }
     * }
     * ```
     *
     * @public
     */
    export interface DefineDirectiveNode extends ASTNode {
        type: "DefineDirective";
        /**
         * @summary 常量标识符
         * @desc 常量的名称
         */
        identifier: IdentifierNode;
        /**
         * @summary 常量值
         * @desc 可以是任何操作数类型（寄存器、设备、数值、标识符等）
         */
        number: OperandNode;
    }

    /**
     * @summary 预处理指令联合类型
     *
     * @desc 表示所有预处理指令的可能类型。
     *
     * @public
     */
    export type PreprocessorDirectiveNode =
        | AliasDirectiveNode
        | DefineDirectiveNode
        | ErrorNode;

    // -------------------------------------------------------------------------
    // 标签定义节点
    // -------------------------------------------------------------------------

    /**
     * @summary 标签定义节点
     *
     * @desc 表示 IC10 程序中的标签定义。
     * 标签是代码中的跳转目标，用于控制程序流程。
     * 标签以冒号（`:`）结尾，位于语句之前。
     *
     * 语法：`<identifier>:`
     *
     * @example
     * ```typescript
     * // main:
     * // 定义名为 main 的标签
     *
     * // JSON 表示
     * {
     *   "type": "LabelDef",
     *   "position": { "line": 0, "column": 0 },
     *   "identifier": { "type": "Identifier", "value": "main", ... }
     * }
     * ```
     *
     * @see {@link JInstructionNode} - 跳转指令
     * @see {@link JrInstructionNode} - 寄存器跳转指令
     *
     * @public
     */
    export interface LabelDefNode extends ASTNode {
        type: "LabelDef";
        /**
         * @summary 标签标识符
         * @desc 标签的名称，用于作为跳转目标
         */
        identifier: IdentifierNode;
    }

    // -------------------------------------------------------------------------
    // 指令节点（按操作数数量分类）
    // -------------------------------------------------------------------------

    /**
     * @summary 指令节点概述
     *
     * @desc 指令是 IC10 程序的基本执行单元，分为以下几类：
     *
     * | 类别 | 操作数数量 | 说明 |
     * |------|-----------|------|
     * | {@link NullaryInstructionNode} | 0 | 无操作数指令（如 HCF、YIELD） |
     * | {@link UnaryInstructionNode} | 1 | 单操作数指令（如 SLEEP、SNANZ） |
     * | {@link BinaryInstructionNode} | 2 | 双操作数指令（如 MOVE、SUB） |
     * | {@link TernaryInstructionNode} | 3 | 三操作数指令（如 ADD、MUL） |
     * | {@link QuaternaryInstructionNode} | 4 | 四操作数指令（如 LERP、LB） |
     * | {@link QuinaryInstructionNode} | 5 | 五操作数指令（如 LBN） |
     * | {@link SenaryInstructionNode} | 6 | 六操作数指令（如 LBNS） |
     *
     * @note
     * 所有指令节点都继承自 {@link ASTNode}，包含 `type` 和 `position` 字段。
     * 指令节点通过 `type` 字段区分具体指令类型（如 `"addInstruction"`）。
     *
     * @example
     * ```typescript
     * // 分析指令类型
     * function analyzeInstruction(node: ExecutableInstructionNode) {
     *     if (node.type === 'addInstruction') {
     *         // node 是 AddInstructionNode 类型
     *         console.log('ADD:', node.operand1, node.operand2, node.operand3);
     *     }
     * }
     * ```
     *
     * @see {@link ExecutableInstructionNode} - 所有可执行指令的联合类型
     * @see {@link StatementNode} - 语句类型（包括指令、标签、预处理指令）
     */

    // 零元指令（无操作数）

    /**
     * @summary 零元指令概述
     *
     * @desc 零元指令是不带任何操作数的指令。
     * 这些指令通常用于控制程序流程或 CPU 状态。
     */

    /**
     * @summary HCF（Hang Crisis Failure）指令节点
     *
     * @desc HCF 指令是 IC10 的最高优先级指令。
     * 执行后，CPU 将进入无限循环状态，无法恢复。
     * 通常用于严重错误处理或程序终止。
     *
     * @warning
     * 此指令会导致 CPU 永久停止，应谨慎使用！
     *
     * @example
     * ```typescript
     * // HCF 指令
     * {
     *   "type": "hcfInstruction",
     *   "position": { "line": 0, "column": 0 }
     * }
     * ```
     *
     * @public
     */
    export interface HcfInstructionNode extends ASTNode {
        type: "hcfInstruction";
    }

    /**
     * @summary YIELD 指令节点
     *
     * @desc YIELD 指令让出 CPU 执行权，允许其他 IC 芯片执行。
     * 用于协作式多任务，避免单个 IC 独占 CPU。
     *
     * @example
     * ```typescript
     * // YIELD 指令
     * {
     *   "type": "yieldInstruction",
     *   "position": { "line": 0, "column": 0 }
     * }
     * ```
     *
     * @public
     */
    export interface YieldInstructionNode extends ASTNode {
        type: "yieldInstruction";
    }

    /**
     * @summary 零元指令联合类型
     *
     * @desc 包含所有零元指令类型的联合，包括可能的错误节点。
     *
     * @public
     */
    export type NullaryInstructionNode =
        | HcfInstructionNode
        | YieldInstructionNode
        | ErrorNode;

    // 一元指令（1 个操作数）

    /**
     * @summary 一元指令概述
     *
     * @desc 一元指令是包含单个操作数的指令。
     * 根据操作数类型可分为以下几类：
     * - **UnaryInstruction_RI**：寄存器或标识符作为操作数
     * - **UnaryInstruction_DR**：设备引用作为操作数
     * - **UnaryInstruction_O**：通用操作数
     *
     * @elseif en
     * @summary Unary Instructions Overview
     *
     * @desc Unary instructions contain a single operand.
     * They are categorized by operand type:
     * - **UnaryInstruction_RI**: Register or identifier as operand
     * - **UnaryInstruction_DR**: Device reference as operand
     * - **UnaryInstruction_O**: General operand
     */

    /**
     * @summary 含单个 RegisterOrIdentifier 操作数的一元指令基类
     *
     * @desc 操作数为寄存器或标识符节点。
     * @elseif en
     * @summary Base interface for unary instructions with single RegisterOrIdentifier operand
     *
     * @desc Operand is a register or identifier node.
     *
     * @public
     */
    interface UnaryInstruction_RI extends ASTNode {
        operand1: RegisterOrIdentifierNode;
    }

    /**
     * @summary SNANZ（Set if Not-A-Number Non-Zero）指令节点
     *
     * @desc 如果操作数不是数字且非零，则将目标寄存器设置为 1，否则设置为 0。
     * 用于检查特殊数值状态。
     * @elseif en
     * @summary SNANZ (Set if Not-A-Number Non-Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand is not a number and is non-zero, otherwise sets it to 0.
     * Used to check special numeric status.
     *
     * @example
     * ```typescript
     * // SNANZ r0 r1
     * // 如果 r1 是 NaN 且非零，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SnanzInstructionNode extends UnaryInstruction_RI {
        type: "snanzInstruction";
    }

    /**
     * @summary PEEK（读取内存）指令节点
     *
     * @desc 从指定地址读取数据到目标寄存器。
     * 用于读取堆栈或内存中的值。
     * @elseif en
     * @summary PEEK instruction node
     *
     * @desc Reads data from a specified address into the target register.
     * Used to read values from stack or memory.
     *
     * @example
     * ```typescript
     * // PEEK r0 r1
     * // 从地址 r1 读取数据到 r0
     * ```
     *
     * @public
     */
    export interface PeekInstructionNode extends UnaryInstruction_RI {
        type: "peekInstruction";
    }

    /**
     * @summary RAND（随机数）指令节点
     *
     * @desc 生成一个随机数并存入目标寄存器。
     * 随机数范围通常为 0 到 1 之间的浮点数。
     * @elseif en
     * @summary RAND (Random) instruction node
     *
     * @desc Generates a random number and stores it in the target register.
     * The random number is typically a floating-point number between 0 and 1.
     *
     * @example
     * ```typescript
     * // RAND r0
     * // 生成随机数并存入 r0
     * ```
     *
     * @public
     */
    export interface RandInstructionNode extends UnaryInstruction_RI {
        type: "randInstruction";
    }

    /**
     * @summary SEQZ（Set if EQual Zero）指令节点
     *
     * @desc 如果操作数等于零，则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SEQZ (Set if EQual Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand equals zero, otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SEQZ r0 r1
     * // 如果 r1 == 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SeqzInstructionNode extends UnaryInstruction_RI {
        type: "seqzInstruction";
    }

    /**
     * @summary SNEZ（Set if Not Equal Zero）指令节点
     *
     * @desc 如果操作数不等于零，则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SNEZ (Set if Not Equal Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand does not equal zero, otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SNEZ r0 r1
     * // 如果 r1 != 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SnezInstructionNode extends UnaryInstruction_RI {
        type: "snezInstruction";
    }

    /**
     * @summary SGEZ（Set if Greater than or Equal Zero）指令节点
     *
     * @desc 如果操作数大于等于零，则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SGEZ (Set if Greater than or Equal Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand is greater than or equal to zero, otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SGEZ r0 r1
     * // 如果 r1 >= 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SgezInstructionNode extends UnaryInstruction_RI {
        type: "sgezInstruction";
    }

    /**
     * @summary SGTZ（Set if Greater Than Zero）指令节点
     *
     * @desc 如果操作数大于零，则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SGTZ (Set if Greater Than Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand is greater than zero, otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SGTZ r0 r1
     * // 如果 r1 > 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SgtzInstructionNode extends UnaryInstruction_RI {
        type: "sgtzInstruction";
    }

    /**
     * @summary SLEZ（Set if Less than or Equal Zero）指令节点
     *
     * @desc 如果操作数小于等于零，则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SLEZ (Set if Less than or Equal Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand is less than or equal to zero, otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SLEZ r0 r1
     * // 如果 r1 <= 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SlezInstructionNode extends UnaryInstruction_RI {
        type: "slezInstruction";
    }

    /**
     * @summary SLTZ（Set if Less Than Zero）指令节点
     *
     * @desc 如果操作数小于零，则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SLTZ (Set if Less Than Zero) instruction node
     *
     * @desc Sets the target register to 1 if the operand is less than zero, otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SLTZ r0 r1
     * // 如果 r1 < 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SltzInstructionNode extends UnaryInstruction_RI {
        type: "sltzInstruction";
    }

    /**
     * @summary SNAN（Set if Not-A-Number）指令节点
     *
     * @desc 如果操作数是 NaN（非数字），则将目标寄存器设置为 1，否则设置为 0。
     * @elseif en
     * @summary SNAN (Set if Not-A-Number) instruction node
     *
     * @desc Sets the target register to 1 if the operand is NaN (Not a Number), otherwise sets it to 0.
     *
     * @example
     * ```typescript
     * // SNAN r0 r1
     * // 如果 r1 是 NaN，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SnanInstructionNode extends UnaryInstruction_RI {
        type: "snanInstruction";
    }

    /**
     * @summary POP（弹出堆栈）指令节点
     *
     * @desc 从堆栈中弹出一个值并存入目标寄存器。
     * 用于恢复之前保存的寄存器值。
     * @elseif en
     * @summary POP instruction node
     *
     * @desc Pops a value from the stack and stores it in the target register.
     * Used to restore previously saved register values.
     *
     * @example
     * ```typescript
     * // POP r0
     * // 弹出堆栈顶部值到 r0
     * ```
     *
     * @public
     */
    export interface PopInstructionNode extends UnaryInstruction_RI {
        type: "popInstruction";
    }

    /**
     * @summary 含单个 DeviceReference 操作数的一元指令基类
     *
     * @desc 操作数为设备引用节点。
     * @elseif en
     * @summary Base interface for unary instructions with single DeviceReference operand
     *
     * @desc Operand is a device reference node.
     *
     * @public
     */
    interface UnaryInstruction_DR extends ASTNode {
        operand1: DeviceReferenceNode;
    }

    /**
     * @summary CLR（Clear Device）指令节点
     *
     * @desc 清除指定设备的所有数据或状态。
     * @elseif en
     * @summary CLR (Clear Device) instruction node
     *
     * @desc Clears all data or state of the specified device.
     *
     * @example
     * ```typescript
     * // CLR console1
     * // 清除 console1 设备的数据
     * ```
     *
     * @public
     */
    export interface ClrInstructionNode extends UnaryInstruction_DR {
        type: "clrInstruction";
    }

    /**
     * @summary 含单个 Operand 操作数的一元指令基类
     *
     * @desc 操作数为通用操作数类型。
     * @elseif en
     * @summary Base interface for unary instructions with single Operand operand
     *
     * @desc Operand is a general operand type.
     *
     * @public
     */
    interface UnaryInstruction_O extends ASTNode {
        operand1: OperandNode;
    }

    /**
     * @summary SLEEP（休眠）指令节点
     *
     * @desc 让 IC 芯片进入休眠状态一段时间。
     * 休眠期间芯片不执行任何操作，用于节流或等待外部事件。
     * @elseif en
     * @summary SLEEP instruction node
     *
     * @desc Puts the IC chip into sleep mode for a specified duration.
     * The chip does not execute any operations during sleep, used for throttling or waiting for external events.
     *
     * @example
     * ```typescript
     * // SLEEP 10
     * // 休眠 10 个时间单位
     * ```
     *
     * @public
     */
    export interface SleepInstructionNode extends UnaryInstruction_O {
        type: "sleepInstruction";
    }

    /**
     * @summary PUSH（压入堆栈）指令节点
     *
     * @desc 将操作数的值压入堆栈。
     * 用于保存寄存器值以便后续恢复。
     * @elseif en
     * @summary PUSH instruction node
     *
     * @desc Pushes the value of the operand onto the stack.
     * Used to save register values for later restoration.
     *
     * @example
     * ```typescript
     * // PUSH r0
     * // 将 r0 的值压入堆栈
     * ```
     *
     * @public
     */
    export interface PushInstructionNode extends UnaryInstruction_O {
        type: "pushInstruction";
    }

    /**
     * @summary JAL（Jump And Link）指令节点
     *
     * @desc 跳转到指定地址并将返回地址保存到链接寄存器（通常是 r15）。
     * 用于函数调用。
     * @elseif en
     * @summary JAL (Jump And Link) instruction node
     *
     * @desc Jumps to the specified address and saves the return address to the link register (typically r15).
     * Used for function calls.
     *
     * @example
     * ```typescript
     * // JAL main
     * // 跳转到 main 标签，保存返回地址到 r15
     * ```
     *
     * @public
     */
    export interface JalInstructionNode extends UnaryInstruction_O {
        type: "jalInstruction";
    }

    /**
     * @summary JR（Jump Register）指令节点
     *
     * @desc 跳转到寄存器中存储的地址。
     * 用于间接跳转，通常配合 JAL 使用。
     * @elseif en
     * @summary JR (Jump Register) instruction node
     *
     * @desc Jumps to the address stored in a register.
     * Used for indirect jumps, often used with JAL.
     *
     * @example
     * ```typescript
     * // JR r15
     * // 跳转到 r15 中存储的地址（返回）
     * ```
     *
     * @public
     */
    export interface JrInstructionNode extends UnaryInstruction_O {
        type: "jrInstruction";
    }

    /**
     * @summary J（Jump）指令节点
     *
     * @desc 无条件跳转到指定地址或标签。
     * @elseif en
     * @summary J (Jump) instruction node
     *
     * @desc Unconditionally jumps to the specified address or label.
     *
     * @example
     * ```typescript
     * // J loop
     * // 跳转到 loop 标签
     * ```
     *
     * @public
     */
    export interface JInstructionNode extends UnaryInstruction_O {
        type: "jInstruction";
    }

    /**
     * @summary 一元指令联合类型
     *
     * @desc 包含所有一元指令类型的联合，包括可能的错误节点。
     * @elseif en
     * @summary Unary instruction union type
     *
     * @desc Union of all unary instruction types, including possible error nodes.
     *
     * @public
     */
    export type UnaryInstructionNode =
        | SnanzInstructionNode
        | PeekInstructionNode
        | RandInstructionNode
        | SeqzInstructionNode
        | SnezInstructionNode
        | SgezInstructionNode
        | SgtzInstructionNode
        | SlezInstructionNode
        | SltzInstructionNode
        | SnanInstructionNode
        | PopInstructionNode
        | ClrInstructionNode
        | SleepInstructionNode
        | PushInstructionNode
        | JalInstructionNode
        | JrInstructionNode
        | JInstructionNode
        | ErrorNode;

    // 二元指令（2 个操作数）

    /**
     * @summary 二元指令概述
     *
     * @desc 二元指令是包含两个操作数的指令。
     * 根据操作数类型可分为以下几类：
     * - **BinaryInstruction_RI_O**：寄存器/标识符 + 操作数
     * - **BinaryInstruction_DR_O**：设备引用 + 操作数
     * - **BinaryInstruction_RI_DR**：寄存器/标识符 + 设备引用
     * - **BinaryInstruction_O_O**：操作数 + 操作数
     *
     * @elseif en
     * @summary Binary Instructions Overview
     *
     * @desc Binary instructions contain two operands.
     * They are categorized by operand types:
     * - **BinaryInstruction_RI_O**: Register/identifier + operand
     * - **BinaryInstruction_DR_O**: Device reference + operand
     * - **BinaryInstruction_RI_DR**: Register/identifier + device reference
     * - **BinaryInstruction_O_O**: Operand + operand
     */

    /**
     * @summary 含 RegisterOrIdentifier + Operand 操作数的二元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为通用操作数。
     * @elseif en
     * @summary Base interface for binary instructions with RegisterOrIdentifier + Operand operands
     *
     * @desc First operand is a register or identifier, second is a general operand.
     *
     * @public
     */
    interface BinaryInstruction_RI_O extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
    }

    /**
     * @summary ABS（绝对值）指令节点
     *
     * @desc 计算操作数的绝对值并存入目标寄存器。
     * @elseif en
     * @summary ABS (Absolute Value) instruction node
     *
     * @desc Computes the absolute value of the operand and stores it in the target register.
     *
     * @example
     * ```typescript
     * // ABS r0 r1
     * // 计算 r1 的绝对值并存入 r0
     * ```
     *
     * @public
     */
    export interface AbsInstructionNode extends BinaryInstruction_RI_O {
        type: "absInstruction";
    }

    /**
     * @summary ACOS（反余弦）指令节点
     *
     * @desc 计算操作数的反余弦值（弧度制）。
     * @elseif en
     * @summary ACOS (Arc Cosine) instruction node
     *
     * @desc Computes the arc cosine of the operand (in radians).
     *
     * @example
     * ```typescript
     * // ACOS r0 r1
     * // 计算 r1 的反余弦值并存入 r0
     * ```
     *
     * @public
     */
    export interface AcosInstructionNode extends BinaryInstruction_RI_O {
        type: "acosInstruction";
    }

    /**
     * @summary ASIN（反正弦）指令节点
     *
     * @desc 计算操作数的反正弦值（弧度制）。
     * @elseif en
     * @summary ASIN (Arc Sine) instruction node
     *
     * @desc Computes the arc sine of the operand (in radians).
     *
     * @example
     * ```typescript
     * // ASIN r0 r1
     * // 计算 r1 的反正弦值并存入 r0
     * ```
     *
     * @public
     */
    export interface AsinInstructionNode extends BinaryInstruction_RI_O {
        type: "asinInstruction";
    }

    /**
     * @summary ATAN（反正切）指令节点
     *
     * @desc 计算操作数的反正切值（弧度制）。
     * @elseif en
     * @summary ATAN (Arc Tangent) instruction node
     *
     * @desc Computes the arc tangent of the operand (in radians).
     *
     * @example
     * ```typescript
     * // ATAN r0 r1
     * // 计算 r1 的反正切值并存入 r0
     * ```
     *
     * @public
     */
    export interface AtanInstructionNode extends BinaryInstruction_RI_O {
        type: "atanInstruction";
    }

    /**
     * @summary CEIL（向上取整）指令节点
     *
     * @desc 计算大于或等于操作数的最小整数。
     * @elseif en
     * @summary CEIL (Ceiling) instruction node
     *
     * @desc Computes the smallest integer greater than or equal to the operand.
     *
     * @example
     * ```typescript
     * // CEIL r0 r1
     * // 计算 r1 向上取整的结果并存入 r0
     * ```
     *
     * @public
     */
    export interface CeilInstructionNode extends BinaryInstruction_RI_O {
        type: "ceilInstruction";
    }

    /**
     * @summary COS（余弦）指令节点
     *
     * @desc 计算操作数的余弦值（弧度制）。
     * @elseif en
     * @summary COS (Cosine) instruction node
     *
     * @desc Computes the cosine of the operand (in radians).
     *
     * @example
     * ```typescript
     * // COS r0 r1
     * // 计算 r1 的余弦值并存入 r0
     * ```
     *
     * @public
     */
    export interface CosInstructionNode extends BinaryInstruction_RI_O {
        type: "cosInstruction";
    }

    /**
     * @summary EXP（指数）指令节点
     *
     * @desc 计算 e 的操作数次幂。
     * @elseif en
     * @summary EXP (Exponential) instruction node
     *
     * @desc Computes e raised to the power of the operand.
     *
     * @example
     * ```typescript
     * // EXP r0 r1
     * // 计算 e^r1 并存入 r0
     * ```
     *
     * @public
     */
    export interface ExpInstructionNode extends BinaryInstruction_RI_O {
        type: "expInstruction";
    }

    /**
     * @summary FLOOR（向下取整）指令节点
     *
     * @desc 计算小于或等于操作数的最大整数。
     * @elseif en
     * @summary FLOOR instruction node
     *
     * @desc Computes the largest integer less than or equal to the operand.
     *
     * @example
     * ```typescript
     * // FLOOR r0 r1
     * // 计算 r1 向下取整的结果并存入 r0
     * ```
     *
     * @public
     */
    export interface FloorInstructionNode extends BinaryInstruction_RI_O {
        type: "floorInstruction";
    }

    /**
     * @summary LOG（自然对数）指令节点
     *
     * @desc 计算操作数的自然对数（以 e 为底）。
     * @elseif en
     * @summary LOG (Natural Logarithm) instruction node
     *
     * @desc Computes the natural logarithm of the operand (base e).
     *
     * @example
     * ```typescript
     * // LOG r0 r1
     * // 计算 ln(r1) 并存入 r0
     * ```
     *
     * @public
     */
    export interface LogInstructionNode extends BinaryInstruction_RI_O {
        type: "logInstruction";
    }

    /**
     * @summary ROUND（四舍五入）指令节点
     *
     * @desc 将操作数四舍五入到最接近的整数。
     * @elseif en
     * @summary ROUND instruction node
     *
     * @desc Rounds the operand to the nearest integer.
     *
     * @example
     * ```typescript
     * // ROUND r0 r1
     * // 计算 r1 四舍五入的结果并存入 r0
     * ```
     *
     * @public
     */
    export interface RoundInstructionNode extends BinaryInstruction_RI_O {
        type: "roundInstruction";
    }

    /**
     * @summary SIN（正弦）指令节点
     *
     * @desc 计算操作数的正弦值（弧度制）。
     * @elseif en
     * @summary SIN (Sine) instruction node
     *
     * @desc Computes the sine of the operand (in radians).
     *
     * @example
     * ```typescript
     * // SIN r0 r1
     * // 计算 r1 的正弦值并存入 r0
     * ```
     *
     * @public
     */
    export interface SinInstructionNode extends BinaryInstruction_RI_O {
        type: "sinInstruction";
    }

    /**
     * @summary SQRT（平方根）指令节点
     *
     * @desc 计算操作数的平方根。
     * @elseif en
     * @summary SQRT (Square Root) instruction node
     *
     * @desc Computes the square root of the operand.
     *
     * @example
     * ```typescript
     * // SQRT r0 r1
     * // 计算 sqrt(r1) 并存入 r0
     * ```
     *
     * @public
     */
    export interface SqrtInstructionNode extends BinaryInstruction_RI_O {
        type: "sqrtInstruction";
    }

    /**
     * @summary TAN（正切）指令节点
     *
     * @desc 计算操作数的正切值（弧度制）。
     * @elseif en
     * @summary TAN (Tangent) instruction node
     *
     * @desc Computes the tangent of the operand (in radians).
     *
     * @example
     * ```typescript
     * // TAN r0 r1
     * // 计算 r1 的正切值并存入 r0
     * ```
     *
     * @public
     */
    export interface TanInstructionNode extends BinaryInstruction_RI_O {
        type: "tanInstruction";
    }

    /**
     * @summary TRUNC（截断）指令节点
     *
     * @desc 截断操作数的小数部分，只保留整数部分。
     * @elseif en
     * @summary TRUNC (Truncate) instruction node
     *
     * @desc Truncates the decimal part of the operand, keeping only the integer part.
     *
     * @example
     * ```typescript
     * // TRUNC r0 r1
     * // 计算 truncate(r1) 并存入 r0
     * ```
     *
     * @public
     */
    export interface TruncInstructionNode extends BinaryInstruction_RI_O {
        type: "truncInstruction";
    }

    /**
     * @summary NOT（按位取反）指令节点
     *
     * @desc 对操作数进行按位取反运算。
     * @elseif en
     * @summary NOT (Bitwise NOT) instruction node
     *
     * @desc Performs bitwise NOT operation on the operand.
     *
     * @example
     * ```typescript
     * // NOT r0 r1
     * // 计算 ~r1 并存入 r0
     * ```
     *
     * @public
     */
    export interface NotInstructionNode extends BinaryInstruction_RI_O {
        type: "notInstruction";
    }

    /**
     * @summary MOVE（移动）指令节点
     *
     * @desc 将第二个操作数的值复制到第一个操作数（目标寄存器）。
     * @elseif en
     * @summary MOVE instruction node
     *
     * @desc Copies the value of the second operand to the first operand (target register).
     *
     * @example
     * ```typescript
     * // MOVE r0 r1
     * // 将 r1 的值复制到 r0
     * ```
     *
     * @public
     */
    export interface MoveInstructionNode extends BinaryInstruction_RI_O {
        type: "moveInstruction";
    }

    /**
     * @summary SUB（减法）指令节点
     *
     * @desc 从第一个操作数中减去第二个操作数，结果存入目标寄存器。
     * @elseif en
     * @summary SUB (Subtraction) instruction node
     *
     * @desc Subtracts the second operand from the first operand, storing the result in the target register.
     *
     * @example
     * ```typescript
     * // SUB r0 r1 r2
     * // 计算 r1 - r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface SubInstructionNode extends BinaryInstruction_RI_O {
        type: "subInstruction";
    }

    /**
     * @summary 含 DeviceReference + Operand 操作数的二元指令基类
     *
     * @desc 第一个操作数为设备引用，第二个为通用操作数。
     * @elseif en
     * @summary Base interface for binary instructions with DeviceReference + Operand operands
     *
     * @desc First operand is a device reference, second is a general operand.
     *
     * @public
     */
    interface BinaryInstruction_DR_O extends ASTNode {
        operand1: DeviceReferenceNode;
        operand2: OperandNode;
    }

    /**
     * @summary BDNS（Branches on Device Not Stalled）指令节点
     *
     * @desc 如果指定设备未处于停滞状态，则跳转到目标地址。
     * @elseif en
     * @summary BDNS (Branches on Device Not Stalled) instruction node
     *
     * @desc Jumps to the target address if the specified device is not stalled.
     *
     * @example
     * ```typescript
     * // BDNS console1 loop
     * // 如果 console1 未停滞，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BdnsInstructionNode extends BinaryInstruction_DR_O {
        type: "bdnsInstruction";
    }

    /**
     * @summary BDNSAL（Branches on Device Not Stalled And Link）指令节点
     *
     * @desc 如果指定设备未处于停滞状态，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BDNSAL (Branches on Device Not Stalled And Link) instruction node
     *
     * @desc Jumps to the target address if the specified device is not stalled, and saves the return address.
     *
     * @example
     * ```typescript
     * // BDNSAL console1 loop
     * // 如果 console1 未停滞，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BdnsalInstructionNode extends BinaryInstruction_DR_O {
        type: "bdnsalInstruction";
    }

    /**
     * @summary BDSE（Branches on Device Stalled）指令节点
     *
     * @desc 如果指定设备处于停滞状态，则跳转到目标地址。
     * @elseif en
     * @summary BDSE (Branches on Device Stalled) instruction node
     *
     * @desc Jumps to the target address if the specified device is stalled.
     *
     * @example
     * ```typescript
     * // BDSE console1 loop
     * // 如果 console1 停滞，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BdseInstructionNode extends BinaryInstruction_DR_O {
        type: "bdseInstruction";
    }

    /**
     * @summary BDSEAL（Branches on Device Stalled And Link）指令节点
     *
     * @desc 如果指定设备处于停滞状态，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BDSEAL (Branches on Device Stalled And Link) instruction node
     *
     * @desc Jumps to the target address if the specified device is stalled, and saves the return address.
     *
     * @example
     * ```typescript
     * // BDSEAL console1 loop
     * // 如果 console1 停滞，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BdsealInstructionNode extends BinaryInstruction_DR_O {
        type: "bdsealInstruction";
    }

    /**
     * @summary BRDNS（Branches Register on Device Not Stalled）指令节点
     *
     * @desc 如果指定设备未处于停滞状态，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRDNS (Branches Register on Device Not Stalled) instruction node
     *
     * @desc Jumps to the address stored in the register if the specified device is not stalled.
     *
     * @example
     * ```typescript
     * // BRDNS r0 console1
     * // 如果 console1 未停滞，跳转到 r0 存储的地址
     * ```
     *
     * @public
     */
    export interface BrdnsInstructionNode extends BinaryInstruction_DR_O {
        type: "brdnsInstruction";
    }

    /**
     * @summary BRDSE（Branches Register on Device Stalled）指令节点
     *
     * @desc 如果指定设备处于停滞状态，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRDSE (Branches Register on Device Stalled) instruction node
     *
     * @desc Jumps to the address stored in the register if the specified device is stalled.
     *
     * @example
     * ```typescript
     * // BRDSE r0 console1
     * // 如果 console1 停滞，跳转到 r0 存储的地址
     * ```
     *
     * @public
     */
    export interface BrdseInstructionNode extends BinaryInstruction_DR_O {
        type: "brdseInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + DeviceReference 操作数的二元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为设备引用。
     * @elseif en
     * @summary Base interface for binary instructions with RegisterOrIdentifier + DeviceReference operands
     *
     * @desc First operand is a register or identifier, second is a device reference.
     *
     * @public
     */
    interface BinaryInstruction_RI_DR extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: DeviceReferenceNode;
    }

    /**
     * @summary SDNS（Sets Device Not Stalled）指令节点
     *
     * @desc 将指定设备设置为非停滞状态。
     * @elseif en
     * @summary SDNS (Sets Device Not Stalled) instruction node
     *
     * @desc Sets the specified device to not stalled state.
     *
     * @example
     * ```typescript
     * // SDNS r0 console1
     * // 将 console1 设置为非停滞状态
     * ```
     *
     * @public
     */
    export interface SdnsInstructionNode extends BinaryInstruction_RI_DR {
        type: "sdnsInstruction";
    }

    /**
     * @summary SDSE（Sets Device Stalled）指令节点
     *
     * @desc 将指定设备设置为停滞状态。
     * @elseif en
     * @summary SDSE (Sets Device Stalled) instruction node
     *
     * @desc Sets the specified device to stalled state.
     *
     * @example
     * ```typescript
     * // SDSE r0 console1
     * // 将 console1 设置为停滞状态
     * ```
     *
     * @public
     */
    export interface SdseInstructionNode extends BinaryInstruction_RI_DR {
        type: "sdseInstruction";
    }

    /**
     * @summary 含 Operand + Operand 操作数的二元指令基类
     *
     * @desc 两个操作数都为通用操作数类型。
     * @elseif en
     * @summary Base interface for binary instructions with Operand + Operand operands
     *
     * @desc Both operands are general operand types.
     *
     * @public
     */
    interface BinaryInstruction_O_O extends ASTNode {
        operand1: OperandNode;
        operand2: OperandNode;
    }

    /**
     * @summary POKE（写入内存）指令节点
     *
     * @desc 将数据写入到指定的内存地址。
     * @elseif en
     * @summary POKE instruction node
     *
     * @desc Writes data to a specified memory address.
     *
     * @example
     * ```typescript
     * // POKE r0 r1
     * // 将 r0 的值写入地址 r1
     * ```
     *
     * @public
     */
    export interface PokeInstructionNode extends BinaryInstruction_O_O {
        type: "pokeInstruction";
    }

    /**
     * @summary BEQZ（Branch if EQual Zero）指令节点
     *
     * @desc 如果操作数等于零，则跳转到目标地址。
     * @elseif en
     * @summary BEQZ (Branch if EQual Zero) instruction node
     *
     * @desc Jumps to the target address if the operand equals zero.
     *
     * @example
     * ```typescript
     * // BEQZ r0 loop
     * // 如果 r0 == 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BeqzInstructionNode extends BinaryInstruction_O_O {
        type: "beqzInstruction";
    }

    /**
     * @summary BEQZAL（Branch if EQual Zero And Link）指令节点
     *
     * @desc 如果操作数等于零，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BEQZAL (Branch if EQual Zero And Link) instruction node
     *
     * @desc Jumps to the target address if the operand equals zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BEQZAL r0 loop
     * // 如果 r0 == 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BeqzalInstructionNode extends BinaryInstruction_O_O {
        type: "beqzalInstruction";
    }

    /**
     * @summary BNEZ（Branch if Not Equal Zero）指令节点
     *
     * @desc 如果操作数不等于零，则跳转到目标地址。
     * @elseif en
     * @summary BNEZ (Branch if Not Equal Zero) instruction node
     *
     * @desc Jumps to the target address if the operand does not equal zero.
     *
     * @example
     * ```typescript
     * // BNEZ r0 loop
     * // 如果 r0 != 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BnezInstructionNode extends BinaryInstruction_O_O {
        type: "bnezInstruction";
    }

    /**
     * @summary BNEZAL（Branch if Not Equal Zero And Link）指令节点
     *
     * @desc 如果操作数不等于零，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BNEZAL (Branch if Not Equal Zero And Link) instruction node
     *
     * @desc Jumps to the target address if the operand does not equal zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BNEZAL r0 loop
     * // 如果 r0 != 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BnezalInstructionNode extends BinaryInstruction_O_O {
        type: "bnezalInstruction";
    }

    /**
     * @summary BGEZ（Branch if Greater than or Equal Zero）指令节点
     *
     * @desc 如果操作数大于等于零，则跳转到目标地址。
     * @elseif en
     * @summary BGEZ (Branch if Greater than or Equal Zero) instruction node
     *
     * @desc Jumps to the target address if the operand is greater than or equal to zero.
     *
     * @example
     * ```typescript
     * // BGEZ r0 loop
     * // 如果 r0 >= 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BgezInstructionNode extends BinaryInstruction_O_O {
        type: "bgezInstruction";
    }

    /**
     * @summary BGEZAL（Branch if Greater than or Equal Zero And Link）指令节点
     *
     * @desc 如果操作数大于等于零，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BGEZAL (Branch if Greater than or Equal Zero And Link) instruction node
     *
     * @desc Jumps to the target address if the operand is greater than or equal to zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BGEZAL r0 loop
     * // 如果 r0 >= 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BgezalInstructionNode extends BinaryInstruction_O_O {
        type: "bgezalInstruction";
    }

    /**
     * @summary BGTZ（Branch if Greater Than Zero）指令节点
     *
     * @desc 如果操作数大于零，则跳转到目标地址。
     * @elseif en
     * @summary BGTZ (Branch if Greater Than Zero) instruction node
     *
     * @desc Jumps to the target address if the operand is greater than zero.
     *
     * @example
     * ```typescript
     * // BGTZ r0 loop
     * // 如果 r0 > 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BgtzInstructionNode extends BinaryInstruction_O_O {
        type: "bgtzInstruction";
    }

    /**
     * @summary BGTZAL（Branch if Greater Than Zero And Link）指令节点
     *
     * @desc 如果操作数大于零，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BGTZAL (Branch if Greater Than Zero And Link) instruction node
     *
     * @desc Jumps to the target address if the operand is greater than zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BGTZAL r0 loop
     * // 如果 r0 > 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BgtzalInstructionNode extends BinaryInstruction_O_O {
        type: "bgtzalInstruction";
    }

    /**
     * @summary BLEZ（Branch if Less than or Equal Zero）指令节点
     *
     * @desc 如果操作数小于等于零，则跳转到目标地址。
     * @elseif en
     * @summary BLEZ (Branch if Less than or Equal Zero) instruction node
     *
     * @desc Jumps to the target address if the operand is less than or equal to zero.
     *
     * @example
     * ```typescript
     * // BLEZ r0 loop
     * // 如果 r0 <= 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BlezInstructionNode extends BinaryInstruction_O_O {
        type: "blezInstruction";
    }

    /**
     * @summary BLEZAL（Branch if Less than or Equal Zero And Link）指令节点
     *
     * @desc 如果操作数小于等于零，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BLEZAL (Branch if Less than or Equal Zero And Link) instruction node
     *
     * @desc Jumps to the target address if the operand is less than or equal to zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BLEZAL r0 loop
     * // 如果 r0 <= 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BlezalInstructionNode extends BinaryInstruction_O_O {
        type: "blezalInstruction";
    }

    /**
     * @summary BLTZ（Branch if Less Than Zero）指令节点
     *
     * @desc 如果操作数小于零，则跳转到目标地址。
     * @elseif en
     * @summary BLTZ (Branch if Less Than Zero) instruction node
     *
     * @desc Jumps to the target address if the operand is less than zero.
     *
     * @example
     * ```typescript
     * // BLTZ r0 loop
     * // 如果 r0 < 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BltzInstructionNode extends BinaryInstruction_O_O {
        type: "bltzInstruction";
    }

    /**
     * @summary BLTZAL（Branch if Less Than Zero And Link）指令节点
     *
     * @desc 如果操作数小于零，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BLTZAL (Branch if Less Than Zero And Link) instruction node
     *
     * @desc Jumps to the target address if the operand is less than zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BLTZAL r0 loop
     * // 如果 r0 < 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BltzalInstructionNode extends BinaryInstruction_O_O {
        type: "bltzalInstruction";
    }

    /**
     * @summary BNAN（Branch if Not-A-Number）指令节点
     *
     * @desc 如果操作数是 NaN，则跳转到目标地址。
     * @elseif en
     * @summary BNAN (Branch if Not-A-Number) instruction node
     *
     * @desc Jumps to the target address if the operand is NaN.
     *
     * @example
     * ```typescript
     * // BNAN r0 loop
     * // 如果 r0 是 NaN，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BnanInstructionNode extends BinaryInstruction_O_O {
        type: "bnanInstruction";
    }

    /**
     * @summary BNANZ（Branch if Not-A-Number Non-Zero）指令节点
     *
     * @desc 如果操作数是 NaN 且非零，则跳转到目标地址。
     * @elseif en
     * @summary BNANZ (Branch if Not-A-Number Non-Zero) instruction node
     *
     * @desc Jumps to the target address if the operand is NaN and non-zero.
     *
     * @example
     * ```typescript
     * // BNANZ r0 loop
     * // 如果 r0 是 NaN 且非零，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BnanzInstructionNode extends BinaryInstruction_O_O {
        type: "bnanzInstruction";
    }

    /**
     * @summary BREQZ（Branches Register if EQual Zero）指令节点
     *
     * @desc 如果操作数等于零，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BREQZ (Branches Register if EQual Zero) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand equals zero.
     *
     * @example
     * ```typescript
     * // BREQZ r0 r1
     * // 如果 r0 == 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BreqzInstructionNode extends BinaryInstruction_O_O {
        type: "breqzInstruction";
    }

    /**
     * @summary BRNZ（Branches Register if Not Zero）指令节点
     *
     * @desc 如果操作数不等于零，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRNZ (Branches Register if Not Zero) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand does not equal zero.
     *
     * @example
     * ```typescript
     * // BRNZ r0 r1
     * // 如果 r0 != 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrnzInstructionNode extends BinaryInstruction_O_O {
        type: "brnzInstruction";
    }

    /**
     * @summary BRGEZ（Branches Register if Greater than or Equal Zero）指令节点
     *
     * @desc 如果操作数大于等于零，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRGEZ (Branches Register if Greater than or Equal Zero) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand is greater than or equal to zero.
     *
     * @example
     * ```typescript
     * // BRGEZ r0 r1
     * // 如果 r0 >= 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrgezInstructionNode extends BinaryInstruction_O_O {
        type: "brgezInstruction";
    }

    /**
     * @summary BRGTZ（Branches Register if Greater Than Zero）指令节点
     *
     * @desc 如果操作数大于零，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRGTZ (Branches Register if Greater Than Zero) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand is greater than zero.
     *
     * @example
     * ```typescript
     * // BRGTZ r0 r1
     * // 如果 r0 > 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrgtzInstructionNode extends BinaryInstruction_O_O {
        type: "brgtzInstruction";
    }

    /**
     * @summary BRLEZ（Branches Register if Less than or Equal Zero）指令节点
     *
     * @desc 如果操作数小于等于零，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRLEZ (Branches Register if Less than or Equal Zero) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand is less than or equal to zero.
     *
     * @example
     * ```typescript
     * // BRLEZ r0 r1
     * // 如果 r0 <= 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrlezInstructionNode extends BinaryInstruction_O_O {
        type: "brlezInstruction";
    }

    /**
     * @summary BRLTZ（Branches Register if Less Than Zero）指令节点
     *
     * @desc 如果操作数小于零，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRLTZ (Branches Register if Less Than Zero) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand is less than zero.
     *
     * @example
     * ```typescript
     * // BRLTZ r0 r1
     * // 如果 r0 < 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrltzInstructionNode extends BinaryInstruction_O_O {
        type: "brltzInstruction";
    }

    /**
     * @summary BRNAN（Branches Register if Not-A-Number）指令节点
     *
     * @desc 如果操作数是 NaN，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRNAN (Branches Register if Not-A-Number) instruction node
     *
     * @desc Jumps to the address stored in the register if the operand is NaN.
     *
     * @example
     * ```typescript
     * // BRNAN r0 r1
     * // 如果 r0 是 NaN，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrnanInstructionNode extends BinaryInstruction_O_O {
        type: "brnanInstruction";
    }

    /**
     * @summary 二元指令联合类型
     *
     * @desc 包含所有二元指令类型的联合，包括可能的错误节点。
     * @elseif en
     * @summary Binary instruction union type
     *
     * @desc Union of all binary instruction types, including possible error nodes.
     *
     * @public
     */
    export type BinaryInstructionNode =
        | AbsInstructionNode
        | AcosInstructionNode
        | AsinInstructionNode
        | AtanInstructionNode
        | CeilInstructionNode
        | CosInstructionNode
        | ExpInstructionNode
        | FloorInstructionNode
        | LogInstructionNode
        | RoundInstructionNode
        | SinInstructionNode
        | SqrtInstructionNode
        | TanInstructionNode
        | TruncInstructionNode
        | NotInstructionNode
        | MoveInstructionNode
        | SubInstructionNode
        | BdnsInstructionNode
        | BdnsalInstructionNode
        | BdseInstructionNode
        | BdsealInstructionNode
        | BrdnsInstructionNode
        | BrdseInstructionNode
        | SdnsInstructionNode
        | SdseInstructionNode
        | PokeInstructionNode
        | BeqzInstructionNode
        | BeqzalInstructionNode
        | BnezInstructionNode
        | BnezalInstructionNode
        | BgezInstructionNode
        | BgezalInstructionNode
        | BgtzInstructionNode
        | BgtzalInstructionNode
        | BlezInstructionNode
        | BlezalInstructionNode
        | BltzInstructionNode
        | BltzalInstructionNode
        | BnanInstructionNode
        | BnanzInstructionNode
        | BreqzInstructionNode
        | BrnzInstructionNode
        | BrgezInstructionNode
        | BrgtzInstructionNode
        | BrlezInstructionNode
        | BrltzInstructionNode
        | BrnanInstructionNode
        | ErrorNode;

    // 三元指令（3 个操作数）

    /**
     * @summary 三元指令概述
     *
     * @desc 三元指令是包含三个操作数的指令。
     * 根据操作数类型可分为以下几类：
     * - **TernaryInstruction_RI_O_O**：寄存器/标识符 + 操作数 + 操作数
     * - **TernaryInstruction_RI_DR_O**：寄存器/标识符 + 设备引用 + 操作数
     * - **TernaryInstruction_DR_O_O**：设备引用 + 操作数 + 操作数
     * - **TernaryInstruction_RI_DR_LT**：寄存器/标识符 + 设备引用 + 逻辑类型
     * - **TernaryInstruction_RI_DR_RM**：寄存器/标识符 + 设备引用 + 试剂模式
     * - **TernaryInstruction_DR_LT_RI**：设备引用 + 逻辑类型 + 寄存器/标识符
     * - **TernaryInstruction_O_LT_RI**：操作数 + 逻辑类型 + 寄存器/标识符
     * - **TernaryInstruction_DR_LT_O**：设备引用 + 逻辑类型 + 操作数
     * - **TernaryInstruction_O_O_O**：操作数 + 操作数 + 操作数
     *
     * @elseif en
     * @summary Ternary Instructions Overview
     *
     * @desc Ternary instructions contain three operands.
     * They are categorized by operand types:
     * - **TernaryInstruction_RI_O_O**: Register/identifier + operand + operand
     * - **TernaryInstruction_RI_DR_O**: Register/identifier + device reference + operand
     * - **TernaryInstruction_DR_O_O**: Device reference + operand + operand
     * - **TernaryInstruction_RI_DR_LT**: Register/identifier + device reference + logic type
     * - **TernaryInstruction_RI_DR_RM**: Register/identifier + device reference + reagent mode
     * - **TernaryInstruction_DR_LT_RI**: Device reference + logic type + register/identifier
     * - **TernaryInstruction_O_LT_RI**: Operand + logic type + register/identifier
     * - **TernaryInstruction_DR_LT_O**: Device reference + logic type + operand
     * - **TernaryInstruction_O_O_O**: Operand + operand + operand
     */

    /**
     * @summary 含 RegisterOrIdentifier + Operand + Operand 操作数的三元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二、三个为通用操作数。
     * @elseif en
     * @summary Base interface for ternary instructions with RegisterOrIdentifier + Operand + Operand operands
     *
     * @desc First operand is a register or identifier, second and third are general operands.
     *
     * @public
     */
    interface TernaryInstruction_RI_O_O extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
        operand3: OperandNode;
    }

    /**
     * @summary ADD（加法）指令节点
     *
     * @desc 将第二个和第三个操作数相加，结果存入目标寄存器。
     * @elseif en
     * @summary ADD (Addition) instruction node
     *
     * @desc Adds the second and third operands, storing the result in the target register.
     *
     * @example
     * ```typescript
     * // ADD r0 r1 r2
     * // 计算 r1 + r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface AddInstructionNode extends TernaryInstruction_RI_O_O {
        type: "addInstruction";
    }

    /**
     * @summary ATAN2（反正切2）指令节点
     *
     * @desc 计算第二个操作数除以第三个操作数的反正切值（弧度制）。
     * @elseif en
     * @summary ATAN2 (Arc Tangent 2) instruction node
     *
     * @desc Computes the arc tangent of the second operand divided by the third operand (in radians).
     *
     * @example
     * ```typescript
     * // ATAN2 r0 r1 r2
     * // 计算 atan2(r1, r2) 并存入 r0
     * ```
     *
     * @public
     */
    export interface Atan2InstructionNode extends TernaryInstruction_RI_O_O {
        type: "atan2Instruction";
    }

    /**
     * @summary DIV（除法）指令节点
     *
     * @desc 将第二个操作数除以第三个操作数，结果存入目标寄存器。
     * @elseif en
     * @summary DIV (Division) instruction node
     *
     * @desc Divides the second operand by the third operand, storing the result in the target register.
     *
     * @example
     * ```typescript
     * // DIV r0 r1 r2
     * // 计算 r1 / r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface DivInstructionNode extends TernaryInstruction_RI_O_O {
        type: "divInstruction";
    }

    /**
     * @summary MAX（最大值）指令节点
     *
     * @desc 比较第二个和第三个操作数，将较大值存入目标寄存器。
     * @elseif en
     * @summary MAX (Maximum) instruction node
     *
     * @desc Compares the second and third operands, storing the larger value in the target register.
     *
     * @example
     * ```typescript
     * // MAX r0 r1 r2
     * // 将 r1 和 r2 中的较大值存入 r0
     * ```
     *
     * @public
     */
    export interface MaxInstructionNode extends TernaryInstruction_RI_O_O {
        type: "maxInstruction";
    }

    /**
     * @summary MIN（最小值）指令节点
     *
     * @desc 比较第二个和第三个操作数，将较小值存入目标寄存器。
     * @elseif en
     * @summary MIN (Minimum) instruction node
     *
     * @desc Compares the second and third operands, storing the smaller value in the target register.
     *
     * @example
     * ```typescript
     * // MIN r0 r1 r2
     * // 将 r1 和 r2 中的较小值存入 r0
     * ```
     *
     * @public
     */
    export interface MinInstructionNode extends TernaryInstruction_RI_O_O {
        type: "minInstruction";
    }

    /**
     * @summary MOD（取模）指令节点
     *
     * @desc 计算第二个操作数除以第三个操作数的余数。
     * @elseif en
     * @summary MOD (Modulo) instruction node
     *
     * @desc Computes the remainder of the second operand divided by the third operand.
     *
     * @example
     * ```typescript
     * // MOD r0 r1 r2
     * // 计算 r1 % r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface ModInstructionNode extends TernaryInstruction_RI_O_O {
        type: "modInstruction";
    }

    /**
     * @summary MUL（乘法）指令节点
     *
     * @desc 将第二个和第三个操作数相乘，结果存入目标寄存器。
     * @elseif en
     * @summary MUL (Multiplication) instruction node
     *
     * @desc Multiplies the second and third operands, storing the result in the target register.
     *
     * @example
     * ```typescript
     * // MUL r0 r1 r2
     * // 计算 r1 * r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface MulInstructionNode extends TernaryInstruction_RI_O_O {
        type: "mulInstruction";
    }

    /**
     * @summary POW（幂运算）指令节点
     *
     * @desc 计算第二个操作数的第三次操作数次幂。
     * @elseif en
     * @summary POW (Power) instruction node
     *
     * @desc Computes the second operand raised to the power of the third operand.
     *
     * @example
     * ```typescript
     * // POW r0 r1 r2
     * // 计算 r1 ^ r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface PowInstructionNode extends TernaryInstruction_RI_O_O {
        type: "powInstruction";
    }

    /**
     * @summary AND（按位与）指令节点
     *
     * @desc 对第二个和第三个操作数进行按位与运算。
     * @elseif en
     * @summary AND (Bitwise AND) instruction node
     *
     * @desc Performs bitwise AND operation on the second and third operands.
     *
     * @example
     * ```typescript
     * // AND r0 r1 r2
     * // 计算 r1 & r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface AndInstructionNode extends TernaryInstruction_RI_O_O {
        type: "andInstruction";
    }

    /**
     * @summary NOR（按位或非）指令节点
     *
     * @desc 对第二个和第三个操作数进行按位或运算，然后取反。
     * @elseif en
     * @summary NOR (Bitwise NOR) instruction node
     *
     * @desc Performs bitwise OR operation on the second and third operands, then negates the result.
     *
     * @example
     * ```typescript
     * // NOR r0 r1 r2
     * // 计算 ~(r1 | r2) 并存入 r0
     * ```
     *
     * @public
     */
    export interface NorInstructionNode extends TernaryInstruction_RI_O_O {
        type: "norInstruction";
    }

    /**
     * @summary OR（按位或）指令节点
     *
     * @desc 对第二个和第三个操作数进行按位或运算。
     * @elseif en
     * @summary OR (Bitwise OR) instruction node
     *
     * @desc Performs bitwise OR operation on the second and third operands.
     *
     * @example
     * ```typescript
     * // OR r0 r1 r2
     * // 计算 r1 | r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface OrInstructionNode extends TernaryInstruction_RI_O_O {
        type: "orInstruction";
    }

    /**
     * @summary SLA（算术左移）指令节点
     *
     * @desc 将第二个操作数左移第三个操作数指定的位数（算术左移）。
     * @elseif en
     * @summary SLA (Shift Left Arithmetic) instruction node
     *
     * @desc Arithmetically shifts the second operand left by the number of bits specified by the third operand.
     *
     * @example
     * ```typescript
     * // SLA r0 r1 r2
     * // 将 r1 左移 r2 位并存入 r0
     * ```
     *
     * @public
     */
    export interface SlaInstructionNode extends TernaryInstruction_RI_O_O {
        type: "slaInstruction";
    }

    /**
     * @summary SLL（逻辑左移）指令节点
     *
     * @desc 将第二个操作数左移第三个操作数指定的位数（逻辑左移）。
     * @elseif en
     * @summary SLL (Shift Left Logical) instruction node
     *
     * @desc Logically shifts the second operand left by the number of bits specified by the third operand.
     *
     * @example
     * ```typescript
     * // SLL r0 r1 r2
     * // 将 r1 左移 r2 位并存入 r0
     * ```
     *
     * @public
     */
    export interface SllInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sllInstruction";
    }

    /**
     * @summary SRA（算术右移）指令节点
     *
     * @desc 将第二个操作数右移第三个操作数指定的位数（算术右移，符号位扩展）。
     * @elseif en
     * @summary SRA (Shift Right Arithmetic) instruction node
     *
     * @desc Arithmetically shifts the second operand right by the number of bits specified by the third operand (sign-extended).
     *
     * @example
     * ```typescript
     * // SRA r0 r1 r2
     * // 将 r1 右移 r2 位并存入 r0
     * ```
     *
     * @public
     */
    export interface SraInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sraInstruction";
    }

    /**
     * @summary SRL（逻辑右移）指令节点
     *
     * @desc 将第二个操作数右移第三个操作数指定的位数（逻辑右移，零扩展）。
     * @elseif en
     * @summary SRL (Shift Right Logical) instruction node
     *
     * @desc Logically shifts the second operand right by the number of bits specified by the third operand (zero-extended).
     *
     * @example
     * ```typescript
     * // SRL r0 r1 r2
     * // 将 r1 右移 r2 位并存入 r0
     * ```
     *
     * @public
     */
    export interface SrlInstructionNode extends TernaryInstruction_RI_O_O {
        type: "srlInstruction";
    }

    /**
     * @summary XOR（按位异或）指令节点
     *
     * @desc 对第二个和第三个操作数进行按位异或运算。
     * @elseif en
     * @summary XOR (Bitwise XOR) instruction node
     *
     * @desc Performs bitwise XOR operation on the second and third operands.
     *
     * @example
     * ```typescript
     * // XOR r0 r1 r2
     * // 计算 r1 ^ r2 并存入 r0
     * ```
     *
     * @public
     */
    export interface XorInstructionNode extends TernaryInstruction_RI_O_O {
        type: "xorInstruction";
    }

    /**
     * @summary SAPZ（Set if And Predicate Zero）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与结果为零，则将目标寄存器设置为1。
     * @elseif en
     * @summary SAPZ (Set if And Predicate Zero) instruction node
     *
     * @desc Sets the target register to 1 if the bitwise AND of the second and third operands is zero.
     *
     * @example
     * ```typescript
     * // SAPZ r0 r1 r2
     * // 如果 (r1 & r2) == 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SapzInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sapzInstruction";
    }

    /**
     * @summary SNAZ（Set if And Predicate Non-Zero）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与结果非零，则将目标寄存器设置为1。
     * @elseif en
     * @summary SNAZ (Set if And Predicate Non-Zero) instruction node
     *
     * @desc Sets the target register to 1 if the bitwise AND of the second and third operands is non-zero.
     *
     * @example
     * ```typescript
     * // SNAZ r0 r1 r2
     * // 如果 (r1 & r2) != 0，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SnazInstructionNode extends TernaryInstruction_RI_O_O {
        type: "snazInstruction";
    }

    /**
     * @summary SEQ（Set if EQual）指令节点
     *
     * @desc 如果第二个操作数等于第三个操作数，则将目标寄存器设置为1。
     * @elseif en
     * @summary SEQ (Set if EQual) instruction node
     *
     * @desc Sets the target register to 1 if the second operand equals the third operand.
     *
     * @example
     * ```typescript
     * // SEQ r0 r1 r2
     * // 如果 r1 == r2，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SeqInstructionNode extends TernaryInstruction_RI_O_O {
        type: "seqInstruction";
    }

    /**
     * @summary SNE（Set if Not Equal）指令节点
     *
     * @desc 如果第二个操作数不等于第三个操作数，则将目标寄存器设置为1。
     * @elseif en
     * @summary SNE (Set if Not Equal) instruction node
     *
     * @desc Sets the target register to 1 if the second operand does not equal the third operand.
     *
     * @example
     * ```typescript
     * // SNE r0 r1 r2
     * // 如果 r1 != r2，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SneInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sneInstruction";
    }

    /**
     * @summary SGE（Set if Greater than or Equal）指令节点
     *
     * @desc 如果第二个操作数大于等于第三个操作数，则将目标寄存器设置为1。
     * @elseif en
     * @summary SGE (Set if Greater than or Equal) instruction node
     *
     * @desc Sets the target register to 1 if the second operand is greater than or equal to the third operand.
     *
     * @example
     * ```typescript
     * // SGE r0 r1 r2
     * // 如果 r1 >= r2，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SgeInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sgeInstruction";
    }

    /**
     * @summary SGT（Set if Greater Than）指令节点
     *
     * @desc 如果第二个操作数大于第三个操作数，则将目标寄存器设置为1。
     * @elseif en
     * @summary SGT (Set if Greater Than) instruction node
     *
     * @desc Sets the target register to 1 if the second operand is greater than the third operand.
     *
     * @example
     * ```typescript
     * // SGT r0 r1 r2
     * // 如果 r1 > r2，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SgtInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sgtInstruction";
    }

    /**
     * @summary SLE（Set if Less than or Equal）指令节点
     *
     * @desc 如果第二个操作数小于等于第三个操作数，则将目标寄存器设置为1。
     * @elseif en
     * @summary SLE (Set if Less than or Equal) instruction node
     *
     * @desc Sets the target register to 1 if the second operand is less than or equal to the third operand.
     *
     * @example
     * ```typescript
     * // SLE r0 r1 r2
     * // 如果 r1 <= r2，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SleInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sleInstruction";
    }

    /**
     * @summary SLT（Set if Less Than）指令节点
     *
     * @desc 如果第二个操作数小于第三个操作数，则将目标寄存器设置为1。
     * @elseif en
     * @summary SLT (Set if Less Than) instruction node
     *
     * @desc Sets the target register to 1 if the second operand is less than the third operand.
     *
     * @example
     * ```typescript
     * // SLT r0 r1 r2
     * // 如果 r1 < r2，则 r0 = 1
     * ```
     *
     * @public
     */
    export interface SltInstructionNode extends TernaryInstruction_RI_O_O {
        type: "sltInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + DeviceReference + Operand 操作数的三元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为通用操作数。
     * @elseif en
     * @summary Base interface for ternary instructions with RegisterOrIdentifier + DeviceReference + Operand operands
     *
     * @desc First operand is a register or identifier, second is a device reference, third is a general operand.
     *
     * @public
     */
    interface TernaryInstruction_RI_DR_O extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: DeviceReferenceNode;
        operand3: OperandNode;
    }

    /**
     * @summary GET（获取设备属性）指令节点
     *
     * @desc 从设备获取指定类型的属性值到目标寄存器。
     * @elseif en
     * @summary GET (Get Device Property) instruction node
     *
     * @desc Gets the property of a specified type from the device into the target register.
     *
     * @example
     * ```typescript
     * // GET r0 console1 Setting
     * // 从 console1 获取 Setting 属性到 r0
     * ```
     *
     * @public
     */
    export interface GetInstructionNode extends TernaryInstruction_RI_DR_O {
        type: "getInstruction";
    }

    /**
     * @summary RMAP（重新映射设备）指令节点
     *
     * @desc 重新映射目标设备到源设备。
     * @elseif en
     * @summary RMAP (ReMAP Device) instruction node
     *
     * @desc Remaps the target device to the source device.
     *
     * @example
     * ```typescript
     * // RMAP r0 console1 console2
     * // 将 console1 重新映射到 console2
     * ```
     *
     * @public
     */
    export interface RmapInstructionNode extends TernaryInstruction_RI_DR_O {
        type: "rmapInstruction";
    }

    /**
     * @summary 含 DeviceReference + Operand + Operand 操作数的三元指令基类
     *
     * @desc 第一个操作数为设备引用，第二、三个为通用操作数。
     * @elseif en
     * @summary Base interface for ternary instructions with DeviceReference + Operand + Operand operands
     *
     * @desc First operand is a device reference, second and third are general operands.
     *
     * @public
     */
    interface TernaryInstruction_DR_O_O extends ASTNode {
        operand1: DeviceReferenceNode;
        operand2: OperandNode;
        operand3: OperandNode;
    }

    /**
     * @summary PUT（放置物品到设备）指令节点
     *
     * @desc 将物品从库存放置到目标设备。
     * @elseif en
     * @summary PUT instruction node
     *
     * @desc Places items from inventory to the target device.
     *
     * @example
     * ```typescript
     * // PUT console1 r0 r1
     * // 将物品从 r0 放置到 console1
     * ```
     *
     * @public
     */
    export interface PutInstructionNode extends TernaryInstruction_DR_O_O {
        type: "putInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + DeviceReference + LogicType 操作数的三元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为逻辑类型。
     * @elseif en
     * @summary Base interface for ternary instructions with RegisterOrIdentifier + DeviceReference + LogicType operands
     *
     * @desc First operand is a register or identifier, second is a device reference, third is a logic type.
     *
     * @public
     */
    interface TernaryInstruction_RI_DR_LT extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: DeviceReferenceNode;
        operand3: LogicTypeNode;
    }

    /**
     * @summary L（读取设备逻辑）指令节点
     *
     * @desc 从目标设备的指定逻辑类型读取数据到寄存器。
     * @elseif en
     * @summary L (Read Device Logic) instruction node
     *
     * @desc Reads data from the target device's specified logic type into the register.
     *
     * @example
     * ```typescript
     * // L r0 console1 Color
     * // 从 console1 读取 Color 数据到 r0
     * ```
     *
     * @public
     */
    export interface LInstructionNode extends TernaryInstruction_RI_DR_LT {
        type: "lInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + DeviceReference + ReagentMode 操作数的三元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为试剂模式。
     * @elseif en
     * @summary Base interface for ternary instructions with RegisterOrIdentifier + DeviceReference + ReagentMode operands
     *
     * @desc First operand is a register or identifier, second is a device reference, third is a reagent mode.
     *
     * @public
     */
    interface TernaryInstruction_RI_DR_RM extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: DeviceReferenceNode;
        operand3: ReagentModeNode;
    }

    /**
     * @summary LR（读取设备库存试剂）指令节点
     *
     * @desc 从目标设备的库存中读取指定试剂的数量到寄存器。
     * @elseif en
     * @summary LR (Read Device Reagent) instruction node
     *
     * @desc Reads the quantity of a specified reagent from the target device's inventory into the register.
     *
     * @example
     * ```typescript
     * // LR r0 tank1 Water
     * // 从 tank1 读取 Water 试剂数量到 r0
     * ```
     *
     * @public
     */
    export interface LrInstructionNode extends TernaryInstruction_RI_DR_RM {
        type: "lrInstruction";
    }

    /**
     * @summary 含 DeviceReference + LogicType + RegisterOrIdentifier 操作数的三元指令基类
     *
     * @desc 第一个操作数为设备引用，第二个为逻辑类型，第三个为寄存器或标识符。
     * @elseif en
     * @summary Base interface for ternary instructions with DeviceReference + LogicType + RegisterOrIdentifier operands
     *
     * @desc First operand is a device reference, second is a logic type, third is a register or identifier.
     *
     * @public
     */
    interface TernaryInstruction_DR_LT_RI extends ASTNode {
        operand1: DeviceReferenceNode;
        operand2: LogicTypeNode;
        operand3: RegisterOrIdentifierNode;
    }

    /**
     * @summary S（设置设备逻辑）指令节点
     *
     * @desc 将寄存器的值设置到目标设备的指定逻辑类型属性。
     * @elseif en
     * @summary S (Set Device Logic) instruction node
     *
     * @desc Sets the value of the register to the specified logic type property of the target device.
     *
     * @example
     * ```typescript
     * // S console1 Color r0
     * // 将 r0 的值设置到 console1 的 Color 属性
     * ```
     *
     * @public
     */
    export interface SInstructionNode extends TernaryInstruction_DR_LT_RI {
        type: "sInstruction";
    }

    /**
     * @summary 含 Operand + LogicType + RegisterOrIdentifier 操作数的三元指令基类
     *
     * @desc 第一个操作数为通用操作数，第二个为逻辑类型，第三个为寄存器或标识符。
     * @elseif en
     * @summary Base interface for ternary instructions with Operand + LogicType + RegisterOrIdentifier operands
     *
     * @desc First operand is a general operand, second is a logic type, third is a register or identifier.
     *
     * @public
     */
    interface TernaryInstruction_O_LT_RI extends ASTNode {
        operand1: OperandNode;
        operand2: LogicTypeNode;
        operand3: RegisterOrIdentifierNode;
    }

    /**
     * @summary SB（设置设备逻辑批量）指令节点
     *
     * @desc 将多个寄存器的值批量设置到目标设备的指定逻辑类型属性。
     * @elseif en
     * @summary SB (Set Device Logic Batch) instruction node
     *
     * @desc Batch sets values from multiple registers to the specified logic type property of the target device.
     *
     * @example
     * ```typescript
     * // SB tank1 Mode r0
     * // 将 r0 的值批量设置到 tank1 的 Mode 属性
     * ```
     *
     * @public
     */
    export interface SbInstructionNode extends TernaryInstruction_O_LT_RI {
        type: "sbInstruction";
    }

    /**
     * @summary 含 DeviceReference + LogicType + Operand 操作数的三元指令基类
     *
     * @desc 第一个操作数为设备引用，第二个为逻辑类型，第三个为通用操作数。
     * @elseif en
     * @summary Base interface for ternary instructions with DeviceReference + LogicType + Operand operands
     *
     * @desc First operand is a device reference, second is a logic type, third is a general operand.
     *
     * @public
     */
    interface TernaryInstruction_DR_LT_O extends ASTNode {
        operand1: DeviceReferenceNode;
        operand2: LogicTypeNode;
        operand3: OperandNode;
    }

    /**
     * @summary BDNVL（Branches on Device No Variant Leak）指令节点
     *
     * @desc 如果设备没有变体泄漏，则跳转到目标地址。
     * @elseif en
     * @summary BDNVL (Branches on Device No Variant Leak) instruction node
     *
     * @desc Jumps to the target address if the device has no variant leak.
     *
     * @example
     * ```typescript
     * // BDNVL tank1 loop
     * // 如果 tank1 没有变体泄漏，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BdnvlInstructionNode extends TernaryInstruction_DR_LT_O {
        type: "bdnvlInstruction";
    }

    /**
     * @summary BDNVS（Branches on Device No Variant Stuck）指令节点
     *
     * @desc 如果设备没有变体卡住，则跳转到目标地址。
     * @elseif en
     * @summary BDNVS (Branches on Device No Variant Stuck) instruction node
     *
     * @desc Jumps to the target address if the device has no variant stuck.
     *
     * @example
     * ```typescript
     * // BDNVS tank1 loop
     * // 如果 tank1 没有变体卡住，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BdnvsInstructionNode extends TernaryInstruction_DR_LT_O {
        type: "bdnvsInstruction";
    }

    /**
     * @summary 含 Operand + Operand + Operand 操作数的三元指令基类
     *
     * @desc 三个操作数都为通用操作数类型。
     * @elseif en
     * @summary Base interface for ternary instructions with Operand + Operand + Operand operands
     *
     * @desc All three operands are general operand types.
     *
     * @public
     */
    interface TernaryInstruction_O_O_O extends ASTNode {
        operand1: OperandNode;
        operand2: OperandNode;
        operand3: OperandNode;
    }

    /**
     * @summary BEQ（Branch if EQual）指令节点
     *
     * @desc 如果第二个操作数等于第三个操作数，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BEQ (Branch if EQual) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand equals the third operand.
     *
     * @example
     * ```typescript
     * // BEQ loop r0 r1
     * // 如果 r0 == r1，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BeqInstructionNode extends TernaryInstruction_O_O_O {
        type: "beqInstruction";
    }

    /**
     * @summary BEQAL（Branch if EQual And Link）指令节点
     *
     * @desc 如果第二个操作数等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BEQAL (Branch if EQual And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand equals the third operand, and saves the return address.
     *
     * @example
     * ```typescript
     * // BEQAL loop r0 r1
     * // 如果 r0 == r1，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BeqalInstructionNode extends TernaryInstruction_O_O_O {
        type: "beqalInstruction";
    }

    /**
     * @summary BNE（Branch if Not Equal）指令节点
     *
     * @desc 如果第二个操作数不等于第三个操作数，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BNE (Branch if Not Equal) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand does not equal the third operand.
     *
     * @example
     * ```typescript
     * // BNE loop r0 r1
     * // 如果 r0 != r1，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BneInstructionNode extends TernaryInstruction_O_O_O {
        type: "bneInstruction";
    }

    /**
     * @summary BNEAL（Branch if Not Equal And Link）指令节点
     *
     * @desc 如果第二个操作数不等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BNEAL (Branch if Not Equal And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand does not equal the third operand, and saves the return address.
     *
     * @example
     * ```typescript
     * // BNEAL loop r0 r1
     * // 如果 r0 != r1，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BnealInstructionNode extends TernaryInstruction_O_O_O {
        type: "bnealInstruction";
    }

    /**
     * @summary BGE（Branch if Greater than or Equal）指令节点
     *
     * @desc 如果第二个操作数大于等于第三个操作数，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BGE (Branch if Greater than or Equal) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is greater than or equal to the third operand.
     *
     * @example
     * ```typescript
     * // BGE loop r0 r1
     * // 如果 r0 >= r1，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BgeInstructionNode extends TernaryInstruction_O_O_O {
        type: "bgeInstruction";
    }

    /**
     * @summary BGEAL（Branch if Greater than or Equal And Link）指令节点
     *
     * @desc 如果第二个操作数大于等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BGEAL (Branch if Greater than or Equal And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is greater than or equal to the third operand, and saves the return address.
     *
     * @example
     * ```typescript
     * // BGEAL loop r0 r1
     * // 如果 r0 >= r1，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BgealInstructionNode extends TernaryInstruction_O_O_O {
        type: "bgealInstruction";
    }

    /**
     * @summary BGT（Branch if Greater Than）指令节点
     *
     * @desc 如果第二个操作数大于第三个操作数，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BGT (Branch if Greater Than) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is greater than the third operand.
     *
     * @example
     * ```typescript
     * // BGT loop r0 r1
     * // 如果 r0 > r1，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BgtInstructionNode extends TernaryInstruction_O_O_O {
        type: "bgtInstruction";
    }

    /**
     * @summary BGTAL（Branch if Greater Than And Link）指令节点
     *
     * @desc 如果第二个操作数大于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BGTAL (Branch if Greater Than And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is greater than the third operand, and saves the return address.
     *
     * @example
     * ```typescript
     * // BGTAL loop r0 r1
     * // 如果 r0 > r1，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BgtalInstructionNode extends TernaryInstruction_O_O_O {
        type: "bgtalInstruction";
    }

    /**
     * @summary BLE（Branch if Less than or Equal）指令节点
     *
     * @desc 如果第二个操作数小于等于第三个操作数，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BLE (Branch if Less than or Equal) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is less than or equal to the third operand.
     *
     * @example
     * ```typescript
     * // BLE loop r0 r1
     * // 如果 r0 <= r1，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BleInstructionNode extends TernaryInstruction_O_O_O {
        type: "bleInstruction";
    }

    /**
     * @summary BLEAL（Branch if Less than or Equal And Link）指令节点
     *
     * @desc 如果第二个操作数小于等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BLEAL (Branch if Less than or Equal And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is less than or equal to the third operand, and saves the return address.
     *
     * @example
     * ```typescript
     * // BLEAL loop r0 r1
     * // 如果 r0 <= r1，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BlealInstructionNode extends TernaryInstruction_O_O_O {
        type: "blealInstruction";
    }

    /**
     * @summary BLT（Branch if Less Than）指令节点
     *
     * @desc 如果第二个操作数小于第三个操作数，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BLT (Branch if Less Than) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is less than the third operand.
     *
     * @example
     * ```typescript
     * // BLT loop r0 r1
     * // 如果 r0 < r1，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BltInstructionNode extends TernaryInstruction_O_O_O {
        type: "bltInstruction";
    }

    /**
     * @summary BLTAL（Branch if Less Than And Link）指令节点
     *
     * @desc 如果第二个操作数小于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BLTAL (Branch if Less Than And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the second operand is less than the third operand, and saves the return address.
     *
     * @example
     * ```typescript
     * // BLTAL loop r0 r1
     * // 如果 r0 < r1，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BltalInstructionNode extends TernaryInstruction_O_O_O {
        type: "bltalInstruction";
    }

    /**
     * @summary BAPZ（Branch if And Predicate Zero）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与结果为零，则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BAPZ (Branch if And Predicate Zero) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is zero.
     *
     * @example
     * ```typescript
     * // BAPZ loop r0 r1
     * // 如果 (r0 & r1) == 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BapzInstructionNode extends TernaryInstruction_O_O_O {
        type: "bapzInstruction";
    }

    /**
     * @summary BAPZAL（Branch if And Predicate Zero And Link）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与结果为零，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BAPZAL (Branch if And Predicate Zero And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BAPZAL loop r0 r1
     * // 如果 (r0 & r1) == 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BapzalInstructionNode extends TernaryInstruction_O_O_O {
        type: "bapzalInstruction";
    }

    /**
     * @summary BNAZ（Branch if Not And Predicate Zero）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与非结果为零（即按位与非零），则跳转到第一个操作数指定的目标地址。
     * @elseif en
     * @summary BNAZ (Branch if Not And Predicate Zero) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is non-zero.
     *
     * @example
     * ```typescript
     * // BNAZ loop r0 r1
     * // 如果 (r0 & r1) != 0，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BnazInstructionNode extends TernaryInstruction_O_O_O {
        type: "bnazInstruction";
    }

    /**
     * @summary BNAZAL（Branch if Not And Predicate Zero And Link）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与非结果为零，则跳转到第一个操作数指定的目标地址并保存返回地址。
     * @elseif en
     * @summary BNAZAL (Branch if Not And Predicate Zero And Link) instruction node
     *
     * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is non-zero, and saves the return address.
     *
     * @example
     * ```typescript
     * // BNAZAL loop r0 r1
     * // 如果 (r0 & r1) != 0，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BnazalInstructionNode extends TernaryInstruction_O_O_O {
        type: "bnazalInstruction";
    }

    /**
     * @summary BREQ（Branches Register if EQual）指令节点
     *
     * @desc 如果第二个操作数等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BREQ (Branches Register if EQual) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the second operand equals the third operand.
     *
     * @example
     * ```typescript
     * // BREQ r0 r1 r2
     * // 如果 r1 == r2，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BreqInstructionNode extends TernaryInstruction_O_O_O {
        type: "breqInstruction";
    }

    /**
     * @summary BRNE（Branches Register if Not Equal）指令节点
     *
     * @desc 如果第二个操作数不等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRNE (Branches Register if Not Equal) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the second operand does not equal the third operand.
     *
     * @example
     * ```typescript
     * // BRNE r0 r1 r2
     * // 如果 r1 != r2，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrneInstructionNode extends TernaryInstruction_O_O_O {
        type: "brneInstruction";
    }

    /**
     * @summary BRGE（Branches Register if Greater than or Equal）指令节点
     *
     * @desc 如果第二个操作数大于等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRGE (Branches Register if Greater than or Equal) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is greater than or equal to the third operand.
     *
     * @example
     * ```typescript
     * // BRGE r0 r1 r2
     * // 如果 r1 >= r2，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrgeInstructionNode extends TernaryInstruction_O_O_O {
        type: "brgeInstruction";
    }

    /**
     * @summary BRGT（Branches Register if Greater Than）指令节点
     *
     * @desc 如果第二个操作数大于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRGT (Branches Register if Greater Than) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is greater than the third operand.
     *
     * @example
     * ```typescript
     * // BRGT r0 r1 r2
     * // 如果 r1 > r2，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrgtInstructionNode extends TernaryInstruction_O_O_O {
        type: "brgtInstruction";
    }

    /**
     * @summary BRLE（Branches Register if Less than or Equal）指令节点
     *
     * @desc 如果第二个操作数小于等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRLE (Branches Register if Less than or Equal) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is less than or equal to the third operand.
     *
     * @example
     * ```typescript
     * // BRLE r0 r1 r2
     * // 如果 r1 <= r2，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrleInstructionNode extends TernaryInstruction_O_O_O {
        type: "brleInstruction";
    }

    /**
     * @summary BRLT（Branches Register if Less Than）指令节点
     *
     * @desc 如果第二个操作数小于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRLT (Branches Register if Less Than) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is less than the third operand.
     *
     * @example
     * ```typescript
     * // BRLT r0 r1 r2
     * // 如果 r1 < r2，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrltInstructionNode extends TernaryInstruction_O_O_O {
        type: "brltInstruction";
    }

    /**
     * @summary BRAPZ（Branches Register if And Predicate Zero）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与结果为零，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRAPZ (Branches Register if And Predicate Zero) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the bitwise AND of the second and third operands is zero.
     *
     * @example
     * ```typescript
     * // BRAPZ r0 r1 r2
     * // 如果 (r1 & r2) == 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrapzInstructionNode extends TernaryInstruction_O_O_O {
        type: "brapzInstruction";
    }

    /**
     * @summary BRNAZ（Branches Register if Not And Predicate Zero）指令节点
     *
     * @desc 如果第二个和第三个操作数的按位与结果非零，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
     * @elseif en
     * @summary BRNAZ (Branches Register if Not And Predicate Zero) instruction node
     *
     * @desc Jumps to the target address specified by the second operand (as address register) if the bitwise AND of the second and third operands is non-zero.
     *
     * @example
     * ```typescript
     * // BRNAZ r0 r1 r2
     * // 如果 (r1 & r2) != 0，跳转到 r1 存储的地址
     * ```
     *
     * @public
     */
    export interface BrnazInstructionNode extends TernaryInstruction_O_O_O {
        type: "brnazInstruction";
    }

    /**
     * @summary 三元指令联合类型
     *
     * @desc 包含所有三元指令类型的联合，包括可能的错误节点。
     * @elseif en
     * @summary Ternary instruction union type
     *
     * @desc Union of all ternary instruction types, including possible error nodes.
     *
     * @public
     */
    export type TernaryInstructionNode =
        | AddInstructionNode
        | Atan2InstructionNode
        | DivInstructionNode
        | MaxInstructionNode
        | MinInstructionNode
        | ModInstructionNode
        | MulInstructionNode
        | PowInstructionNode
        | AndInstructionNode
        | NorInstructionNode
        | OrInstructionNode
        | SlaInstructionNode
        | SllInstructionNode
        | SraInstructionNode
        | SrlInstructionNode
        | XorInstructionNode
        | SapzInstructionNode
        | SnazInstructionNode
        | SeqInstructionNode
        | SneInstructionNode
        | SgeInstructionNode
        | SgtInstructionNode
        | SleInstructionNode
        | SltInstructionNode
        | GetInstructionNode
        | RmapInstructionNode
        | PutInstructionNode
        | LInstructionNode
        | LrInstructionNode
        | SInstructionNode
        | SbInstructionNode
        | BdnvlInstructionNode
        | BdnvsInstructionNode
        | BeqInstructionNode
        | BeqalInstructionNode
        | BneInstructionNode
        | BnealInstructionNode
        | BgeInstructionNode
        | BgealInstructionNode
        | BgtInstructionNode
        | BgtalInstructionNode
        | BleInstructionNode
        | BlealInstructionNode
        | BltInstructionNode
        | BltalInstructionNode
        | BapzInstructionNode
        | BapzalInstructionNode
        | BnazInstructionNode
        | BnazalInstructionNode
        | BreqInstructionNode
        | BrneInstructionNode
        | BrgeInstructionNode
        | BrgtInstructionNode
        | BrleInstructionNode
        | BrltInstructionNode
        | BrapzInstructionNode
        | BrnazInstructionNode
        | ErrorNode;

    // 四元指令（4 个操作数）

    /**
     * @summary 四元指令概述
     *
     * @desc 四元指令是包含四个操作数的指令。
     * 根据操作数类型可分为以下几类：
     * - **QuaternaryInstruction_RI_O_O_O**：寄存器/标识符 + 操作数 + 操作数 + 操作数
     * - **QuaternaryInstruction_DR_SI_LS_RI**：设备引用 + 槽索引 + 逻辑槽类型 + 寄存器/标识符
     * - **QuaternaryInstruction_RI_O_LT_BM**：寄存器/标识符 + 操作数 + 逻辑类型 + 批处理模式
     * - **QuaternaryInstruction_O_O_LT_RI**：操作数 + 操作数 + 逻辑类型 + 寄存器/标识符
     * - **QuaternaryInstruction_O_SI_LS_RI**：操作数 + 槽索引 + 逻辑槽类型 + 寄存器/标识符
     * - **QuaternaryInstruction_O_O_O_O**：操作数 + 操作数 + 操作数 + 操作数
     * - **QuaternaryInstruction_RI_DR_SI_LS**：寄存器/标识符 + 设备引用 + 槽索引 + 逻辑槽类型
     *
     * @elseif en
     * @summary Quaternary Instructions Overview
     *
     * @desc Quaternary instructions contain four operands.
     * They are categorized by operand types:
     * - **QuaternaryInstruction_RI_O_O_O**: Register/identifier + operand + operand + operand
     * - **QuaternaryInstruction_DR_SI_LS_RI**: Device reference + slot index + logic slot type + register/identifier
     * - **QuaternaryInstruction_RI_O_LT_BM**: Register/identifier + operand + logic type + batch mode
     * - **QuaternaryInstruction_O_O_LT_RI**: Operand + operand + logic type + register/identifier
     * - **QuaternaryInstruction_O_SI_LS_RI**: Operand + slot index + logic slot type + register/identifier
     * - **QuaternaryInstruction_O_O_O_O**: Operand + operand + operand + operand
     * - **QuaternaryInstruction_RI_DR_SI_LS**: Register/identifier + device reference + slot index + logic slot type
     */

    /**
     * @summary 含 RegisterOrIdentifier + Operand + Operand + Operand 操作数的四元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，后三个为通用操作数。
     * @elseif en
     * @summary Base interface for quaternary instructions with RegisterOrIdentifier + Operand + Operand + Operand operands
     *
     * @desc First operand is a register or identifier, last three are general operands.
     *
     * @public
     */
    interface QuaternaryInstruction_RI_O_O_O extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
        operand3: OperandNode;
        operand4: OperandNode;
    }

    /**
     * @summary LERP（线性插值）指令节点
     *
     * @desc 计算两个值之间的线性插值。
     * 结果 = start + (end - start) * t
     * @elseif en
     * @summary LERP (Linear Interpolation) instruction node
     *
     * @desc Computes linear interpolation between two values.
     * Result = start + (end - start) * t
     *
     * @example
     * ```typescript
     * // LERP r0 r1 r2 r3
     * // 计算从 r1 到 r2 的线性插值，t = r3
     * ```
     *
     * @public
     */
    export interface LerpInstructionNode extends QuaternaryInstruction_RI_O_O_O {
        type: "lerpInstruction";
    }

    /**
     * @summary EXT（提取位）指令节点
     *
     * @desc 从源操作数中提取指定的位范围。
     * @elseif en
     * @summary EXT (Extract Bits) instruction node
     *
     * @desc Extracts a specified range of bits from the source operand.
     *
     * @example
     * ```typescript
     * // EXT r0 r1 r2 r3
     * // 从 r1 中提取位，结果存入 r0
     * ```
     *
     * @public
     */
    export interface ExtInstructionNode extends QuaternaryInstruction_RI_O_O_O {
        type: "extInstruction";
    }

    /**
     * @summary INS（插入位）指令节点
     *
     * @desc 将位插入到目标操作数中的指定位置。
     * @elseif en
     * @summary INS (Insert Bits) instruction node
     *
     * @desc Inserts bits into a specified position in the target operand.
     *
     * @example
     * ```typescript
     * // INS r0 r1 r2 r3
     * // 将 r1 的位插入到 r0 的指定位置
     * ```
     *
     * @public
     */
    export interface InsInstructionNode extends QuaternaryInstruction_RI_O_O_O {
        type: "insInstruction";
    }

    /**
     * @summary SAP（设置所有属性）指令节点
     *
     * @desc 设置设备的所有属性值。
     * @elseif en
     * @summary SAP (Set All Properties) instruction node
     *
     * @desc Sets all property values of a device.
     *
     * @example
     * ```typescript
     * // SAP r0 r1 r2 r3
     * // 设置设备的多个属性值
     * ```
     *
     * @public
     */
    export interface SapInstructionNode extends QuaternaryInstruction_RI_O_O_O {
        type: "sapInstruction";
    }

    /**
     * @summary SNA（设置数值所有属性）指令节点
     *
     * @desc 设置设备的数值类型所有属性。
     * @elseif en
     * @summary SNA (Set Numerical All) instruction node
     *
     * @desc Sets all numerical properties of a device.
     *
     * @example
     * ```typescript
     * // SNA r0 r1 r2 r3
     * // 设置设备的数值类型属性
     * ```
     *
     * @public
     */
    export interface SnaInstructionNode extends QuaternaryInstruction_RI_O_O_O {
        type: "snaInstruction";
    }

    /**
     * @summary SELECT（选择）指令节点
     *
     * @desc 根据条件从两个值中选择一个。
     * 如果第一个操作数非零，返回第二个操作数，否则返回第三个操作数。
     * @elseif en
     * @summary SELECT instruction node
     *
     * @desc Selects one of two values based on a condition.
     * If the first operand is non-zero, returns the second operand, otherwise returns the third operand.
     *
     * @example
     * ```typescript
     * // SELECT r0 r1 r2 r3
     * // 如果 r1 非零则 r0 = r2，否则 r0 = r3
     * ```
     *
     * @public
     */
    export interface SelectInstructionNode extends QuaternaryInstruction_RI_O_O_O {
        type: "selectInstruction";
    }

    /**
     * @summary 含 DeviceReference + SlotIndex + LogicSlotType + RegisterOrIdentifier 操作数的四元指令基类
     *
     * @desc 第一个操作数为设备引用，第二个为槽索引，第三个为逻辑槽类型，第四个为寄存器或标识符。
     * @elseif en
     * @summary Base interface for quaternary instructions with DeviceReference + SlotIndex + LogicSlotType + RegisterOrIdentifier operands
     *
     * @desc First operand is a device reference, second is a slot index, third is a logic slot type, fourth is a register or identifier.
     *
     * @public
     */
    interface QuaternaryInstruction_DR_SI_LS_RI extends ASTNode {
        operand1: DeviceReferenceNode;
        operand2: SlotIndexNode;
        operand3: LogicSlotTypeNode;
        operand4: RegisterOrIdentifierNode;
    }

    /**
     * @summary SS（设置设备槽）指令节点
     *
     * @desc 将值设置到设备的指定槽中。
     * @elseif en
     * @summary SS (Set Slot) instruction node
     *
     * @desc Sets a value to a specified slot of a device.
     *
     * @example
     * ```typescript
     * // SS console1 0 Color r0
     * // 将 r0 的值设置到 console1 的 0 号槽的 Color 属性
     * ```
     *
     * @public
     */
    export interface SsInstructionNode extends QuaternaryInstruction_DR_SI_LS_RI {
        type: "ssInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + Operand + LogicType + BatchMode 操作数的四元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为操作数，第三个为逻辑类型，第四个为批处理模式。
     * @elseif en
     * @summary Base interface for quaternary instructions with RegisterOrIdentifier + Operand + LogicType + BatchMode operands
     *
     * @desc First operand is a register or identifier, second is an operand, third is a logic type, fourth is a batch mode.
     *
     * @public
     */
    interface QuaternaryInstruction_RI_O_LT_BM extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
        operand3: LogicTypeNode;
        operand4: BatchModeNode;
    }

    /**
     * @summary LB（读取设备库存批量）指令节点
     *
     * @desc 从设备的库存中批量读取多个物品的数据。
     * @elseif en
     * @summary LB (Load Batch) instruction node
     *
     * @desc Batch reads data of multiple items from a device's inventory.
     *
     * @example
     * ```typescript
     * // LB r0 tank1 Mode Exact
     * // 从 tank1 批量读取数据
     * ```
     *
     * @public
     */
    export interface LbInstructionNode extends QuaternaryInstruction_RI_O_LT_BM {
        type: "lbInstruction";
    }

    /**
     * @summary 含 Operand + Operand + LogicType + RegisterOrIdentifier 操作数的四元指令基类
     *
     * @desc 前两个操作数为通用操作数，第三个为逻辑类型，第四个为寄存器或标识符。
     * @elseif en
     * @summary Base interface for quaternary instructions with Operand + Operand + LogicType + RegisterOrIdentifier operands
     *
     * @desc First two operands are general operands, third is a logic type, fourth is a register or identifier.
     *
     * @public
     */
    interface QuaternaryInstruction_O_O_LT_RI extends ASTNode {
        operand1: OperandNode;
        operand2: OperandNode;
        operand3: LogicTypeNode;
        operand4: RegisterOrIdentifierNode;
    }

    /**
     * @summary SBN（设置设备库存批量数值）指令节点
     *
     * @desc 从设备库存批量读取数值数据。
     * @elseif en
     * @summary SBN (Set Batch Numerical) instruction node
     *
     * @desc Batch reads numerical data from a device's inventory.
     *
     * @example
     * ```typescript
     * // SBN r0 r1 Mode r2
     * // 从设备库存批量读取数值数据到 r2
     * ```
     *
     * @public
     */
    export interface SbnInstructionNode extends QuaternaryInstruction_O_O_LT_RI {
        type: "sbnInstruction";
    }

    /**
     * @summary 含 Operand + SlotIndex + LogicSlotType + RegisterOrIdentifier 操作数的四元指令基类
     *
     * @desc 第一个操作数为通用操作数，第二个为槽索引，第三个为逻辑槽类型，第四个为寄存器或标识符。
     * @elseif en
     * @summary Base interface for quaternary instructions with Operand + SlotIndex + LogicSlotType + RegisterOrIdentifier operands
     *
     * @desc First operand is a general operand, second is a slot index, third is a logic slot type, fourth is a register or identifier.
     *
     * @public
     */
    interface QuaternaryInstruction_O_SI_LS_RI extends ASTNode {
        operand1: OperandNode;
        operand2: SlotIndexNode;
        operand3: LogicSlotTypeNode;
        operand4: RegisterOrIdentifierNode;
    }

    /**
     * @summary SBS（设置设备槽批量）指令节点
     *
     * @desc 批量将值设置到设备的多个槽中。
     * @elseif en
     * @summary SBS (Set Batch Slot) instruction node
     *
     * @desc Batch sets values to multiple slots of a device.
     *
     * @example
     * ```typescript
     * // SBS tank1 0 Quantity r0
     * // 批量设置设备的槽值
     * ```
     *
     * @public
     */
    export interface SbsInstructionNode extends QuaternaryInstruction_O_SI_LS_RI {
        type: "sbsInstruction";
    }

    /**
     * @summary 含 Operand + Operand + Operand + Operand 操作数的四元指令基类
     *
     * @desc 四个操作数都为通用操作数类型。
     * @elseif en
     * @summary Base interface for quaternary instructions with Operand + Operand + Operand + Operand operands
     *
     * @desc All four operands are general operand types.
     *
     * @public
     */
    interface QuaternaryInstruction_O_O_O_O extends ASTNode {
        operand1: OperandNode;
        operand2: OperandNode;
        operand3: OperandNode;
        operand4: OperandNode;
    }

    /**
     * @summary BAP（Branches on All Predicate）指令节点
     *
     * @desc 如果所有谓词条件都满足，则跳转到目标地址。
     * @elseif en
     * @summary BAP (Branches on All Predicate) instruction node
     *
     * @desc Jumps to the target address if all predicate conditions are satisfied.
     *
     * @example
     * ```typescript
     * // BAP loop r0 r1 r2
     * // 如果所有条件满足，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BapInstructionNode extends QuaternaryInstruction_O_O_O_O {
        type: "bapInstruction";
    }

    /**
     * @summary BAPAL（Branches on All Predicate And Link）指令节点
     *
     * @desc 如果所有谓词条件都满足，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BAPAL (Branches on All Predicate And Link) instruction node
     *
     * @desc Jumps to the target address if all predicate conditions are satisfied, and saves the return address.
     *
     * @example
     * ```typescript
     * // BAPAL loop r0 r1 r2
     * // 如果所有条件满足，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BapalInstructionNode extends QuaternaryInstruction_O_O_O_O {
        type: "bapalInstruction";
    }

    /**
     * @summary BNA（Branches on No predicate true）指令节点
     *
     * @desc 如果所有谓词条件都不满足，则跳转到目标地址。
     * @elseif en
     * @summary BNA (Branches on No predicate true) instruction node
     *
     * @desc Jumps to the target address if no predicate condition is satisfied.
     *
     * @example
     * ```typescript
     * // BNA loop r0 r1 r2
     * // 如果所有条件都不满足，跳转到 loop
     * ```
     *
     * @public
     */
    export interface BnaInstructionNode extends QuaternaryInstruction_O_O_O_O {
        type: "bnaInstruction";
    }

    /**
     * @summary BNAAL（Branches on No predicate true And Link）指令节点
     *
     * @desc 如果所有谓词条件都不满足，则跳转到目标地址并保存返回地址。
     * @elseif en
     * @summary BNAAL (Branches on No predicate true And Link) instruction node
     *
     * @desc Jumps to the target address if no predicate condition is satisfied, and saves the return address.
     *
     * @example
     * ```typescript
     * // BNAAL loop r0 r1 r2
     * // 如果所有条件都不满足，跳转到 loop 并保存返回地址
     * ```
     *
     * @public
     */
    export interface BnaalInstructionNode extends QuaternaryInstruction_O_O_O_O {
        type: "bnaalInstruction";
    }

    /**
     * @summary BRAP（Branches Register on All Predicate）指令节点
     *
     * @desc 如果所有谓词条件都满足，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRAP (Branches Register on All Predicate) instruction node
     *
     * @desc Jumps to the address stored in the register if all predicate conditions are satisfied.
     *
     * @example
     * ```typescript
     * // BRAP r0 r1 r2 r3
     * // 如果所有条件满足，跳转到 r0 存储的地址
     * ```
     *
     * @public
     */
    export interface BrapInstructionNode extends QuaternaryInstruction_O_O_O_O {
        type: "brapInstruction";
    }

    /**
     * @summary BRNA（Branches Register on No predicate true）指令节点
     *
     * @desc 如果所有谓词条件都不满足，则跳转到寄存器中存储的地址。
     * @elseif en
     * @summary BRNA (Branches Register on No predicate true) instruction node
     *
     * @desc Jumps to the address stored in the register if no predicate condition is satisfied.
     *
     * @example
     * ```typescript
     * // BRNA r0 r1 r2 r3
     * // 如果所有条件都不满足，跳转到 r0 存储的地址
     * ```
     *
     * @public
     */
    export interface BrnaInstructionNode extends QuaternaryInstruction_O_O_O_O {
        type: "brnaInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + DeviceReference + SlotIndex + LogicSlotType 操作数的四元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为槽索引，第四个为逻辑槽类型。
     * @elseif en
     * @summary Base interface for quaternary instructions with RegisterOrIdentifier + DeviceReference + SlotIndex + LogicSlotType operands
     *
     * @desc First operand is a register or identifier, second is a device reference, third is a slot index, fourth is a logic slot type.
     *
     * @public
     */
    interface QuaternaryInstruction_RI_DR_SI_LS extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: DeviceReferenceNode;
        operand3: SlotIndexNode;
        operand4: LogicSlotTypeNode;
    }

    /**
     * @summary LS（读取设备槽）指令节点
     *
     * @desc 从设备的指定槽中读取数据到寄存器。
     * @elseif en
     * @summary LS (Load Slot) instruction node
     *
     * @desc Reads data from a specified slot of a device into the register.
     *
     * @example
     * ```typescript
     * // LS r0 console1 0 Color
     * // 从 console1 的 0 号槽读取 Color 数据到 r0
     * ```
     *
     * @public
     */
    export interface LsInstructionNode extends QuaternaryInstruction_RI_DR_SI_LS {
        type: "lsInstruction";
    }

    /**
     * @summary 四元指令联合类型
     *
     * @desc 包含所有四元指令类型的联合，包括可能的错误节点。
     * @elseif en
     * @summary Quaternary instruction union type
     *
     * @desc Union of all quaternary instruction types, including possible error nodes.
     *
     * @public
     */
    export type QuaternaryInstructionNode =
        | LerpInstructionNode
        | ExtInstructionNode
        | InsInstructionNode
        | SapInstructionNode
        | SnaInstructionNode
        | SelectInstructionNode
        | SsInstructionNode
        | LbInstructionNode
        | SbnInstructionNode
        | SbsInstructionNode
        | BapInstructionNode
        | BapalInstructionNode
        | BnaInstructionNode
        | BnaalInstructionNode
        | BrapInstructionNode
        | BrnaInstructionNode
        | LsInstructionNode
        | ErrorNode;

    // 五元指令（5 个操作数）

    /**
     * @summary 五元指令概述
     *
     * @desc 五元指令是包含五个操作数的指令。
     * 根据操作数类型可分为以下几类：
     * - **QuinaryInstruction_RI_O_O_LT_BM**：寄存器/标识符 + 操作数 + 操作数 + 逻辑类型 + 批处理模式
     * - **QuinaryInstruction_RI_O_SI_LS_BM**：寄存器/标识符 + 操作数 + 槽索引 + 逻辑槽类型 + 批处理模式
     *
     * @elseif en
     * @summary Quinary Instructions Overview
     *
     * @desc Quinary instructions contain five operands.
     * They are categorized by operand types:
     * - **QuinaryInstruction_RI_O_O_LT_BM**: Register/identifier + operand + operand + logic type + batch mode
     * - **QuinaryInstruction_RI_O_SI_LS_BM**: Register/identifier + operand + slot index + logic slot type + batch mode
     */

    /**
     * @summary 含 RegisterOrIdentifier + Operand + Operand + LogicType + BatchMode 操作数的五元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二、三个为操作数，第四个为逻辑类型，第五个为批处理模式。
     * @elseif en
     * @summary Base interface for quinary instructions with RegisterOrIdentifier + Operand + Operand + LogicType + BatchMode operands
     *
     * @desc First operand is a register or identifier, second and third are operands, fourth is a logic type, fifth is a batch mode.
     *
     * @public
     */
    interface QuinaryInstruction_RI_O_O_LT_BM extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
        operand3: OperandNode;
        operand4: LogicTypeNode;
        operand5: BatchModeNode;
    }

    /**
     * @summary LBN（读取设备库存批量数值）指令节点
     *
     * @desc 从设备的库存中批量读取多个数值数据。
     * @elseif en
     * @summary LBN (Load Batch Numerical) instruction node
     *
     * @desc Batch reads multiple numerical data from a device's inventory.
     *
     * @example
     * ```typescript
     * // LBN r0 tank1 Water Oxygen Exact
     * // 从 tank1 批量读取 Water 和 Oxygen 的数值数据
     * ```
     *
     * @public
     */
    export interface LbnInstructionNode extends QuinaryInstruction_RI_O_O_LT_BM {
        type: "lbnInstruction";
    }

    /**
     * @summary 含 RegisterOrIdentifier + Operand + SlotIndex + LogicSlotType + BatchMode 操作数的五元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二个为操作数，第三个为槽索引，第四个为逻辑槽类型，第五个为批处理模式。
     * @elseif en
     * @summary Base interface for quinary instructions with RegisterOrIdentifier + Operand + SlotIndex + LogicSlotType + BatchMode operands
     *
     * @desc First operand is a register or identifier, second is an operand, third is a slot index, fourth is a logic slot type, fifth is a batch mode.
     *
     * @public
     */
    interface QuinaryInstruction_RI_O_SI_LS_BM extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
        operand3: SlotIndexNode;
        operand4: LogicSlotTypeNode;
        operand5: BatchModeNode;
    }

    /**
     * @summary LBS（读取设备库存批量槽）指令节点
     *
     * @desc 从设备的库存槽中批量读取数据。
     * @elseif en
     * @summary LBS (Load Batch Slot) instruction node
     *
     * @desc Batch reads data from a device's inventory slots.
     *
     * @example
     * ```typescript
     * // LBS r0 tank1 0 Quantity Exact
     * // 从 tank1 的槽批量读取数据
     * ```
     *
     * @public
     */
    export interface LbsInstructionNode extends QuinaryInstruction_RI_O_SI_LS_BM {
        type: "lbsInstruction";
    }

    /**
     * @summary 五元指令联合类型
     *
     * @desc 包含所有五元指令类型的联合，包括可能的错误节点。
     * @elseif en
     * @summary Quinary instruction union type
     *
     * @desc Union of all quinary instruction types, including possible error nodes.
     *
     * @public
     */
    export type QuinaryInstructionNode =
        | LbnInstructionNode
        | LbsInstructionNode
        | ErrorNode;

    // 六元指令（6 个操作数）

    /**
     * @summary 六元指令概述
     *
     * @desc 六元指令是包含六个操作数的指令。
     * 主要用于复杂的库存操作指令。
     *
     * @elseif en
     * @summary Senary Instructions Overview
     *
     * @desc Senary instructions contain six operands.
     * Mainly used for complex inventory operations.
     */

    /**
     * @summary 含 RegisterOrIdentifier + Operand + Operand + SlotIndex + LogicSlotType + BatchMode 操作数的六元指令基类
     *
     * @desc 第一个操作数为寄存器或标识符，第二、三个为操作数，第四个为槽索引，第五个为逻辑槽类型，第六个为批处理模式。
     * @elseif en
     * @summary Base interface for senary instructions with RegisterOrIdentifier + Operand + Operand + SlotIndex + LogicSlotType + BatchMode operands
     *
     * @desc First operand is a register or identifier, second and third are operands, fourth is a slot index, fifth is a logic slot type, sixth is a batch mode.
     *
     * @public
     */
    interface SenaryInstruction_RI_O_O_SI_LS_BM extends ASTNode {
        operand1: RegisterOrIdentifierNode;
        operand2: OperandNode;
        operand3: OperandNode;
        operand4: SlotIndexNode;
        operand5: LogicSlotTypeNode;
        operand6: BatchModeNode;
    }

    /**
     * @summary LBNS（读取设备库存批量数值槽）指令节点
     *
     * @desc 从设备的库存槽中批量读取多个数值数据。
     * @elseif en
     * @summary LBNS (Load Batch Numerical Slot) instruction node
     *
     * @desc Batch reads multiple numerical data from a device's inventory slots.
     *
     * @example
     * ```typescript
     * // LBNS r0 tank1 0 Water Oxygen Exact
     * // 从 tank1 的槽批量读取多个数值数据
     * ```
     *
     * @public
     */
    export interface LbnsInstructionNode extends SenaryInstruction_RI_O_O_SI_LS_BM {
        type: "lbnsInstruction";
    }

    /**
     * @summary 六元指令联合类型
     *
     * @desc 包含所有六元指令类型的联合，包括可能的错误节点。
     * @elseif en
     * @summary Senary instruction union type
     *
     * @desc Union of all senary instruction types, including possible error nodes.
     *
     * @public
     */
    export type SenaryInstructionNode =
        | LbnsInstructionNode
        | ErrorNode;

    // -------------------------------------------------------------------------
    // 顶层联合类型
    // -------------------------------------------------------------------------

    /**
     * @summary 可执行指令联合类型
     *
     * @desc 包含所有可执行指令类型的联合，包括零元、一元、二元、三元、四元、五元和六元指令。
     * 这是 IC10 程序中所有可执行指令的完整集合。
     *
     * @elseif en
     * @summary Executable instruction union type
     *
     * @desc Union of all executable instruction types, including nullary, unary, binary, ternary, quaternary, quinary, and senary instructions.
     * This is the complete set of all executable instructions in IC10 programs.
     *
     * @public
     */
    export type ExecutableInstructionNode =
        | NullaryInstructionNode
        | UnaryInstructionNode
        | BinaryInstructionNode
        | TernaryInstructionNode
        | QuaternaryInstructionNode
        | QuinaryInstructionNode
        | SenaryInstructionNode;

    /**
     * @summary 语句联合类型
     *
     * @desc 包含 IC10 程序中所有语句类型的联合：
     * - 可执行指令（{@link ExecutableInstructionNode}）
     * - 标签定义（{@link LabelDefNode}）
     * - 预处理指令（{@link PreprocessorDirectiveNode}）
     *
     * @elseif en
     * @summary Statement union type
     *
     * @desc Union of all statement types in IC10 programs:
     * - Executable instructions ({@link ExecutableInstructionNode})
     * - Label definitions ({@link LabelDefNode})
     * - Preprocessor directives ({@link PreprocessorDirectiveNode})
     *
     * @public
     */
    export type StatementNode =
        | ExecutableInstructionNode
        | LabelDefNode
        | PreprocessorDirectiveNode;

    // -------------------------------------------------------------------------
    // Program 类
    // -------------------------------------------------------------------------

    /**
     * @summary 程序节点类
     *
     * @desc 表示 IC10 程序的根节点。
     * 包含程序的所有语句和结束位置信息。
     *
     * @elseif en
     * @summary Program node class
     *
     * @desc Represents the root node of an IC10 program.
     * Contains all statements of the program and end position information.
     *
     * @example
     * ```typescript
     * // 解析 IC10 代码
     * const parser = new ic10.Parser(tokens);
     * const program = parser.parse();
     *
     * // 访问程序语句
     * console.log(`语句数量: ${program.statements.length}`);
     * ```
     *
     * @public
     */
    export class Program {
        /**
         * @summary 节点名称
         * @desc 通常为 "Program"
         */
        nodeName: string;

        /**
         * @summary 程序语句列表
         * @desc 包含程序中的所有语句（指令、标签、预处理指令等）
         */
        statements: StatementNode[];

        /**
         * @summary 结束位置
         * @desc 程序在源代码中的结束位置
         */
        end: Pos;

        /**
         * @summary 返回可读字符串表示
         *
         * @returns 格式化的程序字符串
         *
         * @desc 用于调试目的，返回人类可读的程序表示。
         */
        toString(): string;

        /**
         * @summary 返回 JSON 字符串表示
         *
         * @returns JSON 格式的程序表示
         *
         * @desc 返回包含 nodeName、statements 和 end 的 JSON 对象字符串。
         */
        toJSON(): string;
    }

    // -------------------------------------------------------------------------
    // Parser 类
    // -------------------------------------------------------------------------

    /**
     * @summary 语法分析器类
     *
     * @desc 将 Token 序列解析为 AST（抽象语法树）。
     * 支持调试模式，可以在解析过程中输出调试信息。
     *
     * @elseif en
     * @summary Parser class
     *
     * @desc Parses a sequence of Tokens into an AST (Abstract Syntax Tree).
     * Supports debug mode to output debug information during parsing.
     *
     * @example
     * ```typescript
     * // 创建语法分析器
     * const parser = new ic10.Parser(tokens, true);
     *
     * // 解析 tokens
     * const program = parser.parse();
     * console.log(JSON.parse(program.toJSON()));
     * ```
     *
     * @public
     */
    export class Parser {
        /**
         * @summary 构造函数
         *
         * @param tokens - Token 数组
         * @param debug - 是否启用调试模式（可选，默认为 false）
         *
         * @desc 创建语法分析器实例。
         * 调用 {@link parse} 方法执行实际的解析操作。
         */
        constructor(tokens: Token[], debug?: boolean);

        /**
         * @summary 解析 Token 序列
         *
         * @returns 程序节点
         *
         * @desc 执行实际的语法分析，将 Token 序列转换为 AST。
         */
        parse(): Program;

        /**
         * @summary 静态方法：解析 Token 序列
         *
         * @param tokens - Token 数组
         * @param debug - 是否启用调试模式（可选，默认为 false）
         * @returns 程序节点
         *
         * @desc 便捷方法，无需创建 Parser 实例即可解析。
         */
        parsing(tokens: Token[], debug?: boolean): Program;
    }

    // -------------------------------------------------------------------------
    // SymbolTable 类
    // -------------------------------------------------------------------------

    /**
     * @summary 符号表类
     *
     * @desc 存储 IC10 程序中的符号信息（如标签、别名定义等）。
     * 用于静态分析和代码验证。
     *
     * @elseif en
     * @summary Symbol table class
     *
     * @desc Stores symbol information in IC10 programs (such as labels, alias definitions, etc.).
     * Used for static analysis and code validation.
     *
     * @example
     * ```typescript
     * // 执行静态分析
     * ic10.Analyser.analyse(program);
     *
     * // 获取符号表
     * const symbolTable = analyser.symbolTable;
     * console.log(JSON.parse(symbolTable.toJSON()));
     * ```
     *
     * @public
     */
    export class SymbolTable {
        /**
         * @summary 返回 JSON 字符串表示
         *
         * @returns JSON 格式的符号表表示
         *
         * @desc 返回包含所有符号信息的 JSON 对象字符串。
         */
        toJSON(): string;
    }

    // -------------------------------------------------------------------------
    // Analyser 类
    // -------------------------------------------------------------------------

    /**
     * @summary 静态分析器类
     *
     * @desc 对 IC10 程序进行静态分析，包括：
     * - 符号表构建
     * - 语义检查
     * - 类型检查
     * - 错误检测
     *
     * @elseif en
     * @summary Static analyser class
     *
     * @desc Performs static analysis of IC10 programs, including:
     * - Symbol table construction
     * - Semantic checking
     * - Type checking
     * - Error detection
     *
     * @example
     * ```typescript
     * // 创建分析器并分析程序
     * const analyser = new ic10.Analyser();
     * analyser.visit(program);
     *
     * // 检查是否有错误
     * if (analyser.errors.length > 0) {
     *     console.log('分析错误:', analyser.errors);
     * }
     *
     * // 获取符号表
     * const symbolTable = analyser.symbolTable;
     * ```
     *
     * @public
     */
    export class Analyser {
        /**
         * @summary 符号表
         *
         * @desc 分析过程中构建的符号表，包含所有定义的符号信息。
         */
        get symbolTable(): SymbolTable;

        /**
         * @summary 错误列表
         *
         * @desc 分析过程中检测到的错误。
         */
        get errors(): Error[];

        /**
         * @summary 静态方法：分析程序
         *
         * @param program - 要分析的 Program 节点
         *
         * @desc 便捷方法，直接分析程序而不需要创建 Analyser 实例。
         */
        static analyse(program: Program): void;

        /**
         * @summary 访问程序节点
         *
         * @param program - 要访问的 Program 节点
         *
         * @desc 执行实际的静态分析，更新符号表和错误列表。
         */
        visit(program: Program): void;
    }

}


