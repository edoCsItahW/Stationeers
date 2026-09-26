# Change Log

[中文](CHANGELOG.zh.md)

## 2026/09/26

- feat: Relax the IC10 grammar operand rules
- feat: Update the IC10 VS Code plugin for the IC10 v3 package
- feat: Publish the IC10 VS Code plugin v2 (client and server bundled with esbuild, packaging reworked)

## 2026/09/25

- feat: Publish the ic10c-node v3 package
- feat: Update the IC10 VS Code plugin for the IC10 v3 package
- fix: Fix the v3.0.0 release CI (artifact naming, duplicate assets and release permissions)

## 2026/09/24

- refactor: Refactor IC10 metadata (split into the `@ic10/metadata` package)

## 2026/09/23

- refactor: Refactor IC10 VS Code plugin package frame (client / common / debugger / server packages)
- feat: Adjust IC10 VS Code plugin for IC10 v3
- fix: Fix some AST position errors
- fix: Fix coroutine chain breaking issue

## 2026/09/22

- perf: Optimize the compilation time and memory consumption of IC10 Core

## 2026/09/21

- feat: Enable Docker container
- fix: Fix coroutine chain breaking issue

## 2026/09/20

- feat: Adjust IC10 grammar 'annotation' rule (@logic / @logic-slot now take a pin index, new @slot tag)
- fix: Patch metadata and standard library

## 2026/09/19

- feat: Adjust IC10 VS Code plugin for IC10 v3

## 2026/09/18

- feat: Adjust IC10 grammar 'link' rule
- feat: Adjust IC10 VS Code plugin for IC10 v3

## 2026/09/12

- feat: Update IC10 standard library
- feat: Adjust IC10 VS Code plugin for IC10 v3
- fix: Resolve predefined constants problem
- fix: Fix DefineDirectiveNode field miss error

## 2026/09/05

- feat: Implement IC10 v3 (flatten instruction variants, drop duplicated AST definitions in language bindings)
- build: Update CMake cache system

## 2026/08/28

- feat: Implement IC10 v3 (rewrite lexer and parser, unified per-arity AST nodes)

## 2026/08/26

- docs: Restructure IC10 v3 grammar files (split into lexer / parser modules, add grammar generator tool)

## 2026/08/22

- feat: Implement VS Code plugin runtime (adapters for the debug session)

## 2026/08/21

- feat: Implement VS Code plugin runtime (debugger, 2 rounds)
- feat: Publish IC10 runtime Node.js package (ic10r-node)
- feat: Implement VS Code plugin hover renderer (svg / markdown selectable)
- fix: Fix ic10r-node stack error
- fix: Fix ic10r-node types
- fix: Fix VS Code crash error

## 2026/08/20

- refactor: Refactor project CI/CD frame
- chore: Delete code/package.json

## 2026/08/19

- refactor: Refactor project CMake frame

## 2026/08/14

- refactor: Refactor project CMake frame

## 2026/08/07

- refactor: Refactor project frame (consolidate into the code/IC10/** layout)
- feat: Implement IC10 runtime (C++ execution engine: memory/stack, device manager and I/O)

## 2026/08/06

- feat: Support packaging VS Code LSP as an executable (ic10-lsp.exe)
- feat: Update IC10 Java wrapper files

## 2026/08/04

- feat: IC10 Java JSON frame (Jackson-based AST deserialization)
- chore: Add AST fields and simplify Java AST

## 2026/08/03

- chore: Rename Java package (com.stationeers.ic10 → io.github.stationeers.ic10)

## 2026/08/02

- feat: Implement IC10 Java support (JNI binding built on fbjni)
- fix: Fix tag push CI (3 rounds)

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
