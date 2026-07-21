# IC10 编译器

[English](README.md)

![Version](https://img.shields.io/badge/version-2.0.0-blue)
![C++](https://img.shields.io/badge/C%2B%2B-23-blue)
![License](https://img.shields.io/badge/license-CC%20BY--NC--SA%204.0-lightgrey)

一个现代 C++23 编写的 **IC10** 和 **StationScript** 编译器套件。IC10 和 StationScript 是游戏《Stationeers》中使用的脚本语言。
本项目提供词法分析、语法分析、语义分析、链接、增量编译，内置基于协程的异步基础设施，并提供 Node.js / Python 绑定。

---

## 特性

- **词法分析器** – 基于状态机的词法分析，支持寄存器 `r0`–`r15`、设备 `d0`–`d5`、多进制数字 `$hex`/`%bin`/十进制/浮点、字符串、`#` 和 `//` 注释、全部指令关键字。
- **语法分析器** – 递归下降解析器，为所有 IC10 指令（0 元到 6 元）构建抽象语法树（AST），支持预处理指令（`alias`、`define`）和文档注释解析。
- **语义分析器** – 使用 `Promise`/`Future` 异步符号表进行符号解析、类型推断和类型检查，通过协程实现前向引用解析。
- **链接器** – 两阶段符号合并，支持多编译单元、跨单元前向引用解析、符号可见性控制（public/private）和循环依赖检测。
- **增量编译器** – 行级词法缓存和语句级语法缓存，编辑场景下仅重新处理变化部分，显著提升重新编译速度。
- **错误报告** – 带源码位置（行:列）和严重级别的详细诊断信息，支持多语言（英文/简体中文）。
- **异步协程基础设施** – 自定义 `Task<T>`、`Promise<T>`、`Future<T>` 及协程状态管理，实现非阻塞符号解析。
- **命令行编译器** – `ic10c` 命令行工具，支持 `--emit-tokens`、`--emit-ast`、`--emit-symbols`、`--link`、`--locale`、`--pretty` 和 `-o` 输出重定向。
- **Node.js 绑定** – 通过 `node-addon-api` 实现原生 Node.js 扩展，导出 11 个适配器（Lexer、Token、Parser、Program/AST、Analyser、SymbolTable、Linker、IC10Local、IncLexer、IncParser、IncCompiler）。
- **Python 绑定** – 通过 `pybind11` 实现原生 Python 扩展，提供与 Node.js 绑定相同的编译器功能。
- **跨平台** – Linux（GCC 13+ / Clang 16+）和 Windows（MSVC 2022）。
- **测试** – C++ 核心使用 GoogleTest 单元测试（词法、语法、语义、链接器、增量、集成、系统测试），Node.js 绑定使用 Jest 测试，Python 绑定使用 pytest 测试。
- **CI/CD** – GitHub Actions 工作流：构建、测试、静态分析（cppcheck、clang-tidy、clang-format），以及标签推送时自动发布构建产物。

---

## 项目结构

```
Stationeers/
├── code/backend/compiler/              # 编译器主源码
│   ├── CMakeLists.txt                  # 顶层 CMake
│   ├── common/                         # 公共基础库
│   │   ├── include/common/async/       # 协程任务、Promise、Future
│   │   ├── include/common/exception/   # 异常与诊断层次
│   │   ├── include/common/locals/      # 国际化框架
│   │   └── include/common/utils/       # FStr、BiMap、enum_to_str、类型列表等
│   ├── IC10/                           # IC10 编译器核心
│   │   ├── include/ic10/lexer/         # Token 和词法分析器
│   │   ├── include/ic10/parser/ast/    # AST 节点定义（0 元到 6 元）
│   │   ├── include/ic10/semantic/      # 符号表、语义分析器、类型系统
│   │   ├── include/ic10/link/          # 链接器（多单元符号合并）
│   │   ├── include/ic10/incremental/   # IncLexer、IncParser、IncCompiler
│   │   ├── include/ic10/locals/        # 语言资源（英文、简体中文）
│   │   ├── src/                        # 实现文件
│   │   ├── main.cpp                    # 命令行入口（ic10c）
│   │   └── main.hpp                    # Doxygen 主页与专题文档
│   ├── StationScript/                  # StationScript 编译器（开发中）
│   ├── exports/                        # 语言绑定
│   │   ├── common/node/                # 公共 Node.js 适配器工具
│   │   ├── common/python/              # 公共 Python 适配器工具
│   │   ├── IC10/node/                  # IC10 Node.js 绑定（11 个适配器）
│   │   └── IC10/python/                # IC10 Python 绑定
│   ├── tests/                          # 单元测试
│   │   ├── ic10/                       # C++ 测试（词法、语法、语义、链接器、增量）
│   │   │   ├── node/                   # Node.js 绑定测试（Jest + TypeScript）
│   │   │   └── python/                 # Python 绑定测试（pytest）
│   │   └── node/ic10/                  # Node.js 包（ic10-node-api）
│   ├── scripts/                        # 构建脚本（PowerShell）
│   ├── cmake/                          # CMake 模块（node.cmake、pybind11.cmake）
│   ├── .clang-format                   # 代码风格配置（4 空格缩进）
│   └── .clang-tidy                     # 静态分析配置
├── code/backend/mateDatas/             # 游戏元数据（指令、枚举、类型）
├── code/plugins/vscode/                # VS Code 语言支持扩展
├── docs/                               # 文档与 Doxygen 资源
├── .github/workflows/                  # CI/CD 工作流
├── CHANGELOG.md                        # 更新日志（英文）
├── CHANGELOG.zh.md                     # 更新日志（中文）
├── CONTRIBUTING.md                     # 贡献指南（英文）
├── CONTRIBUTING.zh.md                  # 贡献指南（中文）
├── VERSION                             # 当前版本
└── LICENSE                             # 许可证文件
```

---

## 构建要求

### C++ 核心

- **CMake** 3.28.1 或更高
- **C++23** 编译器：
  - Linux：GCC 13+ 或 Clang 16+
  - Windows：MSVC 2022（含 Visual Studio 2022 生成工具）
- **Ninja**（推荐）或 Make
- **Git**（用于获取 GoogleTest）

### Node.js 绑定

- **Node.js** 24.x 或 26.x（经测试验证）
- **pnpm** 9.x（包管理器）— 也支持 npm
- **node-gyp** — 构建原生扩展必需
- **node-addon-api** ^8.8.0

> **注意：** 安装 Node.js 后，必须先安装 `node-gyp` 并下载 Node.js 头文件，然后才能构建：
> ```bash
> # 全局安装 node-gyp 或作为开发依赖安装
> pnpm add -g node-gyp   # 或: npm install -g node-gyp
>
> # 为当前 Node 版本下载 Node.js 头文件
> pnpm exec node-gyp install   # 或: npm exec node-gyp install
> ```

### Python 绑定

- **Python** 3.13（经测试验证）
- **pybind11** 2.12+

> **注意：** Python 必须在系统 PATH 中可被找到，以便 CMake 的 `FindPython3` 能够定位它。

---

## 构建步骤

### 1. 克隆仓库

```bash
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
```

### 2. 配置 Node.js 构建环境

如果您计划构建 Node.js 绑定：

```bash
# 安装 node-gyp（构建原生扩展必需）
pnpm add -g node-gyp   # 或: npm install -g node-gyp

# 为当前 Node 版本下载 Node.js 头文件
pnpm exec node-gyp install   # 或: npm exec node-gyp install
```

### 3. 安装 Node.js 依赖

```bash
cd code/backend/compiler
pnpm i --ignore-workspace
```

### 4. 使用 CMake 配置

```bash
cmake -B build -S code/backend/compiler -G Ninja -DCMAKE_BUILD_TYPE=Release
```

在 Windows 上（MSVC）可能需要指定生成器：

```powershell
cmake -B build -S code/backend/compiler -G "Visual Studio 17 2022" -A x64
```

### 5. 编译

```bash
cmake --build build --parallel
```

可执行文件 `ic10c` 将生成在 `build/bin/` 目录下。
Node.js 原生模块 `ic10-node-api.node` 将生成在 `build/exports/IC10/node/` 目录下。
Python 原生模块将生成在 `build/exports/IC10/python/` 目录下。

### 6. 运行测试

#### C++ 测试

```bash
cd build
ctest --output-on-failure
```

#### Node.js 测试

```bash
cd code/backend/compiler
pnpm test
```

#### Python 测试

```bash
cd code/backend/compiler/tests/ic10/python
pytest -v
```

---

## 使用方法

### 命令行（`ic10c`）

命令行编译器支持各编译阶段的独立输出，以及完整的编译流程和多单元链接：

```bash
ic10c input.ic                    # 编译并输出符号表 JSON
ic10c --emit-tokens input.ic      # 输出词法 Token 流
ic10c --emit-ast input.ic         # 输出语法树 AST
ic10c --emit-symbols input.ic     # 输出符号表（默认）
ic10c -o out.json input.ic        # 输出到文件
ic10c --pretty input.ic           # 美化 JSON 输出
ic10c --locale zh-hans input.ic   # 使用简体中文输出消息
ic10c --link a.ic b.ic c.ic       # 链接多个单元并输出合并后的符号表
ic10c -v                          # 显示版本
ic10c -h                          # 显示帮助
```

IC10 程序示例：

```
alias counter r0
define PI 3.14159

start:
    move r0 10
    add r1 r0 PI
    hcf
```

### Node.js

`ic10-node-api` 包提供 JavaScript/TypeScript 绑定：

```typescript
import { Lexer, Parser, Analyser, IC10Local } from 'ic10-node-api';

// 设置语言（可选，默认英文）
IC10Local.setLanguage('zh-hans');

const source = `
alias counter r0
start:
    move r0 10
    add r1 r0 5
    hcf
`;

// 1. 词法分析
const tokens = Lexer.tokenize(source);

// 2. 语法分析
const parser = new Parser(tokens, false);  // debug = false
const program = parser.parse();

// 3. 语义分析（异步 — 使用协程处理前向引用）
const analyser = new Analyser();
await analyser.visit(program);

// 4. 获取结果
console.log(analyser.symbolTable.toJSON());
console.log(analyser.diagnostics);
```

**链接器用法：**

```typescript
import { Linker } from 'ic10-node-api';

const linker = new Linker();
linker.addUnit(source1, 'file1.ic');
linker.addUnit(source2, 'file2.ic');
const symbolTable = linker.link();
console.log(linker.diagnostics);
```

**增量编译器用法：**

```typescript
import { IncCompiler } from 'ic10-node-api';

const compiler = new IncCompiler();
const result1 = compiler.compileFull(source);
const result2 = compiler.compileInc(modifiedSource);
console.log(`增量: ${result2.incremental}, 重新词法分析行数: ${result2.relexedLines}`);
```

### Python

```python
import ic10_python as ic10

# 词法分析
tokens = ic10.Lexer.tokenize(source)

# 语法分析
parser = ic10.Parser(tokens, False)
program = parser.parse()

# 语义分析
analyser = ic10.Analyser()
analyser.visit(program)
print(analyser.symbolTable.toJSON())
```

---

## 国际化（i18n）

编译器支持多语言。目前支持的语言：
- 英语（`en-us`）– 默认
- 简体中文（`zh-hans`）

**C++:**
```cpp
IC10Local::setLanguage("zh-hans");
```

**Node.js:**
```typescript
IC10Local.setLanguage('zh-hans');
```

---

## 版本管理

本项目遵循[语义化版本 2.0.0](https://semver.org/lang/zh-CN/)。当前版本为 **2.0.0**（见 [VERSION](VERSION)）。

- **v2.0.0** — 伴随游戏更新的破坏性语法变更，新增链接器、增量编译器、Python 绑定、类型推断和注释语法。
- **v1.0.x** — 首次发布，包含词法分析器、语法分析器、语义分析器和 Node.js 绑定。

完整发布历史请见 [CHANGELOG.zh.md](CHANGELOG.zh.md)。

---

## 贡献指南

请阅读 [CONTRIBUTING.zh.md](CONTRIBUTING.zh.md) 了解如何报告问题、提交拉取请求以及代码规范。

---

## 许可证

本项目采用 **CC BY-NC-SA 4.0** 许可证。
详见 [LICENSE](LICENSE) 文件。
未经作者许可，不得将本软件用于商业目的。

---

## 作者

Xiao SongTao (edocsitahw) – [edocsitahw@qq.com](mailto:edocsitahw@qq.com)

---

## 致谢

- 灵感来自游戏 [Stationeers](https://stationeers.com/) 中的 IC10 脚本语言。
- 基于 C++23 协程和现代 CMake 构建。
- Node.js 绑定由 [node-addon-api](https://github.com/nodejs/node-addon-api) 提供支持。
- Python 绑定由 [pybind11](https://github.com/pybind/pybind11) 提供支持。
