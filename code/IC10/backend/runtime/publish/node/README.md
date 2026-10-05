# @ic10/runtime

[![npm version](https://badge.fury.io/js/@ic10%2Fruntime.svg)](https://www.npmjs.com/package/@ic10/runtime)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![License: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[中文](./README.zh.md)

## Overview

`@ic10/runtime` is the Node.js native addon for the **IC10 runtime**: it executes the AST and symbol table
produced by the [IC10 compiler](https://www.npmjs.com/package/@ic10/compiler) (`@ic10/compiler`), tick by
tick, simulating the IC10 processor.

IC10 is the assembly-style language used in [Stationeers](https://store.steampowered.com/app/544550/Stationeers/)
to script in-game computers and devices. This package takes the program produced by the compiler and runs
it against a register file, a stack and a device manager: no C++ toolchain required, since the native
module ships prebuilt.

Current version: **2.0.0** (IC10 v3 — the executor follows the unified per-arity AST and the relaxed
operand grammar). See [CHANGELOG.md](./CHANGELOG.md) / [CHANGELOG.zh.md](./CHANGELOG.zh.md).

## Features

- **Execution Engine** – runs IC10 programs tick-by-tick (`runTick`), to completion (`runFull`) or one
  statement at a time (`step`), and reports everything it cannot execute as diagnostics
- **Register File & Stack** – full 16-register file (`r0`–`r15`) and stack memory (`push`/`pop`/`peek`/`poke`)
- **Device Manager** – register external devices and the chip device, query by type/name hash
- **Device I/O** – read/write logic properties, device stacks, slots, and reagent modes
- **IC10 v3 operands** – alias write targets, device aliases, dynamic registers (`rr0`), dynamic device
  ports (`dr0` / `drr0`), register-valued device/name hashes and enum operands evaluated from the type table
- **Execution Control** – `halt`/`sleep` support, instruction limit, tick duration configuration
- **Message language** – diagnostics in English (`en-us`) or Simplified Chinese (`zh-hans`)
- **Complete TypeScript types** – shipped in the package, no `@types/*` needed
- **Cross‑Platform** – builds on Linux (GCC/Clang) and Windows (MSVC)

## Installation

### From npm

```bash
npm install @ic10/runtime @ic10/compiler
```

The published package already contains the compiled `src/ic10r-node.node` addon, so installing it requires
**no** C++ compiler, CMake or node-gyp. Node.js >= 16 is required; the compiler package is a peer
dependency, since a program has to be compiled before it can be executed.

### From source

Building the addon is part of the repository build; see
[DEVELOPER_GUIDE.md](https://github.com/edoCsItahW/Stationeers/blob/main/DEVELOPER_GUIDE.md) for the full
toolchain table. In short you need CMake >= 3.28.1 and a C++23 compiler (GCC 13+ / Clang 16+ / MSVC 2022).

```bash
# Linux / macOS — configures CMake, builds the addon, runs the Node.js tests
code/scripts/bashShell/buildIC10RuntimeNode.sh
```

```powershell
# Windows (PowerShell)
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
pwsh code/scripts/powerShell/BuildIC10RuntimeNode.ps1
```

Both scripts build the CMake target `ic10_runtime_node` and copy the artifact to
`code/IC10/backend/runtime/publish/node/src/ic10r-node.node`. If you prefer to drive CMake yourself:

```bash
cd code
cmake -B build -S . -DBUILD_IC10_RUNTIME_EXPORTS_NODE=ON
cmake --build build --target ic10_runtime_node --config Release
```

## Quick Start

### Basic Usage

```typescript
import * as ic10c from '@ic10/compiler';
import * as ic10r from '@ic10/runtime';

// IC10 source code
const source = `
    main:
        move r0 42
        add r1 r0 10
        hcf
`;

// 1. Compile (using @ic10/compiler)
const tokens = ic10c.Lexer.tokenize(source);
const program = new ic10c.Parser(tokens).parse();
const analyser = new ic10c.Analyser();
await analyser.visit(program);

// 2. Create engine (using @ic10/runtime); the type table is needed for enum operands
const engine = new ic10r.Engine(program, analyser.symbolTable, undefined, analyser.typeTable);

// 3. Execute
engine.runFull();

// 4. Inspect results
const r0 = engine.context.memory.getReg('r0');  // 42
const r1 = engine.context.memory.getReg('r1');  // 52
console.log(`r0 = ${r0}, r1 = ${r1}`);
```

### Tick-by-Tick Execution

```typescript
import * as ic10c from '@ic10/compiler';
import * as ic10r from '@ic10/runtime';

const source = `
    loop:
        move r0 1
        yield
        jal loop
`;

const tokens = ic10c.Lexer.tokenize(source);
const program = new ic10c.Parser(tokens).parse();
const analyser = new ic10c.Analyser();
await analyser.visit(program);

const engine = new ic10r.Engine(program, analyser.symbolTable, undefined, analyser.typeTable);

// Run one tick at a time
engine.runTick();  // executes one tick
engine.runTick();  // executes next tick
```

### Device Interaction

```typescript
import * as ic10c from '@ic10/compiler';
import * as ic10r from '@ic10/runtime';

const source = `
    alias led d0
    main:
        s led Setting 1
        hcf
`;

const tokens = ic10c.Lexer.tokenize(source);
const program = new ic10c.Parser(tokens).parse();
const analyser = new ic10c.Analyser();
await analyser.visit(program);

const engine = new ic10r.Engine(program, analyser.symbolTable, undefined, analyser.typeTable);

// Register an external device on port d0 (the runtime creates a virtual device for it),
// then take the handle back
engine.context.manager.setExternalDevice('d0', undefined as never);
const device = engine.context.manager.getDevice('d0');

engine.runFull();

// Read device logic after execution
console.log(device.readLogic('Setting'));  // 1
```

> How logic property names (`Setting`) and device references (`d0`, `db`, `dr0` and aliases) are
> resolved is described in "Device references and aliases" below.

## API Reference

### Classes

| Class | Description |
|:------|:------------|
| `Engine` | IC10 program execution engine |
| `Context` | Execution context (PC, memory, manager) |
| `Memory` | Register file and stack memory |
| `Manager` | Device manager |
| `Device` | Device I/O interface (logic, slots, reagents) |
| `IC10RuntimeLocal` | Message language of the runtime diagnostics |

### Config

```typescript
import type { Config } from '@ic10/runtime';

const config: Config = {
    tickDuration: 0.5,       // seconds per tick
    maxInstructions: 128,     // max instructions per tick
    maxStackSize: 512         // max stack size
};
```

### Engine

```typescript
import { Engine } from '@ic10/runtime';
import type { Program, SymbolTable, TypeTable } from '@ic10/compiler';

// Create engine with optional config and type table
const engine = new Engine(program, symbolTable, {
    tickDuration: 0.5,
    maxInstructions: 128
}, typeTable);

// Execute
engine.runTick();  // one tick
engine.runFull();  // until halt
engine.step();     // exactly one statement (true while the program can continue)

// Access context
const ctx = engine.context;
console.log(ctx.pc);            // program counter
console.log(ctx.halted);         // halted flag

// Diagnostics of everything the runtime could not execute
for (const d of engine.diagnostics)
    console.log(`${d.level} ${d.id} at line ${d.start.line}: ${d.message}`);
```

> **Type table**: the fourth argument is the type table produced by semantic analysis
> (`analyser.typeTable`). Evaluating enum constant operands (such as `Color.Green` in
> `s d0 Color Color.Green`) depends on it; without it such operands are reported as
> unevaluable (IEM2_1).

### Message language

```typescript
import { IC10RuntimeLocal } from '@ic10/runtime';

IC10RuntimeLocal.setLanguage('zh-hans');   // or 'en-us' (default)
```

### Device references and aliases

Device references are resolved by port name, matching the keys of `Manager`:

| Form | Resolution |
|:---|:---|
| `d0` … `d5` | `Manager::getDevice("d0")` … `("d5")` |
| `d0:1` | the pinned port name `"d0:1"` |
| `db` | the self reference (the device hosting the chip) |
| `dr0` / `drr0` | reads the register for the port number, then looks up `d0`…`d5` (`-1` means `db`) |
| `alias led d0` | a name alias, resolved through the symbol table to `d0` and then as above |

Write targets support aliases (`alias tmp r3` → `r3`) and dynamic registers (`rr0`: the register
number is held in `r0`) in exactly the same way.

### Supported operand forms

The executor accepts every operand form the v3 grammar allows and reports the rest as `IEM2_1`:

| Operand position | Accepted forms |
|:---|:---|
| Write target (`REG_TARGET`) | register (`r0`, `ra`, `sp`), dynamic register (`rr0`), alias of a register (`alias tmp r3`) |
| Device reference (`DEVICE_REF`) | `d0`…`d5`, pinned `d0:1`, `db`, dynamic port `dr0` / `drr0`, alias of a device |
| Device / name hash (`DEVICE_HASH`, `NAME_HASH`) | number, `$hex` / `%bin`, constant alias, register, `HASH("…")` |
| Write value of `s` / `sb` / `ss` / `sbn` / `sbs` | number, constant alias, register, enum constant |
| Logic property / slot property | identifier (`Pressure`, `On`), or a legacy number passed on as its decimal text |
| Aggregate / reagent mode | member name (`Average`, `Contents`), `Enum.Member`, number |
| Numeric value (`NUM_VALUE`) | number, `$hex` / `%bin`, register, constant alias, enum constant (`Color.Green`), `HASH("…")` / `STR("…")` |

Enum members are looked up in the type table: `Color.Green` uses its own enum type, while bare member
names (`Pressure`, `Sum`, `Contents`) are searched in `LogicType`, `LogicSlotType`, `BatchMode` and
`ReagentMode`, exactly like the compiler's operand checks do.

### Context

```typescript
const ctx = engine.context;

// Program counter
ctx.pc = 0;

// Memory access
const memory = ctx.memory;

// Device manager
const manager = ctx.manager;

// Execution control
ctx.halt();           // halt execution
ctx.sleep(1.0);       // sleep for 1 second
console.log(ctx.isSleeping);
```

### Memory

```typescript
const mem = engine.context.memory;

// Register access
mem.setReg('r0', 42);
const r0 = mem.getReg('r0');  // 42

// Stack operations
mem.push(100);
mem.push(200);
const top = mem.peek();  // 200
const val = mem.pop();  // 200

// Direct stack access
mem.setStack(0, 999);
const s0 = mem.getStack(0);  // 999

// Serialize
const json = mem.toJSON();
```

### Manager

```typescript
const mgr = engine.context.manager;

// Device registration — the binding creates and owns a virtual device per port, so register the
// port first and take its handle back afterwards (`setChipDevice` replaces the chip device)
mgr.setExternalDevice('d0', undefined as never);
const dev = mgr.getDevice('d0');
mgr.setChipDevice(undefined as never);
const chip = mgr.getDevice('db');

// Device lookup
const found = mgr.findDeviceByType(typeHash);
const byName = mgr.findDeviceByTypeAndName(typeHash, nameHash);
const all = mgr.findDevicesByType(typeHash);
```

### Device

```typescript
const dev = mgr.getDevice('d0');

// Logic properties
dev.writeLogic('Setting', 1);
const val = dev.readLogic('Setting');

// Device stack
dev.writeStack(0, 42);
const s0 = dev.readStack(0);

// Slots
dev.writeSlot(0, 'Occupied', 1);
const occ = dev.readSlot(0, 'Occupied');

// Reagents
const mode = dev.readReagent(0);
const amount = dev.queryReagentAmount(reagentHash);

// Metadata
const typeHash = dev.getTypeHash();
const nameHash = dev.getNameHash();

// Lifecycle
dev.tick();
dev.clearStack();
```

## IC10 Instruction Examples

### Arithmetic

```ic10
move r0 10
move r1 20
add r2 r0 r1    # r2 = 30
sub r3 r2 5     # r3 = 25
mul r4 r3 2     # r4 = 50
div r5 r4 5     # r5 = 10
```

### Control Flow

```ic10
loop:
    move r0 1
    yield
    jal loop
```

### Device I/O

```ic10
alias led d0
s led Setting 1
l r0 led Setting
```

### IC10 v3 operands

```ic10
alias tmp r3
define Level 3

alias led d0
move tmp Level          # write through an alias of r3
s led Setting Level     # constant alias as the write value
s led Color Color.Green # enum constant (needs the type table)

move r0 1
s dr0 On 1              # dynamic port: the port number is in r0

alias Hash r2
move Hash HASH("StructureDoor")
sb Hash On 1            # device hash taken from a register

lb r4 HASH("StructureDoor") Pressure Average   # bare aggregate mode member name
```

## TypeScript

This module includes complete TypeScript type definitions.

```typescript
import { Engine, Context, Memory, Manager, Device, IC10RuntimeLocal } from '@ic10/runtime';
import type { Config } from '@ic10/runtime';
import type { Program, SymbolTable, TypeTable, Diagnostic } from '@ic10/compiler';

const config: Config = {
    tickDuration: 0.5,
    maxInstructions: 128,
    maxStackSize: 512
};
```

## Build Instructions

### Requirements

- **Node.js**: 16.0.0+
- **CMake**: 3.28.1+
- **C++ Compiler**:
  - Linux: GCC 13+ or Clang 16+
  - Windows: MSVC 2022

### Build Steps

```bash
# Linux / macOS — configure, build the addon and run the Node.js tests
code/scripts/bashShell/buildIC10RuntimeNode.sh
```

```powershell
# Windows (PowerShell)
pwsh code/scripts/powerShell/BuildIC10RuntimeNode.ps1
```

Manually, with CMake:

```bash
cd code
cmake -B build -S . -DBUILD_IC10_RUNTIME_EXPORTS_NODE=ON
cmake --build build --target ic10_runtime_node --config Release

# The addon lands in the package together with its type declarations
# build/IC10/backend/runtime/exports/node/Release/ic10r-node.node
#   → IC10/backend/runtime/publish/node/src/ic10r-node.node
```

### Tests

```bash
cd code/IC10/backend/runtime
pnpm test        # jest — binding-level tests (tests/node)
```

The C++ core tests are GoogleTest targets (`ic10_runtime_tests`) built through CMake; run them with
`ctest` or the target's executable.

## Project Structure

```
@ic10/runtime/
├── src/
│   └── ic10r-node.node    # Native module (built)
├── types/
│   ├── index.d.ts          # TypeScript type definitions (entry)
│   ├── config.d.ts         # Config interface
│   ├── context.d.ts        # Context class
│   ├── device.d.ts         # Device class
│   ├── engine.d.ts         # Engine class
│   ├── locale.d.ts         # IC10RuntimeLocal class
│   ├── manager.d.ts        # Manager class
│   ├── memory.d.ts         # Memory class
│   └── value.d.ts          # HASH / STR / constant helpers
├── CHANGELOG.md
├── CHANGELOG.zh.md
├── tsconfig.json
├── package.json
├── README.md
└── README.zh.md
```

## License

This project is licensed under **CC BY-NC-SA 4.0** (Creative Commons Attribution-NonCommercial-ShareAlike 4.0).

[![License: CC BY-NC-SA](https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Contact

- **Author**: edocsitahw
- **Email**: edocsitahw@qq.com
- **Repository**: [https://github.com/edoCsItahW/Stationeers](https://github.com/edoCsItahW/Stationeers)

## Related Links

- [@ic10/compiler – IC10 Compiler Node.js Bindings](https://www.npmjs.com/package/@ic10/compiler) — the package that produces the AST, symbol table and type table this runtime consumes
- [Stationeers Official Website](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API Documentation](https://nodejs.org/api/n-api.html)
- [node-addon-api Documentation](https://github.com/nodejs/node-addon-api)
