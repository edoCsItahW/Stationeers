# IC10 Compiler

[中文](README.zh.md)

A modern C++23 compiler suite for **IC10** and **StationScript**, the scripting languages used in the game [Stationeers](https://store.steampowered.com/app/544550/Stationeers/).  
This project provides lexical analysis, syntax analysis, semantic analysis, an extensible infrastructure with async coroutine support, and Node.js bindings.

---

## Features

- **Lexer** – tokenizes IC10 source code (supports registers, devices, numbers, strings, comments, keywords).
- **Parser** – builds an Abstract Syntax Tree (AST) for all IC10 instructions (nullary to senary).
- **Semantic Analyser** – performs symbol resolution and type checking using a `Promise`/`Future` based asynchronous symbol table.
- **Error Reporting** – rich error messages with source location, internationalization support (English / Simplified Chinese).
- **Async Coroutine Infrastructure** – custom `Task<T>`, `Promise<T>`, `Future<T>` and coroutine state management for non‑blocking symbol resolution.
- **Node.js Bindings** – native Node.js addon via `node-addon-api`, exposing lexer, parser, AST, and semantic analyser to JavaScript/TypeScript.
- **Cross‑Platform** – builds on Linux (GCC/Clang) and Windows (MSVC).
- **Testing** – GoogleTest unit tests for C++ core, Jest tests for Node.js bindings.
- **CI/CD** – GitHub Actions workflows for build, test, and static analysis (cppcheck, clang‑tidy, clang‑format).

---

## Project Structure

```
IC10_Compiler/
├── code/backend/compiler/           # Main compiler source code
│   ├── CMakeLists.txt               # Top-level CMake
│   ├── common/                      # Shared utilities library
│   │   ├── include/common/async/    # Coroutine tasks, promises, futures
│   │   ├── include/common/exception/# Error hierarchy
│   │   ├── include/common/locals/   # Internationalization framework
│   │   └── include/common/utils/    # FStr, BiMap, enum_to_str, type lists, etc.
│   ├── IC10/                        # IC10 compiler core
│   │   ├── include/ic10/lexer/      # Token & Lexer
│   │   ├── include/ic10/parser/ast/ # AST node definitions (nullary to senary)
│   │   ├── include/ic10/semantic/   # SymbolTable, Analyser
│   │   ├── include/ic10/locals/     # Language resources (en_us, zh_hans)
│   │   ├── src/                     # Implementation files
│   │   └── main.cpp                 # CLI entry point
│   ├── StationScript/               # StationScript compiler (WIP)
│   ├── exports/                     # Language bindings
│   │   ├── common/node/             # Common Node.js adapter utilities
│   │   └── IC10/node/               # IC10 Node.js bindings (ic10-node-api)
│   ├── tests/                       # Unit tests
│   │   ├── common/                  # Test utilities
│   │   ├── ic10/                    # C++ tests (lexer, parser)
│   │   │   └── node/                # Node.js binding tests (Jest + TypeScript)
│   │   ├── station_script/          # StationScript tests
│   │   └── node/ic10/               # Node.js package (ic10-node-api)
│   ├── scripts/                     # Build scripts (PowerShell)
│   ├── cmake/                       # CMake modules (node.cmake, pybind11.cmake)
│   ├── .clang-format                # Code style configuration
│   └── .clang-tidy                  # Static analysis configuration
├── .github/
│   ├── workflows/                   # CI/CD workflows (build.yml, test.yml)
│   └── actions/                     # Reusable GitHub Actions
├── CNAHGELOG.md                     # Changelog
├── CONTRIBUTING.md                  # Contributing guidelines
├── CONTRIBUTING.zh.md               # 贡献指南（中文）
└── LICENSE                          # License file
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

- **Node.js** 16.0+ (26.x recommended)
- **pnpm** 9.x (package manager)
- **node-addon-api** ^8.8.0

---

## Build Instructions

### 1. Clone the repository

```bash
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
```

### 2. Install Node.js dependencies

```bash
cd code/backend/compiler
pnpm i --ignore-workspace
```

### 3. Configure with CMake

```bash
cmake -B build -S code/backend/compiler -G Ninja -DCMAKE_BUILD_TYPE=Release
```

On Windows (MSVC) you may need to specify the generator:

```powershell
cmake -B build -S code/backend/compiler -G "Visual Studio 17 2022" -A x64
```

### 4. Build

```bash
cmake --build build --parallel
```

The executable `IC10_Compiler` will be placed in `build/bin/`.  
The Node.js native module `ic10-node-api.node` will be placed in `build/exports/IC10/node/`.

### 5. Run tests

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

---

## Usage

### CLI

```bash
cd build/bin
./IC10_Compiler
```

> **Note:** Currently the source file path is hard‑coded in `main.cpp` (pointing to `../../tests/grammarTest.ic`).  
> You can modify `main.cpp` to accept command‑line arguments.

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
import { Lexer, Parser, Analyser } from 'ic10-node-api';

const source = `
alias counter r0
start:
    move r0 10
    add r1 r0 5
    hcf
`;

// Tokenize
const tokens = Lexer.tokenize(source);

// Parse
const ast = Parser.parse(tokens);

// Semantic analysis
const analyser = new Analyser();
analyser.analyse(ast);
console.log(analyser.getSymbolTable());
```

---

## Internationalization (i18n)

The compiler supports multiple languages via the `Local<MsgPack>` framework.  
Available languages:
- English (`en_us`) – default
- Simplified Chinese (`zh_hans`)

To switch language, call:
```cpp
Loc::setLanguage("zh-hans");
```

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

Xiao Songtao (edocsitahw) – [2207150234@st.sziit.edu.cn](mailto:2207150234@st.sziit.edu.cn)

---

## Acknowledgements

- Inspired by the IC10 scripting language in [Stationeers](https://stationeers.com/).
- Built with C++23 coroutines and modern CMake.
- Node.js bindings powered by [node-addon-api](https://github.com/nodejs/node-addon-api).
