# IC10 Compiler

[中文](README.zh.md)

A modern C++23 compiler for **IC10**, the assembly‑like scripting language used in the game [Stationeers](https://store.steampowered.com/app/544550/Stationeers/).  
This project provides lexical analysis, syntax analysis, semantic analysis, and an extensible infrastructure with async coroutine support.

---

## Features

- **Lexer** – tokenizes IC10 source code (supports registers, devices, numbers, strings, comments, keywords).
- **Parser** – builds an Abstract Syntax Tree (AST) for all IC10 instructions (nullary to senary).
- **Semantic Analyser** – performs symbol resolution and type checking using a `Promise`/`Future` based asynchronous symbol table.
- **Error Reporting** – rich error messages with source location, internationalization support (English / Simplified Chinese).
- **Async Coroutine Infrastructure** – custom `Task<T>`, `Promise<T>`, `Future<T>` and coroutine state management for non‑blocking symbol resolution.
- **Cross‑Platform** – builds on Linux (GCC/Clang) and Windows (MSVC).
- **Testing** – GoogleTest unit tests for lexer and parser.
- **CI/CD** – GitHub Actions workflows for build, test, static analysis (cppcheck, clang‑tidy).

---

## Project Structure

```
IC10_Compiler/
├── CMakeLists.txt                 # Top‑level CMake
├── common/                        # Shared utilities
│   ├── include/common/async/      # Coroutine tasks, promises, futures
│   ├── include/common/exception/  # Error hierarchy
│   ├── include/common/locals/     # Internationalization framework
│   └── include/common/utils/      # FStr, BiMap, enum_to_str, type lists, etc.
├── IC10/                          # IC10 compiler core
│   ├── include/ic10/lexer/        # Token & Lexer
│   ├── include/ic10/parser/ast/   # AST node definitions
│   ├── include/ic10/semantic/     # SymbolTable, Analyser
│   ├── include/ic10/locals/       # Language resources (en_us, zh_hans)
│   ├── src/                       # Implementation
│   └── main.cpp                   # CLI entry point
├── tests/                         # Unit tests (GoogleTest)
│   ├── common/                    # Test utilities
│   └── ic10/                      # Lexer & Parser tests
├── .github/workflows/             # CI scripts
```

---

## Requirements

- **CMake** 3.28.1 or higher
- **C++23** compiler:
    - Linux: GCC 13+ or Clang 16+
    - Windows: MSVC 2022 (with Visual Studio 2022 build tools)
- **Ninja** (recommended) or Make
- **Git** (for fetching GoogleTest)

---

## Build Instructions

### 1. Clone the repository

```bash
git clone https://github.com/yourusername/IC10_Compiler.git
cd IC10_Compiler
```

### 2. Configure with CMake

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

On Windows (MSVC) you may need to specify the generator:

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
```

### 3. Build

```bash
cmake --build build --parallel
```

The executable `IC10_Compiler` will be placed in `build/bin/`.

### 4. Run tests

```bash
cd build
ctest --output-on-failure
```

---

## Usage

```bash
./IC10_Compiler <source.ic>
```

Currently the source file path is hard‑coded in `main.cpp` (pointing to a test file).  
You can modify `main.cpp` to accept command‑line arguments.

Example IC10 program:

```
alias counter r0
define PI 3.14159

start:
    move r0 10
    add r1 r0 PI
    hcf
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
You may not use this software for commercial purposes without the author’s permission.

---

## Author

Xiao Songtao (edocsitahw) – [2207150234@st.sziit.edu.cn](mailto:2207150234@st.sziit.edu.cn)

---

## Acknowledgements

- Inspired by the IC10 scripting language in [Stationeers](https://stationeers.com/).
- Built with C++23 coroutines and modern CMake.
