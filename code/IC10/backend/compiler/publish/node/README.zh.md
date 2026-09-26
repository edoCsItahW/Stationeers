# ic10c-node

[![npm 版本](https://badge.fury.io/js/ic10c-node.svg)](https://www.npmjs.com/package/ic10c-node)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![许可证: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[English](./README.md)

## 简介

`ic10c-node` 是 **IC10 编译器** 的 Node.js 原生绑定模块：它把整套 C++23 核心 —— 词法分析器、语法分析器、
语义分析器、链接器与增量编译器 —— 暴露给 JavaScript / TypeScript。

IC10 是 [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) 中用来给游戏内计算机与设备
编程的汇编式语言。本模块让你在 Node.js 里编译、分析、链接并检视 IC10 代码；原生模块已预编译随包发布，
**无需** C++ 工具链。

当前版本：**3.0.0**（IC10 v3 —— 重写词法/语法分析并统一按元数生成的 AST）。变更见
[CHANGELOG.zh.md](./CHANGELOG.zh.md) / [CHANGELOG.md](./CHANGELOG.md)。

## 特性

- **词法分析器** —— 切分 IC10 源码（寄存器、设备、`$hex` / `%bin` / 十进制 / 浮点数字、字符串、注解前缀、
  注释与指令关键字）。返回的是**原始 token 流**：`NEWLINE`、`COMMENT`、`END` 都在其中。
- **语法分析器** —— 为全部指令元数（零元到六元）、标签、`alias` / `define` 指令与 `#>` 注解块构建 AST。
- **语义分析器** —— 基于 `Promise`/`Future` 的符号表做符号解析、类型推导与操作数检查，前向引用天然可用；
  同时产出符号表**与类型表**。
- **链接器** —— 合并多个编译单元、解析跨单元引用，并给出合并后的符号表、类型表与各单元诊断。
- **增量编译** —— 行级词法缓存与语句级语法缓存（`IncLexer`、`IncParser`、`IncCompiler`），只重新处理改动的
  行与语句。
- **诊断信息** —— 各阶段统一报 `{level, id, start, end, message}`；消息支持英文（`en-us`）与简体中文
  （`zh-hans`）。
- **完整 TypeScript 类型** —— 随包发布，无需任何 `@types/*`。
- **跨平台** —— 可在 Linux（GCC/Clang）与 Windows（MSVC）上构建。

## 安装

### 从 npm 安装

```bash
npm install ic10c-node
```

已发布的包里就包含编译好的 `src/ic10c-node.node`，因此安装**不需要** C++ 编译器、CMake 或 node-gyp。
要求 Node.js >= 16（项目实际构建与测试使用的是 Node.js 26）。

### 从源码构建

构建原生模块属于仓库构建的一部分，完整工具链见
[DEVELOPER_GUIDE.zh.md](https://github.com/edoCsItahW/Stationeers/blob/main/DEVELOPER_GUIDE.zh.md)。
简要地说需要 CMake >= 3.28.1 与 C++23 编译器（GCC 13+ / Clang 16+ / MSVC 2022）。

```bash
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers

# Linux / macOS —— 配置 CMake、构建模块并跑 Node.js 测试
code/scripts/bashShell/buildIC10CompilerNode.sh
```

```powershell
# Windows（PowerShell）
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
pwsh code/scripts/powerShell/BuildIC10CompilerNode.ps1
```

两个脚本都会构建 CMake 目标 `ic10_compiler_node`，并把产物复制到
`code/IC10/backend/compiler/publish/node/src/ic10c-node.node`。若想自己驱动 CMake：

```bash
cd code
cmake -B build -S . -DBUILD_IC10_COMPILER_EXPORTS_NODE=ON
cmake --build build --target ic10_compiler_node --config Release
```

## 快速开始

```typescript
import * as ic10 from 'ic10c-node';

// 消息切换为简体中文（可选，默认 en-us）
ic10.IC10CompilerLocal.setLanguage('zh-hans');

const source = `
    alias disp d0
    main:
        move r0 42
        yield
        jal main
`;

// 1. 词法分析 —— 原始 token 流，包含 NEWLINE/COMMENT
const tokens = ic10.Lexer.tokenize(source);

// 2. 语法分析 —— token 转 AST
const parser = new ic10.Parser(tokens);
const program = parser.parse();

// 3. 语义分析 —— 解析符号、检查操作数与类型
const analyser = new ic10.Analyser();
await analyser.visit(program);          // 异步：前向引用由协程解析

// 4. 取结果
console.log(analyser.symbolTable.toJSON());   // {"symbols":{...},"builtinSymbols":{"d0":...}}
console.log(analyser.typeTable.toJSON());     // #> 块声明的设备/枚举类型
for (const d of analyser.diagnostics)         // 错误、警告、提示
    console.log(`${d.level} ${d.id} 第 ${d.start.line} 行: ${d.message}`);
```

读取 AST：节点通过 `toJSON()` 序列化，并以 `nodeName` 标识自身。

```typescript
const json = JSON.parse(program.toJSON());
console.log(json.statements.map(s => s.nodeName));
// [ 'AliasDirective', 'LabelDef', 'moveInstruction', 'yieldInstruction', 'jalInstruction' ]

console.log(json.statements[2]);
// {
//   nodeName: 'moveInstruction',
//   position: { line: 3, column: 5, offset: 24 },
//   end:      { line: 3, column: 15, offset: 34 },
//   keyword: 'move',
//   operand1: { nodeName: 'GeneralPurposeRegister', ..., value: 'r0' },
//   type1: 0,                       // OperandType 枚举值（0 = REG_TARGET）
//   operand2: { nodeName: 'Integer', ..., value: '42' },
//   type2: 2                        // （2 = NUM_VALUE）
// }
```

### 静态入口

`Lexer.tokenize` 与 `Parser.parsing` 是便捷方法；`Analyser.analyse` 会跑完整套分析但**丢弃结果**，
需要符号表或诊断时请用实例。

```typescript
import { Lexer, Parser, Analyser } from 'ic10c-node';

const tokens  = Lexer.tokenize('add r0 r1 r2');
const program = Parser.parsing(tokens);         // 等价于 new Parser(tokens).parse()

await Analyser.analyse(program);                // 便捷入口：不返回结果
```

## API 参考

### 导出总览

| 类别 | 名称 |
|:-----|:-----|
| 类 | `Lexer`、`Parser`、`Program`、`Analyser`、`SymbolTable`、`TypeTable`、`Linker`、`IncLexer`、`IncParser`、`IncCompiler`、`Token`、`Pos`、`IC10CompilerLocal` |
| 枚举 | `TokenType`（27 项）、`TokenCategory`（7 项）、`InstructionKeyword`（147 项）、`BasicType`（7 项）、`TypeCategory`（6 项）、`OperandType`（17 项） |
| 值 / 函数 | `TypeOfNode` |
| 错误构造器 | `SyntaxError`、`RuntimeError`、`RedefinitionError`、`TypeError`、`UndefinedSymbolError` |

| 类 | 用途 | 成员 |
|:---|:-----|:-----|
| `Lexer` | 源码 → token | `static tokenize(source)`、`scan()`、`diagnostics` |
| `Parser` | token → AST | `parse()`、`static parsing(tokens, debug?)`、`diagnostics` |
| `Program` | AST 根节点 | `statements`、`end`、`toString()`、`toJSON()` |
| `Analyser` | 符号、类型与操作数检查 | `visit(program)`（异步）、`symbolTable`、`typeTable`、`diagnostics`、`static analyse(program)` |
| `SymbolTable` | 已解析的符号 | `toJSON()` |
| `TypeTable` | `#>` 块声明的设备/枚举类型 | `toJSON()` |
| `Linker` | 多单元编译 | `addUnit(source \| program, path?)`、`link()`、`diagnostics`、`units`、`typeTable` |
| `IncLexer` / `IncParser` / `IncCompiler` | 增量编译 | `tokenizeFull` / `tokenizeInc`、`parseFull` / `parseInc`、`compileFull` / `compileInc`、`hasCache()`、`clear()`，以及结果结构上的 `diagnostics` |
| `Token` | 单个词法标记 | `type`、`pos`、`lexeme`、`category`、`keyword`、`toString()`、`toJSON()` |
| `Pos` | 位置信息 | `line`、`column`、`offset`、`newline()`、`next()`、`move(index)` |
| `IC10CompilerLocal` | 消息语言 | `static setLanguage('en-us' \| 'zh-hans')` |

### Lexer

```typescript
import { Lexer, TokenCategory } from 'ic10c-node';

const tokens = Lexer.tokenize('alias ic d0');

// 实例形式会把诊断留在实例上
const lexer = new Lexer('"unterminated');
lexer.scan();
console.log(lexer.diagnostics[0].id);        // 'IEL2_1'

console.log(tokens.map(t => t.category).map(c => Object.keys(TokenCategory).find(k => TokenCategory[k] === c)));
// [ 'LITERAL', 'LITERAL', 'LITERAL', 'END' ] —— 只有源码里真有换行时才会出现 NEWLINE token
console.log(tokens[0].toString());           // Token<1,1>[KEYWORD_ALIAS]('alias')
```

`TokenCategory` 取值为 `LITERAL: 0`、`SYMBOL: 1`、`COMMENT: 2`、`ANNOTATION: 3`、`WHITESPACE: 4`、
`END: 5`、`INVALID: 6`。注意 `WHITESPACE` 同时涵盖空格与 `NEWLINE`，且注释也在 token 流中（由语法分析器
跳过）。构造函数的 `debug` 参数目前是预留项，**不改变任何行为**。

### Parser

```typescript
import { Lexer, Parser } from 'ic10c-node';

const parser = new Parser(Lexer.tokenize('main:\n    move r0 1\n'));
const program = parser.parse();

console.log(program.statements.length);      // 2
console.log(program.statements[0].nodeName); // 'LabelDef' —— 语句是普通对象
console.log(parser.diagnostics);             // 成功时为 []
```

> 标签名不能是指令关键字：`sub:` **不是**标签 —— `sub` 会被词法分析器识别为关键字，语法分析器于是把它当指令
> 解析并报出操作数错误；`main:`、`helper:` 这类名字才正常。
```

### Analyser

```typescript
import { Lexer, Parser, Analyser } from 'ic10c-node';

const program  = Parser.parsing(Lexer.tokenize('main:\n    move r0 1\n'));
const analyser = new Analyser();

await analyser.visit(program);               // 必须 await：前向引用会在稍后恢复

console.log(analyser.symbolTable.toJSON());
// { symbols: { main: { name: 'main', type: 1, category: 0, value: '1', builtin: false } },
//   builtinSymbols: { d0: {...}, d1: {...}, ... } }
console.log(analyser.typeTable.toJSON());    // 源码未声明 #> 类型时为 {}
console.log(analyser.diagnostics);           // 语义错误，例如未定义标识符的 IEA3_1
```

### Linker

```typescript
import { Linker } from 'ic10c-node';

const linker = new Linker();
linker.addUnit('main:\n    move r0 1\n', 'a.ic');       // 源码 + 路径
linker.addUnit('helper:\n    move r1 2\n', 'b.ic');

const symbols = linker.link();               // 合并后的 SymbolTable
console.log(Object.keys(JSON.parse(symbols.toJSON()).symbols));  // [ 'main', 'helper' ]
console.log(linker.units);                   // [ { path: 'a.ic', diagnostics: [] }, ... ]
console.log(linker.typeTable.toJSON());      // 合并后的类型表
```

各单元是**并发**分析的，因此某个单元可以引用后面单元定义的符号。`linker.diagnostics` 汇总的是每个单元
**语义**阶段的诊断（由各单元自己的报告器合并而来）；以字符串形式加入的单元是在内部完成解析的，所以若还想拿到
语法诊断，请先自行解析源码。
```

### 增量编译

```typescript
import { IncCompiler } from 'ic10c-node';

const compiler = new IncCompiler();
const first  = compiler.compileFull(source);
const second = compiler.compileInc(source.replace('42', '43'));

console.log(first.incremental, second.incremental);   // false、true
console.log(second.relexedLines, second.reparsedStmts); // 重新词法 1 行、重新解析 3 条语句
compiler.clear();                                     // 丢弃缓存
```

`IncLexer`（`tokenizeFull` / `tokenizeInc`）与 `IncParser`（`parseFull` / `parseInc`）把这两半分别暴露出来。

三者的结果结构都带有 `diagnostics`：由各自的缓存汇总，**覆盖整份源码**而不只是变化区间——未变化的
行/语句沿用其缓存时的诊断，变化的行/语句用本次诊断，后缀的行号与偏移变化会同步平移诊断位置。
因此编辑器可以在增量路径下直接提交词法与语法诊断，无需为了拿到诊断而回退到全量解析。

`diagnostics` **不含语义诊断**：语义分析属于 `Linker` / `Analyser`，需要另行执行（见下）。

```typescript
const lexer  = new IncLexer();
const parser = new IncParser();

lexer.tokenizeFull(source);
parser.parseFull(lexer.tokens);

const lex    = lexer.tokenizeInc(edited);
const parsed = parser.parseInc(lex.tokens, lex.changedStartLine);

console.log(lex.diagnostics, parsed.diagnostics);   // 整份源码的词法/语法诊断
```

### 诊断

```typescript
import { Lexer, Parser, Analyser } from 'ic10c-node';

const parser = new Parser(Lexer.tokenize('move r0'));   // 缺少操作数
parser.parse();

for (const d of parser.diagnostics) {
    console.log(`${d.level} ${d.id}: ${d.message}`);
    console.log(`  第 ${d.start.line} 行，第 ${d.start.column} 列`);
    // error IEP34_1: Not one of the types in 'Integer|Float|...|Error' - syntax analysis.
}
```

每个诊断形如 `{ level: 'error' | 'warning' | 'info', id, start, end, message }`，其中 `id` 是稳定的消息编号
（`IEL*` 词法、`IEP*` 语法、`IEA*` 语义），`start` / `end` 是 `Pos` 对象。它们是访问器实现的，请直接读
`d.start.line` —— `JSON.stringify(d)` 会把它们打印成 `{}`。

### 消息语言

```typescript
import { IC10CompilerLocal } from 'ic10c-node';

IC10CompilerLocal.setLanguage('zh-hans');   // 简体中文
IC10CompilerLocal.setLanguage('en-us');     // 英文（默认）
```

### 节点类型表

`TypeOfNode` 把 AST 节点类型名映射为它产出的类型，取值为 `BasicType` / `TypeCategory` 枚举。它是一个普通
查表对象，共 16 项（`Integer`、`Float`、`String`、`StrMacro`、`HashMacro`、四种寄存器、四种设备、`Enum`、
`LabelDef`、`Error`、`BinaryNumber`、`HexNumber`）。

```typescript
import { TypeOfNode, BasicType, TypeCategory } from 'ic10c-node';

TypeOfNode.Integer;                 // { kind: BasicType.INTEGER, category: TypeCategory.NUMBER }
TypeOfNode.GeneralPurposeRegister;  // { kind: BasicType.REGISTER, category: TypeCategory.BASIC }
```

## TypeScript

本包自带类型声明，位于 `types/`（`index.d.ts` 再导出 `lexer`、`parser`、`semantic`、`linker`、
`incremental`、`locale` 与 `common`），无需额外安装 `@types` 包。

```typescript
import * as ic10 from 'ic10c-node';

const tokens: ic10.Token[] = ic10.Lexer.tokenize('move r0 r1');
const program: ic10.Program = ic10.Parser.parsing(tokens);
```

`Token` 与 `Pos` 实例应当**由 API 产出**，不要用 `new` 自行构造；请走 `Lexer` / `Parser` 这条路径。

## 包结构

```
publish/node/
├── src/
│   ├── ic10c-node.node        # 预编译原生模块（由 CMake 产出）
│   └── stdLib.ic              # 随编译器分发的标准库
├── types/                     # TypeScript 类型声明（index/lexer/parser/semantic/linker/incremental/locale/common）
├── CHANGELOG.md
├── CHANGELOG.zh.md
├── README.md
├── README.zh.md
├── tsconfig.json
└── package.json
```

## 许可证

**CC BY-NC-SA 4.0** —— 见仓库 [LICENSE](https://github.com/edoCsItahW/Stationeers/blob/main/LICENSE)。
商用需获得作者许可。

## 贡献

欢迎提交 Issue 与 Pull Request —— 流程见
[CONTRIBUTING.zh.md](https://github.com/edoCsItahW/Stationeers/blob/main/CONTRIBUTING.zh.md)（或
[English](https://github.com/edoCsItahW/Stationeers/blob/main/CONTRIBUTING.md)），构建与测试原生模块见
[DEVELOPER_GUIDE.zh.md](https://github.com/edoCsItahW/Stationeers/blob/main/DEVELOPER_GUIDE.zh.md)。

## 联系方式

- **作者**：edocsitahw
- **邮箱**：[edocsitahw@qq.com](mailto:edocsitahw@qq.com)
- **仓库**：[github.com/edoCsItahW/Stationeers](https://github.com/edoCsItahW/Stationeers)

## 相关链接

- [IC10 编译器核心](https://github.com/edoCsItahW/Stationeers) —— 本模块封装的 C++23 实现
- [IC10 VS Code 插件](https://marketplace.visualstudio.com/items?itemName=edocsitahw.ic10)
- [Stationeers 官方页面](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API](https://nodejs.org/api/n-api.html) · [node-addon-api](https://github.com/nodejs/node-addon-api)
