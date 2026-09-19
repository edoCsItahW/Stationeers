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
import { createConnection, TextDocuments, ProposedFeatures, MessageType } from "vscode-languageserver/node";
import { TextDocument } from "vscode-languageserver-textdocument";
import type { Connection } from "vscode-languageserver/node";

import { ParserPipline, SettingsManager } from "./services";
import { DocumentCache, GlobalCache } from "./cache";
import { Console, COMM_EVENT_NAME, AstRequestEventData, AstResponseEventData } from "common";
import { t } from "../locals";
import {
    SemanticTokenHandler,
    CompletionHandler,
    DiagnosticHandler,
    SignatureHandler,
    FormattingHandler,
    HoverHandler
} from "./handlers";

type OnInitializeHandlerType = Parameters<Connection["onInitialize"]>[0];
type OnInitializedHandlerType = Parameters<Connection["onInitialized"]>[0];
type OnDidChangeConfigurationHandlerType = Parameters<Connection["onDidChangeConfiguration"]>[0];
type OnDidOpenHandlerType = Parameters<TextDocuments<TextDocument>["onDidOpen"]>[0];
type OnDidChangeContentHandlerType = Parameters<TextDocuments<TextDocument>["onDidChangeContent"]>[0];

/**
 * @class Server
 *
 * @summary IC10 语言服务器主入口，负责连接管理和请求路由
 *
 * @summary IC10 language server main entry point, responsible for connection
 * management and request routing
 *
 * @desc 作为整个语言服务器的启动入口和中央编排器。负责：
 * 1. 创建和管理 LSP Connection 及 TextDocuments 实例
 * 2. 初始化所有子系统：缓存（DocumentCache / GlobalCache）、设置管理器、
 *    解析流水线和各类处理器（悬停、诊断、语义令牌、补全、签名帮助、格式化）
 * 3. 在 `run()` 中注册所有 LSP 事件处理器并将请求路由到对应的子系统
 * 4. 管理解析版本号机制以防止异步解析的过期结果覆盖最新状态
 *
 * @desc Serves as the startup entry point and central orchestrator of the entire
 * language server. Responsibilities:
 * 1. Creates and manages LSP Connection and TextDocuments instances
 * 2. Initializes all subsystems: caches (DocumentCache / GlobalCache), settings
 *    manager, parsing pipeline, and handlers (hover, diagnostics, semantic tokens,
 *    completion, signature help, formatting)
 * 3. Registers all LSP event handlers in `run()` and routes requests to the
 *    corresponding subsystem
 * 4. Manages a parse version counter to prevent stale async parse results from
 *    overwriting the latest state
 */
export class Server {
    private readonly semanticHandler: SemanticTokenHandler;
    private readonly diagHandler: DiagnosticHandler;
    private readonly hoverHandler: HoverHandler;
    private readonly globalCache: GlobalCache;
    private readonly docCache: DocumentCache;
    private readonly compHandler: CompletionHandler;
    private readonly signatureHandler: SignatureHandler;
    private readonly fmtHandler: FormattingHandler;
    private settingMgr: SettingsManager;
    private pipline: ParserPipline;
    /** 解析版本号，用于丢弃过期结果 */
    private parseVersion = 0;

    /**
     * @constructor Server
     *
     * @summary 创建语言服务器实例，初始化所有子系统
     *
     * @summary Creates a language server instance, initializes all subsystems
     *
     * @desc 构造函数中完成所有子系统的创建和依赖注入：
     * - 创建文档缓存和全局缓存
     * - 创建设置管理器（注入语言变更回调 handleLocaleChanged）
     * - 创建各类 LSP 功能处理器（悬停、诊断、语义令牌、补全、签名帮助、格式化）
     * - 创建解析流水线
     * 格式化处理器通过 pluginConfigProvider 和 projectRootDirProvider 回调
     * 从设置管理器动态获取最新配置。
     *
     * @desc The constructor completes all subsystem creation and dependency injection:
     * - Creates document cache and global cache
     * - Creates settings manager (injects locale change callback handleLocaleChanged)
     * - Creates all LSP feature handlers (hover, diagnostics, semantic tokens,
     *   completion, signature help, formatting)
     * - Creates the parsing pipeline
     * The formatting handler dynamically obtains the latest configuration from
     * the settings manager via pluginConfigProvider and projectRootDirProvider callbacks.
     *
     * @param connection - LSP 连接实例，默认使用 ProposedFeatures.all 创建
     * @param connection - LSP connection instance, defaults to createConnection(ProposedFeatures.all)
     * @param documents - 文本文档管理器，默认创建新的 TextDocuments 实例
     * @param documents - Text document manager, defaults to creating a new TextDocuments instance
     */
    constructor(
        private readonly connection: Connection = createConnection(ProposedFeatures.all),
        private readonly documents: TextDocuments<TextDocument> = new TextDocuments(TextDocument)
    ) {
        this.docCache = new DocumentCache();

        this.globalCache = new GlobalCache();
        this.settingMgr = new SettingsManager(connection, this.docCache, this.globalCache, () =>
            this.handleLocaleChanged()
        );

        this.hoverHandler = new HoverHandler(this.docCache, this.settingMgr);
        this.diagHandler = new DiagnosticHandler(this.docCache);
        this.semanticHandler = new SemanticTokenHandler(this.docCache);
        this.compHandler = new CompletionHandler(this.docCache);
        this.signatureHandler = new SignatureHandler(this.docCache);
        this.fmtHandler = new FormattingHandler(this.docCache, {
            pluginConfigProvider: () => this.settingMgr.getFormatConfig(),
            projectRootDirProvider: () => this.settingMgr.getProjectRootDir(),
            onConfigError: message => {
                this.connection.sendNotification("window/showMessage", {
                    type: MessageType.Warning,
                    message
                });
            }
        });

        this.pipline = new ParserPipline();
    }

    /**
     * @summary 启动语言服务器，注册所有 LSP 事件处理器并开始监听
     *
     * @summary Starts the language server, registers all LSP event handlers,
     * and begins listening
     *
     * @desc 服务器启动的唯一入口。按顺序执行：
     * 1. 注册 LSP 生命周期事件（onInitialize / onInitialized / onDidChangeConfiguration）
     * 2. 注册 LSP 功能处理器（悬停、诊断、语义令牌、补全、签名帮助、格式化）
     * 3. 注册文档事件监听（onDidOpen / onDidChangeContent）
     * 4. 启动文档监听器和连接监听器
     *
     * @desc The sole entry point for server startup. Executes in order:
     * 1. Registers LSP lifecycle events (onInitialize / onInitialized / onDidChangeConfiguration)
     * 2. Registers LSP feature handlers (hover, diagnostics, semantic tokens,
     *    completion, signature help, formatting)
     * 3. Registers document event listeners (onDidOpen / onDidChangeContent)
     * 4. Starts document listener and connection listener
     *
     * @remarks 所有事件处理器使用 .bind() 确保正确的 this 上下文。
     * parseAndRefresh 中对过期结果的丢弃机制依赖 parseVersion 计数器。
     *
     * @remarks All event handlers use .bind() to ensure correct this context.
     * The stale result discard mechanism in parseAndRefresh relies on the
     * parseVersion counter.
     */
    run() {
        Console.info(t("server.info.start"), "Server");

        try {
            // 初始化
            this.connection.onInitialize(this.onInitialize.bind(this));
            this.connection.onInitialized(this.onInitialized.bind(this));

            // 事件监听
            this.connection.onDidChangeConfiguration(this.onDidChangeConfiguration.bind(this));

            // LSP功能
            this.connection.onHover(this.hoverHandler.handle.bind(this.hoverHandler));
            this.connection.languages.diagnostics.on(this.diagHandler.handle.bind(this.diagHandler));
            this.connection.languages.semanticTokens.on(this.semanticHandler.handle.bind(this.semanticHandler));
            this.connection.languages.semanticTokens.onRange(
                this.semanticHandler.handleRange.bind(this.semanticHandler)
            );
            this.connection.onCompletion(this.compHandler.handle.bind(this.compHandler));
            this.connection.onCompletionResolve(this.compHandler.handleResolve.bind(this.compHandler));
            this.connection.onSignatureHelp(this.signatureHandler.handle.bind(this.signatureHandler));
            this.connection.onDocumentFormatting(this.fmtHandler.handle.bind(this.fmtHandler));

            // 文档监听
            this.documents.onDidOpen(this.onDidOpen.bind(this));
            this.documents.onDidChangeContent(this.onDidChangeContent.bind(this));

            // 启动监听
            this.documents.listen(this.connection);
            this.connection.listen();

            // @ts-ignore
            this.connection.onRequest(
                COMM_EVENT_NAME,
                async (data: AstRequestEventData): Promise<AstResponseEventData> => {
                    const cache = this.docCache.getCache(data.data.uri);
                    return {
                        type: "ast",
                        data: cache ? { source: cache.source } : undefined
                    };
                }
            );
        } catch (error) {
            Console.error((error as Error).message, "Server");
        }
    }

    /**
     * @internal 委托给 SettingsManager 的 LSP 初始化处理
     *
     * @internal Delegates LSP initialization handling to SettingsManager
     */
    private onInitialize(...args: Parameters<OnInitializeHandlerType>): ReturnType<OnInitializeHandlerType> {
        return this.settingMgr.onInitialize(...args);
    }

    /**
     * @internal 委托给 SettingsManager 的 LSP 初始化完成处理
     *
     * @internal Delegates LSP initialized handling to SettingsManager
     */
    private onInitialized(...args: Parameters<OnInitializedHandlerType>): ReturnType<OnInitializedHandlerType> {
        return this.settingMgr.onInitialized(...args);
    }

    /**
     * @internal 委托给 SettingsManager 的配置变更处理
     *
     * @internal Delegates configuration change handling to SettingsManager
     */
    private onDidChangeConfiguration(
        ...args: Parameters<OnDidChangeConfigurationHandlerType>
    ): ReturnType<OnDidChangeConfigurationHandlerType> {
        return this.settingMgr.onDidChangeConfiguration(...args);
    }

    /**
     * @internal 文档打开事件处理器：初始化文档缓存并触发首次解析
     *
     * @internal Document open event handler: initializes document cache and
     * triggers initial parse
     *
     * @desc 当用户在编辑器中打开一个 IC10 文件时被调用。执行流程：
     * 1. 在 DocumentCache 中初始化该文档的缓存条目
     * 2. 将该文档 URI 设置为全局当前 URI
     * 3. 调用 parseAndRefresh 进行全量解析并刷新诊断和语义高亮
     *
     * @desc Called when the user opens an IC10 file in the editor. Flow:
     * 1. Initializes the document's cache entry in DocumentCache
     * 2. Sets this document URI as the global current URI
     * 3. Calls parseAndRefresh for full parsing and refreshes diagnostics/semantic tokens
     */
    private onDidOpen(...[{ document }]: Parameters<OnDidOpenHandlerType>) {
        this.docCache.initDocument(document.uri);
        this.globalCache.uri = document.uri;

        this.parseAndRefresh(document.getText());
    }

    /**
     * @internal 文档内容变更事件处理器：增量解析变更后的代码
     *
     * @internal Document content change event handler: incrementally parses
     * the changed code
     *
     * @desc 当用户在编辑器中修改 IC10 代码时被调用。更新全局当前 URI 后调用
     * parseAndRefresh，内部使用 ParserPipline.parseInc 进行增量解析，
     * 仅重新分析变更区域，大幅降低编辑延迟。
     *
     * @desc Called when the user modifies IC10 code in the editor. Updates the
     * global current URI then calls parseAndRefresh, which internally uses
     * ParserPipline.parseInc for incremental parsing, re-analyzing only changed
     * regions to significantly reduce editing latency.
     */
    private onDidChangeContent(
        ...[{ document }]: Parameters<OnDidChangeContentHandlerType>
    ): ReturnType<OnDidChangeContentHandlerType> {
        this.globalCache.uri = document.uri;

        this.parseAndRefresh(document.getText());
    }

    /**
     * @internal 异步解析代码并刷新诊断和语义高亮
     *
     * @internal Asynchronously parses code and refreshes diagnostics and
     * semantic tokens
     *
     * @desc 核心解析调度方法。执行流程：
     * 1. 递增解析版本号（parseVersion），用于标记本次解析请求
     * 2. 调用 ParserPipline.parseInc 进行增量解析
     * 3. 成功时：检查版本号是否仍为最新（防止过期结果覆盖最新状态），
     *    更新 DocumentCache，然后刷新诊断和语义高亮
     * 4. 失败时（增量解析失败）：自动回退到 ParserPipline.parse 全量解析，
     *    同样进行版本号校验后更新缓存并刷新
     *
     * @desc Core parse scheduling method. Flow:
     * 1. Increments parse version (parseVersion) to tag this parse request
     * 2. Calls ParserPipline.parseInc for incremental parsing
     * 3. On success: checks if version is still the latest (prevents stale
     *    results from overwriting latest state), updates DocumentCache, then
     *    refreshes diagnostics and semantic tokens
     * 4. On failure (incremental parse failed): automatically falls back to
     *    ParserPipline.parse for full parsing, also checks version before
     *    updating cache and refreshing
     *
     * @param code - 当前文档的完整内容
     * @param code - Complete content of the current document
     *
     * @remarks 版本号机制是关键的并发安全保证。当用户快速连续编辑时，
     * 多个异步解析可能同时执行，版本号校验确保只有最新请求的结果被应用。
     *
     * @remarks The version counter is a critical concurrency safety guarantee.
     * When the user edits rapidly in succession, multiple async parses may run
     * concurrently; the version check ensures only the latest request's result
     * is applied.
     */
    private parseAndRefresh(code: string) {
        const uri = this.globalCache.uri;
        const version = ++this.parseVersion;

        this.pipline
            .parseInc(code, this.docCache.getCache(uri))
            .then(res => {
                if (version !== this.parseVersion) return; // 过期结果，丢弃
                if (!res.changed) return;

                this.docCache.updateAfterParse(uri, res);

                this.connection.languages.diagnostics.refresh();
                this.connection.languages.semanticTokens.refresh();
            })
            .catch(err => {
                Console.error(t("server.parser.info.LIE1", { err: err.message }));
                // 增量解析失败时回退到全量解析
                this.pipline
                    .parse(code, this.docCache.getCache(uri))
                    .then(res => {
                        if (version !== this.parseVersion) return;
                        if (!res.changed) return;

                        this.docCache.updateAfterParse(uri, res);

                        this.connection.languages.diagnostics.refresh();
                        this.connection.languages.semanticTokens.refresh();
                    })
                    .catch(err2 => {
                        Console.error(t("server.parser.info.LIE2", { err: err2.message }));
                    });
            });
    }

    /**
     * @internal 语言变更回调：失效缓存并重新解析当前文档
     *
     * @internal Locale change callback: invalidates cache and re-parses
     * the current document
     *
     * @desc 当用户在 VSCode 设置中切换语言时由 SettingsManager 回调触发。
     * 失效当前文档的缓存哈希以强制重新解析（即使代码内容未变），确保
     * 诊断消息、悬停提示等使用新语言的本地化文本。
     *
     * @desc Triggered by SettingsManager callback when the user switches language
     * in VSCode settings. Invalidates the current document's cache hash to force
     * re-parsing (even if code content hasn't changed), ensuring diagnostic
     * messages, hover hints, etc. use localized text for the new language.
     */
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
