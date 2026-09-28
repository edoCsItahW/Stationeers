// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file manager.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 设备管理器：导出 {@link Manager}，按端口名或哈希维护 `Context` 可见的设备。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Device } from "./device";

/**
 * @summary 设备管理器
 *
 * @desc IC10 程序看到的“设备网络”：一张**端口名 → 设备**的表，外加一个始终存在的芯片设备。
 *       执行器解析设备引用（`d0`…`d5`、`db`、`d0:1`、`dr0`/`drr0`、设备别名）后，最终都通过
 *       {@link Manager.getDevice} 取设备；`lb`/`sb` 一类按哈希查找的指令走
 *       {@link Manager.findDeviceByType} 等接口。
 *
 * @note 内置设备一律由运行时创建并持有（`VirtualDevice`），所以注册端口时传入的设备句柄会被忽略：
 *       先注册端口，再用 `getDevice` 取回句柄。
 *
 * @example
 * ```typescript
 * const mgr = engine.context.manager;
 *
 * mgr.setExternalDevice('d0', undefined as never);
 * const led = mgr.getDevice('d0');
 *
 * led.writeLogic('Setting', 1);
 * ```
 *
 * @public
 */
export class Manager {

    /**
     * @summary 按端口名取设备
     *
     * @param name - 端口名：`'d0'`…`'d5'`、带引脚的 `'d0:1'`，或 `'db'`（自身引用，即芯片所在设备）
     *
     * @returns 对应设备；该端口没有注册设备时返回 `null`
     *
     * @desc 芯片设备（`'db'`）始终存在，因此 `'db'` 不会返回 `null`。动态端口（`dr0`、`drr0`）由
     *       执行器换算成 `'d0'`…`'d5'` 之后再调用本方法。
     */
    getDevice(name: string): Device | null;

    /**
     * @summary 注册（或替换）端口上的外部设备
     *
     * @param name - 端口名，如 `'d0'`
     * @param device - 设备句柄
     *
     * @desc 管理器为该端口创建一个内置虚拟设备并持有它，随后可用 {@link Manager.getDevice} 取回
     *       句柄读写逻辑属性。
     *
     * @note `device` 参数当前由绑定层忽略（设备所有权在运行时一侧），传 `undefined` 亦可。
     */
    setExternalDevice(name: string, device: Device): void;

    /**
     * @summary 替换芯片设备
     *
     * @param device - 设备句柄
     *
     * @desc 芯片设备即 `db` 指向的设备（IC10 中“自己所在的设备”）。构造 `Manager` 时已存在一个
     *       内置虚拟设备，本方法用新的虚拟设备替换它。
     *
     * @note `device` 参数当前由绑定层忽略。
     */
    setChipDevice(device: Device): void;

    /**
     * @summary 按设备类型哈希查找设备
     *
     * @param typeHash - 设备类型哈希（见 `hashValue` / `HASH("...")`）
     *
     * @returns 第一个类型哈希匹配的设备；没有匹配时返回 `null`
     *
     * @desc 对应 IC10 的 `sb`：按类型哈希定位单个设备并写入逻辑属性。
     *
     * @note 内置虚拟设备的类型哈希恒为 `0`，因此以 `0` 查找会命中“第一个”内置设备。
     */
    findDeviceByType(typeHash: number): Device | null;

    /**
     * @summary 按设备类型哈希与名称哈希查找设备
     *
     * @param typeHash - 设备类型哈希
     * @param nameHash - 设备名称哈希
     *
     * @returns 第一个两者都匹配的设备；没有匹配时返回 `null`
     *
     * @desc 对应 IC10 的 `sbn`。
     */
    findDeviceByTypeAndName(typeHash: number, nameHash: number): Device | null;

    /**
     * @summary 按类型哈希查找全部设备
     *
     * @param typeHash - 设备类型哈希
     *
     * @returns 匹配的设备数组（可能为空）
     *
     * @desc 对应 IC10 的 `lb` / `lbs`：聚合模式（`Average` / `Sum` / `Minimum` / `Maximum`）会对
     *       返回的每个设备读取同一个属性。
     *
     * @note 结果**包含芯片自身**（`db`），因为内置设备的类型哈希都为 `0`。
     */
    findDevicesByType(typeHash: number): Device[];

    /**
     * @summary 按类型哈希与名称哈希查找全部设备
     *
     * @param typeHash - 设备类型哈希
     * @param nameHash - 设备名称哈希
     *
     * @returns 匹配的设备数组（可能为空）
     *
     * @desc 对应 IC10 的 `lbn` / `lbns`。
     */
    findDevicesByTypeAndName(typeHash: number, nameHash: number): Device[];

    /**
     * @summary 推进一个 tick
     *
     * @desc 按注册顺序对每个外部设备与芯片设备调用 `tick()`，供宿主推进自己的设备仿真。
     *       内置虚拟设备不做任何事。
     */
    tick(): void;

}
