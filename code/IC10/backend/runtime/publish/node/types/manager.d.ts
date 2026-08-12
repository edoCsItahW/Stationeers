// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file manager.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/12 14:40
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Device } from "./device";

export class Manager {

    getDevice(name: string): Device;

    setExternalDevice(name: string, device: Device): void;

    setChipDevice(device: Device): void;

    findDeviceByType(typeHash: number): Device;

    findDeviceByTypeAndName(typeHash: number, nameHash: number): Device;

    findDevicesByType(typeHash: number): Device[];

    findDevicesByTypeAndName(typeHash: number, nameHash: number): Device[];

    tick(): void;

}