# ic10c-node

[![npm version](https://badge.fury.io/js/ic10c-node.svg)](https://www.npmjs.com/package/ic10c-node)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![License: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[中文](./README.zh.md)

## Overview

`ic10c-node` is the Node.js native addon for the **IC10 compiler**: it exposes the whole C++23 core —
lexer, parser, semantic analyser, linker and incremental compiler — to JavaScript and TypeScript.

IC10 is the assembly-style language used in [Stationeers](https://store.steampowered.com/app/544550/Stationeers/)
to script in-game computers and devices. This package lets you compile, analyse, link and inspect IC10 code
from Node.js: no C++ toolchain required, since the native module ships prebuilt.

Current version: **3.0.0** (IC10 v3 — rewritten lexer/parser with the unified per-arity AST). See
[CHANGELOG.md](./CHANGELOG.md) / [CHANGELOG.zh.md](./CHANGELOG.zh.md).

## Features

- **Lexer** — tokenizes IC10 source (registers, devices, `$hex` / `%bin` / decimal / float numbers, strings,
  annotation prefixes, comments and instruction keywords). The stream is raw: `NEWLINE`, `COMMENT` and `END`
  tokens are included.
- **Parser** — builds the AST for every instruction arity (nullary → senary), labels, `alias` / `define`
  directives and `#>` annotation blocks.
- **Semantic analyser** — symbol resolution, type inference and operand checking over a `Promise`/`Future`
  based symbol table, so forward references work naturally. Produces a symbol table **and** a type table.
- **Linker** — merges several compilation units, resolves cross-unit references and exposes the merged symbol
  and type tables plus per-unit diagnostics.
- **Incremental compiler** — line-level lexing cache and statement-level parse cache (`IncLexer`, `IncParser`,
  `IncCompiler`); only the changed lines/statements are reprocessed.
- **Diagnostics** — every stage reports `{level, id, start, end, message}`; messages are available in English
  (`en-us`) and Simplified Chinese (`zh-hans`).
- **Complete TypeScript types** — shipped in the package, no `@types/*` needed.
- **Cross-platform** — buildable on Linux (GCC/Clang) and Windows (MSVC).

## Installation

### From npm

```bash
npm install ic10c-node
```

The published package already contains the compiled `src/ic10c-node.node` addon, so installing it requires
**no** C++ compiler, CMake or node-gyp. Node.js >= 16 is required (Node.js 26 is what the project builds and
tests against).

### From source

Building the addon is part of the repository build; see
[DEVELOPER_GUIDE.md](https://github.com/edoCsItahW/Stationeers/blob/main/DEVELOPER_GUIDE.md) for the full
toolchain table. In short you need CMake >= 3.28.1 and a C++23 compiler (GCC 13+ / Clang 16+ / MSVC 2022).

```bash
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers

# Linux / macOS — configures CMake, builds the addon, runs the Node.js tests
code/scripts/bashShell/buildIC10CompilerNode.sh
```

```powershell
# Windows (PowerShell)
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
pwsh code/scripts/powerShell/BuildIC10CompilerNode.ps1
```

Both scripts build the CMake target `ic10_compiler_node` and copy the artifact to
`code/IC10/backend/compiler/publish/node/src/ic10c-node.node`. If you prefer to drive CMake yourself:

```bash
cd code
cmake -B build -S . -DBUILD_IC10_COMPILER_EXPORTS_NODE=ON
cmake --build build --target ic10_compiler_node --config Release
```

## Quick start

```typescript
import * as ic10 from 'ic10c-node';

// Messages in Simplified Chinese (optional; default is en-us)
ic10.IC10CompilerLocal.setLanguage('zh-hans');

const source = `
    alias disp d0
    main:
        move r0 42
        yield
        jal main
`;

// 1. Lex — the raw token stream, including NEWLINE/COMMENT tokens
const tokens = ic10.Lexer.tokenize(source);

// 2. Parse — tokens to AST
const parser = new ic10.Parser(tokens);
const program = parser.parse();

// 3. Analyse — resolve symbols, check operands and types
const analyser = new ic10.Analyser();
await analyser.visit(program);          // async: forward references are resolved by coroutines

// 4. Results
console.log(analyser.symbolTable.toJSON());   // {"symbols":{...},"builtinSymbols":{"d0":...}}
console.log(analyser.typeTable.toJSON());     // device/enum types declared by #> blocks
for (const d of analyser.diagnostics)         // errors, warnings, info
    console.log(`${d.level} ${d.id} at line ${d.start.line}: ${d.message}`);
```

Reading the AST: nodes serialize through `toJSON()` and identify themselves with `nodeName`.

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
//   type1: 0,                       // OperandType enum value
//   operand2: { nodeName: 'Integer', ..., value: '42' },
//   type2: 2
// }
```

### Static entry points

`Lexer.tokenize` and `Parser.parsing` are conveniences; `Analyser.analyse` runs a whole analysis but **throws
its results away**, so use an instance when you need the symbol table or diagnostics.

```typescript
import { Lexer, Parser, Analyser } from 'ic10c-node';

const tokens  = Lexer.tokenize('add r0 r1 r2');
const program = Parser.parsing(tokens);         // same as new Parser(tokens).parse()

await Analyser.analyse(program);                // convenience: no results returned
```

## API reference

### Exports at a glance

| Kind | Names |
|:-----|:------|
| Classes | `Lexer`, `Parser`, `Program`, `Analyser`, `SymbolTable`, `TypeTable`, `Linker`, `IncLexer`, `IncParser`, `IncCompiler`, `Token`, `Pos`, `IC10CompilerLocal` |
| Enums | `TokenType` (27), `TokenCategory` (7), `InstructionKeyword` (147), `BasicType` (7), `TypeCategory` (6), `OperandType` (17) |
| Values / functions | `TypeOfNode` |
| Error constructors | `SyntaxError`, `RuntimeError`, `RedefinitionError`, `TypeError`, `UndefinedSymbolError` |

| Class | Purpose | Members |
|:------|:--------|:--------|
| `Lexer` | Source → tokens | `static tokenize(source)`, `scan()`, `diagnostics` |
| `Parser` | Tokens → AST | `parse()`, `static parsing(tokens, debug?)`, `diagnostics` |
| `Program` | AST root | `statements`, `end`, `toString()`, `toJSON()` |
| `Analyser` | Symbols, types, operand checks | `visit(program)` (async), `symbolTable`, `typeTable`, `diagnostics`, `static analyse(program)` |
| `SymbolTable` | Resolved symbols | `toJSON()` |
| `TypeTable` | Device/enum types from `#>` blocks | `toJSON()` |
| `Linker` | Multi-unit compilation | `addUnit(source \| program, path?)`, `link()`, `diagnostics`, `units`, `typeTable` |
| `IncLexer` / `IncParser` / `IncCompiler` | Incremental compilation | `tokenizeFull` / `tokenizeInc`, `parseFull` / `parseInc`, `compileFull` / `compileInc`, `hasCache()`, `clear()`, plus `diagnostics` on the result structures |
| `Token` | One lexical token | `type`, `pos`, `lexeme`, `category`, `keyword`, `toString()`, `toJSON()` |
| `Pos` | Position | `line`, `column`, `offset`, `newline()`, `next()`, `move(index)` |
| `IC10CompilerLocal` | Message language | `static setLanguage('en-us' \| 'zh-hans')` |

### Lexer

```typescript
import { Lexer, TokenCategory } from 'ic10c-node';

const tokens = Lexer.tokenize('alias ic d0');

// Instance form keeps the diagnostics for you
const lexer = new Lexer('"unterminated');
lexer.scan();
console.log(lexer.diagnostics[0].id);        // 'IEL2_1'

console.log(tokens.map(t => t.category).map(c => Object.keys(TokenCategory).find(k => TokenCategory[k] === c)));
// [ 'LITERAL', 'LITERAL', 'LITERAL', 'END' ] — a NEWLINE token appears only when the source has one
console.log(tokens[0].toString());           // Token<1,1>[KEYWORD_ALIAS]('alias')
```

`TokenCategory` is `LITERAL: 0`, `SYMBOL: 1`, `COMMENT: 2`, `ANNOTATION: 3`, `WHITESPACE: 4`, `END: 5`,
`INVALID: 6`. Note that `WHITESPACE` covers both spaces and `NEWLINE` tokens, and comments are part of the
stream (the parser skips them). The `debug` constructor argument is reserved and currently changes nothing.

### Parser

```typescript
import { Lexer, Parser } from 'ic10c-node';

const parser = new Parser(Lexer.tokenize('main:\n    move r0 1\n'));
const program = parser.parse();

console.log(program.statements.length);      // 2
console.log(program.statements[0].nodeName); // 'LabelDef' — statements are plain objects
console.log(parser.diagnostics);             // [] on success
```

> Label names must not be instruction keywords: `sub:` is **not** a label, because `sub` lexes as a keyword, so
> the parser reads it as an instruction and reports operand errors. `main:` and `helper:` work as expected.
```

### Analyser

```typescript
import { Lexer, Parser, Analyser } from 'ic10c-node';

const program  = Parser.parsing(Lexer.tokenize('main:\n    move r0 1\n'));
const analyser = new Analyser();

await analyser.visit(program);               // must be awaited: forward references resume later

console.log(analyser.symbolTable.toJSON());
// { symbols: { main: { name: 'main', type: 1, category: 0, value: '1', builtin: false } },
//   builtinSymbols: { d0: {...}, d1: {...}, ... } }
console.log(analyser.typeTable.toJSON());    // {} unless the source declares #> types
console.log(analyser.diagnostics);           // semantic errors, e.g. IEA3_1 for an undefined identifier
```

### Linker

```typescript
import { Linker } from 'ic10c-node';

const linker = new Linker();
linker.addUnit('main:\n    move r0 1\n', 'a.ic');       // source + path
linker.addUnit('helper:\n    move r1 2\n', 'b.ic');

const symbols = linker.link();               // merged SymbolTable
console.log(Object.keys(JSON.parse(symbols.toJSON()).symbols));  // [ 'main', 'helper' ]
console.log(linker.units);                   // [ { path: 'a.ic', diagnostics: [] }, ... ]
console.log(linker.typeTable.toJSON());      // merged type table
```

Units are analysed concurrently, so a unit may reference a symbol defined in a later one. `linker.diagnostics`
collects the **semantic** diagnostics of every unit (merged from per-unit reporters); units passed as source
strings are parsed internally, so parse the source yourself first if you also want syntax diagnostics.
```

### Incremental compilation

```typescript
import { IncCompiler } from 'ic10c-node';

const compiler = new IncCompiler();
const first  = compiler.compileFull(source);
const second = compiler.compileInc(source.replace('42', '43'));

console.log(first.incremental, second.incremental);   // false, true
console.log(second.relexedLines, second.reparsedStmts); // 1 line re-lexed, 3 statements reparsed
compiler.clear();                                     // drop the cache
```

`IncLexer` (`tokenizeFull` / `tokenizeInc`) and `IncParser` (`parseFull` / `parseInc`) expose the two halves
separately.

All three result structures carry `diagnostics`, aggregated from their caches and covering the **whole
source** rather than just the changed range: unchanged lines/statements reuse the diagnostics captured when
they were cached, changed ones use this call's diagnostics, and diagnostics of shifted suffix lines are moved
along with their positions. An editor can therefore publish lexical and syntax diagnostics straight from the
incremental path instead of falling back to a full parse just to obtain them.

`diagnostics` **excludes semantic diagnostics**: semantic analysis belongs to `Linker` / `Analyser` and has to
be run separately (see below).

```typescript
const lexer  = new IncLexer();
const parser = new IncParser();

lexer.tokenizeFull(source);
parser.parseFull(lexer.tokens);

const lex    = lexer.tokenizeInc(edited);
const parsed = parser.parseInc(lex.tokens, lex.changedStartLine);

console.log(lex.diagnostics, parsed.diagnostics);   // whole-source lexical / syntax diagnostics
```

### Diagnostics

```typescript
import { Lexer, Parser, Analyser } from 'ic10c-node';

const parser = new Parser(Lexer.tokenize('move r0'));   // missing operand
parser.parse();

for (const d of parser.diagnostics) {
    console.log(`${d.level} ${d.id}: ${d.message}`);
    console.log(`  line ${d.start.line}, column ${d.start.column}`);
    // error IEP34_1: Not one of the types in 'Integer|Float|...|Error' - syntax analysis.
}
```

Each diagnostic is `{ level: 'error' | 'warning' | 'info', id, start, end, message }`, where `id` is a stable
message id (`IEL*` lexical, `IEP*` parse, `IEA*` analysis) and `start` / `end` are `Pos` objects. `start` and
`end` are accessor-backed, so read `d.start.line` directly — `JSON.stringify(d)` prints them as `{}`.

### Message language

```typescript
import { IC10CompilerLocal } from 'ic10c-node';

IC10CompilerLocal.setLanguage('zh-hans');   // 简体中文
IC10CompilerLocal.setLanguage('en-us');     // English (default)
```

### Node type table

`TypeOfNode` maps an AST node type name to the type that node yields, using `BasicType` / `TypeCategory` enum
values. It is a plain lookup object with 16 entries (`Integer`, `Float`, `String`, `StrMacro`, `HashMacro`, the
four registers, the four devices, `Enum`, `LabelDef`, `Error`, `BinaryNumber`, `HexNumber`).

```typescript
import { TypeOfNode, BasicType, TypeCategory } from 'ic10c-node';

TypeOfNode.Integer;                 // { kind: BasicType.INTEGER, category: TypeCategory.NUMBER }
TypeOfNode.GeneralPurposeRegister;  // { kind: BasicType.REGISTER, category: TypeCategory.BASIC }
```

## TypeScript

The package ships its own declarations under `types/` (`index.d.ts` re-exports `lexer`, `parser`, `semantic`,
`linker`, `incremental`, `locale` and `common`), so no `@types` package is required.

```typescript
import * as ic10 from 'ic10c-node';

const tokens: ic10.Token[] = ic10.Lexer.tokenize('move r0 r1');
const program: ic10.Program = ic10.Parser.parsing(tokens);
```

`Token` and `Pos` instances come **from** the API — do not construct them with `new`; build your programs
through `Lexer` / `Parser` instead.

## Package layout

```
publish/node/
├── src/
│   ├── ic10c-node.node        # prebuilt native addon (built by CMake)
│   └── stdLib.ic              # standard library shipped with the compiler
├── types/                     # TypeScript declarations (index/lexer/parser/semantic/linker/incremental/locale/common)
├── CHANGELOG.md
├── CHANGELOG.zh.md
├── README.md
├── README.zh.md
├── tsconfig.json
└── package.json
```

## License

**CC BY-NC-SA 4.0** — see the repository [LICENSE](https://github.com/edoCsItahW/Stationeers/blob/main/LICENSE).
Commercial use requires the author's permission.

## Contributing

Issues and pull requests are welcome — see
[CONTRIBUTING.md](https://github.com/edoCsItahW/Stationeers/blob/main/CONTRIBUTING.md) (or the
[中文版](https://github.com/edoCsItahW/Stationeers/blob/main/CONTRIBUTING.zh.md)) for the workflow, and
[DEVELOPER_GUIDE.md](https://github.com/edoCsItahW/Stationeers/blob/main/DEVELOPER_GUIDE.md) for building and
testing the native addon.

## Contact

- **Author**: edocsitahw
- **Email**: [edocsitahw@qq.com](mailto:edocsitahw@qq.com)
- **Repository**: [github.com/edoCsItahW/Stationeers](https://github.com/edoCsItahW/Stationeers)

## Related links

- [IC10 compiler core](https://github.com/edoCsItahW/Stationeers) — the C++23 implementation this addon wraps
- [IC10 extension for VS Code](https://marketplace.visualstudio.com/items?itemName=edocsitahw.ic10)
- [Stationeers on Steam](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API](https://nodejs.org/api/n-api.html) · [node-addon-api](https://github.com/nodejs/node-addon-api)
