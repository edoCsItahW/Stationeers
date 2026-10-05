# @ic10/runtime

[![npm 版本](https://badge.fury.io/js/@ic10%2Fruntime.svg)](https://www.npmjs.com/package/@ic10/runtime)
[![Node.js](https://img.shields.io/badge/node-%3E%3D16.0.0-brightgreen)](https://nodejs.org/)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)](https://isocpp.org/)
[![许可证: CC BY-NC-SA](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

[English](README.md)

## 简介

`@ic10/runtime` 是 **IC10 运行时** 的 Node.js 原生扩展：它接收 [IC10 编译器](https://www.npmjs.com/package/@ic10/compiler)（`@ic10/compiler`）产出的 AST 与符号表，逐 tick 执行程序，模拟 IC10 处理器的行为。

IC10 是一种用于 [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) 游戏的汇编式编程语言，用于控制游戏中的计算机和设备。本模块把编译结果放进寄存器文件、栈和设备管理器中执行，安装时无需 C++ 工具链（原生模块随包发布）。

当前版本：**2.0.0**（IC10 v3 —— 执行器对应统一按元数生成的 AST 与放宽后的操作数语法）。详见
[CHANGELOG.zh.md](./CHANGELOG.zh.md) / [CHANGELOG.md](./CHANGELOG.md)。

## 特性

- **执行引擎 (Engine)** - 逐 tick 执行（`runTick`）、一次执行到结束（`runFull`）或单步执行（`step`），无法执行的部分一律作为诊断上报
- **寄存器文件和栈** - 完整的 16 寄存器文件（`r0`–`r15`）和栈内存（`push`/`pop`/`peek`/`poke`）
- **设备管理器 (Manager)** - 注册外部设备和芯片设备，按类型/名称哈希查询
- **设备 I/O** - 读写逻辑属性、设备栈、插槽和试剂模式
- **IC10 v3 操作数** - 别名写入目标、设备别名、动态寄存器（`rr0`）、动态端口（`dr0` / `drr0`）、寄存器传递的设备/名称哈希，以及按类型表求值的枚举操作数
- **执行控制** - 支持 `halt`/`sleep`，可配置指令上限和 tick 时长
- **消息语言** - 诊断可使用英文（`en-us`）或简体中文（`zh-hans`）
- **完整的 TypeScript 类型** - 随包发布，无需额外的 `@types/*`
- **跨平台** - 支持 Linux (GCC/Clang) 和 Windows (MSVC)

## 安装

### 从 npm 安装

```bash
npm install @ic10/runtime @ic10/compiler
```

发布包中已包含编译好的 `src/ic10r-node.node`，安装**不需要** C++ 编译器、CMake 或 node-gyp。要求 Node.js >= 16；编译器包是同级依赖，因为程序必须先编译才能执行。

### 从源码构建

构建属于整个仓库构建的一部分，完整工具链见
[DEVELOPER_GUIDE.md](https://github.com/edoCsItahW/Stationeers/blob/main/DEVELOPER_GUIDE.md)。简而言之需要 CMake >= 3.28.1 与支持 C++23 的编译器（GCC 13+ / Clang 16+ / MSVC 2022）。

```bash
# Linux / macOS —— 配置 CMake、构建原生扩展并运行 Node.js 测试
code/scripts/bashShell/buildIC10RuntimeNode.sh
```

```powershell
# Windows (PowerShell)
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
pwsh code/scripts/powerShell/BuildIC10RuntimeNode.ps1
```

两个脚本都会构建 CMake 目标 `ic10_runtime_node`，并把产物复制到
`code/IC10/backend/runtime/publish/node/src/ic10r-node.node`。若想自己驱动 CMake：

```bash
cd code
cmake -B build -S . -DBUILD_IC10_RUNTIME_EXPORTS_NODE=ON
cmake --build build --target ic10_runtime_node --config Release
```

## 快速开始

### 基本用法

```typescript
import * as ic10c from '@ic10/compiler';
import * as ic10r from '@ic10/runtime';

// IC10 源代码
const source = `
    main:
        move r0 42
        add r1 r0 10
        hcf
`;

// 1. 编译（使用 @ic10/compiler）
const tokens = ic10c.Lexer.tokenize(source);
const program = new ic10c.Parser(tokens).parse();
const analyser = new ic10c.Analyser();
await analyser.visit(program);

// 2. 创建引擎（使用 @ic10/runtime）；类型表用于枚举操作数求值
const engine = new ic10r.Engine(program, analyser.symbolTable, undefined, analyser.typeTable);

// 3. 执行
engine.runFull();

// 4. 查看结果
const r0 = engine.context.memory.getReg('r0');  // 42
const r1 = engine.context.memory.getReg('r1');  // 52
console.log(`r0 = ${r0}, r1 = ${r1}`);
```

### 逐 Tick 执行

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

// 逐 tick 执行
engine.runTick();  // 执行一个 tick
engine.runTick();  // 执行下一个 tick
```

### 设备交互

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

// 在端口 d0 上注册外部设备（运行时为该端口创建虚拟设备），再取回句柄
engine.context.manager.setExternalDevice('d0', undefined as never);
const device = engine.context.manager.getDevice('d0');

engine.runFull();

// 执行后读取设备逻辑
console.log(device.readLogic('Setting'));  // 1
```

> 逻辑属性名（`Setting`）与设备引用（`d0`、`db`、`dr0` 及别名）的解析规则见下文「设备引用与别名」。

## API 文档

### 类

| 类 | 说明 |
|:---|:---|
| `Engine` | IC10 程序执行引擎 |
| `Context` | 执行上下文（PC、内存、管理器） |
| `Memory` | 寄存器文件和栈内存 |
| `Manager` | 设备管理器 |
| `Device` | 设备 I/O 接口（逻辑、插槽、试剂） |
| `IC10RuntimeLocal` | 运行时诊断的消息语言 |

### Config

```typescript
import type { Config } from '@ic10/runtime';

const config: Config = {
    tickDuration: 0.5,       // 每 tick 秒数
    maxInstructions: 128,     // 每 tick 最大指令数
    maxStackSize: 512         // 最大栈大小
};
```

### Engine

```typescript
import { Engine } from '@ic10/runtime';
import type { Program, SymbolTable, TypeTable } from '@ic10/compiler';

// 创建引擎，可选配置与类型表
const engine = new Engine(program, symbolTable, {
    tickDuration: 0.5,
    maxInstructions: 128
}, typeTable);

// 执行
engine.runTick();  // 一个 tick
engine.runFull();  // 执行到 halt
engine.step();     // 恰好一条语句（返回 true 表示仍可继续）

// 访问上下文
const ctx = engine.context;
console.log(ctx.pc);            // 程序计数器
console.log(ctx.halted);         // 停机标志

// 读取运行时无法执行的部分
for (const d of engine.diagnostics)
    console.log(`${d.level} ${d.id} 第 ${d.start.line} 行：${d.message}`);
```

> **类型表**：第 4 个参数是语义分析产出的类型表（`analyser.typeTable`），
> 枚举常量操作数（如 `s d0 Color Color.Green` 中的 `Color.Green`）的求值依赖它；
> 不传时这类操作数会被上报为无法求值（IEM2_1）。

### 消息语言

```typescript
import { IC10RuntimeLocal } from '@ic10/runtime';

IC10RuntimeLocal.setLanguage('zh-hans');   // 或 'en-us'（默认）
```

### 设备引用与别名

运行时按端口名解析设备引用，与 `Manager` 的键一致：

| 写法 | 解析结果 |
|:---|:---|
| `d0` … `d5` | `Manager::getDevice("d0")` … `("d5")` |
| `d0:1` | 端口名含引脚的 `"d0:1"` |
| `db` | 自身引用（芯片所在设备） |
| `dr0` / `drr0` | 读寄存器取端口号后按 `d0`…`d5` 查找（`-1` 为 `db`） |
| `alias led d0` | 名称别名，经符号表解析为 `d0` 后同上 |

写入目标同理支持别名（`alias tmp r3` → `r3`）与动态寄存器（`rr0`：寄存器编号存于 `r0`）。

### 操作数支持范围

执行器接受 v3 语法允许的全部操作数形态，其余一律上报 `IEM2_1`：

| 操作数位置 | 可用形态 |
|:---|:---|
| 写入目标（`REG_TARGET`） | 寄存器（`r0`、`ra`、`sp`）、动态寄存器（`rr0`）、寄存器别名（`alias tmp r3`） |
| 设备引用（`DEVICE_REF`） | `d0`…`d5`、带引脚的 `d0:1`、`db`、动态端口 `dr0` / `drr0`、设备别名 |
| 设备 / 名称哈希（`DEVICE_HASH`、`NAME_HASH`） | 数字、`$`十六进制 / `%`二进制、常量别名、寄存器、`HASH("…")` |
| `s` / `sb` / `ss` / `sbn` / `sbs` 的写入值 | 数字、常量别名、寄存器、枚举常量 |
| 逻辑属性 / 插槽属性 | 标识符（`Pressure`、`On`），或旧语法的数字（按十进制文本传递） |
| 聚合模式 / 试剂模式 | 成员名（`Average`、`Contents`）、`Enum.Member`、数字 |
| 数值（`NUM_VALUE`） | 数字、`$`十六进制 / `%`二进制、寄存器、常量别名、枚举常量（`Color.Green`）、`HASH("…")` / `STR("…")` |

枚举成员从类型表中查找：`Color.Green` 使用它自己的枚举类型，而裸成员名（`Pressure`、`Sum`、
`Contents`）按 `LogicType`、`LogicSlotType`、`BatchMode`、`ReagentMode` 的顺序查找，与编译器的
操作数检查保持一致。

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

// 设备注册 —— 绑定层为每个端口创建并持有虚拟设备，
// 因此先注册端口、再取回句柄（`setChipDevice` 用于替换芯片设备）
mgr.setExternalDevice('d0', undefined as never);
const dev = mgr.getDevice('d0');
mgr.setChipDevice(undefined as never);
const chip = mgr.getDevice('db');

// 设备查找
const found = mgr.findDeviceByType(typeHash);
const byName = mgr.findDeviceByTypeAndName(typeHash, nameHash);
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
l r0 led Setting
```

### IC10 v3 操作数

```ic10
alias tmp r3
define Level 3

alias led d0
move tmp Level          # 通过 r3 的别名写入
s led Setting Level     # 常量别名作为写入值
s led Color Color.Green # 枚举常量（需要类型表）

move r0 1
s dr0 On 1              # 动态端口：端口号存于 r0

alias Hash r2
move Hash HASH("StructureDoor")
sb Hash On 1            # 设备哈希由寄存器传递

lb r4 HASH("StructureDoor") Pressure Average   # 聚合模式的裸成员名
```

## TypeScript

本模块附带完整的 TypeScript 类型定义，无需额外安装 `@types` 包。

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

## 构建说明

### 环境要求

- **Node.js**: 16.0.0+
- **CMake**: 3.28.1+
- **C++ 编译器**:
  - Linux: GCC 13+ 或 Clang 16+
  - Windows: MSVC 2022

### 构建步骤

```bash
# Linux / macOS —— 配置、构建原生扩展并运行 Node.js 测试
code/scripts/bashShell/buildIC10RuntimeNode.sh
```

```powershell
# Windows (PowerShell)
pwsh code/scripts/powerShell/BuildIC10RuntimeNode.ps1
```

手动使用 CMake：

```bash
cd code
cmake -B build -S . -DBUILD_IC10_RUNTIME_EXPORTS_NODE=ON
cmake --build build --target ic10_runtime_node --config Release

# 产物连同类型声明一起进入发布目录
# build/IC10/backend/runtime/exports/node/Release/ic10r-node.node
#   → IC10/backend/runtime/publish/node/src/ic10r-node.node
```

### 测试

```bash
cd code/IC10/backend/runtime
pnpm test        # jest —— 绑定层测试（tests/node）
```

C++ 核心测试是经 CMake 构建的 GoogleTest 目标（`ic10_runtime_tests`），可用 `ctest` 或该目标的
可执行文件运行。

## 项目结构

```
@ic10/runtime/
├── src/
│   └── ic10r-node.node    # 原生模块（编译生成）
├── types/
│   ├── index.d.ts          # TypeScript 类型定义（入口）
│   ├── config.d.ts         # Config 接口
│   ├── context.d.ts        # Context 类
│   ├── device.d.ts         # Device 类
│   ├── engine.d.ts         # Engine 类
│   ├── locale.d.ts         # IC10RuntimeLocal 类
│   ├── manager.d.ts        # Manager 类
│   ├── memory.d.ts         # Memory 类
│   └── value.d.ts          # HASH / STR / 常量工具
├── CHANGELOG.md
├── CHANGELOG.zh.md
├── tsconfig.json
├── package.json
├── README.md
└── README.zh.md
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

- [@ic10/compiler – IC10 编译器 Node.js 绑定](https://www.npmjs.com/package/@ic10/compiler) —— 产出本运行时所需的 AST、符号表与类型表
- [Stationeers 官方网站](https://store.steampowered.com/app/544550/Stationeers/)
- [Node.js N-API 文档](https://nodejs.org/api/n-api.html)
- [node-addon-api 文档](https://github.com/nodejs/node-addon-api)
