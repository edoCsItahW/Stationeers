// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file state.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:23
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionTriggerKind } from "vscode-languageserver";


/**
 * @summary 光标相对于 token 的位置状态
 *
 * @summary Cursor position state relative to surrounding tokens
 *
 * @desc 描述光标在当前行中相对于已有 token 的位置关系，用于补全状态机决策。
 * 由 handle() 方法根据 findRangeTokens 返回的 prev/curr/next 索引推导得出。
 *
 * @desc Describes the cursor's positional relationship to existing tokens on the
 * current line, used by the completion state machine for dispatch decisions.
 * Derived in handle() from the prev/curr/next indices returned by findRangeTokens.
 * */
export enum RelativeState {
    /** @summary 位于一个单词中间 / Inside a word (cursor splits a token) */
    INSIDE_WORD = 0,
    /** @summary 位于一个单词的结束位置 / At the end of a word */
    END_WORD,
    /** @summary 位于一个单词的起始位置 / At the start of a new word */
    START_WORD,
    /** @summary 位于两个单词的空隙处 / In a gap between words */
    INSIDE_GAP
}

const REL_SHIFT = 2;
const TRIG_MASK = 0x03;

/**
 * @summary 将 RelativeState 与 CompletionTriggerKind 打包为单一状态值
 *
 * @summary Pack RelativeState and CompletionTriggerKind into a single state value
 *
 * @desc 使用位运算将两个枚举组合：高 2 位存 RelativeState，低 2 位存 CompletionTriggerKind。
 * 组合后的值用于 State 枚举的 switch 分发。
 *
 * @desc Packs two enums via bit operations: upper 2 bits for RelativeState,
 * lower 2 bits for CompletionTriggerKind. The combined value drives the
 * State enum's switch dispatch.
 * */
export function combine(rel: RelativeState, trig: CompletionTriggerKind) {
    return (rel << 2) | trig;
}

/**
 * @summary 从组合状态值中拆分出 RelativeState 与 CompletionTriggerKind
 *
 * @summary Unpack RelativeState and CompletionTriggerKind from a combined state value
 *
 * @desc combine 的逆操作，用于在 switch 分支内按需提取原始分量。
 *
 * @desc Inverse of combine(), used within switch branches to extract
 * original components on demand.
 * */
export function decompose(state: State): [RelativeState, CompletionTriggerKind] {
    return [state >> REL_SHIFT, (state & TRIG_MASK) as CompletionTriggerKind];
}

/**
 * @summary 补全状态枚举（RelativeState × CompletionTriggerKind 的笛卡尔积）
 *
 * @summary Completion state enum (Cartesian product of RelativeState × CompletionTriggerKind)
 *
 * @desc 每个 State 值由 combine() 将 RelativeState（光标位置）与
 * CompletionTriggerKind（触发方式）打包而成，handle() 方法据此用 switch
 * 分发到对应的补全逻辑分支。共 4×3=12 种状态。
 *
 * @desc Each State value is packed by combine() from RelativeState (cursor position)
 * and CompletionTriggerKind (trigger type). The handle() method uses a switch
 * to dispatch to the corresponding completion logic branch. 4×3=12 states total.
 * */
export enum State {
    // 1. INSIDE_WORD (0)
    INSIDE_WORD_INVOKED = combine(RelativeState.INSIDE_WORD, CompletionTriggerKind.Invoked),
    INSIDE_WORD_TRIGGER_CHAR = combine(RelativeState.INSIDE_WORD, CompletionTriggerKind.TriggerCharacter),
    INSIDE_WORD_TRIGGER_INCOMPLETE = combine(
        RelativeState.INSIDE_WORD,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    ),

    // 2. END_WORD (1)
    END_WORD_INVOKED = combine(RelativeState.END_WORD, CompletionTriggerKind.Invoked),
    END_WORD_TRIGGER_CHAR = combine(RelativeState.END_WORD, CompletionTriggerKind.TriggerCharacter),
    END_WORD_TRIGGER_INCOMPLETE = combine(
        RelativeState.END_WORD,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    ),

    // 3. START_WORD (2)
    START_WORD_INVOKED = combine(RelativeState.START_WORD, CompletionTriggerKind.Invoked),
    START_WORD_TRIGGER_CHAR = combine(RelativeState.START_WORD, CompletionTriggerKind.TriggerCharacter),
    START_WORD_TRIGGER_INCOMPLETE = combine(
        RelativeState.START_WORD,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    ),

    // 4. INSIDE_GAP (3)
    INSIDE_GAP_INVOKED = combine(RelativeState.INSIDE_GAP, CompletionTriggerKind.Invoked),
    INSIDE_GAP_TRIGGER_CHAR = combine(RelativeState.INSIDE_GAP, CompletionTriggerKind.TriggerCharacter),
    INSIDE_GAP_TRIGGER_INCOMPLETE = combine(
        RelativeState.INSIDE_GAP,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    )
}