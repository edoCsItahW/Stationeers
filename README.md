# IC10 Compiler

[中文](README.zh.md)

![Version](https://img.shields.io/badge/version-2.0.0-blue)
![C++](https://img.shields.io/badge/C%2B%2B-23-blue)
![License](https://img.shields.io/badge/license-CC%20BY--NC--SA%204.0-lightgrey)

A modern C++23 compiler suite for **IC10** and **StationScript**, the scripting languages used in the game [Stationeers](https://store.steampowered.com/app/544550/Stationeers/).
This project provides lexical analysis, syntax analysis, semantic analysis, linking, incremental compilation, an extensible infrastructure with async coroutine support, and Node.js / Python bindings.

---

## Features

- **Lexer** – state-machine based tokenizer for IC10 source code (supports registers `r0`–`r15`, devices `d0`–`d5`, multi-base numbers `$hex`/`%bin`/decimal/float, strings, `#` and `//` comments, all instruction keywords).
- **Parser** – recursive-descent parser building an Abstract Syntax Tree (AST) for all IC10 instructions (nullary to senary), with preprocessor directives (`alias`, `define`) and doc-comment annotation parsing.
- **Semantic Analyser** – performs symbol resolution, type inference, and type checking using a `Promise`/`Future` based asynchronous symbol table with coroutine-driven forward reference resolution.
- **Linker** – two-phase symbol merging across multiple compilation units, cross-unit forward reference resolution, symbol visibility control (public/private), and cycle detection.
- **Incremental Compiler** – line-level lexer caching and statement-level parser caching for fast re-compilation in editor scenarios; only changed portions are reprocessed.
- **Error Reporting** – rich diagnostics with source location (line:column), severity levels, and internationalization support (English / Simplified Chinese).
- **Async Coroutine Infrastructure** – custom `Task<T>`, `Promise<T>`, `Future<T>` and coroutine state management for non-blocking symbol resolution.
- **CLI Compiler** – `ic10c` command-line tool supporting `--emit-tokens`, `--emit-ast`, `--emit-symbols`, `--link`, `--locale`, `--pretty`, and `-o` output redirection.
- **Node.js Bindings** – native Node.js addon via `node-addon-api`, exposing 11 adapters (Lexer, Token, Parser, Program/AST, Analyser, SymbolTable, Linker, IC10Local, IncLexer, IncParser, IncCompiler) to JavaScript/TypeScript.
- **Python Bindings** – native Python extension via `pybind11`, exposing the same compiler capabilities to Python.
- **Cross-Platform** – builds on Linux (GCC 13+ / Clang 16+) and Windows (MSVC 2022).
- **Testing** – GoogleTest unit tests for C++ core (lexer, parser, semantic, linker, incremental, integration, system), Jest tests for Node.js bindings, pytest tests for Python bindings.
- **CI/CD** – GitHub Actions workflows for build, test, static analysis (cppcheck, clang-tidy, clang-format), and automatic artifact publishing on tag push.

---

## Project Structure

```
Stationeers/
├── code/backend/compiler/              # Main compiler source code
│   ├── CMakeLists.txt                  # Top-level CMake
│   ├── common/                         # Shared utilities library
│   │   ├── include/common/async/       # Coroutine tasks, promises, futures
│   │   ├── include/common/exception/   # Error & diagnostic hierarchy
│   │   ├── include/common/locals/      # Internationalization framework
│   │   └── include/common/utils/       # FStr, BiMap, enum_to_str, type lists, etc.
│   ├── IC10/                           # IC10 compiler core
│   │   ├── include/ic10/lexer/         # Token & Lexer
│   │   ├── include/ic10/parser/ast/    # AST node definitions (nullary to senary)
│   │   ├── include/ic10/semantic/      # SymbolTable, Analyser, type system
│   │   ├── include/ic10/link/          # Linker (multi-unit symbol merging)
│   │   ├── include/ic10/incremental/   # IncLexer, IncParser, IncCompiler
│   │   ├── include/ic10/locals/        # Language resources (en_us, zh_hans)
│   │   ├── src/                        # Implementation files
│   │   ├── main.cpp                    # CLI entry point (ic10c)
│   │   └── main.hpp                    # Doxygen mainpage & specialized docs
│   ├── StationScript/                  # StationScript compiler (WIP)
│   ├── exports/                        # Language bindings
│   │   ├── common/node/                # Common Node.js adapter utilities
│   │   ├── common/python/             # Common Python adapter utilities
│   │   ├── IC10/node/                  # IC10 Node.js bindings (11 adapters)
│   │   └── IC10/python/                # IC10 Python bindings
│   ├── tests/                          # Unit tests
│   │   ├── ic10/                       # C++ tests (lexer, parser, semantic, linker, incremental)
│   │   │   ├── node/                   # Node.js binding tests (Jest + TypeScript)
│   │   │   └── python/                 # Python binding tests (pytest)
│   │   └── node/ic10/                  # Node.js package (ic10-node-api)
│   ├── scripts/                        # Build scripts (PowerShell)
│   ├── cmake/                          # CMake modules (node.cmake, pybind11.cmake)
│   ├── .clang-format                   # Code style configuration (4-space indent)
│   └── .clang-tidy                     # Static analysis configuration
├── code/backend/mateDatas/             # Game metadata (instructions, enums, types)
├── code/plugins/vscode/                # VS Code language support extension
├── docs/                               # Documentation & Doxygen resources
├── .github/workflows/                  # CI/CD workflows
├── CHANGELOG.md                        # Changelog (English)
├── CHANGELOG.zh.md                     # Changelog (Chinese)
├── CONTRIBUTING.md                     # Contributing guidelines (English)
├── CONTRIBUTING.zh.md                  # Contributing guidelines (Chinese)
├── VERSION                             # Current version
└── LICENSE                             # License file
```

---

## Requirements

### C++ Core

- **CMake** 3.28.1 or higher
- **C++23** compiler:
  - Linux: GCC 13+ or Clang 16+
  - Windows: MSVC 2022 (with Visual Studio 2022 build tools)
- **Ninja** (recommended) or Make
- **Git** (for fetching GoogleTest)

### Node.js Bindings

- **Node.js** 24.x or 26.x (tested and verified)
- **pnpm** 9.x (package manager) — npm is also supported
- **node-gyp** — required for building native addons
- **node-addon-api** ^8.8.0

> **Note:** After installing Node.js, you must install `node-gyp` and download the Node.js header files before building:
> ```bash
> # Install node-gyp globally or as a dev dependency
> pnpm add -g node-gyp   # or: npm install -g node-gyp
>
> # Download Node.js header files for the current Node version
> pnpm exec node-gyp install   # or: npm exec node-gyp install
> ```

### Python Bindings

- **Python** 3.13 (tested and verified)
- **pybind11** 2.12+

> **Note:** Python must be discoverable in your system PATH so that CMake's `FindPython3` can locate it.

---

## Build Instructions

### 1. Clone the repository

```bash
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
```

### 2. Set up Node.js build environment

If you plan to build the Node.js bindings:

```bash
# Install node-gyp (required for native addon builds)
pnpm add -g node-gyp   # or: npm install -g node-gyp

# Download Node.js header files for the current Node version
pnpm exec node-gyp install   # or: npm exec node-gyp install
```

### 3. Install Node.js dependencies

```bash
cd code/backend/compiler
pnpm i --ignore-workspace
```

### 4. Configure with CMake

```bash
cmake -B build -S code/backend/compiler -G Ninja -DCMAKE_BUILD_TYPE=Release
```

On Windows (MSVC) you may need to specify the generator:

```powershell
cmake -B build -S code/backend/compiler -G "Visual Studio 17 2022" -A x64
```

### 5. Build

```bash
cmake --build build --parallel
```

The executable `ic10c` will be placed in `build/bin/`.
The Node.js native module `ic10-node-api.node` will be placed in `build/exports/IC10/node/`.
The Python native module will be placed in `build/exports/IC10/python/`.

### 6. Run tests

#### C++ tests

```bash
cd build
ctest --output-on-failure
```

#### Node.js tests

```bash
cd code/backend/compiler
pnpm test
```

#### Python tests

```bash
cd code/backend/compiler/tests/ic10/python
pytest -v
```

---

## Usage

### CLI (`ic10c`)

The CLI compiler supports independent output of each compilation phase, as well as the complete pipeline and multi-unit linking:

```bash
ic10c input.ic                    # Compile and output symbol table JSON
ic10c --emit-tokens input.ic      # Output lexical token stream
ic10c --emit-ast input.ic         # Output syntax tree AST
ic10c --emit-symbols input.ic     # Output symbol table (default)
ic10c -o out.json input.ic        # Output to file
ic10c --pretty input.ic           # Pretty-print JSON output
ic10c --locale zh-hans input.ic   # Use Simplified Chinese for messages
ic10c --link a.ic b.ic c.ic       # Link multiple units and output merged symbol table
ic10c -v                          # Show version
ic10c -h                          # Show help
```

Example IC10 program:

```
alias counter r0
define PI 3.14159

start:
    move r0 10
    add r1 r0 PI
    hcf
```

### Node.js

The `ic10-node-api` package provides JavaScript/TypeScript bindings:

```typescript
import { Lexer, Parser, Analyser, IC10Local } from 'ic10-node-api';

// Set language (optional, default English)
IC10Local.setLanguage('zh-hans');

const source = `
alias counter r0
start:
    move r0 10
    add r1 r0 5
    hcf
`;

// 1. Lexical analysis
const tokens = Lexer.tokenize(source);

// 2. Syntax analysis
const parser = new Parser(tokens, false);  // debug = false
const program = parser.parse();

// 3. Semantic analysis (async — uses coroutines for forward references)
const analyser = new Analyser();
await analyser.visit(program);

// 4. Get results
console.log(analyser.symbolTable.toJSON());
console.log(analyser.diagnostics);
```

**Linker usage:**

```typescript
import { Linker } from 'ic10-node-api';

const linker = new Linker();
linker.addUnit(source1, 'file1.ic');
linker.addUnit(source2, 'file2.ic');
const symbolTable = linker.link();
console.log(linker.diagnostics);
```

**Incremental compiler usage:**

```typescript
import { IncCompiler } from 'ic10-node-api';

const compiler = new IncCompiler();
const result1 = compiler.compileFull(source);
const result2 = compiler.compileInc(modifiedSource);
console.log(`Incremental: ${result2.incremental}, re-lexed lines: ${result2.relexedLines}`);
```

### Python

```python
import ic10_python as ic10

# Lexical analysis
tokens = ic10.Lexer.tokenize(source)

# Syntax analysis
parser = ic10.Parser(tokens, False)
program = parser.parse()

# Semantic analysis
analyser = ic10.Analyser()
analyser.visit(program)
print(analyser.symbolTable.toJSON())
```

---

## Internationalization (i18n)

The compiler supports multiple languages. Available languages:
- English (`en-us`) — default
- Simplified Chinese (`zh-hans`)

**C++:**
```cpp
IC10Local::setLanguage("zh-hans");
```

**Node.js:**
```typescript
IC10Local.setLanguage('zh-hans');
```

---

## Versioning

This project follows [Semantic Versioning 2.0.0](https://semver.org/). The current version is **2.0.0** (see [VERSION](VERSION)).

- **v2.0.0** — Breaking syntax changes accompanying game updates, plus linker, incremental compiler, Python bindings, type inference, and annotation syntax.
- **v1.0.x** — Initial release with lexer, parser, semantic analyser, and Node.js bindings.

See [CHANGELOG.md](CHANGELOG.md) for full release history.

---

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to report issues, submit pull requests, and our coding standards.

---

## License

This project is licensed under the **CC BY-NC-SA 4.0** license.
See the [LICENSE](LICENSE) file for details.
You may not use this software for commercial purposes without the author's permission.

---

## Author

Xiao Songtao (edocsitahw) – [edocsitahw@qq.com](mailto:edocsitahw@qq.com)

---

## Acknowledgements

- Inspired by the IC10 scripting language in [Stationeers](https://stationeers.com/).
- Built with C++23 coroutines and modern CMake.
- Node.js bindings powered by [node-addon-api](https://github.com/nodejs/node-addon-api).
- Python bindings powered by [pybind11](https://github.com/pybind/pybind11).
