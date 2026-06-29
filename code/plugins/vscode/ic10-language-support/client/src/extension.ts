/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

import * as path from 'path';
import {workspace, ExtensionContext} from 'vscode';

import {
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
    TransportKind
} from 'vscode-languageclient/node';


class Extension {
    private readonly serverModule: string;
    private readonly serverOpt: ServerOptions;
    private readonly clientOpt: LanguageClientOptions;
    private readonly client: LanguageClient;

    constructor(
        private readonly module: string = path.join('server', 'out', 'server', 'src', 'server', 'server.js'),
        private context: ExtensionContext
    ) {
        this.serverModule = this.context.asAbsolutePath(this.module);

        this.serverOpt = {
            run: {module: this.serverModule, transport: TransportKind.ipc},
            debug: {module: this.serverModule, transport: TransportKind.ipc}
        };
        this.clientOpt = {
            documentSelector: [{scheme: 'file', language: 'ic10'}],
            synchronize: {
                fileEvents: workspace.createFileSystemWatcher('**/*.ic')
            }
        };
        this.client = new LanguageClient(
            'ic10',
            'IC10 Language Client',
            this.serverOpt,
            this.clientOpt
        );
    }

    stop(): Thenable<void> | undefined {
        if (!this.client) {
            return undefined;
        }
        return this.client.stop();
    }


    run() {
        this.client.start();
    }
}


let extension: Extension;


export async function activate(context: ExtensionContext) {
    extension = new Extension(
        path.join('server', 'out', 'server', 'src', 'server', 'server.js'),
        context
    );
    extension.run();
}

export async function deactivate() {
    if (extension)
        return extension.stop();

    return undefined;
}
