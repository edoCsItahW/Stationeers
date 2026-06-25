# ic10-node-api

[![npm 版本](https://badge.fury.io/js/ic10-node-api.svg)](https://badge.fury.io/js/ic10-node-api)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![许可证: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[English](README.md)

## 简介

`ic10-node-api` 是 **IC10 编译器** 的 Node.js 原生绑定模块，提供了对 IC10 汇编语言代码的词法分析、语法分析和语义分析能力。

IC10 是一种用于 [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) 游戏的汇编式编程语言，用于控制游戏中的计算机和设备。本模块允许开发者在 Node.js 环境中使用 IC10 编译器的全部核心功能。

## 特性

- **词法分析 (Lexer)** - 词符化 IC10 源代码，支持寄存器、设备号、数字、字符串、注释和关键字
- **语法分析 (Parser)** - 构建抽象语法树 (AST)，覆盖所有 IC10 指令（一元至六元）
- **语义分析 (Analyser)** - 执行符号解析和类型检查，基于 `Promise`/`Future` 的异步符号表
- **错误报告** - 丰富的错误信息，包含源代码位置，支持国际化（中/英）
- **异步协程基础设施** - 自定义的 `Task<T>`、`Promise<T>`、`Future<T>` 和协程状态管理
- **跨平台** - 支持 Linux (GCC/Clang) 和 Windows (MSVC)

## 安装

### 前提条件

- Node.js >= 16.0.0（推荐 Node.js 26.x）
- C++ 编译器 (GCC 13+ / Clang 16+ / MSVC 2022)
- CMake >= 3.28.1

### 从 npm 安装

```bash
npm install ic10-node-api
```

### 从源码构建

```bash
# 克隆仓库
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers/code/backend/compiler

# 安装依赖
npm install

# 下载 Node.js 头文件
npx node-gyp install

# 构建原生模块
npm run build
```

## 快速开始

### 基本用法

```typescript
import * as ic10 from 'ic10-node-api';

// 设置语言（可选，默认英文）
ic10.IC10Local.setLanguage('zh-hans');

// IC10 源代码
const source = `
    alias disp d0
    main:
        move r0 42
        yield
        jal main
`;

// 1. 词法分析
const tokens = ic10.Lexer.tokenize(source);
console.log(`生成了 ${tokens.length} 个 Token`);

// 2. 语法分析
const parser = new ic10.Parser(tokens);
const program = parser.parse();

// 3. 语义分析
const analyser = new ic10.Analyser();
await analyser.visit(program);

// 4. 获取结果
const symbolTable = analyser.symbolTable;
console.log(symbolTable.toJSON());
```

### 使用静态方法

```typescript
import { Lexer, Parser, Analyser } from 'ic10-node-api';

// 词法分析
const tokens = Lexer.tokenize('add r0 r1 r2');

// 语法分析
const program = Parser.parse(tokens);

// 访问 AST
const json = JSON.parse(program.toJSON());
console.log(json.statements[0].type); // "addInstruction"
```

## API 文档

### 类

| 类 | 说明 |
|:---|:---|
| `IC10Local` | 本地化设置 |
| `Pos` | 位置信息 |
| `Token` | 词法标记 |
| `Lexer` | 词法分析器 |
| `Program` | AST 根节点 |
| `Parser` | 语法分析器 |
| `Analyser` | 语义分析器 |
| `SymbolTable` | 符号表（存储变量和标签） |

### IC10Local

```typescript
import { IC10Local } from 'ic10-node-api';

// 设置语言
IC10Local.setLanguage('zh-hans');  // 简体中文
IC10Local.setLanguage('en-us');    // 英文

// 获取当前语言
const lang = IC10Local.getLanguage();
```

### Lexer

```typescript
import { Lexer } from 'ic10-node-api';

// 静态方法
const tokens = Lexer.tokenize('alias ic d0');

// 实例方法
const lexer = new Lexer('move r0 42');
const result = lexer.scan();
```

### Parser

```typescript
import { Lexer, Parser } from 'ic10-node-api';

const tokens = Lexer.tokenize(source);
const parser = new Parser(tokens);
const program = parser.parse();

console.log('语句数量:', program.statements.length);
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
console.log('符号表:', symbolTable.toJSON());
```

## IC10 指令示例

### 一元指令 (Nullary)

```ic10
yield
sleep
pause
break
```

### 二元指令 (Binary)

```ic10
move r0 r1
not r0 r1
```

### 三元指令 (Ternary)

```ic10
add r0 r1 r2
sub r0 r1 r2
```

## 错误处理

```typescript
import { Lexer, Parser } from 'ic10-node-api';

const source = 'move r0';  // 缺少操作数
const tokens = Lexer.tokenize(source);
const program = Parser.parse(tokens);

// 检查错误
if (program.errors && program.errors.length > 0) {
    for (const err of program.errors) {
        console.log(`错误: ${err.message}`);
        console.log(`位置: 行 ${err.pos.line}, 列 ${err.pos.column}`);
    }
}
```

## TypeScript

本模块附带完整的 TypeScript 类型定义，无需额外安装 `@types` 包。

```typescript
import * as ic10 from 'ic10-node-api';

// 完全类型安全
const token: ic10.Token = new ic10.Token(1, new ic10.Pos(), 'a', 3);
```

## 构建说明

### 环境要求

- **Node.js**: 16.0.0+
- **CMake**: 3.28.1+
- **C++ 编译器**:
  - Linux: GCC 13+ 或 Clang 16+
  - Windows: MSVC 2022

### 构建步骤

```bash
# 1. 安装 Node.js 依赖
npm install

# 2. 下载 Node.js 头文件
npx node-gyp install

# 3. 配置 CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 4. 编译
cmake --build build --parallel 4

# 5. 复制生成的 .node 文件
cp build/ic10-node-api.node src/
```

## 项目结构

```
ic10-node-api/
├── src/
│   ├── index.d.ts          # TypeScript 类型定义
│   └── ic10-node-api.node  # 原生模块（编译生成）
├── include/                 # C++ 头文件
├── test/                    # 测试文件
├── package.json
└── README.md
```

## 许可证

本项目采用 **CC BY-NC-SA 4.0** (Creative Commons Attribution-NonCommercial-ShareAlike 4.0) 许可证。

[![License: CC BY-NC-SA](https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

## 贡献

欢迎提交 Issue 和 Pull Request！

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

## 联系方式

- **作者**: Xiao Songtao
- **邮箱**: 2207150234@st.sziit.edu.cn
- **仓库**: [https://github.com/edoCsItahW/Stationeers](https://github.com/edoCsItahW/Stationeers)

## 相关链接

- [IC10 编译器核心文档](https://github.com/edoCsItahW/Stationeers)
- [Stationeers 官方网站](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API 文档](https://nodejs.org/api/n-api.html)
- [node-addon-api 文档](https://github.com/nodejs/node-addon-api)
