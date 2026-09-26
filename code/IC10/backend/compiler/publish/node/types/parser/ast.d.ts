// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 抽象语法树（AST）节点的类型声明：节点基类 {@link ASTNode}、各类字面量与操作数节点、
 *       指令节点，以及 `#>` 注解节点。所有节点都带 `nodeName` 与位置字段，并可 `toJSON()` 序列化。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Position, IC10Utils} from "../common";
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
    /**
     * Register | Identifier
     * r? : 目标寄存器（写入结果），必须是纯寄存器
     * 寄存器目标（寄存器或标识符）
     * */
    REG_TARGET = 0,
    /** 寄存器或设备（alias 专用） */
    REG_OR_DEV,
    /**
     * Register | Identifier | Enum | [Number]
     * r?|num : 通用数值操作数（读取值参与运算/比较/赋值）
     * 数值（数字、寄存器、标识符、枚举）
     * */
    NUM_VALUE,
    /**
     * Identifier | Register | [Number]
     * int, r?|num : （跳转指令中）跳转目标行号（绝对或相对），需解析为整数行号。
     * 跳转目标（数字、寄存器、标识符）
     * */
    JUMP_LINE,
    /**
     * Register | Number（get/put/poke 的堆栈地址索引）
     * address(r?|num) : 堆栈地址索引（用于 get/put/poke）
     * 地址（数字、寄存器、标识符、枚举）
     * */
    ADDRESS,
    /**
     * [Number]
     * slotIdx : 设备槽位索引（需为非负整数）
     * 槽索引（数字、寄存器、标识符、枚举）
     * */
    SLOT_IDX,
    /**
     * Register | Number（clrd 的设备硬件 ID）
     * id(r?|num) | （clrd/getd 等）设备数字硬件 ID（非端口名）
     * 硬件 ID（数字、寄存器、标识符、枚举）
     * */
    HARDWARE_ID,
    /**
     * Register | Number（rmap 的试剂哈希）
     * reagentHash(r?|num) : 试剂材料哈希值（用于配方查询）
     * 试剂哈希（数字、寄存器、标识符、枚举）
     * */
    REAGENT_HASH,
    /**
     * Device | Register | Identifier
     * device(d?|r?|id), d? : 设备引用（可为静态/动态端口或设备别名）
     * 设备引用（设备或标识符）
     * */
    DEVICE_REF,
    /**
     * Device（clr/rmap 的 d?）
     * d? : （clr 指令独有）严格设备端口（不接受别名或动态设备）
     * 严格设备引用（仅设备）
     * */
    DEVICE_REF_STRICT,
    /**
     * Identifier | [Number]
     * logicType : 设备逻辑属性路径（仅单标识符，如 Pressure、Activate）
     * 逻辑属性（标识符）
     * */
    LOGIC_PROP,
    /**
     * Identifier | [Number]
     * logicSlotType : 槽位物品逻辑属性路径（仅单标识符）
     * 逻辑槽属性（标识符）
     * */
    LOGIC_SLOT_PROP,
    /**
     * Identifier | [Number]
     * batchMode : 聚合模式（如 Average、Sum 或枚举常量）
     * 批处理模式（数字、标识符、枚举）
     * */
    AGG_MODE,
    /**
     * Identifier | [Number]
     * reagentMode : 试剂模式（如 Contents、Recipe）
     * 试剂模式（数字、标识符、枚举）
     * */
    REAGENT_MODE,
    /**
     * Number | Register（lb/lbn/lbs/lbns/sb/sbn/sbs 的设备类型哈希）
     * deviceHash : 设备类型哈希值（支持字面量、寄存器、常量别名、HASH 宏）
     * 设备哈希（数字、寄存器、标识符、HASH 宏）
     * */
    DEVICE_HASH,
    /**
     * Number | Register（lbn/lbns/sbn 的设备名称哈希）
     * nameHash : 设备名称哈希值（同 deviceHash，可用寄存器传递）
     * 名称哈希（数字、寄存器、标识符、HASH 宏）
     * */
    NAME_HASH,
    /** 常量数值（仅数字） */
    CONST_NUM
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
    readonly nodeName: string;

    /**
     * @summary 节点位置
     * @desc 节点在源代码中的起始位置
     */
    readonly position: Position;

    /**
     * @summary 节点结束位置
     * @desc 节点在源代码中的结束位置
     */
    readonly end: Position;
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
 * - **宏调用**：{@link HashMacroNode}、{@link StrMacroNode}
 * - **错误节点**：{@link ErrorNode}
 *
 * @see {@link NumberNode} - 数值字面量的联合类型
 * @see {@link OperandNode} - 操作数类型的联合类型
 */

export type Errorable<T> = T | ErrorNode;

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
    readonly nodeName: "Integer";

    /**
     * @summary 整数值
     * @desc 十进制整数，可正可负
     */
    readonly value: number;
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
    readonly nodeName: "Float";

    /**
     * @summary 浮点数值
     * @desc 十进制浮点数
     */
    readonly value: number;
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
    readonly nodeName: "HexNumber";

    /**
     * @summary 十六进制字符串
     * @desc 包含前缀的十六进制字符串，如 "0xFF"
     */
    readonly value: string;
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
    readonly nodeName: "BinaryNumber";

    /**
     * @summary 二进制字符串
     * @desc 包含前缀的二进制字符串，如 "0b1010"
     */
    readonly value: string;
}

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
export type Number = Errorable<IntegerNode | FloatNode | HexNumberNode | BinaryNumberNode>;

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
    readonly nodeName: "Identifier";

    /**
     * @summary 标识符名称
     * @desc 标识符的字符串名称
     */
    readonly value: string;
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
    readonly nodeName: "String";

    /**
     * @summary 字符串内容
     * @desc 包括引号的完整字符串值
     */
    readonly value: string;
}


/**
 * @summary 静态寄存器节点基类
 *
 * @desc 形如 `r0`、`ra`、`sp` 这类**拼写上写死**的寄存器。四种寄存器共享同一 FIRST 集，
 *       因此编译器在语法阶段还要靠额外的形状判断才能区分，这里用泛型参数 `Name` 表达具体种类。
 *
 * @typeParam Name - 节点的 `nodeName`
 */
interface StaticRegisterBase<Name extends string> extends ASTNode {
    readonly nodeName: Name;

    /** 寄存器拼写（如 `"r0"`、`"sp"`） */
    readonly value: string;
}

/** @summary 通用寄存器节点（`r0`-`r15`） */
export type GeneralPurposeRegisterNode = StaticRegisterBase<"GeneralPurposeRegister">;

/** @summary 地址寄存器节点（`ra`） */
export type AddressRegisterNode = StaticRegisterBase<"AddressRegister">;

/** @summary 栈指针寄存器节点（`sp`） */
export type StackPointerRegisterNode = StaticRegisterBase<"StackPointerRegister">;

/** @summary 特殊寄存器：地址寄存器或栈指针寄存器，失败时为 `ErrorNode` */
export type SpecialRegisterNode = Errorable<AddressRegisterNode | StackPointerRegisterNode>;

/** @summary 静态寄存器：特殊寄存器或通用寄存器，失败时为 `ErrorNode` */
export type StaticRegisterNode = Errorable<SpecialRegisterNode | GeneralPurposeRegisterNode>;

/**
 * @summary 动态寄存器节点
 *
 * @desc 寄存器由另一个寄存器或标识符指定（间接寻址）。正因为要**再判断一次**内部寄存器种类，
 *       它的 `parse` 不能是 `noexcept`。
 */
export interface DynamicRegisterNode extends ASTNode {
    readonly nodeName: "DynamicRegister";

    /** 指示目标寄存器的操作数 */
    readonly register: Errorable<DynamicRegisterNode | GeneralPurposeRegisterNode | AddressRegisterNode>;
}

/** @summary 寄存器联合类型：静态寄存器或动态寄存器，失败时为 `ErrorNode` */
export type Register = Errorable<StaticRegisterNode | DynamicRegisterNode>;

/**
 * @summary 静态设备节点基类
 *
 * @desc 形如 `d0`、`db` 这类拼写上写死的设备。与静态寄存器同理，靠泛型参数区分具体种类。
 *
 * @typeParam T - 节点的 `nodeName`
 */
interface StaticDeviceBase<T extends string> extends ASTNode {
    readonly nodeName: T;

    /** 设备拼写（如 `"d0"`、`"db"`） */
    readonly value: string;
}

/** @summary 自引用设备节点（`db`，即 IC10 自身所在的设备） */
export type SelfReferenceDeviceNode = StaticDeviceBase<"SelfReferenceDevice">;

/** @summary 普通设备端口节点（`d0`-`d5`） */
export type OrdinaryDeviceNode = StaticDeviceBase<"OrdinaryDevice">;

/**
 * @summary 静态设备节点
 *
 * @desc 由具体端口或自引用设备构成，可带一个引脚号（`d0:1` 形式）。
 */
export interface StaticDeviceNode extends ASTNode {
    readonly nodeName: "StaticDevice";

    /** 端口本身 */
    readonly device: Errorable<SelfReferenceDeviceNode | OrdinaryDeviceNode>;

    /** 可选引脚号（`d0:1` 中的 `1`） */
    readonly pin: IC10Utils.Optional<IntegerNode>;
}

/**
 * @summary 动态设备节点
 *
 * @desc 设备由寄存器或标识符指定（间接寻址），同样需要在解析时再判断一次，故非 `noexcept`。
 */
export interface DynamicDeviceNode extends ASTNode {
    readonly nodeName: "DynamicDevice";

    /** 指示目标设备的寄存器 */
    readonly register: Errorable<DynamicRegisterNode | GeneralPurposeRegisterNode | AddressRegisterNode>;
}

/** @summary 设备联合类型：静态设备或动态设备，失败时为 `ErrorNode` */
export type Device = Errorable<StaticDeviceNode | DynamicDeviceNode>;

/**
 * @summary 枚举引用节点
 *
 * @desc 形如 `Foo.Bar` 的枚举操作数：`name` 为枚举类型名，`value` 为成员名。
 *       语义阶段目前不做校验，仅保证它不会落进泛型兜底而误报 IEA6。
 */
export interface EnumNode extends ASTNode {
    readonly nodeName: "Enum";

    /** 枚举类型名（`Foo.Bar` 中的 `Foo`） */
    readonly name: Errorable<IdentifierNode>;

    /** 枚举成员名（`Foo.Bar` 中的 `Bar`） */
    readonly value: Errorable<IdentifierNode>;
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
export interface HashMacroNode extends ASTNode {
    readonly nodeName: "HashMacro";

    /**
     * @summary 参数字符串
     * @desc 要计算哈希的字符串参数
     */
    readonly value: Errorable<StringNode>;
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
export interface StrMacroNode extends ASTNode {
    readonly nodeName: "StrMacro";

    /**
     * @summary 参数字符串
     * @desc 要获取长度的字符串参数
     */
    readonly value: Errorable<StringNode>;
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
    readonly nodeName: "Error";

    /**
     * @summary 导致错误的 Token
     * @desc 触发错误的源 Token，用于定位错误位置
     */
    readonly token: Token;

    /**
     * @summary 错误消息
     * @desc 描述错误原因的文本
     */
    readonly message: string;
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
 * @summary 宏调用联合类型
 *
 * @desc 表示宏调用的可能类型，包括 HASH 宏和 STR 宏。
 *
 * @public
 */
export type Macro = Errorable<HashMacroNode | StrMacroNode>;

/** @summary 写入目标：寄存器或标识符（`OperandType.REG_TARGET` 的候选集合） */
export type RegTarget = Errorable<Register | IdentifierNode>;

/**
 * @summary 寄存器或设备联合类型
 *
 * @desc 表示可以作为寄存器或设备的位置，**仅供 `alias` 指令使用**：
 * `alias` 的作用就是把一个寄存器或端口绑定到名字上，因此这里不允许标识符。
 *
 * @public
 */
export type RegOrDev = Errorable<Register | Device>;

/** @summary 通用数值操作数：数字、寄存器、标识符或枚举（`NUM_VALUE`） */
export type NumValue = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** @summary 跳转目标：数字行号、寄存器或标识符（标签），**不含枚举**（`JUMP_LINE`） */
export type JumpLine = Errorable<Number | Register | IdentifierNode>;

/** @summary 堆栈地址索引：数字、寄存器、标识符或枚举（`ADDRESS`） */
export type Address = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** 槽索引 */
export type SlotIdx = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** 硬件 ID */
export type HardwareId = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** 试剂哈希 */
export type ReagentHash = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** 设备引用（允许标识符） */
export type DeviceRef = Errorable<Device | IdentifierNode>;

/** 严格设备引用（仅设备） */
export type DeviceRefStrict = Errorable<Device>;

/** 逻辑属性（兼容旧语法允许数字） */
export type LogicProp = Errorable<IdentifierNode | Number>;

/** 逻辑槽属性（兼容旧语法允许数字） */
export type LogicSlotProp = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** 批处理模式 */
export type AggMode = Errorable<Number | IdentifierNode | EnumNode>;

/** 试剂模式 */
export type ReagentMode = Errorable<Number | Register | IdentifierNode | EnumNode>;

/** 设备哈希（允许 HASH 宏） */
export type DeviceHash = Errorable<Number | IdentifierNode | HashMacroNode>;

/** 名称哈希（允许 STR 宏） */
export type NameHash = Errorable<Number | IdentifierNode | StrMacroNode>;

/** 别名定义（用于预处理指令） */
export type AliasDef = Errorable<IdentifierNode>;

/** 常量定义（用于预处理指令） */
export type ConstDef = Errorable<IdentifierNode>;

/** 常量数值（允许宏） */
export type ConstNum = Errorable<Number | Macro>;

/**
 * @summary 操作数联合类型
 *
 * @desc 表示指令中操作数可能的所有类型。
 * 操作数是指令的参数，可以是：
 * - 寄存器（r0-r17）
 * - 设备引用
 * - 数值（整数、浮点、十六进制、二进制）
 * - 标识符
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
export type Operand = Register | Device | Number | IdentifierNode | EnumNode | Macro;

// -------------------------------------------------------------------------
// 预处理指令节点
// -------------------------------------------------------------------------

/**
 * @summary 链接路径节点
 *
 * @desc 表示 `#:` 类型提示里的链接式描述（如 `.Device.Sensor.Pressure`），
 *       由 `paths`（`/` 分隔的路径段）与 `fields`（`.` 分隔的字段）两段组成。
 */
export interface LinkNode extends ASTNode {
    readonly nodeName: "Link";

    /** 路径段（`/` 分隔部分） */
    readonly paths: string[];

    /** 字段（`.` 分隔部分） */
    readonly fields: string[];
}

/** @summary 描述：链接描述或纯文本字符串，失败时为 `ErrorNode` */
export type Description = Errorable<StringNode | LinkNode>;

/**
 * @summary 类型提示节点
 *
 * @desc 由 `#: @type ... @desc ... @builtin` 类型提示解析而来。
 * 对应 C++ `ic10::TypeHint`。
 */
export interface TypeHintNode extends ASTNode {
    /**
     * @summary 类型名（可选）
     * @desc 由 @type 注解指定的类型名
     */
    readonly type: IC10Utils.Optional<string>;

    /**
     * @summary 描述（可选）
     * @desc 由 @desc 注解指定的描述（链接或文本）
     */
    readonly desc: IC10Utils.Optional<Description>;

    /**
     * @summary 内置常量标记（可选）
     * @desc 由 @builtin 注解标记，用于将 define 字符串解析为预定义常量
     */
    readonly builtin: boolean;
}

/**
 * @summary `alias` 指令节点
 *
 * @desc 形如 `alias disp d0`：把名字绑定到寄存器或设备端口上，可选带 `#:` 类型提示。
 */
export interface AliasDirectiveNode extends ASTNode {
    readonly nodeName: "AliasDirective";

    /**
     * @summary 别名标识符
     * @desc 别名的名称，用于在后续代码中引用
     */
    readonly identifier: AliasDef;

    /**
     * @summary 被绑定的寄存器或设备
     * @desc 实际的目标，可以是寄存器或设备
     */
    readonly registerOrDevice: RegOrDev;

    /**
     * @summary 类型提示（可选）
     * @desc 由 `#: @type ... @desc ... @builtin` 类型提示解析而来
     */
    readonly typeHint: IC10Utils.Optional<TypeHintNode>;
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
    readonly nodeName: "DefineDirective";

    /**
     * @summary 常量标识符
     * @desc 常量的名称
     */
    readonly identifier: ConstDef;

    /**
     * @summary 常量数值
     * @desc 仅接受数值字面量（Integer/Float/HexNumber/BinaryNumber），
     *       或经 @builtin 由字符串解析得到的浮点常量
     */
    readonly operand: ConstNum;

    /**
     * @summary 类型提示（可选）
     * @desc 由 `#: @type ... @desc ... @builtin` 类型提示解析而来
     */
    readonly typeHint: IC10Utils.Optional<TypeHintNode>;
}


/**
 * @summary 预处理指令联合类型
 *
 * @desc 表示所有预处理指令的可能类型。
 *
 * @public
 */
export type PreprocessorDirective = Errorable<
    AliasDirectiveNode
    | DefineDirectiveNode
>;

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
    readonly nodeName: "LabelDef";
    /**
     * @summary 标签标识符
     * @desc 标签的名称，用于作为跳转目标
     */
    readonly identifier: IdentifierNode;
}


/**
 * @summary 零元指令节点（无操作数，如 `yield`、`hfc`）
 *
 * @desc 指令节点族的基类：`nodeName` 由关键字与 `"Instruction"` 拼成（如 `yield` →
 *       `"yieldInstruction"`），`keyword` 保留原始关键字拼写。
 *       一元到六元节点依次在此基础上**逐个追加** `operandN` / `typeN`。
 */
export interface NullaryInstructionNode extends ASTNode {
    readonly nodeName: `${string}Instruction`;

    /** 指令关键字拼写（如 `"move"`） */
    readonly keyword: string;
}

/** @summary 一元指令节点（1 个操作数） */
export interface UnaryInstructionNode extends NullaryInstructionNode {
    /** 第 1 个操作数 */
    readonly operand1: Operand;

    /** 第 1 个操作数的语义类型（`OperandType` 枚举值） */
    readonly type1: OperandType;
}

/** @summary 二元指令节点（2 个操作数） */
export interface BinaryInstructionNode extends UnaryInstructionNode {
    /** 第 2 个操作数 */
    readonly operand2: Operand;

    /** 第 2 个操作数的语义类型 */
    readonly type2: OperandType;
}

/** @summary 三元指令节点（3 个操作数） */
export interface TernaryInstructionNode extends BinaryInstructionNode {
    /** 第 3 个操作数 */
    readonly operand3: Operand;

    /** 第 3 个操作数的语义类型 */
    readonly type3: OperandType;
}

/** @summary 四元指令节点（4 个操作数） */
export interface QuaternaryInstructionNode extends TernaryInstructionNode {
    /** 第 4 个操作数 */
    readonly operand4: Operand;

    /** 第 4 个操作数的语义类型 */
    readonly type4: OperandType;
}

/** @summary 五元指令节点（5 个操作数） */
export interface QuinaryInstructionNode extends QuaternaryInstructionNode {
    /** 第 5 个操作数 */
    readonly operand5: Operand;

    /** 第 5 个操作数的语义类型 */
    readonly type5: OperandType;
}

/** @summary 六元指令节点（6 个操作数） */
export interface SenaryInstructionNode extends QuinaryInstructionNode {
    /** 第 6 个操作数 */
    readonly operand6: Operand;

    /** 第 6 个操作数的语义类型 */
    readonly type6: OperandType;
}

/**
 * @summary 可执行指令联合类型
 *
 * @desc 包含所有可执行指令类型的联合，包括零元、一元、二元、三元、四元、五元和六元指令。
 * 这是 IC10 程序中所有可执行指令的完整集合。
 *
 * @remarks
 * **English:** the union of every executable instruction type — nullary through senary — i.e. the
 * complete set of executable instructions in an IC10 program.
 *
 * @public
 */
export type ExecutableInstruction = Errorable<NullaryInstructionNode | UnaryInstructionNode | BinaryInstructionNode | TernaryInstructionNode | QuaternaryInstructionNode | QuinaryInstructionNode | SenaryInstructionNode>;


/**
 * @summary 枚举注解的成员行
 *
 * @desc 对应 `#> @value Name 0 描述` 一行：`name` 是成员名，`value` 是它的数值，`desc` 为可选描述。
 */
export interface EnumAnnotationValue extends ASTNode {
    readonly nodeName: "EnumAnnotationValue";

    /** 标签名，固定为 `"value"` */
    readonly tag: "value";

    /** 成员名（代码中书写的形式） */
    readonly name: string;

    /** 成员数值（以字符串保存） */
    readonly value: string;

    /** 可选的描述 */
    readonly desc: IC10Utils.Optional<Description>;
}

/** 枚举注解（对应 C++ `ic10::EnumAnnotation`，由 `#> @enum ... #> @end-enum` 定义） */
export interface EnumAnnotation extends ASTNode {
    readonly nodeName: "EnumAnnotation";

    readonly name: string;

    readonly desc: IC10Utils.Optional<Description>;

    readonly values: EnumAnnotationValue[];
}

/**
 * @summary 只有「值」的注解行基类
 *
 * @desc 用于 `@device-hash` / `@name-hash` / `@reagent-hash` 这类只有取值的标签行。
 *
 * @typeParam N - 节点的 `nodeName`
 * @typeParam T - 标签字面量（如 `"name-hash"`）
 */
interface TypeAnnotationValueBase<N extends string, T extends string> extends ASTNode {
    readonly nodeName: N;

    readonly tag: T;

    /** 取值（以字符串保存） */
    readonly value: string;
}

/**
 * @summary 带名字的注解行基类
 *
 * @desc 用于 `@logic` / `@logic-slot` / `@slot` 这类「名字 + 取值（+ 描述）」的标签行。
 *
 * @typeParam N - 节点的 `nodeName`
 * @typeParam T - 标签字面量（如 `"logic"`）
 */
interface TypeAnnotationLineBase<N extends string, T extends string> extends ASTNode {
    readonly nodeName: N;

    readonly tag: T;

    /** 属性名（如逻辑属性名 `Pressure`） */
    readonly name: string;

    /** 取值（如引脚或槽位序号） */
    readonly value: string;

    /** 可选的描述 */
    readonly desc: IC10Utils.Optional<Description>;
}

/** @summary `@logic` 行：设备的一条逻辑属性 */
export type DeviceAnnotationLogic = TypeAnnotationLineBase<"DeviceAnnotationLogic", "logic">;

/** @summary `@logic-slot` 行：槽位物品的一条逻辑属性 */
export type DeviceAnnotationLogicSlot = TypeAnnotationLineBase<"DeviceAnnotationLogicSlot", "logic-slot">;

/** @summary `@slot` 行：设备的一个槽位 */
export type DeviceAnnotationSlot = TypeAnnotationLineBase<"DeviceAnnotationSlot", "slot">;

/** @summary `@device-hash` 行：设备类型哈希 */
export type DeviceAnnotationDeviceHash = TypeAnnotationValueBase<"DeviceAnnotationDeviceHash", "device-hash">;

/** @summary `@name-hash` 行：设备名称哈希 */
export type DeviceAnnotationNameHash = TypeAnnotationValueBase<"DeviceAnnotationNameHash", "name-hash">;

/** @summary `@reagent-hash` 行：试剂哈希 */
export type DeviceAnnotationReagentHash = TypeAnnotationValueBase<"DeviceAnnotationReagentHash", "reagent-hash">;

/**
 * @summary 设备注解节点
 *
 * @desc 对应 `#> @device ... #> @end-device` 整块，是别名类型判定的数据来源：
 *       语义阶段用 `logics` / `logicSlots` / `slots` 判断该设备的逻辑属性与槽位是否合法。
 */
export interface DeviceAnnotation extends ASTNode {
    readonly nodeName: "DeviceAnnotation";

    /** 设备类型名 */
    readonly name: string;

    /** 可选的类型描述 */
    readonly desc: IC10Utils.Optional<Description>;

    /** 可选的设备类型哈希行 */
    readonly deviceHash: IC10Utils.Optional<DeviceAnnotationDeviceHash>;

    /** 可选的设备名称哈希行 */
    readonly nameHash: IC10Utils.Optional<DeviceAnnotationNameHash>;

    /** 逻辑属性行（可多条） */
    readonly logics: DeviceAnnotationLogic[];

    /** 槽位逻辑属性行（可多条） */
    readonly logicSlots: DeviceAnnotationLogicSlot[];

    /** 槽位行（可多条） */
    readonly slots: DeviceAnnotationSlot[];

    /** 试剂哈希行（可多条） */
    readonly reagentHash: DeviceAnnotationReagentHash[];
}

/** @summary 类型注解：设备注解或枚举注解，失败时为 `ErrorNode` */
export type TypeAnnotation = Errorable<DeviceAnnotation | EnumAnnotation>;