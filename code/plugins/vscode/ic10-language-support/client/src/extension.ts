/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

import { LanguageClient, TransportKind, ServerOptions } from "vscode-languageclient/node";
import { LanguageClientOptions } from "vscode-languageclient";
import { workspace, ExtensionContext } from "vscode";
import * as path from "path";

import { COMM_EVENT_NAME, EventData } from "common";

class Extension {
    private readonly serverModule: string;
    private readonly serverOpt: ServerOptions;
    private readonly clientOpt: LanguageClientOptions;
    private readonly client: LanguageClient;

    constructor(
        private readonly module: string = path.join("server", "out", "server", "server.js"),
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

    private handle({ type, data }: EventData) {}
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
    extension = new Extension(path.join("server", "out", "server", "server.js"), context);
    extension.run();
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
