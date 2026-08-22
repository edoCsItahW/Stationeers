// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file debugSession.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/08/22 10:36
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { DebugProtocol } from "@vscode/debugprotocol";
import { Uri } from "vscode";
import {
    LoggingDebugSession,
    ProgressUpdateEvent,
    ProgressStartEvent,
    CapabilitiesEvent,
    LoadedSourceEvent,
    ProgressEndEvent,
    InitializedEvent,
    InvalidatedEvent,
    TerminatedEvent,
    BreakpointEvent,
    ContinuedEvent,
    StoppedEvent,
    ThreadEvent,
    MemoryEvent,
    ModuleEvent,
    OutputEvent,
    ExitedEvent,
    Handles,
    Thread
} from "@vscode/debugadapter";

import { BasicType, TypeCategory } from "ic10c-node";
import { getEnumName, Transfer } from "common";
import { IC10Runtime } from "./runtime";
import { t } from "./locals";


interface IC10LaunchRequestArguments extends DebugProtocol.LaunchRequestArguments {
    program: string;
    stopOnEntry?: boolean;
}


const EVENT_MAP = {
    breakpoint: BreakpointEvent,

    capabilities: CapabilitiesEvent,

    continued: ContinuedEvent,

    exited: ExitedEvent,

    invalidated: InvalidatedEvent,

    loadedSource: LoadedSourceEvent,

    memory: MemoryEvent,

    module: ModuleEvent,

    output: OutputEvent,

    progressEnd: ProgressEndEvent,

    progressStart: ProgressStartEvent,

    progressUpdate: ProgressUpdateEvent,

    stopped: StoppedEvent,

    terminated: TerminatedEvent,

    thread: ThreadEvent
} as const;


export class IC10DebugSession extends LoggingDebugSession {
    private readonly runtime;
    private variableHandles = new Handles<"registers" | "stack" | "variables">();

    private readonly configurationDonePromise: Promise<void>;
    private resolveConfigurationDone!: () => void;

    constructor(
        private handler: Transfer,
        private readonly threadId = 1
    ) {
        super();

        this.runtime = new IC10Runtime(threadId);

        this.configurationDonePromise = new Promise(resolve => (this.resolveConfigurationDone = resolve));
    }

    /**
     * IC10 内部行号从 1 开始（见 position.hpp），与 VSCode 客户端行号一致，
     * 因此覆盖默认的 1-based→0-based 转换为恒等变换。
     */
    protected convertClientLineToDebugger(line: number): number {
        return line;
    }

    protected convertDebuggerLineToClient(line: number): number {
        return line;
    }

    /**
     * initializeRequest（初始化）：握手协议第一步。客户端发送客户端支持的能力（如是否支持列断点）。适配器需在响应中返回自己的 Capabilities（能力集）。
     * */
    protected initializeRequest(
        response: DebugProtocol.InitializeResponse,
        args: DebugProtocol.InitializeRequestArguments
    ) {
        response.body = {
            supportsConfigurationDoneRequest: true
        };

        this.sendResponse(response);

        this.sendEvent(new InitializedEvent());
    }

    /**
     * launchRequest（启动模式）：调试器启动并运行一个新程序
     * */
    protected async launchRequest(response: DebugProtocol.LaunchResponse, args: IC10LaunchRequestArguments) {
        try {
            await Promise.race([
                this.configurationDonePromise,
                new Promise<void>(resolve => setTimeout(resolve, 5000))
            ]);

            Object.entries(EVENT_MAP).forEach(([name, event]) =>
                this.runtime?.on(name, (...args) => this.sendEvent(new (event as any)(...args)))
            );

            const data = await this.handler.handle({
                type: "ast",
                data: { uri: Uri.file(args.program).toString() }
            });

            await this.runtime.start(data.data, args.program, args.stopOnEntry ?? false);

            this.sendResponse(response);
        } catch (e) {
            this.sendErrorResponse(response, { id: 1, format: (e as Error).message });
        }
    }

    /**
     * attachRequest（附加模式）：调试器连接到一个已存在的运行中进程（按 PID 或端口）
     * */
    //    protected attachRequest(response: DebugProtocol.AttachResponse, args: DebugProtocol.AttachRequestArguments, request?: DebugProtocol.Request) {}

    /**
     * disconnectRequest（断开连接）：客户端请求断开调试连接。适配器应终止子进程或分离，并清理资源
     * */
    protected disconnectRequest(
        response: DebugProtocol.DisconnectResponse,
        args: DebugProtocol.DisconnectArguments
    ): void {
        this.runtime?.disconnect();

        super.disconnectRequest(response, args);
    }

    /**
     * （断开连接）：客户端请求断开调试连接。适配器应终止子进程或分离，并清理资源
     * */
    //    protected terminateRequest(response: DebugProtocol.TerminateResponse, args: DebugProtocol.TerminateArguments, request?: DebugProtocol.Request) {}

    /**
     * 重启）：重启当前调试会话（常用于“重启调试”按钮）
     * */
    //    protected restartRequest(response: DebugProtocol.RestartResponse, args: DebugProtocol.RestartArguments, request?: DebugProtocol.Request) {}

    /**
     * （配置完成）：启动流程的关键结束信号。IDE 已发送完所有初始断点配置，通知适配器：“可以正式开始运行程序了”
     * */
    protected configurationDoneRequest(
        response: DebugProtocol.ConfigurationDoneResponse,
        args: DebugProtocol.ConfigurationDoneArguments
    ): void {
        super.configurationDoneRequest(response, args);

        this.resolveConfigurationDone();
    }

    /**
     * （源码断点）：设置/更新源代码行上的断点（最常见）。输入源文件和行号，返回命中的断点 ID
     * */
    protected setBreakPointsRequest(
        response: DebugProtocol.SetBreakpointsResponse,
        args: DebugProtocol.SetBreakpointsArguments
    ) {
        const lines = (args.breakpoints ?? []).map(bp => bp.line);

        // 清除旧断点，再设置新断点（与 mock-debug 一致）
        this.runtime?.clearBreakpoints();

        const bps = lines.map((line): DebugProtocol.Breakpoint => {
            const bp = this.runtime.setBreakpoint(line);

            return {
                verified: bp.verified,
                line: bp.line,
                id: bp.id
            };
        });

        response.body = { breakpoints: bps };

        this.sendResponse(response);
    }

    /**
     * （函数断点）：通过函数名设置断点（忽略具体文件行号），依赖符号表
     * */
    //    protected setFunctionBreakPointsRequest(response: DebugProtocol.SetFunctionBreakpointsResponse, args: DebugProtocol.SetFunctionBreakpointsArguments, request?: DebugProtocol.Request) {}

    /**
     * （异常断点）：设置遇到特定异常（如 UncaughtException）时暂停
     * */
    //    protected setExceptionBreakPointsRequest(response: DebugProtocol.SetExceptionBreakpointsResponse, args: DebugProtocol.SetExceptionBreakpointsArguments, request?: DebugProtocol.Request) {}

    /**
     * （数据断点）：监控变量的内存值变化（写/读时暂停），常用于 C++ 等底层调试
     * */
    //    protected setDataBreakpointsRequest(response: DebugProtocol.SetDataBreakpointsResponse, args: DebugProtocol.SetDataBreakpointsArguments, request?: DebugProtocol.Request) {}

    /**
     * （指令断点）：通过内存地址（PC指针） 设置断点（汇编级调试）
     * */
    //    protected setInstructionBreakpointsRequest(response: DebugProtocol.SetInstructionBreakpointsResponse, args: DebugProtocol.SetInstructionBreakpointsArguments, request?: DebugProtocol.Request) {}

    /**
     * （断点位置查询）：客户端请求“某一行是否可设置断点”或“该行上有几个有效位置”（用于语法高亮和灰显）
     * */
    //    protected breakpointLocationsRequest(response: DebugProtocol.BreakpointLocationsResponse, args: DebugProtocol.BreakpointLocationsArguments, request?: DebugProtocol.Request) {}

    /**
     * （数据断点信息）：客户端询问某个变量/表达式是否能设置数据断点，适配器返回描述。
     * */
    //    protected dataBreakpointInfoRequest(response: DebugProtocol.DataBreakpointInfoResponse, args: DebugProtocol.DataBreakpointInfoArguments, request?: DebugProtocol.Request) {}

    /**
     * （继续运行）：从暂停状态恢复执行，直至遇到下一个断点或结束
     * */
    protected continueRequest(response: DebugProtocol.ContinueResponse, args: DebugProtocol.ContinueArguments) {
        this.runtime?.continue();

        this.sendResponse(response);
    }

    /**
     * （单步跳过 Step Over）：执行下一行，不进入函数内部
     * */
    protected nextRequest(response: DebugProtocol.NextResponse, args: DebugProtocol.NextArguments) {
        this.runtime?.next();

        this.sendResponse(response);
    }

    /**
     * （单步步入 Step Into）：执行下一行，若遇函数调用则进入函数内部停住
     * */
    //    protected stepInRequest(response: DebugProtocol.StepInResponse, args: DebugProtocol.StepInArguments, request?: DebugProtocol.Request) {}

    /**
     * （单步步出 Step Out）：跳出当前函数，停在调用方下一行
     * */
    //    protected stepOutRequest(response: DebugProtocol.StepOutResponse, args: DebugProtocol.StepOutArguments, request?: DebugProtocol.Request) {}

    /**
     * （反向单步）：逆向调试，回退一步执行（需调试器支持时间旅行）
     * */
    //    protected stepBackRequest(response: DebugProtocol.StepBackResponse, args: DebugProtocol.StepBackArguments, request?: DebugProtocol.Request) {}

    /**
     * （反向继续）：逆向执行直到断点（时间旅行）
     * */
    //    protected reverseContinueRequest(response: DebugProtocol.ReverseContinueResponse, args: DebugProtocol.ReverseContinueArguments, request?: DebugProtocol.Request) {}

    /**
     * （暂停）：立即中断正在运行的程序（点击 IDE 暂停按钮）
     * */
    protected pauseRequest(response: DebugProtocol.PauseResponse, args: DebugProtocol.PauseArguments) {
        this.runtime?.pause();

        this.sendResponse(response);
    }

    /**
     * （跳转执行）：将指令指针（IP）强制移动到指定位置（无视断点，常用于“Set Next Statement”）
     * */
    //    protected gotoRequest(response: DebugProtocol.GotoResponse, args: DebugProtocol.GotoArguments, request?: DebugProtocol.Request) {}

    /**
     * （重启栈帧）：重新执行当前栈帧的函数，丢弃当前帧状态（常用于“重试”特定函数）
     * */
    //    protected restartFrameRequest(response: DebugProtocol.RestartFrameResponse, args: DebugProtocol.RestartFrameArguments, request?: DebugProtocol.Request) {}

    /**
     * （获取线程列表）：返回当前所有线程 ID 和名称（IDE 显示在“线程”面板）
     * */
    protected threadsRequest(response: DebugProtocol.ThreadsResponse): void {
        response.body = {
            threads: [new Thread(this.threadId, "main")]
        };

        this.sendResponse(response);
    }

    /**
     * （获取调用栈）：最频繁的请求之一。传入线程 ID，返回该线程的调用栈帧列表（函数名、文件、行号）
     * */
    protected stackTraceRequest(response: DebugProtocol.StackTraceResponse, args: DebugProtocol.StackTraceArguments) {
        const frames = this.runtime.getStackTrace();
        const path = this.runtime.getProgramPath();

        response.body = {
            stackFrames: frames.map((f, i) => ({
                id: i,
                name: f.name,
                source: path ? { path } : undefined,
                line: f.line,
                column: 1
            })),
            totalFrames: frames.length
        };

        this.sendResponse(response);
    }

    /**
     * （获取作用域）：传入栈帧 ID，返回该帧下的作用域（如局部变量、闭包、全局），用于构建“变量面板”的根节点
     * */
    protected scopesRequest(response: DebugProtocol.ScopesResponse, args: DebugProtocol.ScopesArguments) {
        response.body = {
            scopes: [
                {
                    name: t("session.register"),
                    variablesReference: this.variableHandles.create("registers"),
                    expensive: false
                },
                {
                    name: t("session.stack"),
                    variablesReference: this.variableHandles.create("stack"),
                    expensive: false
                },
                {
                    name: t("session.variable"),
                    variablesReference: this.variableHandles.create("variables"),
                    expensive: false
                }
            ]
        };

        this.sendResponse(response);
    }

    /**
     * variablesRequest（获取变量详情）：传入变量引用 ID（来自 scopes 或之前展开的对象），返回该对象的属性/子变量（支持懒加载和递归展开）
     * */
    protected variablesRequest(response: DebugProtocol.VariablesResponse, args: DebugProtocol.VariablesArguments) {
        if (!this.runtime) {
            response.body = { variables: [] };

            this.sendResponse(response);

            return;
        }

        let variables: DebugProtocol.Variable[] = [];

        switch (this.variableHandles.get(args.variablesReference)) {
            case "registers":
                variables = this.runtime.getRegisters().map((r, i) => ({
                    name: r.name,
                    value: r.value.toString(),
                    variablesReference: 0
                }));
                break;
            case "stack":
                variables = this.runtime.getStack().map((v, i) => ({
                    name: `[${i}]`,
                    value: v.toString(),
                    variablesReference: 0
                }));
                break;
            case "variables":
                variables = this.runtime.getVariables().map(([name, symbol], i): DebugProtocol.Variable => ({
                    name,
                    value: symbol.value || "",
                    variablesReference: 0,
                    type:
                        symbol.typeName ||
                        `${getEnumName(TypeCategory, symbol.category)?.toLowerCase()}:${getEnumName(BasicType, symbol.type)?.toLowerCase()}`,
                    presentationHint: { kind: "data" }
                }));
        }

        response.body = { variables };

        this.sendResponse(response);
    }

    /**
     * （获取源代码）：传入 Source 路径，适配器返回该文件的原始文本内容（用于 IDE 展示只读源码，当文件不在磁盘时尤其有用）
     * */
    //    protected sourceRequest(response: DebugProtocol.SourceResponse, args: DebugProtocol.SourceArguments, request?: DebugProtocol.Request) {}

    /**
     * （表达式求值）：在“调试控制台”或“监视”窗口输入表达式（如 a + b），适配器在当前栈帧上下文计算并返回结果
     * */
    //    protected evaluateRequest(response: DebugProtocol.EvaluateResponse, args: DebugProtocol.EvaluateArguments, request?: DebugProtocol.Request) {}

    /**
     * （设置变量值）：修改变量面板中某个具体变量的值（通过 variablesReference + 变量名）
     * */
    //    protected setVariableRequest(response: DebugProtocol.SetVariableResponse, args: DebugProtocol.SetVariableArguments, request?: DebugProtocol.Request) {}

    /**
     * （读内存）：读取指定地址范围的内存字节数据（用于 IDE 的“内存查看器”组件）
     * */
    //    protected readMemoryRequest(response: DebugProtocol.ReadMemoryResponse, args: DebugProtocol.ReadMemoryArguments, request?: DebugProtocol.Request) {}

    /**
     * （写内存）：修改指定地址的内存数据（危险操作，常用于热补丁）
     * */
    //    protected writeMemoryRequest(response: DebugProtocol.WriteMemoryResponse, args: DebugProtocol.WriteMemoryArguments, request?: DebugProtocol.Request) {}

    /**
     * （反汇编）：获取指定地址范围的反汇编指令（用于 IDE 的“反汇编视图”）
     * */
    //    protected disassembleRequest(response: DebugProtocol.DisassembleResponse, args: DebugProtocol.DisassembleArguments, request?: DebugProtocol.Request) {}

    /**
     * （代码补全）：在调试控制台输入时，提供智能提示（自动补全变量名/函数名）
     * */
    //    protected completionsRequest(response: DebugProtocol.CompletionsResponse, args: DebugProtocol.CompletionsArguments, request?: DebugProtocol.Request) {}

    /**
     * （步入目标列表）：当一行有多个函数调用时，让用户选择要步入哪一个（多目标 Step Into）
     * */
    //    protected stepInTargetsRequest(response: DebugProtocol.StepInTargetsResponse, args: DebugProtocol.StepInTargetsArguments, request?: DebugProtocol.Request) {}

    /**
     * （跳转目标列表）：提供当前代码位置可跳转的有效目标列表（如 C++ 中的 goto 标签，或汇编跳转地址）
     * */
    //    protected gotoTargetsRequest(response: DebugProtocol.GotoTargetsResponse, args: DebugProtocol.GotoTargetsArguments, request?: DebugProtocol.Request) {}

    /**
     * （异常详情）：在 stopped 事件 reason='exception' 后，客户端主动请求该异常的详细信息（类型、描述、堆栈）
     * */
    protected exceptionInfoRequest(
        response: DebugProtocol.ExceptionInfoResponse,
        args: DebugProtocol.ExceptionInfoArguments,
        request?: DebugProtocol.Request
    ) {
        console.log("exceptionInfoRequest", request, args);

        this.sendResponse(response);
    }

    /**
     * （已加载源码列表）：返回当前调试会话中所有已加载的源文件清单（用于 IDE 的“已加载脚本”面板）
     * */
    //    protected loadedSourcesRequest(response: DebugProtocol.LoadedSourcesResponse, args: DebugProtocol.LoadedSourcesArguments, request?: DebugProtocol.Request) {}

    /**
     * （终止线程）：强制终止指定的线程（极少见，依赖 OS 支持）
     * */
    //    protected terminateThreadsRequest(response: DebugProtocol.TerminateThreadsResponse, args: DebugProtocol.TerminateThreadsArguments, request?: DebugProtocol.Request) {}

    /**
     * （取消请求）：取消一个正在进行的耗时请求（如超长的 variables 加载），配合进度条使用
     * */
    //    protected cancelRequest(response: DebugProtocol.CancelResponse, args: DebugProtocol.CancelArguments, request?: DebugProtocol.Request) {}

    /**
     * （自定义请求）：万能扩展点。如果你需要实现 DAP 标准之外的自定义协议（如私有 VS Code 扩展命令），可以重写此方法来处理
     * */
    //    protected customRequest(command: string, response: DebugProtocol.Response, args: any, request?: DebugProtocol.Request) {}
}
