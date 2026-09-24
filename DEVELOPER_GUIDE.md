# Developer Guide

[中文](DEVELOPER_GUIDE.zh.md)

So you want to work on the IC10 toolchain itself. This guide is about the **code**: what lives where, how to
build and test it, and how a release is cut.

> Looking for project etiquette instead — how to open an issue, which branch to target, how to phrase a commit?
> That's [CONTRIBUTING.md](CONTRIBUTING.md).

---

## What's in the box

Four shipped things share one build system:

| Deliverable | Source | Ships as |
|---|---|---|
| IC10 compiler core (C++23) | `code/IC10/backend/compiler/` | `ic10c` CLI + `ic10c-node` / `ic10c_python` / Java `ic10` |
| IC10 runtime — the execution engine | `code/IC10/backend/runtime/` | `ic10r-node` (powers the debugger) |
| Editor support | `code/IC10/plugins/` | the [IC10](https://marketplace.visualstudio.com/items?itemName=edocsitahw.ic10) VS Code extension |
| Game metadata | `code/IC10/assets/ic10-matedata/` | `@ic10/metadata` on npm |

Everything is wired through one CMake root (`code/CMakeLists.txt`) and one set of build scripts
(`code/scripts/`), so a full build is a couple of commands no matter which piece you are touching.

---

## Repository layout

```
Stationeers/
├── code/
│   ├── CMakeLists.txt                 # the single CMake root for every target
│   ├── cmake/                         # Find{Node,NodeAddonApi,Pybind11,Fbjni,GoogleTest}.cmake
│   │   └── modules/                   # Functions, CommonSettings, Dependencies, Locale, Debug
│   ├── scripts/                       # build-info.json + bashShell/ + powerShell/
│   ├── third-party/                   # dependency cache (fbjni, GoogleTest)
│   ├── common/cpp/                    # core (utils, i18n, async coroutines), export/, tests/
│   ├── Aero/                          # placeholder for the high-level language
│   └── IC10/
│       ├── assets/
│       │   ├── ic/                    # .ic sources shipped with the project (stdLib.ic, grammarTest.ic)
│       │   └── ic10-matedata/         # @ic10/metadata — device/instruction metadata + stationpedia
│       ├── backend/
│       │   ├── compiler/
│       │   │   ├── core/              # lexer, parser, ast, semantic, link, incremental, locals
│       │   │   ├── exports/           # thin per-language binding layers (node, python, java)
│       │   │   ├── publish/           # the shippable package for each language
│       │   │   └── tests/             # cpp, node, python, java
│       │   └── runtime/               # IC10 execution engine (core, exports, publish, tests)
│       ├── frontend/                  # reserved (currently empty)
│       └── plugins/
│           ├── vscode/ic10-language-support/   # packages/{client,common,server,debugger} pnpm workspace
│           └── jetbrains/                       # JetBrains-side experiments
├── docs/                              # grammar, design notes, reference docs
├── .github/workflows/                 # CI/CD
├── CHANGELOG.md · CONTRIBUTING.md · DEVELOPER_GUIDE.md · VERSION · LICENSE
```

`code/build/`, `code/artifacts/`, `code/node_modules/` and `code/cmake-build-msvc-release/` are generated —
never edit them by hand.

---

## Architecture in one pass

### Compiler — `code/IC10/backend/compiler/core`

The classic pipeline, one header directory per stage under `include/ic10_compiler/`:

```
source → lexer → parser (+ AST, doc-comment annotations) → semantic → link → symbol table / JSON
                     ↘ incremental (line-level lex cache + statement-level parse cache) ↗
```

- **`locals/`** holds the message catalogs; user-facing text goes through `ILoc::msgStr` / `msgFormat`,
  never through string literals. Add a language by adding a catalog, not by touching the phases.
- **`incremental/`** is what makes the editor feel instant: it re-lexes changed lines and re-parses changed
  statements only, then reconciles token positions with the cached snapshot.
- The compiler is **metadata-driven** — instructions, operand types and devices come from `@ic10/metadata`
  plus the `.ic` assets rather than being hard-coded in C++.

### Runtime — `code/IC10/backend/runtime/core`

A C++23 IC10 machine: memory and stack, a device manager with logic/slot I/O, and per-arity instruction
executors. It is deliberately separate from the compiler and exposes the same N-API shape as the compiler, which
is what lets the VS Code debugger single-step an `.ic` file instead of just analysing it.

### Bindings — `exports/` vs `publish/`

`exports/<lang>/` is the adapter (marshal values, expose classes); `publish/<lang>/` is the package users
install. Adding a language means adding both, plus a row in `code/scripts/build-info.json`.

### Editor plugins — `code/IC10/plugins/vscode/ic10-language-support`

A pnpm workspace of four packages:

| Package | Role |
|---|---|
| `client` | extension entry point, contributions (languages, grammars, config, debuggers) |
| `server` | the LSP server — diagnostics, completion, hover, semantic tokens, formatting, signature help |
| `debugger` | a DAP adapter driving the runtime through `ic10r-node` |
| `common` | shared types/utilities copied into both `client` and `server` at package time |

### Metadata — `code/IC10/assets/ic10-matedata`

`@ic10/metadata` publishes `./locals`, `./std` and `./stationpedia/{CN,EN}`, generated from YAML/JSON sources by
`script/build.mjs`. The VS Code server consumes the published package; the compiler reads the same data through
the `.ic` assets. If you touch metadata, rebuild the package before rebuilding the plugins.

---

## Toolchain

Versions below are what CI and the dev container actually use.

| Tool | Version | Notes |
|---|---|---|
| CMake | 4.2.1 | `cmake_minimum_required(VERSION 3.28.1)` |
| C++ | C++23 | GCC 15 (Linux CI), MSVC (Windows CI) |
| Ninja / ccache / lld | distro | optional; ccache is enabled by default when found |
| Node.js | 26.7.0 | packages declare `engines.node >= 16` |
| pnpm | 11.20.0 | |
| Python | 3.13 | |
| JDK | 21 | Gradle toolchain 21 |
| GoogleTest | v1.17.0 | fetched into `code/third-party` |
| fbjni | v0.7.0 | Java bindings |
| node-addon-api / pybind11 | unpinned (`latest`) | resolved at configure time — pin them if you need reproducibility |
| `jq` | — | required by the bash build scripts |

---

## Building

### The short way

Build scripts handle configure + build + (for most targets) run the tests. They locate the repo themselves, so
you can call them from anywhere:

```bash
# Linux / macOS
code/scripts/bashShell/buildIC10CompilerCore.sh
```

```powershell
# Windows
pwsh code/scripts/powerShell/BuildIC10CompilerCore.ps1
```

Extra arguments are forwarded straight to CMake, which is how CI points builds at its artifact directory:

```bash
code/scripts/bashShell/buildIC10CompilerNode.sh -DSTATIONEERS_ARTIFACTS_DIR=/path/to/artifacts
```

| Script (`bashShell/` · `powerShell/`) | Builds | Runs tests | Artifact |
|---|---|---|---|
| `buildIC10CompilerCLI.sh` · `BuildIC10CompilerCLI.ps1` | CLI (bash adds ASan/UBSan) | — | `code/build/bin/Release/ic10c[.exe]` |
| `buildIC10CompilerCore.sh` · `BuildIC10CompilerCore.ps1` | core + C++ tests | ✅ | `code/build/IC10/backend/compiler/tests/Release/IC10_Compiler_Tests[.exe]` |
| `buildIC10CompilerNode.sh` · `BuildIC10CompilerNode.ps1` | Node addon | ✅ Jest | `…/compiler/publish/node/src/ic10c-node.node` |
| `buildIC10CompilerPython.sh` · `BuildIC10CompilerPython.ps1` | Python extension | ✅ pytest | `…/compiler/publish/python/ic10c_python.{pyd,so}` |
| `buildIC10CompilerJava.sh` · `BuildIC10CompilerJava.ps1` | JNI library | ✅ Gradle | `…/compiler/publish/java/src/main/resources/native/` |
| `buildIC10RuntimeNode.sh` · `BuildIC10RuntimeNode.ps1` | runtime addon | ✅ Jest | `…/runtime/publish/node/src/ic10r-node.node` |

The table also lives in machine-readable form in `code/scripts/build-info.json` — targets, configs, test
binaries and publish directories are read from there, so add new targets there rather than hard-coding paths
into scripts.

### The raw CMake way

```bash
cd code
cmake -B build -S .
cmake --build build --target IC10_Compiler_Tests --config Release
```

The build directory and configuration come from `build-info.json`: `build/` and `Release`.

**Options.** The project resolves a *feature tree*: `BUILD_<PATH>` flags such as
`BUILD_IC10_COMPILER_CORE`, `BUILD_IC10_COMPILER_TESTS`, `BUILD_IC10_COMPILER_EXPORTS_NODE`,
`BUILD_IC10_COMPILER_EXPORTS_PYTHON`, `BUILD_IC10_COMPILER_EXPORTS_JAVA` and `BUILD_IC10_RUNTIME_EXPORTS_NODE`
turn subdirectories on, and `ANY_*` aggregates are derived from them (`ANY_BUILD`, `ANY_TESTS`, `ANY_NODE`, …).
The tree itself is declared in `code/CMakeLists.txt` (`BUILD_CHILDREN`, `IC10_CHILDREN`, …), so a new component
becomes buildable by adding it to that tree instead of writing new `if()` blocks.

Infrastructure switches (see `st_l_options` in `code/cmake/modules/Functions.cmake`):

| Option | Default | Effect |
|---|---|---|
| `ENABLE_CCACHE` | ON | use ccache/sccache when available |
| `ENABLE_PAPALLEL` | ON | set `CMAKE_BUILD_PARALLEL_LEVEL` (yes, it is spelled that way in the source) |
| `ENABLE_ARTIFACTS` | ON | hash-based artifact cache; `STATIONEERS_ARTIFACTS_DIR` sets its root |
| `ENABLE_DEP_CACHE` | ON | keep fetched deps in `STATIONEERS_DEP_CACHE_DIR` (defaults to `code/third-party`) |
| `ENABLE_AUTO_LINKER` | ON | pick a fast linker automatically |
| `LIMIT_LINKED_MEMORY` | OFF | cap linked memory on machines with little RAM |
| `STATIONEERS_LANG` | — | build-time default locale (`en-us`, `zh-hans`) |

There is **no** `CMakePresets.json`, and `CMAKE_BUILD_TYPE` is not used by the scripts — the configuration is
always `Release`, because multi-config generators ignore `CMAKE_BUILD_TYPE` anyway.

---

## Testing

```bash
# C++ core (GoogleTest)
code/build/IC10/backend/compiler/tests/Release/IC10_Compiler_Tests      # Windows: …\IC10_Compiler_Tests.exe

# Node bindings (Jest + ts-jest)
cd code/IC10/backend/compiler && pnpm install --ignore-scripts && pnpm run test

# Runtime (Jest)
cd code/IC10/backend/runtime && pnpm install --ignore-scripts && pnpm run test

# Python bindings (pytest)
cd code/IC10/backend/compiler/tests/python && python -m pytest

# Java bindings (JUnit + Gradle)
cd code/IC10/backend/compiler/tests/java && ./../../publish/java/gradlew test --no-daemon --stacktrace
```

`ctest` is **not** wired up: `add_test()` exists, but `enable_testing()` is commented out in
`code/IC10/backend/compiler/CMakeLists.txt`, so run the test binary directly (or use the build scripts, which
do it for you). If you want `ctest`, uncomment that line — the tests are already registered. On single-config
generators the `Release/` path segment disappears — that resolution is handled by `resolve_artifact_path` in
`code/scripts/bashShell/modules/common.sh`.

Test layers are intentionally separated: C++ tests cover compiler behaviour, while binding tests only verify
that the binding marshals correctly — don't duplicate core assertions in four languages.

---

## Code style & static analysis

| Scope | Tool | Config |
|---|---|---|
| C++ (`compiler/`, `runtime/`, `common/cpp/`) | clang-format | `.clang-format` (LLVM-based, 100 columns, 4 spaces) |
| C++ | clang-tidy | `.clang-tidy` in the same three directories |
| TypeScript (extension) | prettier | `code/IC10/plugins/vscode/ic10-language-support/.prettierrc.json` (120 cols, double quotes) |

CI currently runs **build + tests only** — there is no static-analysis workflow, so these are on you to run
locally (C++):

```bash
# formatting
clang-format -i <file>

# clang-tidy, driven by the compile database the build already exports
cd code && cmake -B build -S . && clang-tidy -p build <file>

# ASan + UBSan: the CLI build script already compiles with -fsanitize=address,undefined
code/scripts/bashShell/buildIC10CompilerCLI.sh
```

```bash
# cppcheck (run from the repository root)
cppcheck --enable=all --suppress=missingIncludeSystem --std=c++23 \
  -I code/common/cpp/core/include \
  -I code/IC10/backend/compiler/core/include \
  code/IC10/backend/compiler/core/src/
```

C++ documentation comments are Doxygen and bilingual, using `@if zh` / `@else` / `@endif`.

---

## Dev container

`.devcontainer/` gives you a ready toolchain (Ubuntu 24.04, GCC 15, CMake 4.2.1, Node 26.7.0, pnpm 11.20.0,
Python 3.13, JDK 21, ninja, ccache, lld) with ccache/pnpm/npm volumes mounted for caching. It is a convenience
for local work only — CI does not run inside Docker.

---

## Releasing

`VERSION` is a small hand-edited YAML-ish file:

```yaml
core:
    IC10: 3.0.0

plugins:
    vscode:
        IC10 Language Support: 1.0.2
```

The checklist is [docs/releaseList.md](docs/releaseList.md). In short: update README/CHANGELOG/VERSION and each
package's version, make sure the C++ core and bindings compile, tests pass, and the generated type
declarations (`ic10c-node` `.d.ts`, Python `.pyi`) are regenerated.

| Tag pattern | Workflow | Produces |
|---|---|---|
| `v[0-9]*` (e.g. `v3.0.0`) | `release.yml` | **draft** GitHub Release with `ic10c`, `ic10c-node.node`, `ic10c-python.os`, `ic10c-java.os` per platform |
| `vscode-ic10-language-support-v*` | `vscode-extension-release.yml` | `vsce`-packaged `.vsix` + draft release |

The extension does not need a local toolchain to ship: it depends on the published `ic10c-node` / `ic10r-node`
packages and bundles their prebuilt `.node` binaries into the VSIX.

The Python binding ships as the **`ic10c-python`** wheel: `publish/python/pyproject.toml` carries the metadata,
and `publish/python/setup.py` extends `build_py` to package the CMake-built `ic10c_python.{pyd,so}` together
with its `.pyi` stub (setuptools' `py-modules` only accepts `.py` sources, so metadata alone would build an
empty wheel).

> **Note:** `release.yml` takes its reusable-workflow inputs from the job matrix (`matrix.os`,
> `matrix.script-dir`, …) and the script directory is `code/scripts/…`. The reusable workflows `cd` into
> `script-dir` before running the script, so both have to stay in sync if you edit that file.

---

## Known rough edges

Honest list, so nobody re-discovers it the hard way:

- `ctest` is not wired up — `enable_testing()` is commented out (see [Testing](#testing)). The tests are already
  registered with `add_test()`, so uncommenting that one line is all it takes.
- `node-addon-api` and `pybind11` are resolved as `latest` at configure time, so two builds of the same commit
  can differ.
- `code/Aero/`, `code/IC10/frontend/` and `code/IC10/plugins/jetbrains/` are placeholders or experiments.

---

## Documentation map

| Document | What it's for |
|---|---|
| [README.md](README.md) | what the project is, and how to use it |
| [CONTRIBUTING.md](CONTRIBUTING.md) | issues, PRs, commit conventions, coding standards |
| [CHANGELOG.md](CHANGELOG.md) | release history |
| [docs/releaseList.md](docs/releaseList.md) | release checklist per component |
| [docs/annotationExample.md](docs/annotationExample.md) | the `#:` / `#>` annotation syntax, by example |
| [docs/grammar/](docs/grammar/) | split ANTLR grammar (lexer/parser × instruction arity) |
| [docs/doxygen/](docs/doxygen/) | Doxygen HTML theme (doxygen-awesome) and page header |
| [docs/PROJECT_DEBT.md](docs/PROJECT_DEBT.md) | known debt and its format |
