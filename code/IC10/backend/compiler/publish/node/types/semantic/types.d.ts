// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 语义层的类型设施：基础类型 {@link BasicType}、类型类别 {@link TypeCategory}、
 *       节点类型映射表 {@link TypeOfNode} 与自定义类型表 {@link TypeTable}。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { DeviceAnnotation, EnumAnnotation } from '../parser';


/**
 * @summary 基础类型枚举
 *
 * @desc 定义 IC10 语言支持的基础数据类型。
 *
 * @public
 */
export enum BasicType {
    /** 字符串类型 */
    STRING = 0,
    /** 整数类型 */
    INTEGER,
    /** 浮点类型 */
    FLOAT,
    /** 寄存器类型（`r0`-`r15`、`ra`、`sp`） */
    REGISTER,
    /** 设备类型（`d0`-`d5` 端口或自引用设备 `db`） */
    DEVICE,
    /** 未知类型（解析错误） */
    UNKNOWN,
    /** 枚举类型 */
    ENUM
}


/**
 * @summary 类型类别枚举
 *
 * @desc 定义类型的语义类别，用于区分不同语义用途的类型。
 *
 * @public
 */
export enum TypeCategory {
    /** 标签类别 */
    LABEL = 0,
    /** STR宏调用类别 */
    STR_CALL,
    /** HASH宏调用类别 */
    HASH_CALL,
    /** 常量类别 */
    CONSTANT,
    /** 数字类别 */
    NUMBER,
    /** 基础类别 */
    BASIC
}


/**
 * @summary 节点类型映射接口
 *
 * @desc 定义 AST 节点类型名到 BasicType 和 TypeCategory 的映射。
 *
 * @public
 */
export interface TypeOfNodeEntry {
    /** 基础类型 */
    kind: BasicType;
    /** 类型类别 */
    category: TypeCategory;
}


/**
 * @summary 节点类型映射
 *
 * @desc 将节点类型名映射到对应的类型信息，共 16 项。键名即 AST 节点的 `nodeName`
 *       （注意最后一项的键是 `Error`，对应 `ErrorNode` 的类型）。
 *
 * @public
 */
export const TypeOfNode: {
    Integer: TypeOfNodeEntry;
    Float: TypeOfNodeEntry;
    String: TypeOfNodeEntry;
    Enum: TypeOfNodeEntry;
    StrMacro: TypeOfNodeEntry;
    HashMacro: TypeOfNodeEntry;
    DynamicRegister: TypeOfNodeEntry;
    AddressRegister: TypeOfNodeEntry;
    StackPointerRegister: TypeOfNodeEntry;
    GeneralPurposeRegister: TypeOfNodeEntry;
    DynamicDevice: TypeOfNodeEntry;
    StaticDevice: TypeOfNodeEntry;
    LabelDef: TypeOfNodeEntry;
    Error: TypeOfNodeEntry;
    BinaryNumber: TypeOfNodeEntry;
    HexNumber: TypeOfNodeEntry;
};


/**
 * @summary 类型表类
 *
 * @desc 管理 IC10 程序中的自定义类型定义（设备类型和枚举类型）。
 * 支持 JSON 序列化。
 *
 * @remarks
 * **English:** manages the custom type definitions of an IC10 program — device and enum types declared
 * in `#>` annotation blocks — and serializes them to JSON.
 *
 * @example
 * ```typescript
 * const analyser = new ic10.Analyser();
 * await analyser.visit(program);
 * const typeTable = analyser.typeTable;
 * console.log(JSON.parse(typeTable.toJSON()));
 * ```
 *
 * @public
 */
export class TypeTable {
    /**
     * @summary 返回 JSON 字符串表示
     *
     * @returns JSON 格式的类型表表示
     *
     * @desc 返回包含所有自定义类型信息的 JSON 对象字符串。
     */
    toJSON(): string;
}


/** 自定义类型：设备注解或枚举注解 */
export type CustomType = DeviceAnnotation | EnumAnnotation;

/** 类型表 JSON 映射 */
export interface TypeTableMap {
    [key: string]: CustomType;
}
