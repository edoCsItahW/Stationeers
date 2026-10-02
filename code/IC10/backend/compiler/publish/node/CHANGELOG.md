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
- [3.0.1]: Fix several incremental compilation issues and relax some grammar restrictions

## 2026/09/26

- [3.1.1]: Relaxed operand rules: the value written by `s` / `sb` / `ss` / `sbn` / `sbs` is no longer restricted to a register but accepts any numeric value (a literal, a constant alias, a register or an enum constant), and `DEVICE_HASH` / `NAME_HASH` positions may hold a register directly; `types/parser/ast.d.ts` kept in sync

## 2026/10/01

- [3.1.2]: Fix the start position of several tokens in the lexer (strings, the `#:` / `#>` prefixes, hexadecimal and binary numbers)
- [3.1.2]: Fix a semantic-analysis error around the built-in symbols declared by an `@builtin` hint

## 2026/10/02

- [3.2.2]: New `@default` tag in a `#:` type hint: `@default category field value` declares a device member's default (categories are `logic` / `logic-slot` / `slot`), while `@default value` declares a register's default. The value may be a literal or a constant identifier; declaring the same field of the same category twice reports `IEP37_1` and an unknown category reports `IEP34_1`
- [3.2.2]: Member lines of a `#>` device annotation (`@logic` / `@logic-slot` / `@slot`) accept a default appended at the end (`@logic Setting 12 1`); **omitting it means there is no default**, not that the default is 0
- [3.2.2]: The syntax above is exposed to the language side: the type table and `types/parser/ast.d.ts` provide `TypeHintNode.defaults`, `TypeHintDefaultNode` (`category` / `name` / `value`) and `TypeAnnotationLineBaseNode.defaultValue`
- [3.2.2]: Fix the process abort when `@default` has no value: the `Error` thrown while parsing the value crossed the `noexcept` boundary of the `TypeHint` parser; the diagnostic is now reported and that default is dropped
