# Contributing to IC10 Compiler

[中文](CONTRIBUTING.zh.md)

Thank you for your interest in contributing! This document covers **how we collaborate** — issues, pull requests, commit messages and versioning.

> Setting up a build environment, running tests, or understanding the architecture? That all lives in the [Developer Guide](DEVELOPER_GUIDE.md).

## Code of Conduct

By participating, you agree to uphold a respectful and inclusive environment. Please report unacceptable behavior to the project maintainer.

## How to Contribute

1. **Report bugs** – Use GitHub Issues, describe the problem, include reproduction steps and environment details.
2. **Suggest enhancements** – Open an issue labeled `enhancement` with a clear description of the proposed feature.
3. **Submit pull requests** – Follow the process below.

## Development Workflow

1. **Fork** the repository.
2. **Create a feature branch** from `develop` (or `main` for urgent fixes).
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make changes** – Keep commits logical, write clear messages following [Conventional Commits](#commit-message-convention).
4. **Build and test your change** – toolchain versions, build scripts and per-language test commands are in the [Developer Guide](DEVELOPER_GUIDE.md#building). One script builds the C++ core and runs its tests:
   ```bash
   code/scripts/bashShell/buildIC10CompilerCore.sh   # Windows: pwsh code/scripts/powerShell/BuildIC10CompilerCore.ps1
   ```
5. **Format your change** – the project uses `clang-format` (4-space indentation, 100 columns):
   ```bash
   clang-format -i <file>
   ```
6. **Push** and open a Pull Request to `develop`.

## Commit Message Convention

This project follows [Conventional Commits 1.0.0](https://www.conventionalcommits.org/). All commit messages must use the following format:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

### Commit Types

| Type | Description |
|:-----|:------------|
| `feat` | A new feature |
| `fix` | A bug fix |
| `docs` | Documentation only changes |
| `style` | Changes that do not affect the meaning of the code (formatting, etc.) |
| `refactor` | A code change that neither fixes a bug nor adds a feature |
| `test` | Adding missing tests or correcting existing tests |
| `ci` | Changes to CI configuration files and scripts |
| `chore` | Other changes that don't modify src or test files |
| `build` | Changes that affect the build system or external dependencies |

### Breaking Changes

For breaking changes, add `BREAKING-CHANGE:` in the footer or append `!` after the type/scope:

```
feat!: comprehensive grammar updates accompanying game updates

BREAKING-CHANGE: syntax has changed and language usage methods have been disrupted
```

### Examples

```
feat: implement linker for multi-unit symbol merging
fix(lexer): fix unclosed string swallowing subsequent input
docs: update the developer guide
test(ic10): add linker unit tests
ci: add Python workflow
```

## Coding Standards

- **Language**: C++23 (use coroutines, concepts, ranges where appropriate).
- **Style**: Follow the `.clang-format` of the component you are in — [compiler](code/IC10/backend/compiler/.clang-format), [runtime](code/IC10/backend/runtime/.clang-format), [common](code/common/cpp/.clang-format) (4-space indentation, braces on same line, 100-column limit). TypeScript in the VS Code extension uses prettier (120 columns, double quotes).
- **Naming**:
    - Types: `PascalCase` (e.g., `Lexer`, `SymbolTable`, `IncCompiler`)
    - Functions/variables: `camelCase` (e.g., `extractHexNumber`, `pos_`)
    - Member variables: `camelCase` with trailing underscore (e.g., `source_`, `pos_`)
    - Macros: `UPPER_SNAKE_CASE` (avoid macros when possible)
    - Enum values: `PascalCase` (e.g., `TokenType::KEYWORD_ADD`)
- **Error handling**: The project uses `std::expected<T, Error>` for recoverable errors and custom `Error` hierarchy with exceptions for parsing errors. Follow the pattern in `parser.cpp` and `main.cpp`.
- **Internationalization**: All user-facing strings must go through the localization framework:
    ```cpp
    // Message without arguments
    ILoc::msgStr<IMsgId::IWL1>()

    // Message with arguments
    ILoc::msgFormat<IMsgId::IWL1>(charValue)
    ```
- **Testing**: Add unit tests for new features:
    - C++ tests: `code/IC10/backend/compiler/tests/cpp/` (runtime: `code/IC10/backend/runtime/tests/`)
    - Node.js tests: `code/IC10/backend/compiler/tests/node/`
    - Python tests: `code/IC10/backend/compiler/tests/python/`
    - Java tests: `code/IC10/backend/compiler/tests/java/`

    Test behaviour in the C++ core; binding tests should only verify that the binding marshals correctly, not repeat core assertions.
- **Documentation**: Public API carries bilingual Doxygen comments using `@if zh` / `@else` / `@endif`.

## Module Overview

| Module | Location | Description |
|:-------|:---------|:------------|
| Lexer | `compiler/core/include/ic10_compiler/lexer/` | State-machine based tokenizer |
| Parser | `compiler/core/include/ic10_compiler/parser/` | Recursive-descent parser, AST construction |
| Semantic | `compiler/core/include/ic10_compiler/semantic/` | Symbol table, type inference, type checking |
| Linker | `compiler/core/include/ic10_compiler/link/` | Multi-unit symbol merging, cross-unit resolution |
| Incremental | `compiler/core/include/ic10_compiler/incremental/` | Line/statement-level caching for fast recompilation |
| Locals | `compiler/core/include/ic10_compiler/locals/` | Diagnostic message localization (en-us, zh-hans) |
| Runtime | `runtime/core/` | IC10 execution engine: memory/stack, devices, instruction executors |
| Metadata | `assets/ic10-matedata/` | `@ic10/metadata` — instruction, device and Stationpedia data |
| Common | `code/common/cpp/core/` | Shared utilities (async coroutines, diagnostics, i18n, utils) |

Compiler, runtime and metadata paths are relative to `code/IC10/backend/`.

## Pull Request Guidelines

- Target branch: `develop`.
- Include a clear description, linked issue if applicable.
- Ensure CI passes — it builds and tests every target. Formatting and clang-tidy are **not** run in CI, so check those yourself (see [Code style & static analysis](DEVELOPER_GUIDE.md#code-style--static-analysis)).
- Keep changes focused – one PR per feature/bugfix.
- Update documentation (README, inline Doxygen comments) if needed.
- Follow the [commit message convention](#commit-message-convention).

## Versioning

This project follows [Semantic Versioning 2.0.0](https://semver.org/). Versions live in [VERSION](VERSION), one entry per component:

```yaml
core:
    IC10: 3.0.0

plugins:
    vscode:
        IC10 Language Support: 1.0.2
```

- **MAJOR**: Breaking syntax changes or API incompatibilities
- **MINOR**: New features
- **PATCH**: Bug fixes and minor improvements

Tags map to components: `v<version>` releases the core, `vscode-ic10-language-support-v<version>` releases the VS Code extension. Every release updates [CHANGELOG.md](CHANGELOG.md), and the per-component checklist is [docs/releaseList.md](docs/releaseList.md).

## Questions?

Feel free to open a discussion issue or contact the maintainer at [edocsitahw@qq.com](mailto:edocsitahw@qq.com).
