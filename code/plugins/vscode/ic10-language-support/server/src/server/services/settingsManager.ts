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
import { DidChangeConfigurationNotification } from "vscode-languageserver/node";
import type { Connection, InitializeResult } from "vscode-languageserver";
import { IC10Local } from "ic10-node-api";

import { TOKEN_TYPES, TOKEN_MODIFIERS } from "../handlers/semanticToken";
import { CONFIGURATION_SECTION_NAME } from "../../../../common/utils";
import { DocumentCache, GlobalCache } from "../cache";
import { locale } from "../../locals/locale";


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
    private settings: Settings = { ...DEFAULT_SETTINGS };

    constructor(
        private connection: Connection,
        private docCache: DocumentCache,
        private globalCache: GlobalCache,
        private onLocaleChanged?: () => void
    ) {
    }

    public onInitialize(...[params]: Parameters<OnInitializeHandlerType>) {
        this.globalCache.flag.workspaceCfg = !!params.capabilities.workspace?.configuration;

        const result: InitializeResult = {
            capabilities: {
                // 悬停提示
                hoverProvider: true,

                // 文档诊断
                diagnosticProvider: {
                    interFileDependencies: false,
                    workspaceDiagnostics: false
                },

                // 语义令牌
                semanticTokensProvider: {
                    legend: {
                        tokenTypes: TOKEN_TYPES,
                        tokenModifiers: TOKEN_MODIFIERS
                    },
                    full: {
                        delta: false
                    },
                    range: false
                }
            }
        };

        if (this.globalCache.flag.workspaceCfg)
            result.capabilities.workspace = { workspaceFolders: { supported: true } };

        this.settingGlobalLocale(this.settings.language);

        return result;
    }

    public async onInitialized(...[params]: Parameters<OnInitializedHandlerType>) {
        this.connection.client.register(DidChangeConfigurationNotification.type);
        this.connection.workspace.onDidChangeWorkspaceFolders(e => {
        });

        if (this.globalCache.flag.workspaceCfg)
            this.connection.workspace.getConfiguration(CONFIGURATION_SECTION_NAME).then(cfg => {
                this.settings = { ...this.settings, ...cfg };

                this.settingGlobalLocale(this.settings.language);

                // 语言变更后触发重新解析，确保诊断消息使用正确语言
                this.onLocaleChanged?.();
            });
    }

    public async onDidChangeConfiguration(...[params]: Parameters<OnDidChangeConfigurationHandlerType>) {
        const change: Promise<Settings> | Settings = this.globalCache.flag.workspaceCfg
            ? this.connection.workspace.getConfiguration(CONFIGURATION_SECTION_NAME)
            : params.settings?.[CONFIGURATION_SECTION_NAME] || {};

        if (change) {
            this.settings = { ...this.settings, ...(change instanceof Promise ? await change : change) };

            this.settingGlobalLocale(this.settings.language);

            this.docCache.invalidateHash(this.globalCache.uri);

            // 语言变更后触发重新解析，确保诊断消息使用正确语言
            this.onLocaleChanged?.();
        }
    }

    private settingGlobalLocale(lang: Settings["language"]) {
        IC10Local.setLanguage(lang);

        locale.setLocale(lang);
    }

}
