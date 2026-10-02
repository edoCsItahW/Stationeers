# Change Log

[中文](CHANGELOG.zh.md)

> This package ships the IC10 runtime C++ core as a Node.js native addon, so an entry may describe
> either the binding surface (exported adapters, type declarations, packaging) or the execution
> behaviour that the addon exposes.

## 2026/08/12

- [1.0.0]: publish version 1.0.0

## 2026/08/21

- [1.0.1]: Fix the `ic10r-node` type declarations
- [1.0.4]: Repackaged: the runtime no longer duplicates the compiler's declarations — the lexer, parser, AST and instruction types come from the compiler package, and only `config` / `context` / `device` / `engine` / `manager` / `memory` / `value` remain here

## 2026/08/22

- [1.0.5]: Runtime support for the VS Code plugin: message language switching (`IC10RuntimeLocale.setLanguage`), `Engine.step()` and `Engine.diagnostics`, the `allowErrorStatements` config, and diagnostics reported through a reporter instead of being printed to stderr

## 2026/09/28

- [2.0.0]: **Renamed `ic10r-node` → `@ic10/runtime`**. IC10 v3: the executor is ported to the unified per-arity AST (`Statement` / `ExecutableInstruction` handles, `TypeAnnotation` in place of the old doc-comment nodes)
- [2.0.0]: Operands that the v3 grammar accepts now execute: write targets may be aliases of a register (`alias tmp r3`), device references may be aliases, dynamic registers (`rr0`) and dynamic device ports (`dr0`, `drr0` — the register holds the port number, `-1` being the self reference), and the device/name hash of `sb` / `lb` / `lbn` / `lbs` / `lbns` may live in a register
- [2.0.0]: The value written by `s` / `sb` / `ss` / `sbn` / `sbs` may be a literal, a constant alias, a register or an enum constant, matching the relaxed operand list of the compiler
- [2.0.0]: Enum operands are evaluated from the type table (`Color.Green`, and bare member names such as `Pressure`, `Sum` or `Contents` in logic property / aggregate mode / reagent mode positions); `Engine` and `Context` take an optional `TypeTable` as their fourth argument
- [2.0.0]: Legacy numeric logic properties are passed on as their decimal text, `$hex` / `%bin` text is parsed both in operand values and in constants, and a register / device / operand that cannot be resolved reports `IEM2_1` instead of silently doing nothing

## 2026/10/01

- [2.1.0]: Device types and introspection: `#>` device annotations make up a type registry (`DeviceType` / `DeviceRegistry`), the execution context binds ports automatically from the type declared in the source (`Manager.bindTyped`), and `SimDevice` reports its type name plus type / name hash, which is what makes hash-based device lookups (`lb` / `lbn` / `sb` / `sbn`) meaningful on typed ports
- [2.1.0]: `IDevice.snapshot()` lists a device's members (name, kind, value declared by the annotation, current value, whether the type declares it, whether it was assigned). A member **has a value only once it was assigned**: `value` is empty rather than 0 while it was never assigned (a device's default is not necessarily 0); the binding gained `Device.snapshot()` and the `Manager` type / port APIs, with `types/device.d.ts` and `types/manager.d.ts` kept in sync

## 2026/10/02

- [2.2.0]: Device member defaults: the value appended to an annotation member line becomes that member's default (`DeviceMemberDecl.defaultValue`). Before the member is assigned, `readLogic` answers with the default and `canReadLogic` is true; `IDevice::setMemberDefault()` lets a host override it, and the binding reports the declaration through `DeviceMember.defaultValue`
- [2.2.0]: Defaults declared by type hints are consumed by the execution context (`Context::initHintDefaults`): `alias s d0 #: @type Sensor @default logic Setting 1` overrides the default of that member on the port's device (only members the annotation declares, and only on ports already bound through `@type`), while `alias a r1 #: @default 7` seeds the register with that initial value; dynamic ports (`dr0`) and dynamic registers (`rr0`) are skipped, as their target is unknowable at construction time
- [2.2.0]: New `Config.strictEvaluation` option (enabled by default): an operand that cannot be evaluated reports `IEM2_1`; when disabled such operands take part as 0 (which lets the debugger avoid "unable to evaluate"). `ConfigAdapter` and `types/config.d.ts` kept in sync
- [2.2.0]: Fix macro constants such as `define H HASH("x")` / `define S STR("x")` failing to evaluate (`IEM2_1`): macro constants are now decided before the kind-based numeric branch
