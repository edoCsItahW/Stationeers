/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file settingsManager.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 21:21
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type {Connection, InitializeResult} from "vscode-languageserver";
import {DocumentCache, GlobalCache} from "../cache";
import {DidChangeConfigurationNotification} from "vscode-languageserver/node";
import {CONFIGURATION_SECTION_NAME} from "common/utils";


export interface Settings {
    language: "zh-hans" | "en-us";
    maxNumberOfProblems: number;
}


type OnInitializeHandlerType = Parameters<Connection["onInitialize"]>[0];

type OnInitializedHandlerType = Parameters<Connection["onInitialized"]>[0];

type OnDidChangeConfigurationHandlerType = Parameters<Connection["onDidChangeConfiguration"]>[0];


const DEFAULT_SETTINGS: Settings = {
    language: "en-us",
    maxNumberOfProblems: 100
};


export class SettingsManager {
    private settings: Settings = {...DEFAULT_SETTINGS};

    constructor(
        private connection: Connection,
        private docCache: DocumentCache,
        private globalCache: GlobalCache
    ) {
    }

    public onInitialize(...[params]: Parameters<OnInitializeHandlerType>) {
        this.globalCache.flag.workspaceCfg = !!params.capabilities.workspace?.configuration;

        const result: InitializeResult = {
            capabilities: {
                // 悬停提示
                hoverProvider: false
            }
        };

        if (this.globalCache.flag.workspaceCfg)
            result.capabilities.workspace = {workspaceFolders: {supported: true}};

        // IC10Local 未定义时静默跳过
        if (typeof IC10Local !== "undefined" && IC10Local.setLanguage)
            IC10Local.setLanguage(this.settings.language);

        return result;
    }

    public async onInitialized(...[params]: Parameters<OnInitializedHandlerType>) {
        this.connection.client.register(DidChangeConfigurationNotification.type);
        this.connection.workspace.onDidChangeWorkspaceFolders(e => {
        });

        if (this.globalCache.flag.workspaceCfg)
            this.connection.workspace.getConfiguration(CONFIGURATION_SECTION_NAME).then(cfg => {
                this.settings = {...this.settings, ...cfg};

                // IC10Local 未定义时静默跳过
        if (typeof IC10Local !== "undefined" && IC10Local.setLanguage)
            IC10Local.setLanguage(this.settings.language);
            });
    }

    public async onDidChangeConfiguration(...[params]: Parameters<OnDidChangeConfigurationHandlerType>) {
        const change: Promise<Settings> | Settings = this.globalCache.flag.workspaceCfg
            ? this.connection.workspace.getConfiguration(CONFIGURATION_SECTION_NAME)
            : params.settings?.[CONFIGURATION_SECTION_NAME] || {};

        if (change) {
            this.settings = {...this.settings, ...(change instanceof Promise ? await change : change)};

//            // IC10Local 未定义时静默跳过
        if (typeof IC10Local !== "undefined" && IC10Local.setLanguage)
            IC10Local.setLanguage(this.settings.language);

            this.docCache.invalidateHash(this.globalCache.uri);
        }
    }
}
