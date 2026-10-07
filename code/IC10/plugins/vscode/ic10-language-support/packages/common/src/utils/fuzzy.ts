/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file fuzzy.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 21:30
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Optional } from "../types/utils";


/**
 * @if zh
 * @brief 词首判定用的分隔符
 *
 * @desc 命中这些字符之后的字符算"词首"，加分更高：用户敲 `cs` 期望先看到 `Chemistry Station`
 * 这类词首匹配，而不是词中被硬凑出来的子序列。
 *
 * @else
 * @brief Separators used for the word-start test
 *
 * @desc A character right after one of these counts as a word start and scores higher: someone typing
 * `cs` expects `Chemistry Station` before an accidental mid-word subsequence.
 *
 * @endif
 * */
const WORD_SEPARATORS = new Set([" ", "-", "_", ".", "/", ":", ",", "(", ")", "[", "]", "\t"]);

/**
 * @if zh
 * @brief 命中一个字符的基础分
 *
 * @else
 * @brief Base score for one matched character
 *
 * @endif
 * */
const BASE_SCORE = 1;

/**
 * @if zh
 * @brief 连续命中的递增加分（第 n 个连续字符加 `n * CONTINUITY_BONUS`）
 *
 * @else
 * @brief Incremental bonus for consecutive matches (the n-th one adds `n * CONTINUITY_BONUS`)
 *
 * @endif
 * */
const CONTINUITY_BONUS = 2;

/**
 * @if zh
 * @brief 命中词首的加成
 *
 * @else
 * @brief Bonus for hitting a word start
 *
 * @endif
 * */
const BOUNDARY_BONUS = 3;

/**
 * @if zh
 * @brief 未命中字符的惩罚系数（每 8 个未命中字符扣 1 分）
 *
 * @desc 让更短、更贴近前缀的文本排在前面：同为子序列命中时，`Vent` 应当排在 `Ventilation` 之前。
 *
 * @else
 * @brief Penalty divisor for unmatched characters (one point per 8)
 *
 * @desc Shorter, more prefix-like texts rank first: on equal matches `Vent` should beat
 * `Ventilation`.
 *
 * @endif
 * */
const LENGTH_PENALTY = 8;

/**
 * @if zh
 * @brief 对一段文本做模糊匹配打分
 *
 * @details 匹配是**子序列**式的（大小写不敏感）：查询串的字符按顺序出现在文本里即算命中，
 *          因此 `lqs` 能命中 `Liquid`、`微波` 能命中 `微波炉`。分数由三部分组成——基础分、
 *          连续命中与词首命中的加成、未命中字符的少量惩罚；文本里找不到完整子序列时返回
 *          `undefined`。空查询视为"全部命中"（返回 0），便于调用方按原顺序列出候选。
 *
 * @param query 用户输入
 * @param text 待匹配的文本
 * @return 分数（越大越靠前）；不匹配时为 `undefined`
 *
 * @else
 * @brief Score one text against a fuzzy query
 *
 * @details Matching is **subsequence**-based and case-insensitive: the query's characters only have
 *          to appear in order, so `lqs` hits `Liquid` and `微波` hits `微波炉`. The score combines a
 *          base score, bonuses for consecutive and word-start hits, and a small penalty for unmatched
 *          characters; it returns `undefined` when the text does not contain the whole subsequence.
 *          An empty query matches everything (score 0) so callers can list candidates in order.
 *
 * @param query User input
 * @param text Text to match against
 * @return The score (higher ranks first), or `undefined` when it does not match
 *
 * @endif
 * */
export function fuzzyScore(query: string, text: string): Optional<number> {
    const needle = query.trim().toLowerCase();
    const haystack = text.toLowerCase();

    if (!needle) return 0;
    if (!haystack) return undefined;

    let score = 0;
    let cursor = -1;
    let run = 0;

    for (const char of needle) {
        const found = haystack.indexOf(char, cursor + 1);

        if (found < 0) return undefined;

        run = found === cursor + 1 ? run + 1 : 0;
        score += BASE_SCORE + run * CONTINUITY_BONUS;

        if (found === 0 || WORD_SEPARATORS.has(haystack[found - 1])) score += BOUNDARY_BONUS;

        cursor = found;
    }

    return score - Math.floor((haystack.length - needle.length) / LENGTH_PENALTY);
}

/**
 * @if zh
 * @brief 在多个候选文本里取最高分
 *
 * @details 索引项同时有"本地化显示名""类型名""描述"三段文本：用户可能记得任意一段，
 *          因此取三者中最好的那个分数；三段全不命中才算不命中。
 *
 * @param query 用户输入
 * @param texts 候选文本（可含 `undefined`，会被跳过）
 * @return 最高分；全部不匹配时为 `undefined`
 *
 * @else
 * @brief Take the best score across several candidate texts
 *
 * @details An index entry exposes a localized display name, a type name and a description: a user may
 *          remember any of them, so the best of the three wins, and only a miss on all of them counts
 *          as a miss.
 *
 * @param query User input
 * @param texts Candidate texts (`undefined` entries are skipped)
 * @return The best score, or `undefined` when nothing matches
 *
 * @endif
 * */
export function fuzzyScoreAny(query: string, ...texts: Optional<string>[]): Optional<number> {
    let best: Optional<number>;

    for (const text of texts) {
        if (!text) continue;

        const score = fuzzyScore(query, text);

        if (score === undefined) continue;
        if (best === undefined || score > best) best = score;
    }

    return best;
}
