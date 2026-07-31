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
        return this.entriesWithPrefix(prefix).map(([key]) => key);
    }

    /**
     * @summary 查找所有以 prefix 开头的键值对
     *
     * @summary Find all key-value pairs starting with prefix
     *
     * @desc 通过一次树遍历收集所有匹配前缀的键值对。
     * 处理 prefix 被压缩边部分包含的情况（prefix 长度 < 边长度）。
     *
     * @desc Collects all key-value pairs matching the prefix in a single tree traversal.
     * Handles the case where the prefix is partially contained within a compressed edge
     * (prefix length < edge length).
     *
     * @param prefix 前缀字符串
     * @param prefix Prefix string
     * @returns 匹配的 [key, value] 元组数组
     * @returns Array of matching [key, value] tuples
     */
    entriesWithPrefix(prefix: string): [string, T][] {
        const result: [string, T][] = [];
        this._collectByPrefix(this.root, prefix, "", result);
        return result;
    }

    // ---------- 私有辅助方法 ----------

    /**
     * @summary 基于前缀收集所有匹配的键值对
     *
     * @summary Collect all key-value pairs matching a prefix
     *
     * @desc 递归遍历基数树，处理两种匹配情况：
     * 1. remaining 完全消耗 → 从当前节点收集所有子项
     * 2. remaining 是 edge 的前缀 → edge 子树全部匹配
     * 3. edge 是 remaining 的前缀 → 继续深层搜索
     *
     * @desc Recursively traverses the radix tree, handling two match cases:
     * 1. remaining fully consumed → collect all entries from this node
     * 2. remaining is a prefix of edge → entire edge subtree matches
     * 3. edge is a prefix of remaining → continue deeper search
     *
     * @param node 当前节点
     * @param node Current node
     * @param remaining 剩余未匹配的前缀部分
     * @param remaining Remaining unmatched prefix portion
     * @param currentPath 从根节点到当前节点的完整路径
     * @param currentPath Full path from root to current node
     * @param result 收集结果的数组
     * @param result Array to collect results
     */
    private _collectByPrefix(
        node: RadixNode<T>,
        remaining: string,
        currentPath: string,
        result: [string, T][]
    ): void {
        if (remaining === "") {
            if (node.value !== null) result.push([currentPath, node.value]);
            for (const [edge, child] of node.children)
                this._collectByPrefix(child, "", currentPath + edge, result);
            return;
        }

        for (const [edge, child] of node.children) {
            const lcp = this._lcpLength(remaining, edge);
            if (lcp === 0) continue;

            if (lcp === remaining.length)
                // remaining 完全包含在 edge 中 → 子树全部匹配
                this._collectByPrefix(child, "", currentPath + edge, result);
            else if (lcp === edge.length)
                // edge 完全包含在 remaining 中 → 继续搜索
                this._collectByPrefix(child, remaining.slice(edge.length), currentPath + edge, result);
        }
    }

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

    private _collectEntries(node: RadixNode<T>, current: string, result: [string, T][]): void {
        if (node.value !== null) result.push([current, node.value]);

        for (const [edge, child] of node.children) this._collectEntries(child, current + edge, result);
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
