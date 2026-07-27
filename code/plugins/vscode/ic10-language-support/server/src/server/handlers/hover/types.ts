/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file types.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import type { Hover } from "vscode-languageserver/node";
import type { StatementNode, SymbolMap } from "ic10-node-api";
import {Nullable} from "../../../../../common/types/utils";

/**
 * Context passed to each hover provider.
 */
export interface HoverContext {
    line: number;
    character: number;
    symbols: Nullable<SymbolMap>;
    statements: StatementNode[];
    getLocale(): "zh-hans" | "en-us";
    t(key: any, ...args: any[]): string;
}

/**
 * A provider that can generate hover content for a specific statement type.
 */
export interface HoverProvider {
    /** Whether this provider can handle the given statement. */
    canHandle(node: StatementNode): boolean;
    /** Generate hover content. Return null if the cursor is not on a relevant position. */
    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover>;
}
