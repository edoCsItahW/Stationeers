# Change Log

[中文](CHANGELOG.zh.md)

## 2026/08/01

- chore: The main and develop branches have been synchronized and are now rebased to main

## 2026/07/31

- release: IC10 Language Support v1.0.2 — officially published on VS Code Marketplace
- fix: Fix some plugin errors (completion provider, signature help, AST helper)
- chore: Packaging correction for extension distribution

## 2026/07/30

- release: Ready to publish (extension packaging)
- fix: Fix annotation truncation error (`#:` type hint parsing)
- fix: Fix jump target range error
- docs: Optimize IC10 language support plugin performance (parser pipeline, completion provider, semantic token, utils)
- docs: Improve IC10 language support plugin docs (server structure, settings manager, handlers, locales, themes, SVG builder)

## 2026/07/29

- feat: Enhance error recovery (2 rounds)
- feat: Supplement LSP configuration
- fix: Fix escape error (lexer string handling)
- fix: Fix misalignment caused by refreshing cache token position during incremental parsing
- fix: Fix synchronization point error

## 2026/07/28

- feat: Implement IC10 LSP format function (column alignment, trailing comment alignment, indentation, empty line compression, aggressive operand alignment)
- feat: Implement IC10 LSP signature function (instruction parameter signature and active-arg highlighting)

## 2026/07/27

- feat: Implement IC10 LSP completion function (3 passes — keyword completion, operand-type-driven completion, device-context filtering)
- feat: Export type table from linker
- fix: Fix symbol read error

## 2026/07/26

- feat: Implement IC10 LSP completion function
- feat: Export type table (for LSP semantic analysis consumption)
- feat: LSP hover update

## 2026/07/25

- feat: LSP hover update
- feat: Implement IC10 LSP semantic token (2 passes — compiler symbol-table-driven semantic coloring)
- fix: Fix mistake recovery error

## 2026/07/24

- feat: Implement IC10 LSP semantic token

## 2026/07/23

- feat: Implement IC10 LSP diagnostic report function (real-time incremental re-analysis + Problems panel integration)
- feat: Implement IC10 LSP hover function
- fix: Fix word read error

## 2026/07/22

- feat: Implement IC10 LSP hover function (aliases, labels, constants, instruction keywords)
- feat: Update IC10 Node.js package (ic10-node-api)

## 2026/07/21

- chore: Add issue template
- docs: Update document
- feat: Export Linker to Node.js and Python
- feat: Export linker

## 2026/07/20

- feat: Implement standard library
- feat: Improve IC10 C++ tests
- feat: Improve IC10 Node.js and Python tests

## 2026/07/19

- feat: Implement type inference
- feat: Implement linker
- feat: Optimize localization output
- feat: More precise semantic analysis

## 2026/07/18

- feat: Implement annotation syntax
- feat: Update metadata
- feat: More precise semantic analysis

## 2026/07/17

- refactor: Update metadata

## 2026/07/15

- feat: Write and organize metadata

## 2026/07/14

- feat: Write and organize metadata

## 2026/07/12

- feat: Comprehensive syntax updates accompanying game update
- test: Improve IC10 Node.js tests
- ci: Update CI/CD to automatically upload artifacts when pushing tags
- ci: Improve IC10 Python tests

## 2026/07/11

- fix: Fix the issue of unclosed strings swallowing subsequent input
- fix: Fix some lexer and parser issues
- test: Improve IC10 C++ tests (integration and system tests)
- test: Improve IC10 C++ tests (lexer improvements)
- ci: Add Python workflow
- build: Update build framework

## 2026/07/10

- feat: Support IC10 compiler command-line mode (ic10c CLI)

## 2026/07/09

- feat: Implement incremental compiler (line-level lexer cache + statement-level parser cache)
- chore: Adjust export directory structure
- chore: Optimize compilation speed

## 2026/07/03

- feat: Unify Node.js usage method
- feat: Optimize compiler (2 passes)

## 2026/07/01

- feat: Complete Python bindings and export for IC10
- chore: WIP intermediate checkpoint

## 2026/06/29

- feat: Implement LSP hover prompt function (2 passes — initial prototype)
- fix: Fix the issue of crashing when importing the module with VSCode

## 2026/06/28

- docs: Add ANTLR4 Grammar File (IC10.g4)
- chore: Update Copyright

## 2026/06/25

- docs: Doxygen documentation generation support
- docs: Complete TypeScript type definitions in index file

## 2026/06/24

- fix: Solve LexerAdapter lifetime problem (#22)
- docs: Update README.md
- docs: Update README and CHANGELOG files
- docs: Improve the Doxygen documentation of the code
- ci: Refactor the CI

## 2026/06/23

- feat: IC10 fully supports Node.js API (11 adapters: Lexer, Token, Parser, Program/AST, Analyser, SymbolTable, Linker, IC10Local, IncLexer, IncParser, IncCompiler)
- ci: Solve the CI/CI problem (stuck jobs)
- ci: Solve the CI/CI stuck problem

## 2026/06/18

- feat: IC10 supports Node.js interface

## 2026/06/08

- Introduce Google Test framework
- Add MSVC compilation support
- Add GitHub workflow
- Improve README and CONTRIBUTING files
- Release v1.0.0

## 2026/06/07

- Implement IC10 compiler

## 2026/06/02

- Initialize project structure and basic files.
- Add CMake configuration for backend compilation
