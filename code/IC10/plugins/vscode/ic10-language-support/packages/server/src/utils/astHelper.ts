// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file astHelper.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 21:33
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Position, Optional } from "@ic10/common";
import {
    PureExeInstructionNode,
    TokenCategory,
    OperandType,
    Description,
    StringNode,
    TokenType,
    Statement,
    LinkNode,
    Operand,
    ASTNode,
    Token
} from "@ic10/compiler";

import { resources } from "../locals";


type NodeType = Statement | Operand | StringNode;

type ExtractNodeByNodeName<T extends NodeType["nodeName"]> = Extract<
    NodeType,
    {
        readonly nodeName: T;
    }
>;

type ExtractNodeBySuffix<S extends string> = Extract<NodeType, { readonly nodeName: `${string}${S}` }>;

/**
 * @summary AST 节点访问器，根据节点类型分发到对应的处理函数
 *
 * @summary AST node visitor that dispatches to type-specific handlers
 *
 * @desc 泛型访问器模式实现。传入一个按节点 type 分发的处理器映射和一个节点，
 * 自动调用对应类型的处理函数。适用于遍历 IC10 程序的 AST。
 *
 * @desc Generic visitor pattern. Given a handler map keyed by node type and
 * a node, calls the matching handler. Used for traversing IC10 program ASTs.
 * */
export function visit<T extends NodeType = NodeType, R = any>(
    visitor: { [K in T["nodeName"]]: (node: ExtractNodeByNodeName<K>) => R },
    node: T
): R {
    return visitor[node.nodeName as T["nodeName"]](node as any);
}

/**
 * @summary 操作数节点访问器，专门用于 OperandNode 类型分发
 *
 * @summary Operand-specific visitor that dispatches on OperandNode subtypes
 *
 * @desc 与 visit 类似，但限定于 OperandNode 及其子类型（如 Device、Register、
 * Integer、Float、Identifier 等），提供更精确的类型推导。
 *
 * @desc Similar to visit but scoped to OperandNode and its subtypes
 * (Device, Register, Integer, Float, Identifier, etc.) for precise type inference.
 * */
export function visitOperand<R = any>(
    visitor: { [K in Operand["nodeName"]]: (node: ExtractNodeByNodeName<K>) => R },
    node: Operand
): R {
    return visitor[node.nodeName](node as any);
}

type ASTChecker = {
    [K in NodeType["nodeName"] as `is${K}`]: (node: ASTNode) => node is ExtractNodeByNodeName<K>;
};

type KnownSuffix = "Directive" | "Instruction";

type ASTBelongChecker = {
    [K in KnownSuffix as `belong${K}`]: (node: { readonly nodeName: string }) => node is ExtractNodeBySuffix<K>;
};

export const AST = new Proxy({} as ASTChecker & ASTBelongChecker, {
    get(target, prop: string | symbol, receiver) {
        if (typeof prop !== "string") return Reflect.get(target, prop, receiver);

        // isXxx：精确匹配（类型谓词在 ASTChecker 里声明）
        if (prop.startsWith("is") && prop.length > 2) {
            const nodeName = prop.slice(2);
            return (node: ASTNode) => {
                if (!node.nodeName) throw Error(`"nodeName" not in ${node}`);

                return node.nodeName === nodeName;
            };
        }

        // belongXxx：后缀匹配（类型谓词在 ASTBelongChecker 里声明）
        if (prop.startsWith("belong") && prop.length > 6) {
            const suffix = prop.slice(6);
            return (node: { readonly nodeName: string }) => {
                if (!node.nodeName) throw Error(`"nodeName" not in ${node}`);

                return node.nodeName.endsWith(suffix);
            };
        }

        return Reflect.get(target, prop, receiver);
    }
});

/**
 * @summary 节点类型到处理函数的映射组
 *
 * @summary Mapping from node types to handler functions
 *
 * @desc 将一组 AST 节点 type 映射到对应的处理函数类型。
 * 通常与 visit 函数配合使用以构建类型安全的访问器。
 *
 * @desc Maps a set of AST node types to their handler function types.
 * Typically used with visit() to construct type-safe visitors.
 * */
export type HandleGroup<T extends NodeType["nodeName"] = NodeType["nodeName"], R = any> = {
    [K in T]: (node: ExtractNodeByNodeName<K>) => R;
};

/**
 * @summary 将多个节点类型批量绑定到同一个处理函数
 *
 * @summary Batch-binds multiple node types to a single handler function
 *
 * @desc 便捷工具函数，接收一个类型数组和一个处理函数，生成一个 HandleGroup。
 * 常用于 visit/visitOperand 中多种节点共享同一处理逻辑的场景。
 *
 * @desc Convenience utility that takes a type array and a single handler,
 * producing a HandleGroup. Useful when multiple node types share the same logic.
 * */
export function groupHandlers<T extends NodeType["nodeName"], R = any>(
    types: T[],
    handler: (node: ExtractNodeByNodeName<T>) => R
): HandleGroup<T, R> {
    const result = {} as HandleGroup<T, R>;

    for (const t of types) result[t] = handler;

    return result;
}

/**
 * @summary 获取操作数节点的显示字符长度
 *
 * @summary Gets the display character length of an operand node
 *
 * @desc 根据操作数类型返回其字符串表示的长度（字符数），
 * 用于计算格式化对齐等场景。
 *
 * @desc Returns the character count of an operand's string representation
 * based on its type. Used for formatting/alignment calculations.
 * */
export function operandValueLength(node: Operand): number {
    return node.end.column - node.position.column;
}

/**
 * @summary 将操作数节点转换为其源代码字符串表示
 *
 * @summary Converts an operand node to its source-code string representation
 *
 * @desc 根据操作数类型还原其对应的源代码文本。例如 HashCall 还原为 `HASH("...")`，
 * StrCall 还原为 `STR("...")`，普通标识符和数字直接返回值。
 *
 * @desc Reconstructs the source-code text for an operand node based on its type.
 * E.g., HashCall → `HASH("...")`, StrCall → `STR("...")`, plain identifiers
 * and numbers are returned as-is.
 * */
export function operandToString(node: Operand): string {
    return visitOperand<string>(
        {
            Error: error => error.token.lexeme,
            ...groupHandlers(
                [
                    "GeneralPurposeRegister",
                    "AddressRegister",
                    "StackPointerRegister",
                    "Identifier",
                    "String",
                    "BinaryNumber",
                    "HexNumber"
                ],
                node => node.value
            ),
            Enum: node => {
                // 值缺失时（`Foo.` 未输入完）点号记在 value 的错误节点上，此时只还原已输入的部分
                if (AST.isError(node.name)) return AST.isError(node.value) ? "" : node.value.value;

                return AST.isError(node.value) ? node.name.value : `${node.name.value}.${node.value.value}`;
            },
            DynamicRegister: dr => `r${operandToString(dr.register)}`,
            DynamicDevice: dd => `d${operandToString(dd.register)}`,
            StaticDevice(sd) {
                let result: string = "";

                switch (sd.device.nodeName) {
                    case "Error":
                        return operandToString(sd.device);
                    case "OrdinaryDevice":
                    case "SelfReferenceDevice":
                        result += sd.device.value;
                }

                if (sd.pin) result += `:${sd.pin.value}`;

                return result;
            },
            HashMacro: hm => `HASH("${AST.isError(hm.value) ? operandToString(hm.value) : hm.value.value}")`,
            StrMacro: sm => `STR("${AST.isError(sm.value) ? operandToString(sm.value) : sm.value.value}")`,
            ...groupHandlers(["Integer", "Float"], n => n.value.toString())
        },
        node
    );
}

export function end(node: Operand): Position;
export function end(token: Token): Position;
export function end(item: Operand | Token): Position {
    if ("pos" in item) return { line: item.pos.line /* 无换行 */, column: item.pos.column + item.lexeme.length };

    return item.end;
}

export function getOperandType(ins: PureExeInstructionNode, idx: number): Optional<OperandType> {
    return (ins as any)[`type${idx}`];
}

/**
 * @summary 操作数内部的子段（多 token 操作数特有）
 *
 * @summary Sub-segment inside an operand (specific to multi-token operands)
 *
 * @desc 一个操作数可能由多个 token 组成：静态设备的引脚 `d0:1`、枚举的枚举值 `Foo.Bar`。
 * 光标落在这些子段内时，补全语义与其首段（`d0`、`Foo`）不同。
 *
 * @desc An operand may consist of several tokens: the pin of a static device (`d0:1`) and the
 * value of an enum (`Foo.Bar`). A cursor inside such a sub-segment has different completion
 * semantics from the leading segment (`d0`, `Foo`).
 * */
export type OperandSegment = "pin" | "enumValue";

/**
 * @summary 光标在语句中的定位结果
 *
 * @summary Cursor location within a statement
 *
 * @desc 由 locateOperand 依据 AST 中操作数的源码列区间计算，与行内 token 序号无关，
 * 因此一个操作数无论由几个 token 组成都只占一个槽位。
 *
 * @desc Computed by locateOperand from the source column ranges of the AST operands, independent
 * of the token ordinal within the line: an operand occupies exactly one slot no matter how many
 * tokens it consists of.
 * */
export interface OperandLocation {
    /**
     * @summary 光标所在（或将要输入）的操作数槽位
     *
     * @desc 0 表示语句头部（关键字/标签名）；1..n 对应 operand1..operandN；
     * alias/define 伪指令的槽位 1 为自定义标识符、槽位 2 为其操作数。
     * */
    slot: number;

    /** @summary 光标命中的操作数节点（含多 token 操作数位于光标之后的子段） */
    operand?: Operand;

    /** @summary 命中操作数内部的子段 */
    segment?: OperandSegment;

    /** @summary 光标之前最近的操作数（供悬停等「就近」语义使用） */
    previous?: {
        slot: number;
        operand: Operand;
    };
}

/**
 * @summary 语句头部（关键字或标签名）在源码中的结束列
 *
 * @summary End column of the statement head (keyword or label name) in the source
 * */
function statementHeadEnd(stmt: Statement): number {
    if (AST.belongInstruction(stmt)) return stmt.position.column + stmt.keyword.length;

    if (AST.isAliasDirective(stmt)) return stmt.position.column + 5; // "alias"
    if (AST.isDefineDirective(stmt)) return stmt.position.column + 6; // "define"

    // 标签定义：标签名即头部；其余（错误语句等）退化为语句起点
    if (AST.isLabelDef(stmt) && AST.isIdentifier(stmt.identifier)) return stmt.identifier.end.column;

    return stmt.position.column;
}

/**
 * @summary 按槽位升序列出语句的操作数
 *
 * @summary List the statement operands in slot order
 *
 * @desc 可执行指令取 `operand1..operandN`；alias/define 伪指令取自定义标识符与目标/值，
 * 与补全现有的槽位约定一致（槽位 1 为标识符、槽位 2 为操作数）。
 *
 * @desc Executable instructions yield `operand1..operandN`; alias/define directives yield the
 * user identifier and the target/value, matching the slot convention already used by completion
 * (slot 1 is the identifier, slot 2 is the operand).
 * */
function statementOperands(stmt: Statement): Operand[] {
    if (AST.belongInstruction(stmt)) {
        const result: Operand[] = [];

        for (let i = 1; ; i++) {
            const operand: Optional<Operand> = (stmt as any)[`operand${i}`];

            if (!operand) break;

            result.push(operand);
        }

        return result;
    }

    if (AST.isAliasDirective(stmt)) return [stmt.identifier, stmt.registerOrDevice];

    if (AST.isDefineDirective(stmt)) return [stmt.identifier, stmt.operand];

    return [];
}

/**
 * @summary 判断光标落在操作数内部的哪个子段
 *
 * @summary Determine which sub-segment of an operand the cursor falls into
 *
 * @desc 依据子节点的列区间判断，不依赖 token：静态设备以引脚（或无引脚时被消费的冒号）为子段，
 * 枚举以点号之后的值部分为子段。
 *
 * @desc Decided by the column ranges of the child nodes rather than tokens: a static device has
 * the pin (or the consumed colon when the pin is missing) as its sub-segment, and an enum has the
 * part after the dot as its sub-segment.
 * */
function operandSegmentAt(operand: Operand, column: number): Optional<OperandSegment> {
    if (AST.isStaticDevice(operand) && !AST.isError(operand.device))
        return column > operand.device.end.column ? "pin" : undefined;

    if (AST.isEnum(operand) && AST.isIdentifier(operand.name))
        return column > operand.name.end.column ? "enumValue" : undefined;

    return undefined;
}

/**
 * @summary 依据 AST 定位光标所在的语句槽位与操作数子段
 *
 * @summary Locate the statement slot and operand sub-segment at the cursor using the AST
 *
 * @desc 取代「行内 token 序号即操作数序号」的推导方式：后者在 `d0:1`、`Foo.Bar` 这类
 * 多 token 操作数出现时会整体错位。判定规则：
 * 1. 语句未被识别（`ErrorNode`，如正在输入的关键字 `mo`）→ 槽位 0，此时不存在任何操作数；
 * 2. 光标不越过语句头部（关键字/标签名）→ 槽位 0；
 * 3. 光标落在某个非空操作数的列区间内（含终点）→ 该操作数的槽位；
 * 4. 其余（操作数之后的空隙）→ 光标之前操作数个数 + 1，即下一个槽位。
 *
 * @desc 其中「操作数」只算**属于本行、且不是缺失占位**的节点：语句还没敲完时（如刚输入 `s `），
 * 解析器会为缺失的操作数造占位 `ErrorNode`，并可能把它们挂到行尾的换行或下一行的 token 上；
 * 这类节点若参与判定，光标一过行尾就会把占位当成已输入的操作数，槽位随之漂移到后面的 `typeN`。
 *
 * @desc Replaces the "token ordinal equals operand ordinal" derivation, which is shifted as soon as
 * a multi-token operand such as `d0:1` or `Foo.Bar` appears. Rules:
 * 1. unrecognized statement (`ErrorNode`, e.g. while typing the keyword `mo`) → slot 0, since no
 * operand exists at all;
 * 2. cursor not past the statement head (keyword/label name) → slot 0;
 * 3. cursor inside the column range (end inclusive) of a non-empty operand → that operand's slot;
 * 4. otherwise (a gap after some operand) → operands before the cursor + 1, i.e. the next slot.
 *
 * @desc Only nodes **on this line and not a missing-operand placeholder** count as operands: while a
 * statement is still being typed (say `s `), the parser fills the missing operands with placeholder
 * `ErrorNode`s and may hang them on the trailing newline or on the next line's tokens. Letting those
 * take part would make everything past the end of the line look like an already entered operand and
 * drift the slot to a later `typeN`.
 *
 * @param stmt 光标所在行的语句节点
 * @param column 光标列（1-based）
 * */
export function locateOperand(stmt: Statement, column: number): OperandLocation {
    // 未识别的语句没有可定位的操作数（如输入到一半的关键字 `mo`）：整条语句都算语句头部，
    // 否则会误报槽位 1 而让补全既给不出操作数、也给不出关键字
    if (AST.isError(stmt)) return { slot: 0 };

    if (column <= statementHeadEnd(stmt)) return { slot: 0 };

    const operands = statementOperands(stmt);
    const line = stmt.position.line;

    let slot = 0;
    let previous: Optional<OperandLocation["previous"]> = undefined;

    for (let i = 0; i < operands.length; i++) {
        const operand = operands[i];
        const start = operand.position.column;
        const end = operand.end.column; // 多 token 操作数的 end 覆盖其全部源码文本

        // 不属于本行的节点不参与命中：IC10 以换行分隔语句，正常操作数不可能落在别的行上；
        // 只有「缺失操作数」的占位节点会被解析器挂到下一行的 token 上（见 isMissingOperand）
        if (operand.position.line !== line) continue;

        // 缺失操作数对应的空占位节点不参与命中（宽度为 0，或占位在行尾空白/文件结束上）
        if (start === end || isMissingOperand(operand)) continue;

        if (start <= column && column <= end)
            return { slot: i + 1, operand, segment: operandSegmentAt(operand, column), previous };

        if (end <= column) {
            slot = i + 1;
            previous = { slot: i + 1, operand };
        }
    }

    return { slot: slot + 1, previous };
}

/**
 * @summary 判断操作数是否是解析器为「缺失的操作数」补的占位错误节点
 *
 * @summary Tell whether the operand is a placeholder error node for a missing operand
 *
 * @desc 操作数缺失时，解析器拿其后第一个 token 造 `ErrorNode`：行尾是 `NEWLINE`（行号仍在语句
 * 所在行上，列号却已越过该行的可见文本），文件末尾是 `END`。这类占位不对应任何真实文本，
 * 因此既不能让光标命中，也不能让槽位前进——否则光标只要过了行尾，就会把占位当成「已经输入
 * 的操作数」，槽位顺次漂移到后面的 `typeN`（例如刚敲完 `s ` 却按 `LOGIC_PROP`/`REG_TARGET`
 * 给出候选）。宽度为 0 的 `END` 占位由调用方的列区间判断自然排除，这里只处理有宽度的空白占位。
 *
 * @desc When an operand is missing the parser builds an `ErrorNode` from the next token: a `NEWLINE`
 * at the end of the line (still on the statement's line, but past its visible text) or `END` at the end
 * of the file. Such a placeholder covers no real text, so it must neither match the cursor nor advance
 * the slot: otherwise anything past the end of the line would count as an already-entered operand and
 * the slot would drift to a later `typeN` (typing `s ` and getting `LOGIC_PROP`/`REG_TARGET` candidates
 * instead of a device). Zero-width `END` placeholders are already excluded by the column range check,
 * so only blank placeholders with width are handled here.
 * */
function isMissingOperand(operand: Operand): boolean {
    if (!AST.isError(operand)) return false;

    return (
        operand.token.category === TokenCategory.WHITESPACE ||
        operand.token.category === TokenCategory.COMMENT ||
        operand.token.type === TokenType.END
    );
}

/**
 * @summary 操作数的书写形式（补全类别）
 *
 * @summary Operand writing form (completion category)
 *
 * @desc {@link SemanticMap} 的每一项表示该槽位允许的**书写形式**，并对应一个补全提供器：
 * `register` / `device` / `number` 各提供该形式的候选；`identifier` 表示允许标识符，
 * 但它自身不提供候选——标识符的具体语义由同一列表中的其它项决定（见 identifier.ts 的分派）。
 *
 * @desc 枚举值不在此列表：标准库枚举的标识符写法（如 `Setting`）是**语义**，由
 * {@link EnumKeyMap} 声明；显式枚举写法 `Foo.Bar` 是**语法**，由补全处理器依据 AST
 * （`AST.isEnum` 与子段定位）处理。因此本表既不表示也不驱动 `Foo.Bar` 语法。
 *
 * @remarks 值数组中的顺序会影响解析顺序从而影响补全项顺序
 *
 * @desc Each entry of {@link SemanticMap} is a **writing form** allowed at that slot and maps to a
 * completion provider: `register` / `device` / `number` supply candidates in that form, while
 * `identifier` merely states that an identifier is allowed — it supplies nothing itself, because the
 * identifier's concrete semantics are decided by the other entries of the same list (see the
 * dispatch in identifier.ts).
 *
 * @desc Enum values are absent from this list: an identifier that denotes a standard-library enum
 * member (`Setting`) is a **semantic** fact declared by {@link EnumKeyMap}, whereas the explicit
 * `Foo.Bar` form is a **syntax** fact handled by the completion handler from the AST
 * (`AST.isEnum` plus sub-segment location). This table neither expresses nor drives `Foo.Bar`.
 * */
export type GenericOperandType = "register" | "device" | "identifier" | "number" | "enum";
export const SemanticMap = {
    [OperandType.REG_TARGET]: ["identifier", "register"] as const,
    [OperandType.REG_OR_DEV]: ["device", "register"] as const,
    [OperandType.NUM_VALUE]: ["identifier", "number", "register", "enum"] as const,
    [OperandType.JUMP_LINE]: ["identifier", "number", "register"] as const,
    [OperandType.ADDRESS]: ["number", "register"] as const,
    [OperandType.SLOT_IDX]: ["number"] as const,
    [OperandType.HARDWARE_ID]: ["number", "register"] as const,
    [OperandType.REAGENT_HASH]: ["number", "register"] as const,
    [OperandType.DEVICE_REF]: ["identifier", "device", "register"] as const,
    [OperandType.DEVICE_REF_STRICT]: ["device"] as const,
    [OperandType.LOGIC_PROP]: ["identifier", "number"] as const,
    [OperandType.LOGIC_SLOT_PROP]: ["identifier", "number"] as const,
    [OperandType.AGG_MODE]: ["identifier", "number", "enum"] as const,
    [OperandType.REAGENT_MODE]: ["identifier", "number", "enum"] as const,
    [OperandType.DEVICE_HASH]: ["identifier", "number", "register"] as const,
    [OperandType.NAME_HASH]: ["identifier", "number", "register"] as const,
    [OperandType.CONST_NUM]: ["number"] as const
} satisfies Record<OperandType, GenericOperandType[]>;

/**
 * @summary 操作数类型对应的标准库枚举类型（枚举补全的语义开关）
 *
 * @summary Standard-library enum type of an operand type (the semantic switch for enum completion)
 *
 * @desc 该槽位上的标识符在**语义**上是标准库声明的枚举值（`LOGIC_PROP` 的 `Setting` 属于
 * `LogicType`），因此需要枚举候选：全局枚举成员，以及设备注解声明该设备拥有的逻辑属性、
 * 逻辑槽属性、槽位列表（见 providers/enum.ts）。此表是纯语义的，与 {@link SemanticMap}
 * 的书写形式正交——`LOGIC_PROP` 只接受标识符写法，语义上却是枚举。
 *
 * @desc 对应编译器侧的 `operand_type_name`（LOGIC_PROP / LOGIC_SLOT_PROP / REAGENT_MODE /
 * AGG_MODE 映射到标准库枚举）；此处多出的 `SLOT_IDX` 没有同名标准库枚举（`SlotIdx` 在标准库中
 * 并不存在，全局查找会落空），列出它只为启用设备注解的槽位列表补全。
 *
 * @desc The identifier at that slot **semantically** denotes a standard-library enum member
 * (`Setting` at `LOGIC_PROP` belongs to `LogicType`), so enum candidates are wanted: global enum
 * members plus the device annotation's logic properties, logic-slot properties and slot list
 * (see providers/enum.ts). The table is purely semantic and orthogonal to the writing forms of
 * {@link SemanticMap} — `LOGIC_PROP` accepts nothing but an identifier yet is an enum semantically.
 *
 * @desc Mirrors the compiler's `operand_type_name` (LOGIC_PROP / LOGIC_SLOT_PROP / REAGENT_MODE /
 * AGG_MODE map to standard-library enums); the extra `SLOT_IDX` has no such enum (`SlotIdx` does not
 * exist in the standard library, so the global lookup finds nothing) and is listed only to enable the
 * device annotation's slot list.
 * */
export const EnumKeyMap = {
    [OperandType.LOGIC_PROP]: "LogicType",
    [OperandType.LOGIC_SLOT_PROP]: "LogicSlotType",
    [OperandType.AGG_MODE]: "BatchMode",
    [OperandType.REAGENT_MODE]: "ReagentMode",
    [OperandType.SLOT_IDX]: "SlotIdx"
} as const;

/**
 * @summary 寻找所在列附近范围内的token
 *
 * @desc column位于单词中间则curr为该单词在tokens的索引，否则为-1表示没有，prev总是为curr之前的单词索引，无论curr是否命中单词
 *
 * @param tokens 某代码行对应的token数组
 * @param column 该代码中的某一列（应为1-based）
 * */
export function findRangeTokens(
    tokens: Token[],
    column: number
): {
    prev: number;
    curr: number;
    next: number;
} {
    const result = { prev: -1, curr: -1, next: -1 };

    tokens.some((token, i) => {
        // 排除空内容token
        const idx =
            token.category === TokenCategory.WHITESPACE ||
            token.category === TokenCategory.COMMENT ||
            token.type === TokenType.END
                ? -1
                : i;

        // 列前的
        if (end(token).column <= column) result.prev = idx;

        // 列后的
        else if (token.pos.column >= column) {
            result.next = idx;
            return true;
        }

        // 否则为当前的
        else result.curr = idx;
    });

    return result;
}

export class DescriptionSolver {
    public static ROOT = "@ic10/metadata";

    static solve(description: Description, lang: keyof typeof resources): Optional<string> {
        switch (description.nodeName) {
            case "Link":
                return this.link(description)?.[lang];
            case "String":
                return description.value;
            default:
                return;
        }
    }

    static parse(input: string): Optional<{
        nodeName: "Link";
        paths: string[];
        fields: string[];
    }> {
        let i = 0;
        const n = input.length;

        // 辅助：读一个 identifier [A-Za-z_][A-Za-z0-9_]*
        const readIdentifier = (): string => {
            const start = i;
            if (i >= n) throw new Error();
            const c = input[i];
            if (!/[A-Za-z_]/.test(c)) throw new Error();
            i++;
            while (i < n && /[A-Za-z0-9_]/.test(input[i])) i++;
            return input.slice(start, i);
        };

        try {
            // 起始必须为 '.'
            if (i >= n || input[i] !== ".") throw new Error();
            i++;

            const paths: string[] = [];
            const fields: string[] = [];

            // ( DIV Identifier )+  —— 至少一个 /xxx
            if (i >= n || input[i] !== "/") throw new Error();
            do {
                i++; // 消费 '/'
                paths.push(readIdentifier());
            } while (i < n && input[i] === "/");

            // ( DOT Identifier )*  —— 零个或多个 .xxx
            while (i < n && input[i] === ".") {
                i++; // 消费 '.'
                fields.push(readIdentifier());
            }

            if (i !== n) throw new Error();
            return { nodeName: "Link", paths, fields };
        } catch {}
    }

    static link(link: LinkNode): Optional<{
        [K in keyof typeof resources]: string;
    }> {
        let path = DescriptionSolver.ROOT;

        for (const p of link.paths) path += `/${p}`;

        let mod = require(path);

        // 兼容 ESM 编译产物：\`export default data\` 会变成 \`{ __esModule: true, default: data }\`
        let json = mod && mod.__esModule && mod.default ? mod.default : (mod?.default ?? mod);

        for (const field of link.fields) json = json[field];

        return json;
    }
}
