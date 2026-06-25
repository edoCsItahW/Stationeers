# ic10-node-api

[![npm version](https://badge.fury.io/js/ic10-node-api.svg)](https://badge.fury.io/js/ic10-node-api)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![License: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[中文](README.zh.md)

## Overview

`ic10-node-api` is the Node.js native binding module for the **IC10 Compiler**, providing lexical analysis, syntax analysis, and semantic analysis capabilities for IC10 assembly language code.

IC10 is an assembly-style programming language used in the game [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) to control computers and devices in the game. This module allows developers to use all core features of the IC10 compiler in the Node.js environment.

## Features

- **Lexer** – tokenizes IC10 source code (supports registers, devices, numbers, strings, comments, keywords)
- **Parser** – builds an Abstract Syntax Tree (AST) for all IC10 instructions (nullary to senary)
- **Semantic Analyser** – performs symbol resolution and type checking using a `Promise`/`Future` based asynchronous symbol table
- **Error Reporting** – rich error messages with source location, internationalization support (English / Simplified Chinese)
- **Async Coroutine Infrastructure** – custom `Task<T>`, `Promise<T>`, `Future<T>` and coroutine state management
- **Cross‑Platform** – builds on Linux (GCC/Clang) and Windows (MSVC)

## Installation

### Prerequisites

- Node.js >= 16.0.0 (Node.js 26.x recommended)
- C++ compiler (GCC 13+ / Clang 16+ / MSVC 2022)
- CMake >= 3.28.1

### Install from npm

```bash
npm install ic10-node-api
```

### Build from Source

```bash
# Clone repository
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers/code/backend/compiler

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
import * as ic10 from 'ic10-node-api';

// Set language (optional, default English)
ic10.IC10Local.setLanguage('zh-hans');

// IC10 source code
const source = `
    alias disp d0
    main:
        move r0 42
        yield
        jal main
`;

// 1. Lexical analysis
const tokens = ic10.Lexer.tokenize(source);
console.log(`Generated ${tokens.length} Tokens`);

// 2. Syntax analysis
const parser = new ic10.Parser(tokens);
const program = parser.parse();

// 3. Semantic analysis
const analyser = new ic10.Analyser();
await analyser.visit(program);

// 4. Get results
const symbolTable = analyser.symbolTable;
console.log(symbolTable.toJSON());
```

### Using Static Methods

```typescript
import { Lexer, Parser, Analyser } from 'ic10-node-api';

// Lexical analysis
const tokens = Lexer.tokenize('add r0 r1 r2');

// Syntax analysis
const program = Parser.parse(tokens);

// Access AST
const json = JSON.parse(program.toJSON());
console.log(json.statements[0].type); // "addInstruction"
```

## API Reference

### Classes

| Class | Description |
|:------|:------------|
| `IC10Local` | Localization settings |
| `Pos` | Position information |
| `Token` | Lexical token |
| `Lexer` | Lexical analyzer |
| `Program` | AST root node |
| `Parser` | Syntax analyzer |
| `Analyser` | Semantic analyzer |
| `SymbolTable` | Symbol table for variables and labels |

### IC10Local

```typescript
import { IC10Local } from 'ic10-node-api';

// Set language
IC10Local.setLanguage('zh-hans');  // Simplified Chinese
IC10Local.setLanguage('en-us');    // English

// Get current language
const lang = IC10Local.getLanguage();
```

### Lexer

```typescript
import { Lexer } from 'ic10-node-api';

// Static method
const tokens = Lexer.tokenize('alias ic d0');

// Instance method
const lexer = new Lexer('move r0 42');
const result = lexer.scan();
```

### Parser

```typescript
import { Lexer, Parser } from 'ic10-node-api';

const tokens = Lexer.tokenize(source);
const parser = new Parser(tokens);
const program = parser.parse();

console.log('Statement count:', program.statements.length);
console.log('AST:', program.toJSON());
```

### Analyser

```typescript
import { Lexer, Parser, Analyser } from 'ic10-node-api';

const tokens = Lexer.tokenize(source);
const program = Parser.parse(tokens);
const analyser = new Analyser();

await analyser.visit(program);

const symbolTable = analyser.symbolTable;
console.log('Symbol table:', symbolTable.toJSON());
```

## IC10 Instruction Examples

### Nullary Instructions

```ic10
yield
sleep
pause
break
```

### Binary Instructions

```ic10
move r0 r1
not r0 r1
```

### Ternary Instructions

```ic10
add r0 r1 r2
sub r0 r1 r2
```

## Error Handling

```typescript
import { Lexer, Parser } from 'ic10-node-api';

const source = 'move r0';  // Missing operand
const tokens = Lexer.tokenize(source);
const program = Parser.parse(tokens);

// Check errors
if (program.errors && program.errors.length > 0) {
    for (const err of program.errors) {
        console.log(`Error: ${err.message}`);
        console.log(`Position: Line ${err.pos.line}, Column ${err.pos.column}`);
    }
}
```

## TypeScript

This module includes complete TypeScript type definitions.

```typescript
import * as ic10 from 'ic10-node-api';

// Fully type-safe
const token: ic10.Token = new ic10.Token(1, new ic10.Pos(), 'a', 3);
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
cp build/ic10-node-api.node src/
```

## Project Structure

```
ic10-node-api/
├── src/
│   ├── index.d.ts          # TypeScript type definitions
│   └── ic10-node-api.node  # Native module (built)
├── include/                 # C++ headers
├── test/                    # Test files
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

- **Author**: Xiao Songtao
- **Email**: 2207150234@st.sziit.edu.cn
- **Repository**: [https://github.com/edoCsItahW/Stationeers](https://github.com/edoCsItahW/Stationeers)

## Related Links

- [IC10 Compiler Core Documentation](https://github.com/edoCsItahW/Stationeers)
- [Stationeers Official Website](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API Documentation](https://nodejs.org/api/n-api.html)
- [node-addon-api Documentation](https://github.com/nodejs/node-addon-api)
