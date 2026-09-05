// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_table.d.ts
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/26
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { DeviceAnnotation, EnumAnnotation } from '../parser';

/**
 * @summary 类型表类
 *
 * @desc 管理 IC10 程序中的自定义类型定义（设备类型和枚举类型）。
 * 支持 JSON 序列化。
 *
 * @elseif en
 * @summary Type table class
 *
 * @desc Manages custom type definitions in IC10 programs (device types and enum types).
 * Supports JSON serialization.
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
