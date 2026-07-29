// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file utils.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/26 15:01
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Nullable } from "common";

interface RadixNode<T> {
    children: Map<string, RadixNode<T>>;
    value: Nullable<T>;
}

/**
 * @summary 基数树（压缩前缀树），用于高效的字符串键存储和前缀查找
 *
 * @summary Radix tree (compressed prefix tree) for efficient string-keyed storage and prefix lookup
 *
 * @desc 一种空间优化的 Trie 变体，通过合并单子节点路径压缩存储空间。
 * 支持 insert、search、delete、keysWithPrefix 操作。
 * 适用于 LSP 场景中的符号名快速查找和补全。
 *
 * @desc A space-optimized Trie variant that compresses paths with single-child nodes.
 * Supports insert, search, delete, and keysWithPrefix operations.
 * Ideal for symbol name lookup and completion in LSP contexts.
 *
 * @remarks 通过静态工厂方法 fromObject / fromMap 批量构建。内部使用递归实现。
 *
 * @remarks Bulk construction via static factory methods fromObject / fromMap. Internally recursive.
 * */
export class RadixTree<T> {
    private root: RadixNode<T> = { children: new Map(), value: null };

    constructor(entries?: Record<string, T>) {
        if (entries) for (const [key, value] of Object.entries(entries)) this.insert(key, value);
    }

    static fromObject<T>(obj: Record<string, T>): RadixTree<T> {
        const tree = new RadixTree<T>();

        for (const [key, value] of Object.entries(obj)) tree.insert(key, value);

        return tree;
    }

    static fromMap<T>(map: Map<string, T>): RadixTree<T> {
        const tree = new RadixTree<T>();

        for (const [key, value] of map) tree.insert(key, value);

        return tree;
    }

    insert(key: string, value: T): void {
        this._insert(this.root, key, value);
    }

    search(key: string): Nullable<T> {
        const node = this._search(this.root, key);

        return node ? node.value : null;
    }

    delete(key: string): boolean {
        return this._delete(this.root, key);
    }

    keysWithPrefix(prefix: string): string[] {
        const result: string[] = [];
        const node = this._search(this.root, prefix);
        if (node) this._collectKeys(node, prefix, result);

        return result;
    }

    // ---------- 私有辅助方法 ----------

    private _insert(node: RadixNode<T>, key: string, value: T): void {
        if (key === "") {
            node.value = value;
            return;
        }

        for (const [edge, child] of node.children) {
            const lcp = this._lcpLength(key, edge);
            if (lcp === 0) continue;

            if (lcp === edge.length) {
                // 边完全匹配
                if (lcp === key.length)
                    child.value = value; // 完全命中，更新值
                else this._insert(child, key.slice(lcp), value); // 递归进入子节点

                return;
            } else {
                // 需要拆分边 (lcp < edge.length)
                const common = edge.slice(0, lcp);
                const remaining = edge.slice(lcp);

                const mid: RadixNode<T> = {
                    children: new Map(),
                    value: null
                };

                // 将原子节点移到中间节点下
                mid.children.set(remaining, child);

                // 当前节点替换为新的中间节点
                node.children.delete(edge);
                node.children.set(common, mid);

                // 插入剩余部分
                if (lcp === key.length) mid.value = value;
                else this._insert(mid, key.slice(lcp), value);

                return;
            }
        }

        // 没有匹配的子边，直接添加新节点
        node.children.set(key, { children: new Map(), value });
    }

    private _search(node: RadixNode<T>, key: string): Nullable<RadixNode<T>> {
        if (key === "") return node;

        for (const [edge, child] of node.children)
            if (key.startsWith(edge)) return this._search(child, key.slice(edge.length));

        return null;
    }

    private _delete(node: RadixNode<T>, key: string): boolean {
        if (key === "") {
            if (node.value !== null) {
                node.value = null;
                return true;
            }

            return false;
        }

        for (const [edge, child] of node.children)
            if (key.startsWith(edge)) {
                const deleted = this._delete(child, key.slice(edge.length));
                if (deleted) {
                    // 删除后检查是否需要合并
                    if (child.value === null && child.children.size === 1) {
                        // 只有一个子节点，合并
                        const [[onlyEdge, onlyChild]] = [...child.children.entries()];
                        node.children.delete(edge);
                        node.children.set(edge + onlyEdge, onlyChild);
                    } else if (child.value === null && child.children.size === 0)
                        // 没有子节点且无值，移除
                        node.children.delete(edge);

                    return true;
                }

                return false;
            }

        return false;
    }

    private _collectKeys(node: RadixNode<T>, current: string, result: string[]): void {
        if (node.value !== null) result.push(current);

        for (const [edge, child] of node.children) this._collectKeys(child, current + edge, result);
    }

    private _lcpLength(a: string, b: string): number {
        let i = 0;
        const len = Math.min(a.length, b.length);

        while (i < len && a[i] === b[i]) i++;
        return i;
    }
}

/**
 * @summary 将 file:// URI 转换为本地文件系统路径
 *
 * @summary Converts file:// URI to local filesystem path
 *
 * @desc 移除 `file:///` 前缀并将 `%3A` 还原为冒号 `:`。
 * 用于 LSP 服务端接收客户端 URI 后获取磁盘路径。
 *
 * @desc Strips the `file:///` prefix and decodes `%3A` back to `:`.
 * Used by the LSP server to resolve disk paths from client URIs.
 * */
export function uriToPath(uri: string): string {
    return uri.replace("file:///", "").replace("%3A", ":");
}
