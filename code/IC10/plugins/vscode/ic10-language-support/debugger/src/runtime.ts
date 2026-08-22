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
import { EventEmitter } from "node:events";

import { Lexer, Parser, Linker, SymbolMap } from "ic10c-node";
import type { AstResponseEventData, Optional } from "common";
import stdLibJson from "ic10c-node/static/stdLib.ic.json";
import type { IC10RuntimeEvents } from "./types";
import { Engine, MemoryInfo } from "ic10r-node";
import { t } from "./locals";


export interface IRuntimeBreakpoint {
    id: number;
    line: number;
    address: number;
    verified: boolean;
}

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

    private static breakpointId = 1;

    constructor(private threadId: number) {
        super();
    }

    private parse(code: string) {
        const stdlib = stdLibJson.content;

        const tokens = Lexer.tokenize(code);

        const ast = Parser.parsing(tokens);

        const linker = new Linker();

        linker.addUnit(stdlib);

        linker.addUnit(ast);

        return {
            ast,
            symbolTable: linker.link()
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

        this.engine = new Engine(parseResult.ast, parseResult.symbolTable);

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
        if (this.symbols) return Object.entries(this.symbols);

        return [];
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
