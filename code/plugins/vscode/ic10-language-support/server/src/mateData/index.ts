// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file index.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 20:59
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import logicSlotLocalData from "./locals/logicSlotType.json";
import insLocalData from "./locals/instructions.json";
import logicLocalData from "./locals/logicType.json";
import insMeteData from "./std/instructions.json";
import enumsLocalData from "./locals/enums.json";


export type LogicSlotLocalDataType = typeof logicSlotLocalData;
export type InsLocalDataType = typeof insLocalData;
export type LogicLocalDataType = typeof logicLocalData;
export type InsMetaDataType = typeof insMeteData;
export type EnumsLocalDataType = typeof enumsLocalData;

export const LOGIC_SLOT_LOCAL_MAP: Map<string, LogicLocalDataType[keyof LogicLocalDataType]> = new Map(Object.entries(logicSlotLocalData));
export const INS_META_MAP: Map<string, InsMetaDataType[keyof InsMetaDataType]> = new Map(Object.entries(insMeteData));
export const LOGIC_LOCAL_MAP: Map<string, LogicSlotLocalDataType[keyof LogicSlotLocalDataType]> = new Map(Object.entries(logicLocalData));
export const INS_LOCAL_MAP: Map<string, InsLocalDataType[keyof InsLocalDataType]> = new Map(Object.entries(insLocalData));
export const ENUMS_LOCAL_MAP: Map<string, EnumsLocalDataType[keyof EnumsLocalDataType]> = new Map(Object.entries(enumsLocalData));
