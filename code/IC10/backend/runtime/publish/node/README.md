# ic10r-node

[![npm version](https://badge.fury.io/js/ic10r_node.svg)](https://badge.fury.io/js/ic10r_node)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![License: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[中文](./README.zh.md)

## Overview

`ic10r-node` is the Node.js native binding module for the **IC10 Runtime**, providing a complete execution engine for IC10 programs.

IC10 is an assembly-style programming language used in the game [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) to control computers and devices in the game. This module takes the compiled AST and symbol table (produced by [`ic10c-node`](https://www.npmjs.com/package/ic10c-node)) and executes the program tick-by-tick, simulating the IC10 processor's behavior.

## Features

- **Execution Engine** – runs IC10 programs tick-by-tick (`runTick`) or to completion (`runFull`)
- **Register File & Stack** – full 16-register file (`r0`–`r15`) and stack memory (`push`/`pop`/`peek`/`poke`)
- **Device Manager** – register external devices and the chip device, query by type/name hash
- **Device I/O** – read/write logic properties, device stacks, slots, and reagent modes
- **Execution Control** – `halt`/`sleep` support, instruction limit, tick duration configuration
- **Cross‑Platform** – builds on Linux (GCC/Clang) and Windows (MSVC)

## Installation

### Prerequisites

- Node.js >= 16.0.0 (Node.js 26.x recommended)
- C++ compiler (GCC 13+ / Clang 16+ / MSVC 2022)
- CMake >= 3.28.1
- [`ic10c-node`](https://www.npmjs.com/package/ic10c-node) (peer dependency for compilation)

### Install from npm

```bash
npm install ic10r-node ic10c-node
```

### Build from Source

```bash
# Clone repository
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers/code/IC10/backend/runtime

# Install dependencies
npm install

# Download Node.js headers
npx node-gyp install

# Build native module
npm run build
```

## Quick Start

### Basic Usage

```typescript
import * as ic10c from 'ic10c-node';
import * as ic10r from 'ic10r-node';

// IC10 source code
const source = `
    main:
        move r0 42
        add r1 r0 10
        hcf
`;

// 1. Compile (using ic10c-node)
const tokens = ic10c.Lexer.tokenize(source);
const program = new ic10c.Parser(tokens).parse();
const analyser = new ic10c.Analyser();
await analyser.visit(program);

// 2. Create engine (using ic10r-node)
const engine = new ic10r.Engine(program, analyser.symbolTable);

// 3. Execute
engine.runFull();

// 4. Inspect results
const r0 = engine.context.memory.getReg('r0');  // 42
const r1 = engine.context.memory.getReg('r1');  // 52
console.log(`r0 = ${r0}, r1 = ${r1}`);
```

### Tick-by-Tick Execution

```typescript
import * as ic10c from 'ic10c-node';
import * as ic10r from 'ic10r-node';

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

const engine = new ic10r.Engine(program, analyser.symbolTable);

// Run one tick at a time
engine.runTick();  // executes one tick
engine.runTick();  // executes next tick
```

### Device Interaction

```typescript
import * as ic10c from 'ic10c-node';
import * as ic10r from 'ic10r-node';

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

const engine = new ic10r.Engine(program, analyser.symbolTable);

// Register an external device
const device = engine.context.manager.getDevice('d0');
engine.context.manager.setExternalDevice('d0', device);

engine.runFull();

// Read device logic after execution
console.log(device.readLogic('Setting'));  // 1
```

## API Reference

### Classes

| Class | Description |
|:------|:------------|
| `Engine` | IC10 program execution engine |
| `Context` | Execution context (PC, memory, manager) |
| `Memory` | Register file and stack memory |
| `Manager` | Device manager |
| `Device` | Device I/O interface (logic, slots, reagents) |

### Config

```typescript
import type { Config } from 'ic10r-node';

const config: Config = {
    tickDuration: 0.5,       // seconds per tick
    maxInstructions: 128,     // max instructions per tick
    maxStackSize: 512         // max stack size
};
```

### Engine

```typescript
import { Engine } from 'ic10r-node';
import type { Program, SymbolTable } from 'ic10c-node';

// Create engine with optional config
const engine = new Engine(program, symbolTable, {
    tickDuration: 0.5,
    maxInstructions: 128
});

// Execute
engine.runTick();  // one tick
engine.runFull();  // until halt

// Access context
const ctx = engine.context;
console.log(ctx.pc);            // program counter
console.log(ctx.halted);         // halted flag
```

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

// Device registration
mgr.setExternalDevice('d0', device);
mgr.setChipDevice(chipDevice);

// Device lookup
const dev = mgr.getDevice('d0');
const found = mgr.findDeviceByType(typeHash);
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
r r0 led Setting
```

## TypeScript

This module includes complete TypeScript type definitions.

```typescript
import { Engine, Context, Memory, Manager, Device } from 'ic10r-node';
import type { Config } from 'ic10r-node';

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
# 1. Install Node.js dependencies
npm install

# 2. Download Node.js headers
npx node-gyp install

# 3. Configure CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 4. Build
cmake --build build --parallel 4

# 5. Copy generated .node file
cp build/ic10r-node.node src/
```

## Project Structure

```
ic10r-node/
├── src/
│   └── ic10r-node.node   # Native module (built)
├── types/
│   ├── index.d.ts         # TypeScript type definitions (entry)
│   ├── config.d.ts         # Config interface
│   ├── context.d.ts        # Context class
│   ├── device.d.ts         # Device class
│   ├── engine.d.ts         # Engine class
│   ├── manager.d.ts        # Manager class
│   └── memory.d.ts         # Memory class
├── tsconfig.json
├── package.json
└── README.md
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

- [ic10c-node – IC10 Compiler Node.js Bindings](https://www.npmjs.com/package/ic10c-node)
- [Stationeers Official Website](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API Documentation](https://nodejs.org/api/n-api.html)
- [node-addon-api Documentation](https://github.com/nodejs/node-addon-api)
