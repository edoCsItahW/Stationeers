/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file server.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/26 22:24
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {createConnection, TextDocuments, ProposedFeatures} from "vscode-languageserver/node";
import {TextDocument} from "vscode-languageserver-textdocument";
import type {Connection} from "vscode-languageserver/node";

import {SettingsManager} from "./services/settingsManager";
import {DiagnosticHandler} from "./handlers/diagnostic";
import {ParserPipline} from "./services/parserPipline";
import {DocumentCache, GlobalCache} from "./cache";
import {HoverHandler} from "./handlers/hover";
import {t} from "../locals/locale";


type OnInitializeHandlerType = Parameters<Connection["onInitialize"]>[0];
type OnInitializedHandlerType = Parameters<Connection["onInitialized"]>[0];
type OnDidChangeConfigurationHandlerType = Parameters<Connection["onDidChangeConfiguration"]>[0];
type OnDidOpenHandlerType = Parameters<TextDocuments<TextDocument>["onDidOpen"]>[0];
type OnDidChangeContentHandlerType = Parameters<TextDocuments<TextDocument>["onDidChangeContent"]>[0];


export class Server {
    private settingMgr: SettingsManager;
    private readonly docCache: DocumentCache;
    private readonly globalCache: GlobalCache;
    private readonly hoverHandler: HoverHandler;
    private diagHandler: DiagnosticHandler;
    private pipline: ParserPipline;

    constructor(
        private readonly connection: Connection = createConnection(ProposedFeatures.all),
        private readonly documents: TextDocuments<TextDocument> = new TextDocuments(TextDocument)
    ) {
        this.docCache = new DocumentCache();

        this.globalCache = new GlobalCache();
        this.settingMgr = new SettingsManager(connection, this.docCache, this.globalCache,
            () => this.handleLocaleChanged());

        this.hoverHandler = new HoverHandler(this.docCache);
        this.diagHandler = new DiagnosticHandler(this.docCache);

        this.pipline = new ParserPipline();
    }

    run() {
        console.log("[IC10 LSP] - Server: ", t("server.info.start"));

        try {
            // 初始化
            this.connection.onInitialize(this.onInitialize.bind(this));
            this.connection.onInitialized(this.onInitialized.bind(this))

            // 事件监听
            this.connection.onDidChangeConfiguration(this.onDidChangeConfiguration.bind(this));

            // LSP功能
            this.connection.onHover(this.hoverHandler.handle.bind(this.hoverHandler));
            this.connection.languages.diagnostics.on(this.diagHandler.handle.bind(this.diagHandler))

            // 文档监听
            this.documents.onDidOpen(this.onDidOpen.bind(this));
            this.documents.onDidChangeContent(this.onDidChangeContent.bind(this));

            // 启动监听
            this.documents.listen(this.connection);
            this.connection.listen();
        } catch (error) {
            console.error("[IC10 LSP] - Server: ", error);
        }
    }

    private onInitialize(...args: Parameters<OnInitializeHandlerType>): ReturnType<OnInitializeHandlerType> {
        return this.settingMgr.onInitialize(...args);
    }

    private onInitialized(...args: Parameters<OnInitializedHandlerType>): ReturnType<OnInitializedHandlerType> {
        return this.settingMgr.onInitialized(...args);
    }

    private onDidChangeConfiguration(...args: Parameters<OnDidChangeConfigurationHandlerType>): ReturnType<OnDidChangeConfigurationHandlerType> {
        return this.settingMgr.onDidChangeConfiguration(...args);
    }

    private async onDidOpen(...[{document}]: Parameters<OnDidOpenHandlerType>) {
        this.docCache.initDocument(document.uri);
        this.globalCache.uri = document.uri;

        this.parseAndRefresh(document.getText());
    }

    private onDidChangeContent(...[{document}]: Parameters<OnDidChangeContentHandlerType>): ReturnType<OnDidChangeContentHandlerType> {
        this.globalCache.uri = document.uri;

        this.parseAndRefresh(document.getText());
    }

    private async parseAndRefresh(code: string) {
        const uri = this.globalCache.uri;
        this.pipline.parse(code, this.docCache.getCache(uri)).then(res => {
            if (!res.changed) return;

            this.docCache.updateAfterParse(uri, res);

            this.connection.languages.diagnostics.refresh();
        });
    }

    private handleLocaleChanged() {
        const doc = this.documents.get(this.globalCache.uri);
        if (doc) {
            this.docCache.invalidateHash(this.globalCache.uri);
            this.parseAndRefresh(doc.getText());
        }
    }

}


const server = new Server();
server.run();
