/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file pseudocode.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 16:10
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { OperandType, type Operand, type PureExeInstructionNode } from "@ic10/compiler";
import type { Optional } from "@ic10/common";

import { getDeviceTitlesByModel } from "../../services";
import { getOperandType, operandToString } from "../../../utils";


/**
 * @if zh
 * @summary 一条指令的伪代码
 *
 * @details `text` 是**单行**文本（内联提示是行内渲染的，`\n` 会被折叠），`tooltip` 给需要在提示上
 *          悬停才能看到的多行形态（例如分支的比较与跳转分两行）。
 *
 * @else
 * @summary The pseudocode of one instruction
 *
 * @details `text` is **single-line** (inlay hints render inline and a `\n` gets folded) while `tooltip`
 *          carries the multi-line form shown when hovering the hint (e.g. a branch's comparison and its jump
 *          on separate lines).
 *
 * @endif
 * */
export interface Pseudocode {
    text: string;
    tooltip?: string;
}

/**
 * @if zh
 * @summary 指令 → 伪代码模板
 *
 * @details 占位符 `{0}` 是 `operand1`、`{1}` 是 `operand2`，依次类推。模板以**可读的写法**为准：
 *          赋值用 `=`、跳转用 `goto`、条件分支用 `if … goto …`，只有设备名等操作数需要本地化，
 *          因此模板本身与语言无关，不必为中英各写一份。
 *
 * @details 第一批只覆盖常用指令，**没有模板的指令不出提示**（宁缺毋滥，也便于后续逐条补）。
 *
 * @else
 * @summary Instruction → pseudocode template
 *
 * @details The placeholder `{0}` is `operand1`, `{1}` is `operand2` and so on. Templates favour a
 *          **readable** spelling: `=` for assignment, `goto` for jumps and `if … goto …` for conditional
 *          branches. Only operands such as device names need localization, so the templates themselves stay
 *          language independent and need no Chinese/English duplicates.
 *
 * @details This first batch covers common instructions only; **instructions without a template produce no
 *          hint** (better none than a wrong one, and it leaves room to fill them in one by one).
 *
 * @endif
 * */
const TEMPLATES: Record<string, Pseudocode> = {
    // 传送与算术 / moves and arithmetic
    move: { text: "{0} = {1}" },
    add: { text: "{0} = {1} + {2}" },
    sub: { text: "{0} = {1} - {2}" },
    mul: { text: "{0} = {1} * {2}" },
    div: { text: "{0} = {1} / {2}" },
    mod: { text: "{0} = {1} % {2}" },

    // 位运算 / bitwise
    and: { text: "{0} = {1} & {2}" },
    or: { text: "{0} = {1} | {2}" },
    xor: { text: "{0} = {1} ^ {2}" },
    not: { text: "{0} = ~{1}" },
    abs: { text: "{0} = |{1}|" },

    // 设备读写 / device access
    s: { text: "{0}.{1} = {2}" },
    l: { text: "{0} = {1}.{2}" },
    ls: { text: "{0} = {1}[{2}].{3}" },
    lbn: { text: "{0} = sum {1}.{2} ({3})" },
    sb: { text: "{0}.{1} = {2}" },

    // 分支与跳转 / branches and jumps（多行形态放在 tooltip 里）
    beq: { text: "if {0} == {1} goto {2}", tooltip: "if {0} == {1}\n    goto {2}" },
    bne: { text: "if {0} != {1} goto {2}", tooltip: "if {0} != {1}\n    goto {2}" },
    blt: { text: "if {0} < {1} goto {2}", tooltip: "if {0} < {1}\n    goto {2}" },
    bgt: { text: "if {0} > {1} goto {2}", tooltip: "if {0} > {1}\n    goto {2}" },
    ble: { text: "if {0} <= {1} goto {2}", tooltip: "if {0} <= {1}\n    goto {2}" },
    bge: { text: "if {0} >= {1} goto {2}", tooltip: "if {0} >= {1}\n    goto {2}" },
    j: { text: "goto {0}" },
    jal: { text: "call {0}" },
    jr: { text: "goto {0}" }
};

/**
 * @if zh
 * @brief 渲染一条指令的伪代码
 *
 * @details 操作数按**语义类型**渲染：设备/名称哈希位置的 `HASH("型号名")` 会还原成当前语言下的设备名
 *          （比原字面量可读得多），其余交给 `operandToString`（寄存器、设备、枚举成员、数字都按源码
 *          形态还原）。
 *
 * @details 模板引用的操作数还没写完时（例如刚敲到 `add r0 r1`）返回 `undefined`：宁可不出提示，
 *          也不要给出半截伪代码。
 *
 * @param node 指令语句
 * @param language 当前界面语言（用于设备名）
 * @return 伪代码；没有模板或语句未写完时为 `undefined`
 *
 * @else
 * @brief Render the pseudocode of one instruction
 *
 * @details Operands are rendered by their **semantic type**: a `HASH("model")` in a device/name-hash slot is
 *          resolved to the device name of the current language (far more readable than the literal), and
 *          everything else goes through `operandToString` (registers, devices, enum members and numbers keep
 *          their source form).
 *
 * @details When a referenced operand is not written yet (e.g. right after `add r0 r1`) it returns `undefined`:
 *          better no hint than half a pseudocode line.
 *
 * @param node The instruction statement
 * @param language Current interface language (for device names)
 * @return The pseudocode, or `undefined` when there is no template or the statement is incomplete
 *
 * @endif
 * */
export function pseudocode(node: PureExeInstructionNode, language: string): Optional<Pseudocode> {
    const template = TEMPLATES[node.keyword];

    if (!template) return undefined;

    const operands: string[] = [];

    for (let slot = 1; ; slot++) {
        const operand = (node as unknown as Record<string, Optional<Operand>>)[`operand${slot}`];
        const type = getOperandType(node, slot);

        if (!operand || type === undefined) break;

        operands.push(renderOperand(operand, type, language));
    }

    const inline = render(template.text, operands);

    if (!inline) return undefined;

    const tooltip = template.tooltip ? render(template.tooltip, operands) : undefined;

    // 多行形态放进围栏代码块：Markdown 里只有代码块会保留换行与缩进
    // The multi-line form goes into a fenced code block: only a code block keeps the line break in Markdown
    return { text: inline, tooltip: tooltip ? `\`\`\`\n${tooltip}\n\`\`\`` : undefined };
}

/**
 * @if zh
 * @brief 用渲染好的操作数填充模板
 *
 * @details 模板引用了没有的操作数时返回 `undefined`（语句还没写完）。
 *
 * @else
 * @brief Fill a template with the rendered operands
 *
 * @details Returns `undefined` when the template references an operand that is not written yet.
 *
 * @endif
 * */
function render(template: string, operands: string[]): Optional<string> {
    const referenced = [...template.matchAll(/\{(\d+)\}/g)].map(match => Number(match[1]));

    if (referenced.some(index => operands[index] === undefined)) return undefined;

    return template.replace(/\{(\d+)\}/g, (_match, index: string) => operands[Number(index)]);
}

/**
 * @if zh
 * @brief 渲染一个操作数
 *
 * @param operand 操作数节点
 * @param type 语义操作数类型
 * @param language 当前界面语言
 * @return 渲染后的文本
 *
 * @else
 * @brief Render one operand
 *
 * @param operand The operand node
 * @param type Its semantic operand type
 * @param language Current interface language
 * @return The rendered text
 *
 * @endif
 * */
function renderOperand(operand: Operand, type: OperandType, language: string): string {
    if ((type === OperandType.DEVICE_HASH || type === OperandType.NAME_HASH) && operand.nodeName === "HashMacro") {
        const model = stringLiteral(operand);

        if (model) {
            const { title, englishTitle } = getDeviceTitlesByModel(model, language);
            const name = title ?? englishTitle;

            if (name) return name;
        }
    }

    return operandToString(operand);
}

/**
 * @if zh
 * @brief 取 `HASH("…")` 里的字面量（词法上含引号，这里去掉）
 *
 * @else
 * @brief The literal inside `HASH("…")` (the lexeme carries the quotes; they are stripped here)
 *
 * @endif
 * */
function stringLiteral(macro: Operand): Optional<string> {
    if (macro.nodeName !== "HashMacro") return undefined;

    const value = macro.value;

    if (value.nodeName !== "String") return undefined;

    const text = value.value.replace(/^"|"$/g, "");

    return text || undefined;
}
