// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 17:13
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

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
    /** 寄存器类型 (r0-r15) */
    REGISTER,
    /** 设备类型 (@开头) */
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
 * @summary AST 节点类型映射
 *
 * @desc 将节点类型名映射到对应的类型信息。
 *
 * @public
 */
export const TypeOfNode: {
    Integer: TypeOfNodeEntry;
    Float: TypeOfNodeEntry;
    String: TypeOfNodeEntry;
    StrCall: TypeOfNodeEntry;
    HashCall: TypeOfNodeEntry;
    Register: TypeOfNodeEntry;
    Device: TypeOfNodeEntry;
    LabelDef: TypeOfNodeEntry;
    ErrorNode: TypeOfNodeEntry;
    Constant: TypeOfNodeEntry;
    BinaryNumber: TypeOfNodeEntry;
    HexNumber: TypeOfNodeEntry;
};
