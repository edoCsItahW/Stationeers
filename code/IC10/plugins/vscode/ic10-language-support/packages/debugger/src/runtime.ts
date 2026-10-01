// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file runtime.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/08/22 11:44
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { AstResponseEventData, Optional } from "@ic10/common";
import { Lexer, OperandType, Parser, Linker, SymbolMap, type Program, type TypeTableMap } from "@ic10/compiler";
import { Engine, MemoryInfo } from "@ic10/runtime";
import { EventEmitter } from "node:events";
import { readFileSync } from "node:fs";
import { t } from "./locals";

import type { IC10RuntimeEvents } from "./types";


const STAND_LIB = readFileSync(require.resolve("@ic10/compiler/src/stdLib.ic"), "utf-8");


export interface IRuntimeBreakpoint {
    id: number;
    line: number;
    address: number;
    verified: boolean;
}

/** 设备成员分组：与 `#>` 设备块里的注解标签一一对应 */
export type DeviceGroup = "logics" | "slotLogics" | "slots" | "hashes";

/** 分组下的一个字段（设备的成员） */
export interface DeviceField {
    /** 字段名（逻辑属性名、槽位名等） */
    name: string;
    /** 显示值；从未被赋过值时为 undefined（设备默认值不一定是 0，不能用 0 冒充） */
    value?: string;
}

/** 设备的一个分组：可以看作设备的一个子对象 */
export interface DeviceGroupView {
    group: DeviceGroup;
    fields: DeviceField[];
}

/** 程序在某个设备端口上引用过的成员 */
interface DeviceUsage {
    /** 槽位逻辑属性：属性名 → 同一条语句里的槽位序号（序号无法静态确定时为 undefined） */
    logicSlots: Map<string, Optional<number>>;
}

/** 虚拟设备堆栈的长度（见 runtime core 的 device.cpp：`stack_(512, 0.0)`；越界读会读到相邻内存） */
const DEVICE_STACK_SIZE = 512;

/** 静态设备端口：`d0`-`d5`（可带引脚 `d0:1`）或自引用设备 `db`；动态端口（`dr0`/`drr0`）无法静态确定 */
const DEVICE_PORT = /^(?:d[0-5](?::\d+)?|db)$/;

/**
 * 解析静态设备端口
 *
 * @param name 名称（端口名本身，或指向端口的别名）
 * @param value 符号值（别名指向的端口，如 `alias sensor d0` 的 `d0`）
 *
 * @returns 端口名；不是静态设备时返回 undefined
 */
export function staticPort(name: string, value?: string): Optional<string> {
    const port = value || name;

    return DEVICE_PORT.test(port) ? port : undefined;
}

/** AST 操作数节点上能取到的字面量（标识符、整数以字符串存放，枚举引用嵌一层标识符） */
interface OperandNode {
    value?: string | OperandNode;
}

/** 取操作数的名字；寄存器等动态操作数取不到名字时为 undefined */
const operandName = (operand: unknown): Optional<string> => {
    const value = (operand as Optional<OperandNode>)?.value;

    return typeof value === "string" ? value : typeof value?.value === "string" ? value.value : undefined;
};

/** 取操作数的数值；动态操作数（如寄存器下标）取不到数值时为 undefined */
const operandNumber = (operand: unknown): Optional<number> => {
    const name = operandName(operand);
    const value = name === undefined ? NaN : Number(name);

    return Number.isFinite(value) ? value : undefined;
};

export class IC10Runtime extends EventEmitter<IC10RuntimeEvents> {
    private engine?: Engine;

    private flags = {
        /**
         * @summary 当前调试器状态
         * - ture: 程序正在运行（执行指令循环）
         * - false: 程序已停止，等待用户操作（如继续、单步等）
         * */
        state: false
    };

    private pendingBreakpoints: Record<number, number> = {};
    private breakpoints: Map<number, IRuntimeBreakpoint> = new Map();

    private programPath?: string;

    private diagIdx: number = 0;

    private pending: Partial<IC10RuntimeEvents> = {};

    /** continue 时跳过当前位置的断点一次（避免从断点继续时原地再次停住） */
    private skipPc?: number;

    private symbols: Optional<SymbolMap>;

    /** 类型表（设备注解的出处），start() 后可用 */
    private types: Optional<TypeTableMap>;

    /** 各设备端口上被程序引用过的成员，start() 时从 AST 收集 */
    private usage: Map<string, DeviceUsage> = new Map();

    /** 逻辑属性名的全集（类型表里的 `LogicType` 枚举），用于探测端口上被赋过值的属性 */
    private logicTypeNames: string[] = [];

    private static breakpointId = 1;

    constructor(private threadId: number) {
        super();
    }

    private parse(code: string) {
        const stdlib = STAND_LIB;

        const tokens = Lexer.tokenize(code);

        const ast = Parser.parsing(tokens);

        const linker = new Linker();

        linker.addUnit(stdlib);

        linker.addUnit(ast);

        const symbolTable = linker.link();

        return {
            ast,
            symbolTable,
            typeTable: linker.typeTable
        };
    }

    /**
     * 使用 source 在 debugger 进程内本地重新 Lexer→Parser→Analyser→Engine，
     * 避免跨 JSON-RPC 进程传递 C++ 包装对象 (Program/SymbolTable)
     */
    async start(data: AstResponseEventData["data"], programPath: string, stopOnEntry: boolean) {
        this.programPath = programPath;

        if (!data || !data.source) throw new Error("No source available from language server");

        const parseResult = this.parse(data.source);

        this.symbols = JSON.parse(parseResult.symbolTable.toJSON());

        this.types = JSON.parse(parseResult.typeTable.toJSON());

        // 逻辑属性名的全集：用来探测某个端口上究竟被赋过哪些属性
        this.logicTypeNames = this.enumNames("LogicType");

        this.indexUsage(parseResult.ast);

        // 枚举常量操作数（如 `s d0 Color Color.Green`）的求值依赖类型表
        this.engine = new Engine(parseResult.ast, parseResult.symbolTable, undefined, parseResult.typeTable);

        // 端口默认没有设备（见 runtime 的 manager.d.ts），注册后调试器才读得到程序写入的逻辑属性与设备堆栈。
        // 副作用：按类型哈希查找（`lb`、`sbn` 等）会连同这些端口一起命中。
        for (let i = 0; i < 6; i++) this.engine.context.manager.setExternalDevice(`d${i}`, undefined as never);

        this.applyPendingBreakpoint();

        if (stopOnEntry) {
            this.flags.state = false;
            this.emit("stopped", t("runtime.entry"), this.threadId);
        } else this.continue();
    }

    setBreakpoint(line: number): IRuntimeBreakpoint {
        // 引擎未就绪（launch 前设置的断点）：暂存行号，start() 创建引擎后再计算地址
        if (!this.engine) {
            this.pendingBreakpoints[line] ||= IC10Runtime.breakpointId++;

            return {
                line,
                id: this.pendingBreakpoints[line],
                address: -1,
                verified: false
            };
        }

        const address = this.engine.context.getAddr(line);

        const bp: IRuntimeBreakpoint = {
            id: IC10Runtime.breakpointId++,
            line,
            address: address ?? -1,
            verified: address !== undefined
        };

        if (address !== undefined) this.breakpoints.set(address, bp);

        return bp;
    }

    clearBreakpoints(): void {
        this.breakpoints.clear();
        this.pendingBreakpoints = {};
    }

    /** 暂停请求 */
    pause(): void {
        this.flags.state = false;
    }

    /** 断开连接请求 */
    disconnect(): void {
        this.flags.state = false;
        this.pending.terminated = [];
    }

    /** 继续请求 */
    continue(): void {
        if (!this.engine || this.flags.state) return;

        this.applyPending();

        // 记录当前位置：continue 时不重复触发当前断点
        this.skipPc = this.engine.context.pc;

        this.flags.state = true;

        void this.runLoop();
    }

    /** 逐过程请求 */
    next() {
        if (!this.engine) return;

        this.applyPending();

        let ctx = this.engine.context;

        if (ctx.halted) {
            this.emit("exited", 0);
            return;
        }

        try {
            this.engine.step();

            for (; this.diagIdx < this.engine.diagnostics.length; this.diagIdx++) {
                const diag = this.engine.diagnostics[this.diagIdx];

                switch (diag.level) {
                    case "error":
                        this.flags.state = true;
                        this.emit("output", `${diag.id}: ${diag.message}`, diag.level, { test: 1 });

                        this.pending.stopped = [t("runtime.critical"), this.threadId, diag.message];
                        this.pending.exited = [parseInt(diag.id, 36)];
                        return;

                    case "warning":
                    case "info":
                        this.emit("output", `${diag.id}: ${diag.message}`, diag.level, { test: 2 });
                        break;
                }
            }

            if (ctx.halted) this.emit("exited", 0);
            else this.emit("stopped", t("runtime.step"), this.threadId);

        } catch (e) {
            this.emit("output", (e as Error).message, (e as Error).name);
            this.emit("exited", 1);
        }
    }

    getStackTrace(): { name: string; line: number }[] {
        if (!this.engine) return [];

        return [
            {
                name: "main",
                line: this.engine.context.getLine(this.engine.context.pc) ?? 1
            }
        ];
    }

    getProgramPath(): string | undefined {
        return this.programPath;
    }

    getRegisters() {
        if (!this.engine) return [];

        const mem = this.engine.context.memory;

        const info: MemoryInfo = JSON.parse(mem.toJSON());

        return Object.entries(info.registers).map(([name, value]) => ({ name, value }));
    }

    getVariables() {
        return this.symbols;
    }

    /**
     * 展开一个设备端口
     *
     * @param port 端口名（`d0`-`d5`、`db`）
     * @param typeName 声明类型名（`#: @type` 提示），给出各分组字段的范围
     *
     * @returns 设备的分组（`logics` / `slotLogics` / `slots` / `hashes`），空分组不返回
     *
     * @desc 每个分组看作设备的一个子对象：字段范围来自型号注解，没有注解时就只有被赋过值的字段。
     *       字段**只有被赋过值才有值**——设备默认值不一定是 0，用 0 冒充会把"从未赋值"与
     *       "赋值为 0"混为一谈。
     */
    getDeviceGroups(port: string, typeName?: string): DeviceGroupView[] {
        const device = this.engine?.context.manager.getDevice(port);
        const annotation = this.deviceAnnotation(typeName);
        const usage = this.usage.get(port);

        const logics = new Map<string, DeviceField>();
        const slotLogics = new Map<string, DeviceField>();
        const slots = new Map<string, DeviceField>();
        const hashes = new Map<string, DeviceField>();

        // 1) 型号注解给出的字段范围（先占位，值稍后按运行期状态补）
        annotation?.logics.forEach(({ name }) => logics.set(name, { name }));
        annotation?.logicSlots.forEach(({ name }) => slotLogics.set(name, { name }));
        annotation?.slots.forEach(({ name }) => slots.set(name, { name }));

        // 2) 运行期被赋过值的逻辑属性：注解只是范围，值只认赋过值的
        for (const name of this.logicTypeNames)
            if (device?.canReadLogic(name)) logics.set(name, { name, value: device.readLogic(name).toString() });

        // 3) 程序引用过的槽位属性：属性名读不出值，槽位序号只能从同一条语句里拿
        usage?.logicSlots.forEach((index, prop) => {
            const name = index === undefined ? prop : `${prop}[${index}]`;

            slotLogics.set(name, { name, value: index === undefined ? undefined : this.readSlot(port, index, prop) });
        });

        // 4) 被写过的槽位：虚拟设备的槽位就是设备堆栈，非 0 项即被赋过值（赋值为 0 的项看不出来）
        for (let index = 0; index < DEVICE_STACK_SIZE; index++) {
            const value = this.readStack(port, index);

            if (value === undefined || value === 0) continue;

            const declared = annotation?.slots.find(slot => Number(slot.value) === index)?.name ?? `[${index}]`;

            slots.set(declared, { name: declared, value: value.toString() });
        }

        // 5) 注解声明的哈希是型号常量，恒有值
        if (annotation?.deviceHash)
            hashes.set("deviceHash", { name: "deviceHash", value: annotation.deviceHash.value });

        if (annotation?.nameHash) hashes.set("nameHash", { name: "nameHash", value: annotation.nameHash.value });

        const groups: DeviceGroupView[] = [
            { group: "logics", fields: [...logics.values()] },
            { group: "slotLogics", fields: [...slotLogics.values()] },
            { group: "slots", fields: [...slots.values()] },
            { group: "hashes", fields: [...hashes.values()] }
        ];

        return groups.filter(view => view.fields.length > 0);
    }

    /**
     * 收集程序在各设备端口上引用过的槽位属性
     *
     * @desc 槽位属性值要按"槽位序号 + 属性名"读，而注解里的序号是属性自身的编号而非槽位序号，
     *       所以只能从程序语句的操作数里还原（操作数类型由词法与语法分析写在 `type1`…`type6` 上）。
     *       逻辑属性不在这里收：它可以直接按名字在设备上探测。
     */
    private indexUsage(program: Program) {
        this.usage.clear();

        for (const statement of program.statements as unknown as Record<string, unknown>[]) {
            let device: Optional<string>;
            let slotProp: Optional<string>;
            let slotIndex: Optional<number>;

            for (let i = 1; i <= 6; i++) {
                const type = statement[`type${i}`] as Optional<OperandType>;
                const operand = statement[`operand${i}`];

                switch (type) {
                    case OperandType.DEVICE_REF:
                    case OperandType.DEVICE_REF_STRICT:
                        device ??= operandName(operand);
                        break;
                    case OperandType.LOGIC_SLOT_PROP:
                        slotProp ??= operandName(operand);
                        break;
                    case OperandType.SLOT_IDX:
                        slotIndex ??= operandNumber(operand);
                        break;
                }
            }

            const port = device ? staticPort(device, this.symbols?.symbols[device]?.value) : undefined;

            if (!port || !slotProp) continue;

            const usage = this.usage.get(port) ?? { logicSlots: new Map() };

            usage.logicSlots.set(slotProp, slotIndex);

            this.usage.set(port, usage);
        }
    }

    /** 取设备注解；没有该类型或该类型不是设备注解时为 undefined */
    private deviceAnnotation(typeName?: string) {
        const type = typeName ? this.types?.[typeName] : undefined;

        return type?.nodeName === "DeviceAnnotation" ? type : undefined;
    }

    /** 类型表里某个枚举的成员名（如 `LogicType`：逻辑属性名的全集） */
    private enumNames(name: string): string[] {
        const type = this.types?.[name];

        return type?.nodeName === "EnumAnnotation" ? type.values.map(value => value.name) : [];
    }

    /** 读取虚拟设备的槽位属性值；序号越界（见 {@link DEVICE_STACK_SIZE}）或从未被写过时为 undefined */
    private readSlot(port: string, index: number, prop: string): Optional<string> {
        if (index < 0 || index >= DEVICE_STACK_SIZE) return undefined;

        const value = this.engine?.context.manager.getDevice(port)?.readSlot(index, prop);

        // 虚拟设备里读不出"赋值为 0"与"从未赋值"的区别，只有非 0 才当作有值
        return value ? value.toString() : undefined;
    }

    /** 读取虚拟设备的堆栈项；下标越界（见 {@link DEVICE_STACK_SIZE}）时返回 undefined */
    private readStack(port: string, index: number): Optional<number> {
        if (index < 0 || index >= DEVICE_STACK_SIZE) return undefined;

        return this.engine?.context.manager.getDevice(port)?.readStack(index);
    }

    getStack() {
        if (!this.engine) return [];

        const mem = this.engine.context.memory;

        const info: MemoryInfo = JSON.parse(mem.toJSON());

        return info.stack;
    }

    private async runLoop() {
        if (!this.engine) return;

        const ctx = this.engine.context;
        let tickCount = 0;

        while (!ctx.halted && this.flags.state) {
            const bp = this.breakpoints.get(ctx.pc);

            // 到达一个断点（continue 时跳过当前位置的断点一次）
            if (bp && bp.verified && ctx.pc !== this.skipPc) {
                this.flags.state = false;
                this.emit("stopped", t("runtime.breakpoint"), this.threadId);

                return;
            }

            this.skipPc = undefined;

            this.engine.step();

            if (ctx.halted) {
                this.flags.state = false;
                this.emit("exited", 0);

                return;
            }

            // 定期让出事件循环，避免阻塞 DAP 请求处理
            if (++tickCount % 10000 === 0) await new Promise(resolve => setImmediate(resolve));
        }

        if (!this.flags.state)
            this.emit("stopped", t("runtime.step"), ctx.getLine(ctx.pc));

    }

    private applyPending() {
        Object.entries(this.pending).forEach(([name, args]) => this.emit(name, ...args));

        this.pending = {};
    }

    private applyPendingBreakpoint() {
        Object.entries(this.pendingBreakpoints).forEach(([_line, id]) => {
            const line = parseInt(_line);
            const address = this.engine!.context.getAddr(line);

            const bp: IRuntimeBreakpoint = {
                id,
                line,
                address: address ?? -1,
                verified: address !== undefined
            };

            if (address !== undefined) this.breakpoints.set(address, bp);

            this.emit("breakpoint", "changed", bp);
        });

        this.pendingBreakpoints = {};
    }
}
