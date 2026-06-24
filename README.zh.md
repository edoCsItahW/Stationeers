# IC10 编译器

[English](README.md)

一个现代 C++23 编写的 **IC10** 和 **StationScript** 编译器套件。IC10 和 StationScript 是游戏《Stationeers》中使用的脚本语言。  
本项目提供词法分析、语法分析、语义分析，内置基于协程的异步基础设施，并提供 Node.js 绑定。

---

## 特性

- **词法分析器** – 将 IC10 源码转换为 Token（支持寄存器、设备、数字、字符串、注释、关键字）。
- **语法分析器** – 构建抽象语法树（AST），覆盖所有 IC10 指令（0 元到 6 元指令）。
- **语义分析器** – 使用 `Promise`/`Future` 异步符号表进行符号解析和类型检查。
- **错误报告** – 带源码位置的详细错误信息，支持多语言（英文/简体中文）。
- **异步协程基础设施** – 自定义 `Task<T>`、`Promise<T>`、`Future<T>` 及协程状态管理，实现非阻塞符号解析。
- **Node.js 绑定** – 通过 `node-addon-api` 实现原生 Node.js 扩展，向 JavaScript/TypeScript 暴露词法分析器、语法分析器、AST 和语义分析器。
- **跨平台** – Linux（GCC/Clang）和 Windows（MSVC）。
- **测试** – C++ 核心使用 GoogleTest 单元测试，Node.js 绑定使用 Jest 测试。
- **CI/CD** – GitHub Actions 工作流：构建、测试、静态分析（cppcheck、clang‑tidy、clang‑format）。

---

## 项目结构

```
IC10_Compiler/
├── code/backend/compiler/           # 编译器主源码
│   ├── CMakeLists.txt               # 顶层 CMake
│   ├── common/                      # 公共基础库
│   │   ├── include/common/async/    # 协程任务、Promise、Future
│   │   ├── include/common/exception/# 异常层次
│   │   ├── include/common/locals/   # 国际化框架
│   │   └── include/common/utils/    # FStr、BiMap、enum_to_str、类型列表等
│   ├── IC10/                        # IC10 编译器核心
│   │   ├── include/ic10/lexer/      # Token 和词法分析器
│   │   ├── include/ic10/parser/ast/ # AST 节点定义（0 元到 6 元）
│   │   ├── include/ic10/semantic/   # 符号表、语义分析器
│   │   ├── include/ic10/locals/     # 语言资源（英文、简体中文）
│   │   ├── src/                     # 实现文件
│   │   └── main.cpp                 # 命令行入口
│   ├── StationScript/               # StationScript 编译器（开发中）
│   ├── exports/                     # 语言绑定
│   │   ├── common/node/             # 公共 Node.js 适配器工具
│   │   └── IC10/node/               # IC10 Node.js 绑定（ic10-node-api）
│   ├── tests/                       # 单元测试
│   │   ├── common/                  # 测试工具
│   │   ├── ic10/                    # C++ 测试（词法、语法分析）
│   │   │   └── node/                # Node.js 绑定测试（Jest + TypeScript）
│   │   ├── station_script/          # StationScript 测试
│   │   └── node/ic10/               # Node.js 包（ic10-node-api）
│   ├── scripts/                     # 构建脚本（PowerShell）
│   ├── cmake/                       # CMake 模块（node.cmake、pybind11.cmake）
│   ├── .clang-format                # 代码风格配置
│   └── .clang-tidy                  # 静态分析配置
├── .github/
│   ├── workflows/                   # CI/CD 工作流（build.yml、test.yml）
│   └── actions/                     # 可复用的 GitHub Actions
├── CNAHGELOG.md                     # 更新日志
├── CONTRIBUTING.md                  # 贡献指南（英文）
├── CONTRIBUTING.zh.md               # 贡献指南（中文）
└── LICENSE                          # 许可证文件
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

- **Node.js** 16.0+（推荐 26.x）
- **pnpm** 9.x（包管理器）
- **node-addon-api** ^8.8.0

---

## 构建步骤

### 1. 克隆仓库

```bash
git clone https://github.com/edoCsItahW/Stationeers.git
cd Stationeers
```

### 2. 安装 Node.js 依赖

```bash
cd code/backend/compiler
pnpm i --ignore-workspace
```

### 3. 使用 CMake 配置

```bash
cmake -B build -S code/backend/compiler -G Ninja -DCMAKE_BUILD_TYPE=Release
```

在 Windows 上（MSVC）可能需要指定生成器：

```powershell
cmake -B build -S code/backend/compiler -G "Visual Studio 17 2022" -A x64
```

### 4. 编译

```bash
cmake --build build --parallel
```

可执行文件 `IC10_Compiler` 将生成在 `build/bin/` 目录下。  
Node.js 原生模块 `ic10-node-api.node` 将生成在 `build/exports/IC10/node/` 目录下。

### 5. 运行测试

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

---

## 使用方法

### 命令行

```bash
cd build/bin
./IC10_Compiler
```

> **注意：** 目前 `main.cpp` 中硬编码了源文件路径（指向 `../../tests/grammarTest.ic`）。  
> 你可以修改 `main.cpp` 以接受命令行参数。

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
import { Lexer, Parser, Analyser } from 'ic10-node-api';

const source = `
alias counter r0
start:
    move r0 10
    add r1 r0 5
    hcf
`;

// 词法分析
const tokens = Lexer.tokenize(source);

// 语法分析
const ast = Parser.parse(tokens);

// 语义分析
const analyser = new Analyser();
analyser.analyse(ast);
console.log(analyser.getSymbolTable());
```

---

## 国际化（i18n）

编译器通过 `Local<MsgPack>` 框架支持多语言。  
目前支持的语言：
- 英语（`en_us`）– 默认
- 简体中文（`zh_hans`）

切换语言的示例代码：
```cpp
Loc::setLanguage("zh-hans");
```

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

肖松涛 (edocsitahw) – [2207150234@st.sziit.edu.cn](mailto:2207150234@st.sziit.edu.cn)

---

## 致谢

- 灵感来自游戏 [Stationeers](https://stationeers.com/) 中的 IC10 脚本语言。
- 基于 C++23 协程和现代 CMake 构建。
- Node.js 绑定由 [node-addon-api](https://github.com/nodejs/node-addon-api) 提供支持。
