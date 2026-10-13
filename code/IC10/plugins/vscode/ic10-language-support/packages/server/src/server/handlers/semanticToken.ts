// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semanticToken.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/23 12:37
 * @desc IC10 语义令牌处理器
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Console, Optional, Position, upperBound, debug } from "@ic10/common";
import { Languages } from "vscode-languageserver";
import {
    PureExeInstructionNode,
    DefineDirectiveNode,
    DynamicRegisterNode,
    AliasDirectiveNode,
    DynamicDeviceNode,
    StaticDeviceNode,
    DeviceAnnotation,
    EnumAnnotation,
    IdentifierNode,
    HashMacroNode,
    TokenCategory,
    TypeCategory,
    LabelDefNode,
    TypeHintNode,
    StrMacroNode,
    OperandType,
    TypeOfNode,
    TokenType,
    ErrorNode,
    BasicType,
    Statement,
    SymbolMap,
    RegOrDev,
    EnumNode,
    Operand,
    Program,
    Token
} from "@ic10/compiler";


import { AST, groupHandlers, visit } from "../../utils";
import { DocumentCache } from "../cache";
import { t } from "../../locals";

type OnHandlerType = Parameters<Languages["semanticTokens"]["on"]>[0];
type OnRangeHandlerType = Parameters<Languages["semanticTokens"]["onRange"]>[0];

/**
 * @summary IC10 语义令牌类型枚举，对应 LSP legend 中的 tokenTypes
 *
 * @summary IC10 semantic token type enumeration, corresponding to tokenTypes in LSP legend
 *
 * @desc 定义 IC10 语言的所有语义令牌类型，使用自定义类型避免与 VS Code 标准类型混淆，
 * 并可通过 package.json 中的 semanticTokenColors 精确控制颜色。
 * 枚举值的顺序即为 LSP legend 中 tokenTypes 的顺序。
 *
 * @desc Defines all semantic token types for the IC10 language. Custom types are used to avoid
 * confusion with VS Code standard types, and colors can be precisely controlled via
 * semanticTokenColors in package.json. The order of enum values is the order of
 * tokenTypes in the LSP legend.
 *
 * @see https://code.visualstudio.com/api/language-extensions/semantic-highlight-guide
 * */
export enum TokenLegend {
    /** 指令关键字（move, add, sub 等）和预处理指令（alias, define） */
    Keyword = 0,
    /** 寄存器（r0, r1, ra, sp 等） */
    Register,
    RegisterIdentifier,
    /** 函数/宏调用（hash, str） */
    Macro,
    /** 设备引用（d0, d1, db 等） */
    Device,
    DeviceIdentifier,
    /** 数字字面量（整数、浮点、十六进制、二进制） */
    Number,
    NumberIdentifier,
    /** 字符串字面量 */
    String,
    Constant,
    /** 注释 */
    Comment,
    /** 类型提示前缀（`#:` / `#>`） */
    Decorator,
    /** 类型提示里的标签（`@type` / `@desc` / `@builtin` 等） */
    AnnotationTag,
    /** 标识符（标签名、别名、define 常量名） */
    Label,
    LabelIdentifier,
    /** 枚举类型名（`Foo.Bar` 的 `Foo`） */
    Enum,
    /** 枚举成员（`Foo.Bar` 的 `Bar`） */
    EnumMember,
    Unknown
}

/**
 * @summary IC10 语义令牌修饰符枚举，对应 LSP legend 中的 tokenModifiers
 *
 * @summary IC10 semantic token modifier enumeration, corresponding to tokenModifiers in LSP legend
 *
 * @desc 定义语义令牌的修饰符位掩码。枚举值的顺序即为 LSP legend 中 tokenModifiers 的顺序。
 * 每个修饰符对应一个位，可通过位运算组合多个修饰符。
 *
 * @desc Defines bitmask modifiers for semantic tokens. The order of enum values is the order
 * of tokenModifiers in the LSP legend. Each modifier corresponds to one bit and can be
 * combined via bitwise operations.
 * */
export enum TokenModifier {
    /** 声明（alias、define 引入的新符号） */
    Declaration = 0,
    /** 只读（数学常量：nan, pi, tau 等） */
    Readonly
}

/**
 * @summary LSP legend 的 tokenTypes 字符串数组
 *
 * @summary String array of tokenTypes for the LSP legend
 *
 * @desc 从 TokenLegend 枚举中提取的字符串键名数组，用于初始化 LSP 语义令牌 legend。
 * 过滤掉数字键，仅保留枚举成员名称。
 *
 * @desc Array of string keys extracted from the TokenLegend enum, used to initialize the
 * LSP semantic tokens legend. Numeric keys are filtered out, leaving only enum member names.
 * */
export const TOKEN_TYPES = Object.keys(TokenLegend).filter(k => isNaN(Number(k)));

/**
 * @summary LSP legend 的 tokenModifiers 字符串数组
 *
 * @summary String array of tokenModifiers for the LSP legend
 *
 * @desc 从 TokenModifier 枚举中提取的字符串键名数组，用于初始化 LSP 语义令牌修饰符 legend。
 * 过滤掉数字键，仅保留枚举成员名称。
 *
 * @desc Array of string keys extracted from the TokenModifier enum, used to initialize the
 * LSP semantic tokens modifier legend. Numeric keys are filtered out, leaving only enum member names.
 * */
export const TOKEN_MODIFIERS = Object.keys(TokenModifier).filter(k => isNaN(Number(k)));

/**
 * @summary 块注解里名字代表"声明的成员"的标签
 *
 * @summary Annotation tags whose following name declares a member
 *
 * @desc 这些标签后面的标识符是注解**声明出来的成员**（`@logic Setting 12` 的 `Setting`、
 * `@slot Slot0 0` 的 `Slot0`、枚举注解 `@value Green 2` 的 `Green`），按成员着色。
 *
 * @desc The identifier following these tags is a member **declared** by the annotation (the
 * `Setting` of `@logic Setting 12`, the `Slot0` of `@slot Slot0 0`, the `Green` of an enum
 * annotation's `@value Green 2`), and is colored as a member.
 * */
const ANNOTATION_MEMBER_TAGS = new Set(["@logic", "@logic-slot", "@slot", "@value"]);


interface SemanticToken {
    /**
     * @summary 行偏移量
     * @desc 相对于前一个行号的偏移量
     * @remarks 必须大于等于0
     * */
    line: number;

    /**
     * @summary 起始位置偏移量
     * @desc 相对于该行起始位置的字符偏移量
     * @remarks 同一行必须 ≥ 0，新行可 ≥ 0
     * */
    start: number;

    /**
     * @summary 令牌长度
     * @desc 令牌的字符长度
     * */
    length: number;

    /**
     * @summary 令牌类型
     * @desc 令牌类型在 legend.types 中的索引
     * @remarks 从0开始
     * */
    type: TokenLegend;

    /**
     * @summary 修饰符位
     * @desc 修饰符的位掩码
     * @remarks 每个 bit 对应 legend.modifiers 中的一个修饰符,并且无论是语法修饰符还是语义修饰符都可以
     * @example 0b01
     * */
    modifier: TokenModifier;
}

interface HandlerContext {
    prev: Position;
    table: SymbolMap;
    /** 该文档的词法 token：类型提示等"由多个片段组成"的位置要靠它逐段着色 */
    tokens: Token[];
}

/**
 * @summary IC10 语义令牌处理器
 *
 * @summary IC10 semantic token handler
 *
 * @desc 负责将 IC10 程序的 AST 转换为 LSP 语义令牌数据。支持全文档和范围级别的语义令牌请求。
 * 遍历 AST 中的语句节点和操作数节点，根据符号表和类型信息为每个 token 分配
 * 合适的 TokenLegend 类型和 TokenModifier 修饰符。
 *
 * @desc Converts IC10 program AST into LSP semantic token data. Supports both full-document
 * and range-level semantic token requests. Traverses statement and operand nodes in the AST,
 * assigning appropriate TokenLegend types and TokenModifier modifiers to each token based
 * on the symbol table and type information.
 * */
export class SemanticTokenHandler {
    /** 语义令牌缓存：按 URI 索引，通过 hash 判断是否过期 */
    private tokenCache: Map<string, { hash: string; data: number[] }> = new Map();

    constructor(private readonly docCache: DocumentCache) {}

    @debug({
        message: err => t("server.handler.error", { name: "semantic token", err: (err as Error).message }),
        logger: msg => Console.error(msg, "semantic token"),
        rethrow: false
    })
    handle(...[params]: Parameters<OnHandlerType>): ReturnType<OnHandlerType> {
        const uri = params.textDocument.uri;
        const cache = this.docCache.getCache(uri);

        if (!cache || !cache.ast || !cache.symbols) return { data: [] };

        // 命中令牌缓存则直接返回，避免重复遍历 AST
        const cached = this.tokenCache.get(uri);
        if (cached && cached.hash === cache.hash) return { data: cached.data };

        const context: HandlerContext = {
            prev: { line: 1, column: 1 },
            table: cache.symbols,
            tokens: cache.tokens ?? []
        };

        const data = this.visitProgram(cache.ast, context);
        this.tokenCache.set(uri, { hash: cache.hash, data });

        return { data };
    }

    @debug({
        message: err => t("server.handler.error", { name: "semantic token range", err: (err as Error).message }),
        logger: msg => Console.error(msg, "semantic token range"),
        rethrow: false
    })
    handleRange(
        ...[
            {
                textDocument,
                range: { start, end }
            }
        ]: Parameters<OnRangeHandlerType>
    ): ReturnType<OnRangeHandlerType> {
        try {
            const cache = this.docCache.getCache(textDocument.uri);

            if (!cache || !cache.ast || !cache.symbols) return { data: [] };

            const compPos = (a: Position, b: Position): number => a.line - b.line || a.column - b.column;

            const lastLE = (target: Position) =>
                upperBound(cache.ast!.statements, stmt => compPos(stmt.position, target) <= 0);

            let startIdx = lastLE({ line: start.line + 1, column: start.character + 1 });
            let endIdx = lastLE({ line: end.line + 1, column: end.character + 1 });

            if (startIdx === -1) startIdx = 0;
            if (endIdx === -1) endIdx = 0;

            if (startIdx > endIdx) {
                startIdx = 0;
                endIdx = 0;
            }

            const rangs = cache.ast.statements.slice(startIdx, endIdx + 1);

            const context: HandlerContext = {
                prev: { line: 1, column: 1 },
                table: cache.symbols,
                tokens: cache.tokens ?? []
            };

            // 注意：范围请求不补注释 token。注释可能落在请求区间之外，要正确裁剪得再加一层边界判断，
            // 而两个真实客户端（VS Code 与 IntelliJ）都用全文档请求，这里不值得为它增加复杂度。
            // Note: range requests do not add comment tokens. A comment may sit outside the requested range,
            // and clipping it correctly needs another boundary check — while both real clients (VS Code and
            // IntelliJ) ask for the full document, so the complexity is not worth it here.
            return {
                data: rangs.flatMap(n => {
                    const tks = this.visitStatement(n, context);

                    return tks.flatMap(t => [t.line, t.start, t.length, t.type, t.modifier]);
                })
            };
        } catch (error) {
            Console.error((error as Error).message, "Semantic Range");
        }
    }

    private visitProgram(program: Program, context: HandlerContext): number[] {
        const data: number[] = [];
        const comments = this.plainComments(context);
        let next = 0;

        for (const statement of program.statements) {
            next = this.appendCommentsBefore(data, comments, next, context, statement.position);

            for (const token of this.visitStatement(statement, context))
                data.push(token.line, token.start, token.length, token.type, token.modifier);
        }

        // 文件末尾（最后一条语句之后）的注释
        // Comments after the last statement
        this.appendCommentsBefore(data, comments, next, context, { line: Number.MAX_SAFE_INTEGER, column: Number.MAX_SAFE_INTEGER });

        return data;
    }

    /**
     * @if zh
     * @summary 取出纯注释 token
     *
     * @details 只保留 `TokenCategory.COMMENT`（`# …` 与 `// …`）：类型提示与注解块的前缀（`#:`、`#>`）
     *          是独立的 token 类型、类别为注解，且已由各自的访问器逐段着色，整段再涂一遍会与之重叠。
     *
     * @else
     * @summary Picks out the plain comment tokens
     *
     * @details Only `TokenCategory.COMMENT` is kept (`# …` and `// …`): the type-hint and annotation-block
     *          prefixes (`#:`, `#>`) are token types of their own with the annotation category, and their
     *          visitors already colour them segment by segment — painting the whole range again would
     *          overlap those.
     *
     * @endif
     * */
    private plainComments(context: HandlerContext): Token[] {
        return context.tokens.filter(token => token.category === TokenCategory.COMMENT);
    }

    /**
     * @if zh
     * @summary 把位于 `position` 之前的注释按文档顺序写入
     *
     * @details 语义 token 是**增量编码**（见 {@link getGap}），所以注释必须与语句在同一次遍历中按文档顺序
     *          交织，不能先遍历完语句再追加——否则增量全是负数，客户端会解出错误的列。
     *
     * @else
     * @summary Writes the comments that precede `position`, in document order
     *
     * @details Semantic tokens are **delta-encoded** (see {@link getGap}), so comments have to be interleaved
     *          with the statements in one document-order traversal rather than appended afterwards; otherwise
     *          the deltas go negative and the client resolves the wrong columns.
     *
     * @endif
     *
     * @param data - 编码目标 / the encoded output being built
     * @param comments - 待写注释（文档顺序）/ the comments, in document order
     * @param from - 从该下标开始 / index to start at
     * @param context - 处理器上下文 / the handler context
     * @param position - 写到哪里为止 / the position to stop before
     * @returns 下一次应使用的下标 / the index to use next time
     * */
    private appendCommentsBefore(
        data: number[],
        comments: Token[],
        from: number,
        context: HandlerContext,
        position: Position
    ): number {
        let next = from;

        while (next < comments.length && this.isBefore(comments[next].pos, position)) {
            const token = comments[next++];
            const gap = this.getGap(context, token.pos);

            data.push(gap.line, gap.column, token.lexeme.length, TokenLegend.Comment, 0);
        }

        return next;
    }

    /** @if zh 1 基位置的先后比较 / @else Document-order comparison of 1-based positions / @endif */
    private isBefore(a: Position, b: Position): boolean {
        return a.line < b.line || (a.line === b.line && a.column < b.column);
    }

    private visitStatement(statement: Statement, context: HandlerContext): SemanticToken[] {
        if (AST.belongInstruction(statement)) return this.visitInstruction(statement, context);

        const mthName = `visit${statement.nodeName}` as const;

        if (mthName in this) return (this as any)[mthName](statement, context);

        Console.warning(`Unknown statement type: ${statement.nodeName}`, "SemanticToken");
        return [];
    }

    private visitInstruction(instruction: PureExeInstructionNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        const gap = this.getGap(context, instruction.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: instruction.keyword.length,
            type: TokenLegend.Keyword,
            modifier: 0
        });

        Object.entries(instruction).forEach(([key, value]) => {
            if (key.startsWith("operand")) {
                const typeKey = key.replace("operand", "type");

                result.push(...this.handleOperand(value, context, (instruction as any)[typeKey]));
            }
        });

        return result;
    }

    private visitLabelDef(labelDef: LabelDefNode, context: HandlerContext): SemanticToken[] {
        if (AST.isIdentifier(labelDef.identifier)) {
            const gap = this.getGap(context, labelDef.position);

            return [
                {
                    line: gap.line,
                    start: gap.column,
                    length: labelDef.identifier.value.length,
                    type: TokenLegend.Label,
                    modifier: 0
                }
            ];
        }

        return [];
    }

    private visitAliasDirective(aliasDirective: AliasDirectiveNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, aliasDirective.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: 5,
            type: TokenLegend.Keyword,
            modifier: 0
        });

        if (AST.isIdentifier(aliasDirective.identifier)) {
            gap = this.getGap(context, aliasDirective.identifier.position);

            result.push({
                line: gap.line,
                start: gap.column,
                length: aliasDirective.identifier.value.length,
                type: visit<RegOrDev, TokenLegend>(
                    {
                        ...groupHandlers<RegOrDev["nodeName"]>(
                            ["GeneralPurposeRegister", "AddressRegister", "StackPointerRegister", "DynamicRegister"],
                            () => TokenLegend.RegisterIdentifier
                        ),
                        ...groupHandlers<RegOrDev["nodeName"]>(
                            ["StaticDevice", "DynamicDevice"],
                            () => TokenLegend.DeviceIdentifier
                        ),
                        Error: () => TokenLegend.Unknown
                    },
                    aliasDirective.registerOrDevice
                ),
                modifier: this.modifierBits(TokenModifier.Declaration)
            });
        }

        result.push(...this.handleOperand(aliasDirective.registerOrDevice, context));

        if (aliasDirective.typeHint) result.push(...this.handleTypeHint(aliasDirective.typeHint, context));

        return result;
    }

    private visitDefineDirective(defineDirective: DefineDirectiveNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, defineDirective.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: 6,
            type: TokenLegend.Keyword,
            modifier: 0
        });

        if (AST.isIdentifier(defineDirective.identifier)) {
            gap = this.getGap(context, defineDirective.identifier.position);

            result.push({
                line: gap.line,
                start: gap.column,
                length: defineDirective.identifier.value.length,
                type: TokenLegend.NumberIdentifier,
                modifier: this.modifierBits(TokenModifier.Declaration)
            });
        }

        result.push(...this.handleOperand(defineDirective.operand, context));

        if (defineDirective.typeHint) result.push(...this.handleTypeHint(defineDirective.typeHint, context));

        return result;
    }

    /**
     * @summary 类型提示的分段着色
     *
     * @summary Per-segment coloring of a type hint
     *
     * @desc 类型提示 `#: @type Foo @desc "文字"` 本身就是由多个词法 token 拼成的：`#:` 前缀、
     * 每个 `@标签`、标签的取值（类型名或描述串）。整段发一个 token 只能整体着色，因此这里按词法
     * token 逐段发射：`#:` → Decorator，`@标签` → AnnotationTag，类型名 → Enum（与 `Foo.Bar`
     * 的类型名同色），描述串 → String。未映射的片段（例如 `@desc` 的链接写法）不着色，交给语法着色。
     *
     * @desc A type hint like `#: @type Foo @desc "text"` is a sequence of lexical tokens: the `#:`
     * prefix, each `@tag`, and each tag's value (a type name or a description string). A single token
     * for the whole hint can only color it as a whole, so this emits one semantic token per lexical
     * token: `#:` → Decorator, `@tag` → AnnotationTag, the type name → Enum (same color as the type
     * name in `Foo.Bar`), the description string → String. Unmapped segments (e.g. a `@desc` link)
     * are left to the syntax grammar.
     *
     * @param hint 类型提示节点
     * @param hint Type hint node
     * @param context 处理器上下文（其中 `tokens` 为该文档的词法 token）
     * @param context Handler context (`tokens` holds the document's lexical tokens)
     *
     * @returns 该提示各片段对应的语义 token
     * @returns The semantic tokens for the hint's segments
     * */
    private handleTypeHint(hint: TypeHintNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        /** 最近一个标签的写法（`@type` / `@desc` / `@builtin`）：决定后面的取值怎么着色 */
        let tag: Optional<string>;

        const emit = (pos: Position, length: number, type: TokenLegend) => {
            const gap = this.getGap(context, pos);

            result.push({ line: gap.line, start: gap.column, length, type, modifier: 0 });
        };

        for (const token of context.tokens) {
            const pos = token.pos;

            if (pos.line < hint.position.line) continue;

            // 类型提示不跨行：越过该行即结束
            if (pos.line > hint.position.line) break;

            // 提示之前的部分（关键字、别名、设备）由语句自身处理
            if (pos.column < hint.position.column) continue;

            // 同一行还有下一条语句（罕见）时交回语句处理，避免把关键字当成提示片段
            if (token.type === TokenType.NEWLINE || token.type === TokenType.END) break;
            if (token.type === TokenType.KEYWORD) break;

            switch (token.type) {
                // `#:` 前缀是提示标记本身
                case TokenType.TYPE_HINT_PREFIX:
                case TokenType.TYPE_ANNOTATION_PREFIX:
                    emit(pos, token.lexeme.length, TokenLegend.Decorator);
                    break;
                // 每个 `@标签` 单独取色（主题里的 AnnotationTag）
                case TokenType.TAG:
                    tag = token.lexeme;
                    emit(pos, token.lexeme.length, TokenLegend.AnnotationTag);
                    break;
                // 只有紧跟 `@type` 的标识符才是类型名；`@desc` 的链接写法由路径片段组成，不做处理
                case TokenType.IDENTIFIER:
                    if (tag === "@type") emit(pos, token.lexeme.length, TokenLegend.Enum);
                    break;
                // 描述文本
                case TokenType.STRING:
                    emit(pos, token.lexeme.length, TokenLegend.String);
                    break;
            }
        }

        return result;
    }

    private visitError(error: ErrorNode, context: HandlerContext): SemanticToken[] {
        return this.handleError(error, context);
    }

    /**
     * @summary 设备块注解（`#>`）的分段着色
     *
     * @summary Per-segment coloring of a device annotation block (`#>`)
     *
     * @desc 设备块是一段多行语句（`#> @device` … `#> @end-device`），逐段着色见
     * {@link handleAnnotationLines}。
     *
     * @desc A device block is a single multi-line statement (`#> @device` … `#> @end-device`); see
     * {@link handleAnnotationLines} for the per-segment mapping.
     * */
    private visitDeviceAnnotation(annotation: DeviceAnnotation, context: HandlerContext): SemanticToken[] {
        return this.handleAnnotationLines(annotation.position.line, annotation.end.line, context);
    }

    /**
     * @summary 枚举块注解（`#>`）的分段着色
     *
     * @summary Per-segment coloring of an enum annotation block (`#>`)
     *
     * @desc 枚举块与设备块共用同一套标签行写法，故复用 {@link handleAnnotationLines}。
     *
     * @desc An enum block uses the same tag lines as a device block, so it reuses
     * {@link handleAnnotationLines}.
     * */
    private visitEnumAnnotation(annotation: EnumAnnotation, context: HandlerContext): SemanticToken[] {
        return this.handleAnnotationLines(annotation.position.line, annotation.end.line, context);
    }

    /**
     * @summary 块注解标签行的分段着色
     *
     * @summary Per-segment coloring of the tag lines of an annotation block
     *
     * @desc `#>` 块由多行标签行组成（`#> @logic Setting 12 1`），与 {@link handleTypeHint} 共用
     * 同一套按词法 token 的分段映射：`#>` 前缀 → Decorator，`@标签` → AnnotationTag，
     * `@name` 后的类型名 → Enum，成员名（`@logic Setting 12` 的 `Setting`、`@value Green 2` 的
     * `Green`）→ EnumMember，编号/序号/默认值/哈希 → Number，描述串 → String。
     * 未映射的片段（如 `@desc` 的链接写法）不着色，交给语法着色。
     *
     * @desc A `#>` block is a sequence of tag lines (`#> @logic Setting 12 1`) and shares the
     * per-lexical-token mapping of {@link handleTypeHint}: the `#>` prefix → Decorator, each `@tag` →
     * AnnotationTag, the type name after `@name` → Enum, member names (the `Setting` of
     * `@logic Setting 12`, the `Green` of `@value Green 2`) → EnumMember, indices/slot numbers/
     * defaults/hashes → Number, and description strings → String. Unmapped segments (e.g. a `@desc`
     * link) are left to the syntax grammar.
     *
     * @param firstLine 块的起始行（`#> @device` 所在行）
     * @param firstLine The block's first line (the `#> @device` line)
     * @param lastLine 块的结束行（`#> @end-device` 所在行）
     * @param lastLine The block's last line (the `#> @end-device` line)
     * @param context 处理器上下文（其中 `tokens` 为该文档的词法 token）
     * @param context Handler context (`tokens` holds the document's lexical tokens)
     *
     * @returns 各标签行片段对应的语义 token
     * @returns The semantic tokens for the tag lines' segments
     * */
    private handleAnnotationLines(firstLine: number, lastLine: number, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        /** 最近一个标签的写法：决定后面的取值怎么着色 */
        let tag: Optional<string>;

        const emit = (pos: Position, length: number, type: TokenLegend) => {
            const gap = this.getGap(context, pos);

            result.push({ line: gap.line, start: gap.column, length, type, modifier: 0 });
        };

        for (const token of context.tokens) {
            const line = token.pos.line;

            if (line < firstLine) continue;

            // 块注解不跨块：越过结束行即结束
            if (line > lastLine) break;

            switch (token.type) {
                // `#>` 前缀是注解标记本身
                case TokenType.TYPE_ANNOTATION_PREFIX:
                case TokenType.TYPE_HINT_PREFIX:
                    emit(token.pos, token.lexeme.length, TokenLegend.Decorator);
                    break;
                // 每个 `@标签` 单独取色（主题里的 AnnotationTag）
                case TokenType.TAG:
                    tag = token.lexeme;
                    emit(token.pos, token.lexeme.length, TokenLegend.AnnotationTag);
                    break;
                case TokenType.IDENTIFIER:
                    // `@name Sensor` 声明的是型号/枚举名；成员行的名字是声明出来的成员（`Setting`、`Slot0`）
                    if (tag === "@name") emit(token.pos, token.lexeme.length, TokenLegend.Enum);
                    else if (tag && ANNOTATION_MEMBER_TAGS.has(tag))
                        emit(token.pos, token.lexeme.length, TokenLegend.EnumMember);
                    break;
                // 属性编号、槽位序号、默认值、哈希
                case TokenType.INTEGER:
                case TokenType.FLOAT:
                case TokenType.HEX_NUMBER:
                case TokenType.BINARY_NUMBER:
                    emit(token.pos, token.lexeme.length, TokenLegend.Number);
                    break;
                // 描述文本
                case TokenType.STRING:
                    emit(token.pos, token.lexeme.length, TokenLegend.String);
                    break;
            }
        }

        return result;
    }

    private handleError(error: ErrorNode, context: HandlerContext): SemanticToken[] {
        const gap = this.getGap(context, error.position);

        return [
            {
                line: gap.line,
                start: gap.column,
                length: error.end.column - error.position.column,
                type: TokenLegend.Unknown,
                modifier: 0
            }
        ];
    }

    private handleStaticDevice(
        staticDevice: StaticDeviceNode,
        context: HandlerContext,
        operandType?: OperandType
    ): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, staticDevice.position);

        const type = TypeOfNode[staticDevice.nodeName];

        result.push({
            line: gap.line,
            start: gap.column,
            length: staticDevice.device.end.column - staticDevice.device.position.column,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        if (staticDevice.pin) result.push(...this.handleOperand(staticDevice.pin, context));

        return result;
    }

    private handleDynamicDevice(dynamicDevice: DynamicDeviceNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, dynamicDevice.position);

        const type = TypeOfNode[dynamicDevice.nodeName];

        result.push({
            line: gap.line,
            start: gap.column,
            length: dynamicDevice.end.column - dynamicDevice.position.column,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        return result;
    }
    
    private handleDynamicRegister(dynamicRegister: DynamicRegisterNode, context: HandlerContext) {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, dynamicRegister.position);

        const type = TypeOfNode[dynamicRegister.nodeName];

        result.push({
            line: gap.line,
            start: gap.column,
            length: dynamicRegister.end.column - dynamicRegister.position.column,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        return result;
    }

    /**
     * @summary 处理枚举操作数（多 token 操作数）
     *
     * @summary Handle an enum operand (a multi-token operand)
     *
     * @desc 枚举操作数 `Foo.Bar` 由两个 token 组成，需要分别发射：类型名 `Foo` 与成员 `Bar`，
     * 两者之间以点号分隔（点号本身不着色）。值缺失（`Foo.` 未输入完）时只有类型名一个 token。
     *
     * @desc An enum operand `Foo.Bar` consists of two tokens that must be emitted separately:
     * the type name `Foo` and the member `Bar`, separated by a dot (the dot itself is not
     * colored). When the value is missing (an incomplete `Foo.` ), only the type name is emitted.
     * */
    private handleEnum(enumNode: EnumNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        const gap = this.getGap(context, enumNode.position);

        if (!AST.isIdentifier(enumNode.name)) return result;

        result.push({
            line: gap.line,
            start: gap.column,
            length: enumNode.name.value.length,
            type: TokenLegend.Enum,
            modifier: 0
        });

        if (AST.isIdentifier(enumNode.value)) {
            const valueGap = this.getGap(context, enumNode.value.position);

            result.push({
                line: valueGap.line,
                start: valueGap.column,
                length: enumNode.value.value.length,
                type: TokenLegend.EnumMember,
                modifier: 0
            });
        }

        return result;
    }

    private handleOperand(node: Operand, context: HandlerContext, operandType?: OperandType): SemanticToken[] {
        // Identifier 需要 operandType 参数，优先处理
        if (AST.isIdentifier(node)) return [this.handleIdentifier(node, context, operandType)];

        // 派发到专用 handler（HashCall / StrCall 等），
        // 避免被下面泛化的 "value" in node 检查误匹配
        const mthName = `handle${node.nodeName}` as const;

        if (mthName in this) {
            const result = (this as any)[mthName](node, context);

            return Array.isArray(result) ? result : [result];
        }

        if ("value" in node) {
            const gap = this.getGap(context, node.position);

            const type = TypeOfNode[node.nodeName];

            return [
                {
                    line: gap.line,
                    start: gap.column,
                    length: typeof node.value === "string" ? node.value.length : node.value.toString().length,
                    type: this.toLegend(type.kind, type.category),
                    modifier: 0
                }
            ];
        }

        Console.warning(`Unknown operand type: ${node.nodeName}`, "SemanticToken");
        return [];
    }

    private handleIdentifier(
        identifier: IdentifierNode,
        context: HandlerContext,
        operandType?: OperandType
    ): SemanticToken {
        const gap = this.getGap(context, identifier.position);

        const symbol = context.table.symbols[identifier.value];

        let type: Optional<TokenLegend>;
        operandType ??= OperandType.CONST_NUM; // 使operandType落在default
        switch (operandType) {
            case OperandType.LOGIC_PROP:
            case OperandType.LOGIC_SLOT_PROP:
            case OperandType.AGG_MODE:
                type = TokenLegend.Constant;
                break;
            default:
                type = symbol ? this.toLegend(symbol.type, symbol.category, true) : TokenLegend.Unknown;
                break;
        }

        return {
            line: gap.line,
            start: gap.column,
            length: identifier.value.length,
            type,
            modifier: 0
        };
    }

    private handleStrMacro(strMacro: StrMacroNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, strMacro.position);

        const type = TypeOfNode[strMacro.nodeName];

        result.push({
            line: gap.line,
            start: gap.column,
            length: 3,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        if (AST.isString(strMacro.value)) {
            gap = this.getGap(context, strMacro.value.position);

            result.push({
                line: gap.line,
                start: gap.column,
                length: strMacro.value.value.length,
                type: TokenLegend.String,
                modifier: 0
            });
        }

        return result;
    }

    private handleHashMacro(hashMacro: HashMacroNode, context: HandlerContext): SemanticToken[] {
        if (!hashMacro.value || hashMacro.value.nodeName === "Error") {
            Console.warning("handleHashCall: missing value data", "SemanticToken");
            return [];
        }

        const result: SemanticToken[] = [];

        let gap = this.getGap(context, hashMacro.position);

        const type = TypeOfNode[hashMacro.nodeName];

        result.push({
            line: gap.line,
            start: gap.column,
            length: 4,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        if (AST.isString(hashMacro.value)) {
            gap = this.getGap(context, hashMacro.value.position);

            result.push({
                line: gap.line,
                start: gap.column,
                length: hashMacro.value.value.length,
                type: TokenLegend.String,
                modifier: 0
            });
        }

        return result;
    }

    private modifierBits(...modifiers: TokenModifier[]): number {
        let bits = 0;
        for (const m of modifiers) bits |= 1 << m;

        return bits;
    }

    private toLegend(type: BasicType, category: TypeCategory, isIdentifier?: boolean): TokenLegend {
        switch (category) {
            case TypeCategory.LABEL:
                return isIdentifier ? TokenLegend.LabelIdentifier : TokenLegend.Label;
            case TypeCategory.NUMBER:
            case TypeCategory.CONSTANT:
                return isIdentifier ? TokenLegend.NumberIdentifier : TokenLegend.Number;
            case TypeCategory.HASH_CALL:
            case TypeCategory.STR_CALL:
                return isIdentifier ? TokenLegend.NumberIdentifier : TokenLegend.Macro;
        }

        switch (type) {
            case BasicType.DEVICE:
                return isIdentifier ? TokenLegend.DeviceIdentifier : TokenLegend.Device;
            case BasicType.REGISTER:
                return isIdentifier ? TokenLegend.RegisterIdentifier : TokenLegend.Register;
            case BasicType.STRING:
                return TokenLegend.String;
        }

        return TokenLegend.Unknown;
    }

    private getGap(context: HandlerContext, pos: Position): Position {
        const lineGap = pos.line - context.prev.line;

        const result = { line: lineGap, column: lineGap <= 0 ? pos.column - context.prev.column : pos.column - 1 };

        context.prev = pos;

        return result;
    }
}
