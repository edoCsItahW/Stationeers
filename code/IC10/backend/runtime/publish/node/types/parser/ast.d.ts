// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Position} from "../common";
import {Token} from "../lexer";


/**
 * IC10 操作数类型枚举。
 *
 * 对应 C++ `ic10::OperandType`，值从 0 开始递增。
 * 运行时由原生模块通过 `enum_to_str` 编译期反射导出，
 * 用于 AST JSON 序列化中 `type1`/`type2`/... 字段的数值表示。
 *
 * @public
 */
export enum OperandType {
    /** 寄存器或标识符 */
    REG_IDENT = 0,
    /** 设备别名引用 */
    DEV_ALIAS,
    /** 寄存器或数字 */
    REG_NUM,
    /** 设备引用 */
    DEV_REF,
    /** 逻辑槽类型 */
    LOGIC_SLOT,
    /** 试剂模式 */
    REAGENT_MODE,
    /** 跳转目标 */
    JUMP_TARGET,
    /** 逻辑类型 */
    LOGIC_TYPE,
    /** 槽索引 */
    SLOT_IDX,
    /** 批处理模式 */
    BATCH_MODE
}


// =========================================================================
// AST 节点类型定义
// AST 节点通过 toJSON() 序列化为普通 JavaScript 对象
// 每个节点都包含 type 和 position 字段
// =========================================================================

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

    /**
    * @summary 节点结束位置
    * @desc 节点在源代码中的结束位置
    */
    end: Position;
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
    /**
     * @summary 类型名（可选）
     * @desc 由 @type 注解指定的设备类型名
     */
    typeName?: string;
    /**
     * @summary 描述（可选）
     * @desc 由 @desc 注解指定的描述（与 typeName 互斥）
     */
    desc?: string;
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
    /**
     * @summary 类型名（可选）
     * @desc 由 @type 注解指定的类型名
     */
    typeName?: string;
    /**
     * @summary 描述（可选）
     * @desc 由 @desc 注解指定的描述（与 typeName 互斥）
     */
    desc?: string;
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


export interface InstructionNode extends ASTNode {
    keyword: string;
}
