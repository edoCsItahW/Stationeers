/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

import { LanguageClient, TransportKind, ServerOptions } from "vscode-languageclient/node";
import { DebugAdapterDescriptor, DebugAdapterInlineImplementation } from "vscode";
import { LanguageClientOptions } from "vscode-languageclient";
import { IC10DebugSession, locale as debuggerLocale } from "@ic10/debugger";
import * as path from "path";
import {
    DebugAdapterDescriptorFactory,
    DebugAdapterExecutable,
    ExtensionContext,
    ProviderResult,
    DebugSession,
    workspace,
    commands,
    window,
    debug
} from "vscode";
import {
    COMPLETION_SCOPE_EVENT_NAME,
    CONFIGURATION_SECTION_NAME,
    ResponseEventData,
    RequestEventData,
    CompletionScope,
    COMM_EVENT_NAME,
    Optional,
    Transfer
} from "@ic10/common";
import { applyLanguage, t } from "./locals";


/**
 * @summary 收窄补全范围的命令 → 范围
 *
 * @summary Narrow-scope commands → scopes
 *
 * @desc 每个命令对应一类操作数。命令既可在补全列表弹出时按下（刷新为收窄后的候选），
 * 也可在补全之前按下（范围作用于紧接着的那次补全）。范围绑定到"按键后第一次补全所在的操作数槽位"，
 * 走到下一个操作数即失效，不影响后续补全。
 *
 * @desc Each command maps to one kind of operand. It can be pressed while the suggest widget is open
 * (the list is refreshed as the narrowed candidates) or before completing (the scope applies to the
 * very next completion). The scope binds to the operand slot of the first completion after the key
 * press and expires at the next operand, so later completions are unaffected.
 * */
const COMPLETION_SCOPE_COMMANDS: [CompletionScope, string][] = [
    ["device", "ic10.completionScope.device"],
    ["register", "ic10.completionScope.register"],
    ["number", "ic10.completionScope.number"],
    ["enum", "ic10.completionScope.enum"],
    ["identifier", "ic10.completionScope.identifier"],
    ["keyword", "ic10.completionScope.keyword"],
    ["all", "ic10.completionScope.all"]
];

/**
 * @summary 范围 → 翻译路径（类型化的路径才能被 `t` 接受，因此不能用字符串拼接）
 *
 * @summary Scope → translation path (typed paths are required by `t`, so no string concatenation)
 * */
const COMPLETION_SCOPE_PATHS = {
    device: "completion.scope.device",
    register: "completion.scope.register",
    number: "completion.scope.number",
    enum: "completion.scope.enum",
    identifier: "completion.scope.identifier",
    keyword: "completion.scope.keyword",
    all: "completion.scope.all"
} as const;


class Extension implements Transfer {
    private readonly serverModule: string;
    private readonly serverOpt: ServerOptions;
    private readonly clientOpt: LanguageClientOptions;
    private readonly client: LanguageClient;

    constructor(
        private readonly module: string = path.join("server", "dist", "server.js"),
        private context: ExtensionContext
    ) {
        this.serverModule = this.context.asAbsolutePath(this.module);

        this.serverOpt = {
            run: { module: this.serverModule, transport: TransportKind.ipc },
            debug: { module: this.serverModule, transport: TransportKind.ipc }
        };
        this.clientOpt = {
            documentSelector: [{ scheme: "file", language: "ic10" }],
            synchronize: {
                fileEvents: workspace.createFileSystemWatcher("**/*.ic")
            }
        };
        this.client = new LanguageClient("ic10", "IC10 Language Client", this.serverOpt, this.clientOpt);
    }

    stop(): Thenable<void> | undefined {
        if (!this.client) return;

        return this.client.stop();
    }

    run() {
        this.client.onRequest(COMM_EVENT_NAME, this.handle.bind(this));
        this.client.start();
    }

    async handle(data: RequestEventData): Promise<ResponseEventData> {
        return this.client.sendRequest(COMM_EVENT_NAME, data);
    }

    /**
     * @summary 推送补全范围
     *
     * @summary Push a completion scope
     *
     * @param uri - 文档 URI
     * @param uri - Document URI
     * @param scope - 期望的操作数类别
     * @param scope - The expected operand kind
     * @param line - 光标所在行（1-based）；范围只在这一行内生效
     * @param line - The cursor line (1-based); the scope only applies there
     * */
    setCompletionScope(uri: string, scope: CompletionScope, line: number) {
        this.client.sendNotification(COMPLETION_SCOPE_EVENT_NAME, { uri, scope, line });
    }
}

let extension: Extension;

/**
 * @summary 激活 IC10 语言支持扩展
 *
 * @summary Activate the IC10 Language Support extension
 *
 * @desc VS Code 扩展激活入口函数。创建 Extension 实例，配置 LSP 服务端模块路径，
 * 启动 Language Client 以连接服务端，使 IC10 语言文件的语法高亮、诊断、
 * 补全等功能生效。
 *
 * @desc VS Code extension activation entry point. Creates the Extension instance,
 * configures the LSP server module path, and starts the Language Client to connect
 * to the server, enabling syntax highlighting, diagnostics, completion, and other
 * features for IC10 language files.
 *
 * @param context - VS Code 扩展上下文 / VS Code extension context
 * */
export async function activate(context: ExtensionContext) {
    // 客户端界面文本跟随插件设置 ic10.language（与服务端同一个设置）
    applyLanguages();

    context.subscriptions.push(
        workspace.onDidChangeConfiguration(e => {
            if (e.affectsConfiguration(`${CONFIGURATION_SECTION_NAME}.language`)) applyLanguages();
        })
    );

    extension = new Extension(path.join("packages", "server", "dist", "server.js"), context);
    extension.run();

    registerCompletionScopes(context);

    const factor: DebugAdapterDescriptorFactory = {
        createDebugAdapterDescriptor: (
            session: DebugSession,
            executable: Optional<DebugAdapterExecutable>
        ): ProviderResult<DebugAdapterDescriptor> => new DebugAdapterInlineImplementation(
            new IC10DebugSession(extension)
        )
    };
    context.subscriptions.push(
        debug.registerDebugAdapterDescriptorFactory("ic10", factor)
    );
}

/**
 * @summary 应用界面语言：客户端与调试器都跟随 `ic10.language`
 *
 * @summary Apply the UI language: both the client and the debugger follow `ic10.language`
 *
 * @desc 调试器运行在扩展宿主里，它的界面文本（变量面板的作用域名、停止原因）是同一套资源机制，
 * 因此这里用 `applyLanguage()` 的返回值把它一并切到同一语言。
 *
 * @desc The debug adapter runs in the extension host and its UI text (scope names in the variables
 * panel, stop reasons) uses the same resource mechanism, so the return value of `applyLanguage()` is
 * used to switch it to the same language.
 * */
function applyLanguages() {
    const language = applyLanguage();

    if (language) debuggerLocale.setLocale(language);
}

/**
 * @summary 注册"收窄补全范围"的命令
 *
 * @summary Register the "narrow completion scope" commands
 *
 * @desc 命令把范围推给语言服务端，然后强制刷新补全列表：先关掉可能已经弹出的列表再重新触发，
 * 保证两种情况都生效——列表已弹出时就地换成收窄后的候选，列表未弹出时直接以收窄后的候选弹出。
 * 范围绑定到"按键后第一次补全所在的操作数槽位"（见服务端 `setScope`），走到下一个操作数即失效。
 *
 * @desc Each command pushes the scope to the language server and forces a refresh of the suggest
 * list: it hides a list that may already be open and triggers it again, so both cases work — an open
 * list is replaced by the narrowed candidates in place, and a closed one opens already narrowed. The
 * scope binds to the operand slot of the first completion after the key press (see the server's
 * `setScope`) and expires at the next operand.
 * */
function registerCompletionScopes(context: ExtensionContext) {
    const run = (scope: CompletionScope) => async () => {
        const editor = window.activeTextEditor;

        if (!editor || editor.document.languageId !== "ic10") return;

        extension.setCompletionScope(editor.document.uri.toString(), scope, editor.selection.active.line + 1);

        // 先关闭再触发：列表已弹出时也能拿到新请求（只 triggerSuggest 不会刷新已打开的列表）
        await commands.executeCommand("hideSuggestWidget");
        await commands.executeCommand("editor.action.triggerSuggest");

        // 提示本次生效的范围：看不到提示说明按键没被命令接住（便于区分"按键没生效"与"列表没刷新"）
        window.setStatusBarMessage(t("completion.scope.message", { scope: t(COMPLETION_SCOPE_PATHS[scope]) }), 2000);
    };

    for (const [scope, command] of COMPLETION_SCOPE_COMMANDS)
        context.subscriptions.push(commands.registerCommand(command, run(scope)));
}

/**
 * @summary 停用 IC10 语言支持扩展
 *
 * @summary Deactivate the IC10 Language Support extension
 *
 * @desc VS Code 扩展停用入口函数。停止 Language Client 连接，释放相关资源。
 * 如果扩展尚未初始化则返回 undefined。
 *
 * @desc VS Code extension deactivation entry point. Stops the Language Client
 * connection and releases associated resources. Returns undefined if the extension
 * has not been initialized.
 *
 * @returns 停止操作的 Thenable，或 undefined（扩展未运行时）
 *
 * @returns A Thenable for the stop operation, or undefined if the extension was never started
 * */
export async function deactivate() {
    if (extension) return extension.stop();

    return undefined;
}
