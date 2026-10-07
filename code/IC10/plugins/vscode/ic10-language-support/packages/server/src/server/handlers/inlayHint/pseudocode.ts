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
 * @date 2026/10/07 17:20
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
 *          悬停才能看到的多行形态：条件分支自动拆成"条件一行、跳转一行"。
 *
 * @else
 * @summary The pseudocode of one instruction
 *
 * @details `text` is **single-line** (inlay hints render inline and a `\n` gets folded) while `tooltip`
 *          carries the multi-line form shown when hovering the hint: a conditional branch is split into one
 *          line for the condition and one for the jump.
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
 * @details 占位符 `{0}` 是 `operand1`、`{1}` 是 `operand2`，依次类推；模板只引用自己用得上的槽位
 *          （`bapz` 这类中间槽位不参与语义的指令会跳过它）。
 *
 * @details 覆盖标准库里**全部 147 条指令**（`alias`、`define`、`#` 是预处理指令，在语法树里不是指令语句，
 *          不会出提示）。写法以可读为准：赋值用 `=`、跳转用 `goto`（`+n` 表示相对当前行的偏移）、
 *          条件用 `if …`、调用用 `call`、批量读写用 `batch`，无法用符号表意的用函数名
 *          （`valid`、`isNaN`、`approx`、`stack` 等）。只有设备名等操作数需要本地化，因此模板本身
 *          与语言无关，不必为中英各写一份。
 *
 * @else
 * @summary Instruction → pseudocode template
 *
 * @details The placeholder `{0}` is `operand1`, `{1}` is `operand2` and so on; a template only references the
 *          slots it needs (an instruction such as `bapz`, whose middle slot carries no meaning, skips it).
 *
 * @details Covers **all 147 instructions** in the standard library (`alias`, `define` and `#` are directives
 *          rather than instruction statements in the syntax tree, so they produce no hint). The spelling
 *          favours readability: `=` for assignment, `goto` for jumps (`+n` marks an offset from the current
 *          line), `if …` for conditions, `call` for a jump that stores `ra`, `batch` for network-wide reads
 *          and writes, and a function name wherever a symbol would be vague (`valid`, `isNaN`, `approx`,
 *          `stack`, …). Only operands such as device names need localization, so the templates themselves stay
 *          language independent and need no Chinese/English duplicates.
 *
 * @endif
 * */
const TEMPLATES: Record<string, string> = {
    // ── 传送与算术 / moves and arithmetic
    move: "{0} = {1}",
    add: "{0} = {1} + {2}",
    sub: "{0} = {1} - {2}",
    mul: "{0} = {1} * {2}",
    div: "{0} = {1} / {2}",
    mod: "{0} = {1} % {2}",
    pow: "{0} = {1} ** {2}",
    max: "{0} = max({1}, {2})",
    min: "{0} = min({1}, {2})",
    clamp: "{0} = clamp({1}, {2}, {3})",
    lerp: "{0} = lerp({1}, {2}, {3})",
    select: "{0} = {1} ? {2} : {3}",
    sgn: "{0} = sign({1})",
    abs: "{0} = |{1}|",
    ceil: "{0} = ceil({1})",
    floor: "{0} = floor({1})",
    round: "{0} = round({1})",
    trunc: "{0} = trunc({1})",

    // ── 指数、对数、三角 / exponential, logarithmic, trigonometric
    exp: "{0} = exp({1})",
    log: "{0} = log({1})",
    sqrt: "{0} = sqrt({1})",
    sin: "{0} = sin({1})",
    cos: "{0} = cos({1})",
    tan: "{0} = tan({1})",
    asin: "{0} = asin({1})",
    acos: "{0} = acos({1})",
    atan: "{0} = atan({1})",
    atan2: "{0} = atan2({1}, {2})",

    // ── 位运算与移位 / bitwise and shifts
    and: "{0} = {1} & {2}",
    or: "{0} = {1} | {2}",
    xor: "{0} = {1} ^ {2}",
    not: "{0} = ~{1}",
    nor: "{0} = ~({1} | {2})",
    sll: "{0} = shl({1}, {2})",
    srl: "{0} = shr({1}, {2})",
    sla: "{0} = sal({1}, {2})",
    sra: "{0} = sar({1}, {2})",
    rol: "{0} = rotl({1}, {2})",
    ror: "{0} = rotr({1}, {2})",
    ext: "{0} = bits({1}, {2}, {3})",
    ins: "{0} = insert({1}, {2}, {3})",
    rand: "{0} = rand()",

    // ── 比较与近似比较（结果 1/0）/ comparisons and approximate comparisons (1/0)
    seq: "{0} = ({1} == {2})",
    sne: "{0} = ({1} != {2})",
    slt: "{0} = ({1} < {2})",
    sle: "{0} = ({1} <= {2})",
    sgt: "{0} = ({1} > {2})",
    sge: "{0} = ({1} >= {2})",
    seqz: "{0} = ({1} == 0)",
    snez: "{0} = ({1} != 0)",
    sltz: "{0} = ({1} < 0)",
    slez: "{0} = ({1} <= 0)",
    sgtz: "{0} = ({1} > 0)",
    sgez: "{0} = ({1} >= 0)",
    sap: "{0} = approx({1}, {2}, {3})",
    sna: "{0} = notApprox({1}, {2}, {3})",
    sapz: "{0} = approx0({1})",
    snaz: "{0} = notApprox0({1})",
    snan: "{0} = isNaN({1})",
    snanz: "{0} = !isNaN({1})",

    // ── 设备读写 / device access
    s: "{0}.{1} = {2}",
    l: "{0} = {1}.{2}",
    ss: "{0}[{1}].{2} = {3}",
    ls: "{0} = {1}[{2}].{3}",
    lr: "{0} = reagent({1}, {2}, {3})",
    rmap: "{0} = recipe({1}, {2})",
    sdns: "{0} = !valid({1})",
    sdse: "{0} = valid({1})",

    // ── 网络内同型号设备的批量读写 / batch access across matching devices in the network
    lb: "{0} = batch {1}.{2} ({3})",
    lbn: "{0} = batch {1}/{2}.{3} ({4})",
    lbs: "{0} = batch {1}[{2}].{3} ({4})",
    lbns: "{0} = batch {1}/{2}[{3}].{4} ({5})",
    sb: "batch {0}.{1} = {2}",
    sbn: "batch {0}/{1}.{2} = {3}",
    sbs: "batch {0}[{1}].{2} = {3}",

    // ── 栈与设备内存 / stack and device memory
    push: "push({0})",
    pop: "{0} = pop()",
    peek: "{0} = peek()",
    poke: "stack[{0}] = {1}",
    get: "{0} = stack({1})[{2}]",
    put: "stack({0})[{1}] = {2}",
    clr: "{0}.ClearStack()",
    clrd: "ClearStack({0})",

    // ── 跳转与停机 / jumps and halting
    j: "goto {0}",
    jr: "goto +{0}",
    jal: "call {0}",
    sleep: "sleep({0})",
    yield: "yield",
    hcf: "halt",

    // ── 条件分支（绝对行号）/ conditional branches (absolute line)
    beq: "if {0} == {1} goto {2}",
    bne: "if {0} != {1} goto {2}",
    blt: "if {0} < {1} goto {2}",
    bgt: "if {0} > {1} goto {2}",
    ble: "if {0} <= {1} goto {2}",
    bge: "if {0} >= {1} goto {2}",
    beqz: "if {0} == 0 goto {1}",
    bnez: "if {0} != 0 goto {1}",
    bltz: "if {0} < 0 goto {1}",
    bgtz: "if {0} > 0 goto {1}",
    blez: "if {0} <= 0 goto {1}",
    bgez: "if {0} >= 0 goto {1}",

    // ── 条件分支并存 ra（`*al`）/ conditional branches that also store ra (`*al`)
    beqal: "if {0} == {1} call {2}",
    bneal: "if {0} != {1} call {2}",
    bltal: "if {0} < {1} call {2}",
    bgtal: "if {0} > {1} call {2}",
    bleal: "if {0} <= {1} call {2}",
    bgeal: "if {0} >= {1} call {2}",
    beqzal: "if {0} == 0 call {1}",
    bnezal: "if {0} != 0 call {1}",
    bltzal: "if {0} < 0 call {1}",
    bgtzal: "if {0} > 0 call {1}",
    blezal: "if {0} <= 0 call {1}",
    bgezal: "if {0} >= 0 call {1}",

    // ── 近似比较与 NaN 分支 / approximate comparison and NaN branches
    bap: "if {0} ≈ {1} ({2}) goto {3}",
    bapal: "if {0} ≈ {1} ({2}) call {3}",
    bapz: "if {0} ≈ 0 goto {2}",
    bapzal: "if {0} ≈ 0 call {2}",
    bna: "if {0} !≈ {1} ({2}) goto {3}",
    bnaal: "if {0} !≈ {1} ({2}) call {3}",
    bnaz: "if {0} !≈ 0 goto {2}",
    bnazal: "if {0} !≈ 0 call {2}",
    bnan: "if {0} is NaN goto {1}",

    // ── 设备有效性分支 / device validity branches
    bdns: "if !valid({0}) goto {1}",
    bdse: "if valid({0}) goto {1}",
    bdnsal: "if !valid({0}) call {1}",
    bdseal: "if valid({0}) call {1}",
    bdnvl: "if !validRead({0}.{1}) goto {2}",
    bdnvs: "if !validWrite({0}.{1}) goto {2}",

    // ── 相对分支（当前行 + 偏移）/ relative branches (current line + offset)
    breq: "if {0} == {1} goto +{2}",
    brne: "if {0} != {1} goto +{2}",
    brlt: "if {0} < {1} goto +{2}",
    brgt: "if {0} > {1} goto +{2}",
    brle: "if {0} <= {1} goto +{2}",
    brge: "if {0} >= {1} goto +{2}",
    breqz: "if {0} == 0 goto +{1}",
    brnez: "if {0} != 0 goto +{1}",
    brltz: "if {0} < 0 goto +{1}",
    brgtz: "if {0} > 0 goto +{1}",
    brlez: "if {0} <= 0 goto +{1}",
    brgez: "if {0} >= 0 goto +{1}",
    brna: "if {0} !≈ {1} ({2}) goto +{3}",
    brap: "if {0} ≈ {1} ({2}) goto +{3}",
    brapz: "if {0} ≈ 0 goto +{2}",
    brnaz: "if {0} !≈ 0 goto +{2}",
    brnan: "if {0} is NaN goto +{1}",
    brdns: "if !valid({0}) goto +{1}",
    brdse: "if valid({0}) goto +{1}"
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

        // 还没写完的操作数在语法树里是 `Error` 节点（取它的文本会落到行尾的换行符上），一律当作缺失，
        // 于是模板引用到它就整条不出提示——宁可没有，也不要给出半截伪代码
        // An operand that is not written yet shows up as an `Error` node (its text falls back to the line
        // ending); it counts as missing, so a template referencing it produces no hint at all — better none
        // than half a pseudocode line
        if (!operand || operand.nodeName === "Error" || type === undefined) break;

        operands.push(renderOperand(operand, type, language));
    }

    const text = render(template, operands);

    if (!text) return undefined;

    return { text, tooltip: branchTooltip(text) };
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
 * @brief 条件分支的多行形态：条件一行、跳转一行
 *
 * @details 内联提示只能单行渲染，所以两行形态放进 tooltip。只对 `if … goto/call …` 形式的模板生效，
 *          因此不必逐条维护。
 *
 * @param text 单行伪代码
 * @return 围栏代码块包裹的两行文本；不是条件分支时为 `undefined`
 *
 * @else
 * @brief The multi-line form of a conditional branch: one line for the condition, one for the jump
 *
 * @details An inlay hint renders inline on a single line, so the two-line form goes into the tooltip. It
 *          applies to `if … goto/call …` templates only, which is why there is nothing to maintain per entry.
 *
 * @param text The single-line pseudocode
 * @return The two lines wrapped in a fenced code block, or `undefined` when this is not a branch
 *
 * @endif
 * */
function branchTooltip(text: string): Optional<string> {
    const match = /^if (.+?) (goto|call) (.+)$/.exec(text);

    if (!match) return undefined;

    return `\`\`\`\nif ${match[1]}\n    ${match[2]} ${match[3]}\n\`\`\``;
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
