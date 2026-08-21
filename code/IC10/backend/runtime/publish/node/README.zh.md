# ic10r_node

[![npm 版本](https://badge.fury.io/js/ic10r-node.svg)](https://badge.fury.io/js/ic10r_node)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![许可证: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[English](README.md)

## 简介

`ic10r-node` 是 **IC10 运行时** 的 Node.js 原生绑定模块，提供了完整的 IC10 程序执行引擎。

IC10 是一种用于 [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) 游戏的汇编式编程语言，用于控制游戏中的计算机和设备。本模块接收编译后的 AST 和符号表（由 [`ic10c-node`](https://www.npmjs.com/package/ic10c-node) 生成），逐 tick 执行程序，模拟 IC10 处理器的行为。

## 特性

- **执行引擎 (Engine)** - 逐 tick 执行（`runTick`）或一次性执行到结束（`runFull`）
- **寄存器文件和栈** - 完整的 16 寄存器文件（`r0`–`r15`）和栈内存（`push`/`pop`/`peek`/`poke`）
- **设备管理器 (Manager)** - 注册外部设备和芯片设备，按类型/名称哈希查询
- **设备 I/O** - 读写逻辑属性、设备栈、插槽和试剂模式
- **执行控制** - 支持 `halt`/`sleep`，可配置指令上限和 tick 时长
- **跨平台** - 支持 Linux (GCC/Clang) 和 Windows (MSVC)

## 安装

### 前提条件

- Node.js >= 16.0.0（推荐 Node.js 26.x）
- C++ 编译器 (GCC 13+ / Clang 16+ / MSVC 2022)
- CMake >= 3.28.1
- [`ic10c-node`](https://www.npmjs.com/package/ic10c-node)（编译阶段的依赖）

### 从 npm 安装

```bash
npm install ic10r-node ic10c-node
```

### 从源码构建

```bash
# 克隆仓库
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers/code/IC10/backend/runtime

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
import * as ic10c from 'ic10c-node';
import * as ic10r from 'ic10r-node';

// IC10 源代码
const source = `
    main:
        move r0 42
        add r1 r0 10
        hcf
`;

// 1. 编译（使用 ic10c-node）
const tokens = ic10c.Lexer.tokenize(source);
const program = new ic10c.Parser(tokens).parse();
const analyser = new ic10c.Analyser();
await analyser.visit(program);

// 2. 创建引擎（使用 ic10r-node）
const engine = new ic10r.Engine(program, analyser.symbolTable);

// 3. 执行
engine.runFull();

// 4. 查看结果
const r0 = engine.context.memory.getReg('r0');  // 42
const r1 = engine.context.memory.getReg('r1');  // 52
console.log(`r0 = ${r0}, r1 = ${r1}`);
```

### 逐 Tick 执行

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

// 逐 tick 执行
engine.runTick();  // 执行一个 tick
engine.runTick();  // 执行下一个 tick
```

### 设备交互

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

// 注册外部设备
const device = engine.context.manager.getDevice('d0');
engine.context.manager.setExternalDevice('d0', device);

engine.runFull();

// 执行后读取设备逻辑
console.log(device.readLogic('Setting'));  // 1
```

## API 文档

### 类

| 类 | 说明 |
|:---|:---|
| `Engine` | IC10 程序执行引擎 |
| `Context` | 执行上下文（PC、内存、管理器） |
| `Memory` | 寄存器文件和栈内存 |
| `Manager` | 设备管理器 |
| `Device` | 设备 I/O 接口（逻辑、插槽、试剂） |

### Config

```typescript
import type { Config } from 'ic10r-node';

const config: Config = {
    tickDuration: 0.5,       // 每 tick 秒数
    maxInstructions: 128,     // 每 tick 最大指令数
    maxStackSize: 512         // 最大栈大小
};
```

### Engine

```typescript
import { Engine } from 'ic10r-node';
import type { Program, SymbolTable } from 'ic10c-node';

// 创建引擎，可选配置
const engine = new Engine(program, symbolTable, {
    tickDuration: 0.5,
    maxInstructions: 128
});

// 执行
engine.runTick();  // 一个 tick
engine.runFull();  // 执行到 halt

// 访问上下文
const ctx = engine.context;
console.log(ctx.pc);            // 程序计数器
console.log(ctx.halted);         // 停机标志
```

### Context

```typescript
const ctx = engine.context;

// 程序计数器
ctx.pc = 0;

// 内存访问
const memory = ctx.memory;

// 设备管理器
const manager = ctx.manager;

// 执行控制
ctx.halt();           // 停止执行
ctx.sleep(1.0);       // 休眠 1 秒
console.log(ctx.isSleeping);
```

### Memory

```typescript
const mem = engine.context.memory;

// 寄存器访问
mem.setReg('r0', 42);
const r0 = mem.getReg('r0');  // 42

// 栈操作
mem.push(100);
mem.push(200);
const top = mem.peek();  // 200
const val = mem.pop();  // 200

// 直接栈访问
mem.setStack(0, 999);
const s0 = mem.getStack(0);  // 999

// 序列化
const json = mem.toJSON();
```

### Manager

```typescript
const mgr = engine.context.manager;

// 设备注册
mgr.setExternalDevice('d0', device);
mgr.setChipDevice(chipDevice);

// 设备查找
const dev = mgr.getDevice('d0');
const found = mgr.findDeviceByType(typeHash);
const all = mgr.findDevicesByType(typeHash);
```

### Device

```typescript
const dev = mgr.getDevice('d0');

// 逻辑属性
dev.writeLogic('Setting', 1);
const val = dev.readLogic('Setting');

// 设备栈
dev.writeStack(0, 42);
const s0 = dev.readStack(0);

// 插槽
dev.writeSlot(0, 'Occupied', 1);
const occ = dev.readSlot(0, 'Occupied');

// 试剂
const mode = dev.readReagent(0);
const amount = dev.queryReagentAmount(reagentHash);

// 元数据
const typeHash = dev.getTypeHash();
const nameHash = dev.getNameHash();

// 生命周期
dev.tick();
dev.clearStack();
```

## IC10 指令示例

### 算术运算

```ic10
move r0 10
move r1 20
add r2 r0 r1    # r2 = 30
sub r3 r2 5     # r3 = 25
mul r4 r3 2     # r4 = 50
div r5 r4 5     # r5 = 10
```

### 控制流

```ic10
loop:
    move r0 1
    yield
    jal loop
```

### 设备 I/O

```ic10
alias led d0
s led Setting 1
r r0 led Setting
```

## TypeScript

本模块附带完整的 TypeScript 类型定义，无需额外安装 `@types` 包。

```typescript
import { Engine, Context, Memory, Manager, Device } from 'ic10r-node';
import type { Config } from 'ic10r-node';

const config: Config = {
    tickDuration: 0.5,
    maxInstructions: 128,
    maxStackSize: 512
};
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
cp build/ic10r-node.node src/
```

## 项目结构

```
ic10r-node/
├── src/
│   └── ic10r-node.node   # 原生模块（编译生成）
├── types/
│   ├── index.d.ts         # TypeScript 类型定义（入口）
│   ├── config.d.ts         # Config 接口
│   ├── context.d.ts        # Context 类
│   ├── device.d.ts         # Device 类
│   ├── engine.d.ts         # Engine 类
│   ├── manager.d.ts        # Manager 类
│   └── memory.d.ts         # Memory 类
├── tsconfig.json
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

- **作者**: edocsitahw
- **邮箱**: edocsitahw@qq.com
- **仓库**: [https://github.com/edoCsItahW/Stationeers](https://github.com/edoCsItahW/Stationeers)

## 相关链接

- [ic10c-node – IC10 编译器 Node.js 绑定](https://www.npmjs.com/package/ic10c-node)
- [Stationeers 官方网站](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API 文档](https://nodejs.org/api/n-api.html)
- [node-addon-api 文档](https://github.com/nodejs/node-addon-api)
