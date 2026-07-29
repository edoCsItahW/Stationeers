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

import { CONFIGURATION_SECTION_NAME, Optional } from "common";
import { TOKEN_TYPES, TOKEN_MODIFIERS } from "../handlers";
import { DocumentCache, GlobalCache } from "../cache";
import { locale } from "../../locals/locale";
import { uriToPath } from "../../utils";

/**
 * @summary IC10 代码格式化配置
 *
 * @summary IC10 code formatting configuration
 *
 * @desc 定义 IC10 语言服务的代码格式化行为，与 VSCode 用户设置同步。
 * 包括缩进风格、空行保留、注释对齐、语句对齐等格式化选项。
 *
 * @desc Defines code formatting behavior for the IC10 language service,
 * synchronized with VSCode user settings. Includes indentation style,
 * blank line retention, comment alignment, statement alignment, etc.
 */
export interface FormatSettings {
    /** 是否使用 Tab 缩进（否则使用空格） */
    useTab: boolean;
    /** 空格缩进宽度（仅当 useTab 为 false 时生效） */
    indentWidth: number;
    /** 尾随注释前的空格数 */
    spacesBeforeTrailingComments: number;
    /** 最多保留的连续空行数 */
    maxEmptyLinesToKeep: number;
    /** 标签前最少空行数 */
    minEmptyLinesBeforeLabels: number;
    /** 是否对齐连续的赋值/声明语句 */
    alignConsecutiveStatements: boolean;
    /** 是否对齐尾随注释 */
    alignTrailingComments: boolean;
}

/**
 * @summary IC10 语言服务全局设置
 *
 * @summary IC10 language service global settings
 *
 * @desc 聚合语言服务所有用户可配置项，包括界面语言、诊断问题上限、
 * 项目根目录和格式化选项。通过 VSCode 的 workspace configuration 机制同步。
 *
 * @desc Aggregates all user-configurable options for the language service,
 * including UI language, diagnostic problem limit, project root directory,
 * and formatting options. Synchronized via VSCode's workspace configuration mechanism.
 */
export interface Settings {
    /** 界面语言：简体中文或英文 */
    language: "zh-hans" | "en-us";
    /** 诊断报告的最大问题数量 */
    maxNumberOfProblems: number;
    /** 项目根目录路径（可选） */
    projectRootDir: Optional<string>;
    /** 代码格式化子配置 */
    format: FormatSettings;
}

type OnInitializeHandlerType = Parameters<Connection["onInitialize"]>[0];
type OnInitializedHandlerType = Parameters<Connection["onInitialized"]>[0];
type OnDidChangeConfigurationHandlerType = Parameters<Connection["onDidChangeConfiguration"]>[0];

const DEFAULT_SETTINGS: Settings = {
    language: "en-us",
    maxNumberOfProblems: 100,
    projectRootDir: undefined,
    format: {
        useTab: false,
        indentWidth: 4,
        spacesBeforeTrailingComments: 2,
        maxEmptyLinesToKeep: 1,
        minEmptyLinesBeforeLabels: 0,
        alignConsecutiveStatements: true,
        alignTrailingComments: true
    }
};

/**
 * @class SettingsManager
 *
 * @summary 设置管理器，负责聚合 VSCode 配置同步、LSP 能力注册和语言环境切换
 *
 * @summary Settings manager that aggregates VSCode configuration synchronization,
 * LSP capability registration, and locale switching
 *
 * @desc 作为语言服务器与 VSCode 配置系统之间的桥梁。管理三类职责：
 * 1. LSP 初始化（onInitialize）：声明服务器能力（悬停、诊断、语义令牌、补全等）
 * 2. 配置同步（onInitialized / onDidChangeConfiguration）：从 VSCode 拉取用户设置并合并
 * 3. 语言环境管理：同步 IC10Local 和 locale 的语言设置，并在语言变更时通知上层回调
 *
 * @desc Serves as the bridge between the language server and VSCode's configuration
 * system. Manages three responsibilities:
 * 1. LSP initialization (onInitialize): declares server capabilities (hover, diagnostics,
 *    semantic tokens, completion, etc.)
 * 2. Configuration sync (onInitialized / onDidChangeConfiguration): pulls user settings
 *    from VSCode and merges them
 * 3. Locale management: syncs IC10Local and locale language settings, and notifies
 *    upper-layer callbacks on language changes
 */
export class SettingsManager {
    private settings: Settings = { ...DEFAULT_SETTINGS };

    /**
     * @constructor SettingsManager
     *
     * @summary 创建设置管理器实例
     *
     * @summary Creates a settings manager instance
     *
     * @param connection - LSP 连接实例，用于与客户端通信
     * @param connection - LSP connection instance for client communication
     * @param docCache - 文档缓存，配置变更时用于失效哈希
     * @param docCache - Document cache, used to invalidate hashes on config change
     * @param globalCache - 全局缓存，存储工作区标志和当前 URI
     * @param globalCache - Global cache, stores workspace flags and current URI
     * @param onLocaleChanged - 语言变更回调，触发时通知上层重新解析
     * @param onLocaleChanged - Locale change callback, notifies upper layer to re-parse on trigger
     */
    constructor(
        private connection: Connection,
        private docCache: DocumentCache,
        private globalCache: GlobalCache,
        private onLocaleChanged?: () => void
    ) {}

    /**
     * @summary 获取项目根目录路径
     *
     * @summary Gets the project root directory path
     *
     * @returns 项目根目录的本地文件系统路径，未设置时返回 undefined
     * @returns Local filesystem path of the project root, or undefined if not set
     */
    public getProjectRootDir(): string | undefined {
        return this.settings.projectRootDir;
    }

    /**
     * @summary 获取格式化配置（转换为 FormattingHandler 所需的格式）
     *
     * @summary Gets formatting configuration (converted to FormattingHandler-compatible format)
     *
     * @desc 将内部 Settings.format 转换为 FormatConfig 兼容结构，供 FormattingHandler 使用。
     * 此转换封装了 useTab → indent 的映射逻辑。
     *
     * @desc Converts internal Settings.format to a FormatConfig-compatible structure
     * for use by FormattingHandler. This conversion encapsulates the useTab → indent mapping logic.
     *
     * @returns 与 FormattingHandler 兼容的部分格式化配置对象
     * @returns Partial formatting configuration object compatible with FormattingHandler
     */
    public getFormatConfig() {
        const f = this.settings.format;
        return {
            indent: f.useTab ? { useTab: true as const } : { useTab: false as const, width: f.indentWidth },
            spacesBeforeTrailingComments: f.spacesBeforeTrailingComments,
            maxEmptyLinesToKeep: f.maxEmptyLinesToKeep,
            minEmptyLinesBeforeLabels: f.minEmptyLinesBeforeLabels,
            alignConsecutiveStatements: f.alignConsecutiveStatements,
            alignTrailingComments: f.alignTrailingComments
        };
    }

    /**
     * @summary LSP 初始化处理器：声明服务器能力
     *
     * @summary LSP initialize handler: declares server capabilities
     *
     * @desc 在 LSP 握手阶段调用，向客户端声明语言服务器支持的功能：
     * 悬停提示、诊断、语义令牌、代码补全、签名帮助、格式化等。
     * 同时检测客户端是否支持 workspace/configuration 以决定配置同步策略。
     *
     * @desc Called during the LSP handshake phase, declares supported features
     * to the client: hover, diagnostics, semantic tokens, completion, signature
     * help, formatting, etc. Also detects whether the client supports
     * workspace/configuration to determine the configuration sync strategy.
     *
     * @param capabilities - 客户端能力信息
     * @param capabilities - Client capability information
     * @returns 服务器能力声明结果
     * @returns Server capability declaration result
     */
    public onInitialize(...[{ capabilities }]: Parameters<OnInitializeHandlerType>) {
        this.globalCache.flag.workspaceCfg = !!capabilities.workspace?.configuration;

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
                    range: true
                },

                // 代码补全
                completionProvider: {
                    resolveProvider: true,
                    triggerCharacters: [
                        " ", "a", "b", "c", "d",
                        "e", "f", "g", "h", "i",
                        "j", "l", "m", "n", "o",
                        "p", "r", "s", "t", "x",
                        "y"
                    ]
                },

                // 签名帮助
                signatureHelpProvider: {
                    triggerCharacters: [" "],
                    retriggerCharacters: [" "]
                },

                // 代码格式化
                documentFormattingProvider: true
            }
        };

        if (this.globalCache.flag.workspaceCfg)
            result.capabilities.workspace = { workspaceFolders: { supported: true } };

        this.settingGlobalLocale(this.settings.language);

        return result;
    }

    /**
     * @summary LSP 初始化完成处理器：同步配置并注册事件监听
     *
     * @summary LSP initialized handler: syncs configuration and registers event listeners
     *
     * @desc 在 LSP 握手完成后调用，执行配置相关的初始化工作：
     * 1. 注册 DidChangeConfiguration 通知监听
     * 2. 注册工作区文件夹变更监听（用于自动更新 projectRootDir）
     * 3. 从 VSCode 拉取当前 workspace 配置并合并到内部设置
     * 4. 获取当前工作区文件夹作为项目根目录
     *
     * @desc Called after LSP handshake completes, performs configuration-related
     * initialization:
     * 1. Registers DidChangeConfiguration notification listener
     * 2. Registers workspace folder change listener (auto-updates projectRootDir)
     * 3. Pulls current workspace configuration from VSCode and merges into internal settings
     * 4. Retrieves current workspace folder as project root directory
     */
    public async onInitialized(...[]: Parameters<OnInitializedHandlerType>) {
        this.connection.client.register(DidChangeConfigurationNotification.type);
        this.connection.workspace.onDidChangeWorkspaceFolders(e => {
            if (e.added.length > 0) this.settings.projectRootDir = uriToPath(e.added[0].uri);
        });

        if (this.globalCache.flag.workspaceCfg)
            this.connection.workspace.getConfiguration(CONFIGURATION_SECTION_NAME).then(cfg => {
                this.settings = { ...this.settings, ...cfg };

                this.settingGlobalLocale(this.settings.language);

                // 语言变更后触发重新解析，确保诊断消息使用正确语言
                this.onLocaleChanged?.();
            });

        const workspaceFolders = await this.connection.workspace.getWorkspaceFolders();
        if (workspaceFolders) this.settings.projectRootDir = uriToPath(workspaceFolders[0].uri);
    }

    /**
     * @summary 配置变更处理器：当用户修改 VSCode 设置时同步更新
     *
     * @summary Configuration change handler: syncs when user modifies VSCode settings
     *
     * @desc 当用户在 VSCode 中修改 IC10 相关设置时被调用。根据客户端是否支持
     * workspace/configuration 采用不同的设置获取方式：
     * - 支持时：主动调用 workspace.getConfiguration 拉取最新设置
     * - 不支持时：从事件参数中提取设置
     * 更新设置后会失效文档缓存并触发语言变更回调，确保后续诊断使用新语言。
     *
     * @desc Called when the user modifies IC10-related settings in VSCode. Uses
     * different setting retrieval strategies based on whether the client supports
     * workspace/configuration:
     * - Supported: actively calls workspace.getConfiguration to pull latest settings
     * - Not supported: extracts settings from event parameters
     * After updating settings, invalidates document cache and triggers locale change
     * callback to ensure subsequent diagnostics use the new language.
     *
     * @param settings - 客户端推送的变更后设置
     * @param settings - Changed settings pushed by the client
     */
    public async onDidChangeConfiguration(...[{ settings }]: Parameters<OnDidChangeConfigurationHandlerType>) {
        const change: Promise<Settings> | Settings = this.globalCache.flag.workspaceCfg
            ? this.connection.workspace.getConfiguration(CONFIGURATION_SECTION_NAME)
            : settings?.[CONFIGURATION_SECTION_NAME] || {};

        if (change) {
            this.settings = { ...this.settings, ...(change instanceof Promise ? await change : change) };

            this.settingGlobalLocale(this.settings.language);

            this.docCache.invalidateHash(this.globalCache.uri);

            // 语言变更后触发重新解析，确保诊断消息使用正确语言
            this.onLocaleChanged?.();
        }
    }

    /**
     * @internal 内部方法：同步全局语言环境设置
     *
     * @internal Internal method: synchronizes global locale settings
     *
     * @desc 同时更新 IC10Local（ic10-node-api 库的语言）和 locale（UI 本地化模块），
     * 确保诊断消息、悬停提示等所有输出使用用户选择的语言。
     *
     * @desc Simultaneously updates IC10Local (language for ic10-node-api library) and
     * locale (UI localization module), ensuring all outputs like diagnostic messages
     * and hover hints use the user-selected language.
     *
     * @param lang - 语言标识
     * @param lang - Language identifier
     */
    private settingGlobalLocale(lang: Settings["language"]) {
        IC10Local.setLanguage(lang);

        locale.setLocale(lang);
    }
}
