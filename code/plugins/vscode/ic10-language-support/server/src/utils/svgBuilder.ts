/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file svgBuilder.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 11:25
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { s } from "../style";

export interface TextSegment {
    text: string; // 要显示的文本
    color?: string; // CSS 颜色值，如 'red'、'#ff0000'
    underline?: boolean; // 是否下划线
    bold?: boolean; // 是否粗体
    italic?: boolean; // 是否斜体
    fontFamily?: string; // 字体，默认 'sans-serif'
    fontSize?: number; // 字号，默认 16
}

export class SvgBuilder {
    private static instance: SvgBuilder | null = null;
    private segments: TextSegment[] = [];
    private altText: string = ""; // 纯文本备份，用于复制
    private defaultFontSize = 16;
    private defaultFontFamily = "sans-serif";

    private constructor() {}

    /**
     * 获取 SvgBuilder 单例
     */
    static getInstance(): SvgBuilder {
        if (!SvgBuilder.instance) SvgBuilder.instance = new SvgBuilder();

        return SvgBuilder.instance;
    }

    /**
     * 重置构建器状态
     */
    reset(): this {
        this.segments = [];
        this.altText = "";
        return this;
    }

    /**
     * 添加一个文本片段
     */
    addSegment(segment: TextSegment): this {
        this.segments.push(segment);
        // 累积纯文本（不包含样式）
        this.altText += segment.text;
        return this;
    }

    /**
     * 批量添加（链式调用）
     */
    addSegments(segments: TextSegment[]): this {
        segments.forEach(s => this.addSegment(s));
        return this;
    }

    /**
     * 构建最终的 Markdown 图片字符串
     */
    build(): string {
        if (this.segments.length === 0) {
            this.reset();
            return ""; // 或抛出异常
        }

        const svg = this.buildSvg();
        const encoded = encodeURIComponent(svg);
        const dataUri = `data:image/svg+xml;charset=utf-8,${encoded}`;
        // 转义 alt 中的 Markdown 特殊字符
        const altEscaped = this.escapeMarkdown(this.altText);
        const result = `![${altEscaped}](${dataUri})`;
        this.reset();
        return result;
    }

    private buildSvg(): string {
        const padding = 10;
        const maxFontSize = Math.max(
            ...this.segments.map(s => s.fontSize || this.defaultFontSize),
            this.defaultFontSize
        );
        let totalWidth = 0;
        let tspans: string[] = [];
        let xPos = padding;

        for (const seg of this.segments) {
            const fs = seg.fontSize || this.defaultFontSize;
            const family = seg.fontFamily || this.defaultFontFamily;
            const color = seg.color || s("common.text") || "inherit";
            const decoration = seg.underline ? "underline" : "none";
            const weight = seg.bold ? "bold" : "normal";
            const style = seg.italic ? "italic" : "normal";

            const approxWidth = this.estimateTextWidth(seg.text, fs);
            totalWidth += approxWidth;

            const safeText = this.escapeXml(seg.text);

            const tspan = `<tspan x="${xPos}" fill="${color}" text-decoration="${decoration}" font-weight="${weight}" font-style="${style}" font-size="${fs}" font-family="${family}">${safeText}</tspan>`;
            tspans.push(tspan);
            xPos += approxWidth + 2;
        }

        const svgWidth = totalWidth + padding * 2;
        const svgHeight = maxFontSize * 2 + padding * 2;
        const middleY = svgHeight / 2;

        return `<svg xmlns="http://www.w3.org/2000/svg" width="${svgWidth}" height="${svgHeight}" xml:space="preserve">
  <text x="0" y="${middleY}" dominant-baseline="middle">
    ${tspans.join("\n    ")}
  </text>
</svg>`.trim();
    }

    private estimateTextWidth(text: string, fontSize: number): number {
        // 粗略估算：中文字符约 1.0 个em，英文约 0.6 个em
        let width = 0;
        for (const ch of text) {
            if (/[\u4e00-\u9fff]/.test(ch)) {
                width += fontSize * 0.9; // 中文略宽
            } else {
                width += fontSize * 0.55;
            }
        }
        return width;
    }

    private escapeXml(str: string): string {
        return str
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
            .replace(/'/g, "&apos;");
    }

    private escapeMarkdown(str: string): string {
        // 只转义可能破坏图片语法的字符
        return str
            .replace(/\[/g, "\\[")
            .replace(/\]/g, "\\]")
            .replace(/\(/g, "\\(")
            .replace(/\)/g, "\\)")
            .replace(/\!/g, "\\!");
    }
}

const svgBuilder = SvgBuilder.getInstance();

export default svgBuilder;
