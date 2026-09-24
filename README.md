# IC10

[中文](README.zh.md)

![Version](https://img.shields.io/badge/version-3.0.0-blue)
![C%2B%2B](https://img.shields.io/badge/C%2B%2B-23-blue)
![License](https://img.shields.io/badge/license-CC%20BY--NC--SA%204.0-lightgrey)
![VS Code](https://img.shields.io/badge/VS%20Code-edocsitahw.ic10-007ACC)

<!--
  ===== 图片位 =====
  在此插入横幅图与功能截图（HTML 注释不会渲染，替换成下面这种写法即可）：

  <p align="center">
    <img src="code/IC10/plugins/vscode/ic10-language-support/static/你的图.png" alt="IC10" width="720">
  </p>
  ==================
-->

**IC10** is the assembly-level language you program your base with in
[Stationeers](https://store.steampowered.com/app/544550/Stationeers/) — registers, devices, no types, no
autocomplete, and in-game error reporting that mostly amounts to a red X.

This project is the toolchain that makes writing it pleasant: a C++23 compiler that actually understands the
language, a runtime you can step through, and an editor extension that puts diagnostics, completion, hover and
formatting right next to your code.

```ic10
alias sensor d0
define TARGET 300

main:
    l r0 sensor Temperature
    blt r0 TARGET heating_off
    s sensor On 1
    j main

heating_off:
    s sensor On 0
    j main
```

---

## What you get

- **An editor that gets IC10** — install the [IC10 extension](https://marketplace.visualstudio.com/items?itemName=edocsitahw.ic10),
  open a `.ic` file, done. Highlighting, live diagnostics, hover, completion, signature help, formatting and a
  debugger, no toolchain required.
- **A compiler that tells you what's wrong** — lexer → parser → semantic analysis → linker, with real source
  locations, severities and bilingual messages, plus an incremental mode that keeps up with you as you type.
- **Bindings for your own tools** — drive the same compiler from **Node.js**, **Python** or **Java**, or use the
  `ic10c` command line to dump tokens, AST or the symbol table as JSON.
- **A runtime you can debug against** — `ic10r-node` executes IC10 with simulated devices, which is what lets the
  extension single-step your program instead of merely checking it.

---

## Features

**Compiler**

- Handles all IC10 instruction arities (nullary → senary), registers `r0`–`r15`, devices `d0`–`d5`, multi-base
  numbers (`$hex`, `%bin`, decimal, float), strings, `#`/`//` comments, `alias`/`define` directives and
  doc-comment annotations (`#:`, `#>`).
- Semantic analysis with symbol resolution, type inference and type checking over an async symbol table, so
  forward references just work.
- First-class linker: multi-unit merging, cross-unit forward references, symbol visibility, cycle detection.
- Incremental compilation with line-level lexing caches and statement-level parse caches — only what changed
  gets reprocessed.
- Diagnostics in English and Simplified Chinese, and every message goes through the localization layer.

**Editor extension**

| | |
|---|---|
| Syntax & semantic highlighting | Symbols are coloured from the compiler's own symbol table, so an alias looks different from a raw register |
| Real-time diagnostics | Incremental re-analysis on every keystroke, categorized in the Problems panel |
| Hover & completion | Types, values and descriptions on hover; instruction and operand completion filtered by device context |
| Signature help & formatting | Active-parameter highlighting; formatting configurable via `.ic.yaml` / `.ic.yml` / `.ic.json` |
| Debugger | Launch `program` with `stopOnEntry`, set breakpoints, inspect the simulated machine |
| Type annotations | `#:` hints and `#>` doc comments for declaring device and enum types |

**Runtime, bindings and metadata**

- `ic10r-node` — the execution engine, N-API bindings included.
- `ic10c-node` (Node.js), `ic10c_python` (Python) and `io.github.stationeers:ic10` (Java) all expose the same
  compiler phases.
- [`@ic10/metadata`](https://www.npmjs.com/package/@ic10/metadata) — instructions, enums, devices and
  Stationpedia text as a typed package, so tooling can stay in sync with the game.

---

## Quick start

### In VS Code (recommended)

```bash
code --install-extension edocsitahw.ic10
```

Then open any `.ic` or `.ic10` file. The extension bundles the prebuilt compiler and runtime as native addons —
there is no C++ toolchain, CMake or compiler to install.

### From a terminal

Grab `ic10c` from the [releases page](https://github.com/edoCsItahW/Stationeers/releases), then:

```bash
ic10c main.ic                # compile; prints the symbol table as JSON
ic10c --emit-ast main.ic     # inspect the syntax tree
ic10c --link a.ic b.ic       # link multiple units
```

Exit codes are useful in scripts: `0` success, `1` bad usage or unreadable file, `2` the compiler reported
warnings or errors.

### From your own code

```typescript
import { Lexer, Parser, Analyser, IC10Local } from 'ic10c-node';

IC10Local.setLanguage('zh-hans');            // optional, defaults to English

const tokens = Lexer.tokenize(source);
const program = new Parser(tokens, false).parse();

const analyser = new Analyser();
await analyser.visit(program);               // async: forward references are resolved by coroutines

console.log(analyser.symbolTable.toJSON(), analyser.diagnostics);
```

```python
from ic10c_python import Lexer, Parser, Analyser

tokens = Lexer.tokenize(source)
program = Parser(tokens, False).parse()

analyser = Analyser()
analyser.visit(program)
print(analyser.symbolTable.toJSON())
```

Linking and incremental compilation are exposed the same way — see `Linker` and `IncCompiler` in the
[Node.js package](code/IC10/backend/compiler/publish/node) or the
[Python stubs](code/IC10/backend/compiler/publish/python/ic10c_python.pyi).

---

## Documentation

| Where | What |
|---|---|
| [IC10 extension README](code/IC10/plugins/vscode/ic10-language-support/README.md) | every setting and feature of the editor extension |
| [DEVELOPER_GUIDE.md](DEVELOPER_GUIDE.md) | architecture, build, test, release — for people working on this repo |
| [CONTRIBUTING.md](CONTRIBUTING.md) | issues, pull requests, commit conventions |
| [CHANGELOG.md](CHANGELOG.md) | what changed, release by release |
| [docs/annotationExample.md](docs/annotationExample.md) | the `#:` / `#>` annotation syntax, by example |
| [docs/grammar/](docs/grammar/) | the IC10 grammar, split by lexer/parser and instruction arity |

---

## Build from source

You need CMake ≥ 3.28.1 and a C++23 compiler — plus `jq` if you use the bash scripts. They do the rest:

```bash
git clone https://github.com/edoCsItahW/Stationeers.git && cd Stationeers
code/scripts/bashShell/buildIC10CompilerCore.sh      # Windows: pwsh code/scripts/powerShell/BuildIC10CompilerCore.ps1
```

That configures, builds and runs the C++ test suite. For the Node.js, Python, Java, runtime and CLI targets, the
toolchain versions, the CMake options and how to cut a release, see the
**[Developer Guide](DEVELOPER_GUIDE.md)**.

---

## Internationalization

Compiler messages ship in English (`en-us`, default) and Simplified Chinese (`zh-hans`):

```cpp
IC10Local::setLanguage("zh-hans");     // C++
```
```typescript
IC10Local.setLanguage('zh-hans');      // Node.js
```

---

## Versioning

This project follows [Semantic Versioning](https://semver.org/). `VERSION` currently reads **3.0.0** for the
IC10 core and **1.0.2** for the VS Code extension; the most recent tag is `v2.6.6`.

- **v3.0.0** — the IC10 v3 rework: rewritten lexer and parser with unified per-arity AST nodes, adjusted
  `annotation` and `link` grammar rules, a new execution runtime with debugging support, Java bindings, and
  metadata split out into `@ic10/metadata`.
- **v2.x** — linker, incremental compiler, type inference, annotation syntax, Python bindings, and the syntax
  changes that came with a game update.
- **v1.0.x** — first release: lexer, parser, semantic analysis and Node.js bindings.

Full history in [CHANGELOG.md](CHANGELOG.md).

---

## License

**CC BY-NC-SA 4.0** — see [LICENSE](LICENSE). Please don't use this commercially without asking first.

---

## Author & thanks

**edocsitahw** — [edocsitahw@qq.com](mailto:edocsitahw@qq.com)

Thanks to [Stationeers](https://stationeers.com/) for the language worth writing a compiler for, and to
[node-addon-api](https://github.com/nodejs/node-addon-api), [pybind11](https://github.com/pybind/pybind11) and
[fbjni](https://github.com/facebookincubator/fbjni), which make the bindings possible.
