# Change Log

[中文](CHANGELOG.zh.md)

> This package ships the IC10 compiler C++ core as a Node.js native addon, so an entry may describe
> either the binding surface (exported adapters, type declarations, packaging) or the compiler behaviour
> that the addon exposes.

## 2026/06/25

- [1.0.0]: publish version 1.0.0 

## 2026/06/29

- [1.0.1]: Fix the issue of crashing when importing the module with VSCode 
- [1.0.2]: Hide debug output

## 2026/07/03

- [2.0.0]: Compiler optimisation (two passes), and a unified way of using the Node.js binding

## 2026/07/09

- [2.1.0]: Incremental compilation support (line-level lexing cache + statement-level parse cache)

## 2026/07/22

- [2.3.0]: Repackaged: ship `types/` (`index.d.ts` plus the lexer/parser/semantic declarations) and `static/`, and add the `@types/node` dev dependency
- [2.3.0]: Annotation syntax in the language, and the linker exported to Node.js

## 2026/07/23

- [2.4.0]: LSP hover data exposed to the binding (aliases, labels, constants, instruction keywords)

## 2026/07/25

- [2.4.1]: LSP semantic tokens exposed to the binding

## 2026/07/26

- [2.5.0]: LSP completion exposed to the binding, and the type table exported

## 2026/07/27

- [2.6.0]: Type table exported from the linker

## 2026/07/29

- [2.6.1]: Fix escape handling in the lexer
- [2.6.3]: Fix token position misalignment when the incremental parser refreshes its cache; enhanced error recovery

## 2026/07/30

- [2.6.4]: Fix the jump target range error

## 2026/07/31

- [2.6.6]: Fix several plugin-reported errors

## 2026/08/21

- [2.7.0]: **Renamed `ic10-node-api` → `ic10c-node`** (native module `src/ic10-node-api.node` → `src/ic10c-node.node`); fix the VS Code import crash
- [2.7.2]: Runtime support for the VS Code plugin

## 2026/09/24

- [3.0.0]: IC10 v3 — rewritten lexer and parser with the unified per-arity AST, adjusted `annotation` and `link` grammar rules, resolved predefined constants, and patched metadata plus the standard library
