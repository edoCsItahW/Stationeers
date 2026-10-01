// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 设备 I/O 接口：导出 {@link Device}，对应 C++ 侧的 `IDevice`，由 `Manager` 创建并持有。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 设备 I/O 接口
 *
 * @desc IC10 程序与外界设备之间的全部通道：逻辑属性、设备堆栈、槽位与试剂查询。执行 `l`/`s`/
 *       `sb`/`get`/`put`/`ls`/`ss`/`lb`/`lr` 等指令时，执行器就是通过这些方法读写设备。
 *
 * @note 绑定层返回的实例背后是运行时的**内置模拟设备**：逻辑属性是一张字符串到数值的表，
 *       槽位读写委托给设备堆栈（槽位名不参与运算）。端口一旦声明了设备型号（`alias sensor d0
 *       #: @type Sensor`，或 `Manager.setExternalDevice('d0', 'Sensor')`），该设备就按型号注解提供
 *       成员、类型／名称哈希取自注解，并可用 {@link Device.snapshot} 内省；没有型号的设备（含芯片
 *       `db`）的试剂查询与哈希仍返回 `0`，因此按类型哈希查找（`findDevicesByType(0)`）会命中所有
 *       无型号设备。
 *
 * @example
 * ```typescript
 * const led = engine.context.manager.getDevice('d0');
 *
 * led.writeLogic('Setting', 1);
 * console.log(led.readLogic('Setting'));   // 1
 * ```
 *
 * @public
 */
export class Device {
    /**
     * @summary 读取逻辑属性
     *
     * @param prop - 逻辑属性名，如 `'Setting'`、`'On'`、`'Pressure'`
     *
     * @returns 属性当前值；从未写入过的属性返回 `0`
     *
     * @desc 对应 IC10 的 `l`；若属性从未被写入，虚拟设备返回 `0` 而非报错。
     */
    readLogic(prop: string): number;

    /**
     * @summary 写入逻辑属性
     *
     * @param prop - 逻辑属性名
     * @param value - 待写入的值
     *
     * @desc 对应 IC10 的 `s` / `sb` / `sbn` 等写入指令；虚拟设备会为该属性建表项。
     */
    writeLogic(prop: string, value: number): void;

    /**
     * @summary 查询逻辑属性是否可读
     *
     * @param prop - 逻辑属性名
     *
     * @returns 该属性是否已存在（虚拟设备以“属性表中是否已有该键”为准）
     *
     * @desc 对应 `bdnvl`：为 `false` 时该指令会跳转。
     */
    canReadLogic(prop: string): boolean;

    /**
     * @summary 查询逻辑属性是否可写
     *
     * @param prop - 逻辑属性名
     *
     * @returns 内置虚拟设备恒为 `true`
     *
     * @desc 对应 `bdnvs`：为 `false` 时该指令会跳转。
     */
    canWriteLogic(prop: string): boolean;

    /**
     * @summary 读取设备堆栈
     *
     * @param index - 堆栈下标（`0` 起）
     *
     * @returns 该槽位的数值
     *
     * @desc 对应 IC10 的 `get`；下标超出设备堆栈范围属于宿主侧的越界访问，请自行约束。
     */
    readStack(index: number): number;

    /**
     * @summary 写入设备堆栈
     *
     * @param index - 堆栈下标（`0` 起）
     * @param value - 待写入的值
     *
     * @desc 对应 IC10 的 `put`。
     */
    writeStack(index: number, value: number): void;

    /**
     * @summary 读取槽位
     *
     * @param index - 槽位下标（`0` 起）
     * @param slot - 槽位逻辑属性名，如 `'Occupied'`、`'Quantity'`
     *
     * @returns 该槽位属性的值
     *
     * @desc 对应 IC10 的 `ls` / `lbs` / `lbns`。虚拟设备忽略 `slot` 名，直接读设备堆栈的同下标
     *       元素。
     */
    readSlot(index: number, slot: string): number;

    /**
     * @summary 写入槽位
     *
     * @param index - 槽位下标（`0` 起）
     * @param slot - 槽位逻辑属性名
     * @param value - 待写入的值
     *
     * @desc 对应 IC10 的 `ss` / `sbs`；虚拟设备忽略 `slot` 名，直接写设备堆栈的同下标元素。
     */
    writeSlot(index: number, slot: string, value: number): void;

    /**
     * @summary 按模式读取试剂
     *
     * @param mode - 试剂模式（`Contents` / `Recipe` / `Required` / `TotalContents` 对应的数值）
     *
     * @returns 试剂查询结果；内置虚拟设备恒为 `0`
     *
     * @desc 对应 IC10 的 `lr`。
     */
    readReagent(mode: number): number;

    /**
     * @summary 查询指定试剂的含量
     *
     * @param reagentHash - 试剂材料哈希（见 `hashValue` / `HASH("...")`）
     *
     * @returns 该试剂的含量；内置虚拟设备恒为 `0`
     *
     * @desc 对应 IC10 的 `rmap`。
     */
    queryReagentAmount(reagentHash: number): number;

    /**
     * @summary 设备类型哈希
     *
     * @returns 设备类型哈希；无型号设备恒为 `0`，型号化设备取自注解的 `@device-hash`
     *
     * @desc `lb` / `lbn` / `lbs` / `lbns` / `sb` / `sbn` / `sbs` 用它匹配设备。无型号设备都返回
     *       `0`，以 `0` 为哈希查找会命中全部无型号设备（含芯片自身）。
     */
    getTypeHash(): number;

    /**
     * @summary 设备名称哈希
     *
     * @returns 设备名称哈希；无型号设备恒为 `0`，型号化设备取自注解的 `@name-hash`
     *
     * @desc `lbn` / `lbns` / `sbn` 在类型哈希之外再按名称哈希缩小范围。
     */
    getNameHash(): number;

    /**
     * @summary 清空设备堆栈
     *
     * @desc 对应 IC10 的 `clr` / `clrd`：把整个堆栈置 `0`。
     */
    clearStack(): void;

    /**
     * @summary 设备型号名
     *
     * @returns 型号名；未声明型号时为空字符串
     *
     * @desc 型号来自源码里的 `alias x d0 #: @type Sensor`（执行上下文按符号表自动绑定）或
     *       `Manager.setExternalDevice('d0', 'Sensor')` 的显式指定。型号名只用于显示与定位，
     *       成员寻址始终按名字进行。
     */
    typeName(): string;

    /**
     * @summary 成员与各自的状态
     *
     * @returns 成员列表：型号注解声明的成员（`isDeclared` 为 `true`，顺序与注解一致）在前，程序写过
     *          但没有声明的成员（`isDeclared` 为 `false`）在后
     *
     * @desc 设备注解给出的是成员（字段）的**范围**，因此未声明的设备只有"被赋过值"的成员。
     *       成员只有被赋过值才有值（`assigned` 为 `true`）：`value` 为 `null` 表示从未赋值，
     *       与"赋值为 `0`"是两回事——设备的默认值不一定是 0。
     *
     * @example
     * ```typescript
     * const sensor = engine.context.manager.getDevice('d0');
     *
     * for (const member of sensor.snapshot())
     *     console.log(`${member.kind} ${member.name} = ${member.assigned ? member.value : '(未赋值)'}`);
     * ```
     */
    snapshot(): DeviceMember[];

    /**
     * @summary 推进一个 tick
     *
     * @desc 由 `Manager.tick()` 逐个设备调用；内置模拟设备无内部状态，因此不做任何事，外部设备
     *       可借此推进自身的仿真。
     */
    tick(): void;
}

/**
 * @summary 设备成员的状态
 *
 * @desc 由 {@link Device.snapshot} 产出。成员种类 `kind` 对应 `#>` 设备块里的注解标签：
 *       `logic`（`@logic`）、`logic-slot`（`@logic-slot`）、`slot`（`@slot`）、`hash`
 *       （`@device-hash` / `@name-hash`）、`reagent-hash`（`@reagent-hash`）；程序写过的未声明成员
 *       一律标为 `logic`。
 *
 * @public
 */
export interface DeviceMember {
    /** 成员名（源码里的写法，如 `Setting`） */
    name: string;

    /** 成员种类：`logic` / `logic-slot` / `slot` / `hash` / `reagent-hash` */
    kind: string;

    /** 注解声明的取值（属性编号、槽位序号或哈希值）；未声明时为空字符串 */
    declaredValue: string;

    /** 当前值；从未赋值时为 `null` */
    value: number | null;

    /** 该成员是否由型号注解声明 */
    isDeclared: boolean;

    /** 该成员是否已被赋值 */
    assigned: boolean;
}
