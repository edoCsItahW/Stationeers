# Contributing to IC10 Compiler

[中文](CONTRIBUTING.zh.md)

Thank you for your interest in contributing! This document outlines the process and guidelines for contributing to the IC10 Compiler project.

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
4. **Set up build environment** (for Node.js and Python bindings):
   ```bash
   # Node.js bindings: install node-gyp and download Node.js headers
   pnpm add -g node-gyp          # or: npm install -g node-gyp
   pnpm exec node-gyp install    # or: npm exec node-gyp install

   # Python bindings: ensure Python 3.13 is in PATH
   python3 --version             # should show 3.13.x
   ```

5. **Run tests** locally:
   ```bash
   # C++ tests
   cmake -B build -S code/backend/compiler -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build --parallel
   cd build && ctest --output-on-failure

   # Node.js tests
   cd code/backend/compiler && pnpm test

   # Python tests
   cd code/backend/compiler/tests/ic10/python && pytest -v
   ```
6. **Ensure code style** – The project uses `clang-format` (4-space indentation). Run:
   ```bash
   clang-format -i <file>
   ```
7. **Push** and open a Pull Request to `develop`.

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
docs: update README for v2.0.0
test(ic10): add linker unit tests
ci: add Python workflow
```

## Coding Standards

- **Language**: C++23 (use coroutines, concepts, ranges where appropriate).
- **Style**: Follow `.clang-format` (4-space indentation, braces on same line, 100-column limit). See [`.clang-format`](code/backend/compiler/.clang-format) for full configuration.
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
    - C++ tests: `code/backend/compiler/tests/ic10/`
    - Node.js tests: `code/backend/compiler/tests/ic10/node/__tests__/`
    - Python tests: `code/backend/compiler/tests/ic10/python/`
- **Documentation**: Use Doxygen-style comments with bilingual (`@if zh / @elseif en`) tags for public API.

## Module Overview

| Module | Location | Description |
|:-------|:---------|:------------|
| Lexer | `ic10/lexer/` | State-machine based tokenizer |
| Parser | `ic10/parser/` | Recursive-descent parser, AST construction |
| Semantic | `ic10/semantic/` | Symbol table, type inference, type checking |
| Linker | `ic10/link/` | Multi-unit symbol merging, cross-unit resolution |
| Incremental | `ic10/incremental/` | Line/statement-level caching for fast recompilation |
| Locals | `ic10/locals/` | Diagnostic message localization (en-us, zh-hans) |
| Common | `common/` | Shared utilities (async, exception, utils) |

## Pull Request Guidelines

- Target branch: `develop`.
- Include a clear description, linked issue if applicable.
- Ensure CI passes (build, test, static analysis).
- Keep changes focused – one PR per feature/bugfix.
- Update documentation (README, inline Doxygen comments) if needed.
- Follow the [commit message convention](#commit-message-convention).

## Building with Extra Checks

- **Static analysis** (clang-tidy):
  ```bash
  cmake -B build -S code/backend/compiler -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_CLANG_TIDY="clang-tidy;--config-file=code/backend/compiler/.clang-tidy"
  cmake --build build
  ```

- **cppcheck**:
  ```bash
  cppcheck --enable=all --suppress=missingIncludeSystem --std=c++23 \
    -I code/backend/compiler/common/include \
    -I code/backend/compiler/IC10/include \
    code/backend/compiler/IC10/src/
  ```

- **Address sanitizer** (Linux):
  ```bash
  cmake -B build-san -S code/backend/compiler -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"
  ```

## Versioning

This project follows [Semantic Versioning 2.0.0](https://semver.org/). The version is tracked in [VERSION](VERSION).

- **MAJOR**: Breaking syntax changes or API incompatibilities
- **MINOR**: New features (linker, incremental compiler, Python bindings, etc.)
- **PATCH**: Bug fixes and minor improvements

## Questions?

Feel free to open a discussion issue or contact the maintainer at [edocsitahw@qq.com](mailto:edocsitahw@qq.com).
