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
 * @date 2026/08/24 16:32
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    ProgressUpdateEvent,
    ProgressStartEvent,
    LoadedSourceEvent,
    CapabilitiesEvent,
    ProgressEndEvent,
    InvalidatedEvent,
    TerminatedEvent,
    BreakpointEvent,
    ContinuedEvent,
    StoppedEvent,
    ExitedEvent,
    MemoryEvent,
    ModuleEvent,
    OutputEvent,
    ThreadEvent
} from "@vscode/debugadapter";

export interface IC10RuntimeEvents {
    /**
     * （初始化就绪）：最关键的开端信号。适配器在收到 initialize 请求并返回后，发送此事件。
     * 它告诉 IDE：“我已经准备好了，快把之前存着的断点配置全发给我吧”。
     * IDE 收到后，会依次发送 setBreakpoints、setExceptionBreakpoints，最后发送 configurationDone 结束配置阶段
     * */
    //    initialized: ConstructorParameters<typeof InitializedEvent>;

    /**
     * （断点变更）：当适配器内部因动态编译、重定位等原因修改了断点状态（新增/删除/改变）时，通知 IDE 同步更新 UI 上的断点标记（红点）
     * */
    breakpoint: ConstructorParameters<typeof BreakpointEvent>;

    /**
     * （动态能力变更）：极其罕见。
     * 允许适配器在会话运行中动态修改自身的能力集（如原本不支持，后来支持了某种断点），提示 IDE 尝试使用新功能
     * */
    capabilities: ConstructorParameters<typeof CapabilitiesEvent>;

    /**
     * （执行继续）：程序恢复运行。IDE 收到后会清除暂停状态，将高亮行消失。
     * 特别注意：如果是 IDE 主动发起的 continue 或 step 请求，适配器默认不需要发送此事件（请求成功即隐含继续）；
     * 只有在非请求导致的继续（如用户点击了操作系统的“恢复”按钮）时才需发送
     * */
    continued: ConstructorParameters<typeof ContinuedEvent>;

    /**
     * （调试目标退出）：被调试的程序本身（debuggee）正常或异常退出了，并附带退出码（exitCode）
     * */
    exited: ConstructorParameters<typeof ExitedEvent>;

    /**
     * （数据失效）：缓存刷新信号。
     * 当调试适配器内部状态变了（例如用户修改了数据显示格式），但程序并未暂停/继续，它会发此事件提示 IDE：“旧的堆栈/变量数据快照过时了，请重新请求最新数据”
     * */
    invalidated: ConstructorParameters<typeof InvalidatedEvent>;

    loadedSource: ConstructorParameters<typeof LoadedSourceEvent>;

    /**
     * （内存更新）：专为“内存查看器”（Memory Viewer）设计。当调试器通过 setVariable 等操作修改了某块内存时，发此事件通知 IDE 刷新对应地址范围的内存展示
     * */
    memory: ConstructorParameters<typeof MemoryEvent>;

    /**
     * （模块加载） 和 loadedSource（源码加载）：告知 IDE 程序加载了新的动态库（.dll/.so）或新的源文件（如 eval 产生的代码），便于 IDE 在“模块”或“已加载脚本”面板中展示
     * */
    module: ConstructorParameters<typeof ModuleEvent>;

    /**
     * （输出内容）：最繁忙的事件。将程序的 stdout/stderr 打印到 IDE 的“调试控制台”。
     * 支持 category（区分普通日志、重要弹窗、遥测数据），还支持 group（start/end）来折叠/展开日志块，并且支持 ANSI 转义颜色代码（如果双方都支持）
     * */
    output: ConstructorParameters<typeof OutputEvent>;

    /**
     * （进度条）：用于耗时较长的内部操作（如加载大型符号表）。
     * IDE 会在状态栏显示进度条，且如果 cancellable 为 true，用户可点击取消按钮（对应 cancel 请求）
     * */
    progressStart: ConstructorParameters<typeof ProgressStartEvent>;

    progressUpdate: ConstructorParameters<typeof ProgressUpdateEvent>;

    progressEnd: ConstructorParameters<typeof ProgressEndEvent>;

    /**
     * （执行暂停）：最核心的事件之一。程序因断点、单步（Step）、异常、暂停按钮等而停止。
     * IDE 收到后必须立即刷新“调用堆栈”和“变量”面板。
     * reason 字段告知停止原因，hitBreakpointIds 标明命中了哪个断点，allThreadsStopped 提示是否所有线程都停了
     * */
    stopped: ConstructorParameters<typeof StoppedEvent>;

    /**
     * （调试会话终止）：调试会话结束（注意，此时被调试程序可能还在运行，例如“分离”操作）。
     * 它携带一个 restart 字段，IDE 可利用该字段实现“自动重启调试”功能
     * */
    terminated: ConstructorParameters<typeof TerminatedEvent>;

    /**
     * （线程变化）：当调试目标创建或销毁了线程时发送，用于更新 IDE 的线程下拉列表
     * */
    thread: ConstructorParameters<typeof ThreadEvent>;
}
