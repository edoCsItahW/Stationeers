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
 * @note 设备一律由运行时创建并持有（型号化设备为 `SimDevice`，无型号设备为 `VirtualDevice`），
 *       所以注册端口**不需要**传设备句柄：先注册端口（可选地带上型号名），再用 `getDevice` 取回句柄。
 *
 * @example
 * ```typescript
 * const mgr = engine.context.manager;
 *
 * mgr.setExternalDevice('d0', 'Sensor');
 * const sensor = mgr.getDevice('d0');
 *
 * sensor.writeLogic('Setting', 1);
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
     * @summary 注册（或替换）端口上的设备
     *
     * @param name - 端口名，如 `'d0'`；`'db'` 表示自引用设备（替换芯片设备）
     * @param typeName - 型号名（`#>` 设备块里的 `@name`），可选
     *
     * @desc 管理器创建并持有该端口的设备，随后可用 {@link Manager.getDevice} 取回句柄读写逻辑属性。
     *       给了型号名且型号表里有该型号时，创建注解所描述的型号化设备（成员、类型／名称哈希都取自
     *       注解）；型号未知时退化为无型号设备。重复调用会**替换**该端口的设备并把状态重置为新建状态，
     *       型号沿用该端口已声明的型号；想显式换型号就传第二个参数。
     *
     * @note 源码里用 `alias sensor d0 #: @type Sensor` 声明了型号的端口，在执行上下文构造时已经自动
     *       绑定，通常无需手工调用本方法。第二个参数旧版是设备句柄（一直由绑定层忽略），因此旧调用
     *       `setExternalDevice('d0', undefined as never)` 依然有效。
     */
    setExternalDevice(name: string, typeName?: string): void;

    /**
     * @summary 已绑定的端口名
     *
     * @returns 端口名列表（按名字排序，不含芯片设备 `db`）
     *
     * @desc 供宿主与调试器枚举"这块芯片看得见哪些端口"。
     */
    ports(): string[];

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
