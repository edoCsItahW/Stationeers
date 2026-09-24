# 开发者指引

[English](DEVELOPER_GUIDE.md)

想动手改 IC10 工具链本身的代码？这份指引讲的是**代码**：什么放在哪里、怎么构建和测试、以及一个版本是怎么发出去的。

> 如果你要找的是协作规范 —— 怎么提 issue、该往哪个分支提 PR、提交信息怎么写 —— 请看 [CONTRIBUTING.zh.md](CONTRIBUTING.zh.md)。

---

## 项目里都有什么

四个交付物共用一套构建系统：

| 交付物 | 源码 | 发布形态 |
|---|---|---|
| IC10 编译器核心（C++23） | `code/IC10/backend/compiler/` | `ic10c` 命令行 + `ic10c-node` / `ic10c_python` / Java `ic10` |
| IC10 运行时（执行引擎） | `code/IC10/backend/runtime/` | `ic10r-node`（调试器的后端） |
| 编辑器支持 | `code/IC10/plugins/` | [IC10](https://marketplace.visualstudio.com/items?itemName=edocsitahw.ic10) VS Code 扩展 |
| 游戏元数据 | `code/IC10/assets/ic10-matedata/` | npm 包 `@ic10/metadata` |

所有目标都挂在同一个 CMake 根（`code/CMakeLists.txt`）和同一套构建脚本（`code/scripts/`）上，所以无论你改哪一块，完整构建都只是几条命令。

---

## 仓库结构

```
Stationeers/
├── code/
│   ├── CMakeLists.txt                 # 所有目标唯一的 CMake 根
│   ├── cmake/                         # Find{Node,NodeAddonApi,Pybind11,Fbjni,GoogleTest}.cmake
│   │   └── modules/                   # Functions、CommonSettings、Dependencies、Locale、Debug
│   ├── scripts/                       # build-info.json + bashShell/ + powerShell/
│   ├── third-party/                   # 依赖缓存（fbjni、GoogleTest）
│   ├── common/cpp/                    # core（工具、国际化、异步协程）、export/、tests/
│   ├── Aero/                          # 上层高级语言的占位目录
│   └── IC10/
│       ├── assets/
│       │   ├── ic/                    # 随项目分发的 .ic 源文件（stdLib.ic、grammarTest.ic）
│       │   └── ic10-matedata/         # @ic10/metadata —— 设备/指令元数据与 Stationpedia 文本
│       ├── backend/
│       │   ├── compiler/
│       │   │   ├── core/              # 词法、语法、AST、语义、链接、增量、本地化
│       │   │   ├── exports/           # 各语言的薄绑定层（node、python、java）
│       │   │   ├── publish/           # 各语言对外发布的包
│       │   │   └── tests/             # cpp、node、python、java
│       │   └── runtime/               # IC10 执行引擎（core、exports、publish、tests）
│       ├── frontend/                  # 预留（当前为空）
│       └── plugins/
│           ├── vscode/ic10-language-support/   # packages/{client,common,server,debugger} pnpm 工作区
│           └── jetbrains/                       # JetBrains 方向的试验
├── docs/                              # 语法、设计笔记、参考资料
├── .github/workflows/                 # CI/CD
├── CHANGELOG.md · CONTRIBUTING.md · DEVELOPER_GUIDE.md · VERSION · LICENSE
```

`code/build/`、`code/artifacts/`、`code/node_modules/`、`code/cmake-build-msvc-release/` 都是生成目录，不要手改。

---

## 架构速览

### 编译器 —— `code/IC10/backend/compiler/core`

经典流水线，每个阶段在 `include/ic10_compiler/` 下各占一个目录：

```
源码 → 词法 → 语法（+ AST、文档注释注解） → 语义 → 链接 → 符号表 / JSON
                ↘ 增量（行级词法缓存 + 语句级语法缓存） ↗
```

- **`locals/`** 存放消息目录：面向用户的文本一律走 `ILoc::msgStr` / `msgFormat`，不写字符串字面量。新增语言是加一份目录，而不是改各阶段代码。
- **`incremental/`** 是编辑器「秒回」的关键：只重新词法化改动行、只重新解析改动语句，再把 token 位置与缓存快照对齐。
- 编译器是**元数据驱动**的：指令、操作数类型、设备都来自 `@ic10/metadata` 与 `.ic` 资源，而不是硬编码在 C++ 里。

### 运行时 —— `code/IC10/backend/runtime/core`

一台用 C++23 写的 IC10 机器：内存与栈、带 logic/slot 读写的设备管理器、按元数划分的指令执行器。它刻意与编译器分离，并暴露和编译器一致的 N-API 形态 —— 这正是 VS Code 调试器能单步执行 `.ic` 文件、而不只是做静态分析的原因。

### 绑定 —— `exports/` 与 `publish/` 的分工

`exports/<lang>/` 是适配层（搬运数据、暴露类）；`publish/<lang>/` 是用户真正安装的包。新增一门语言意味着两边都要加，并在 `code/scripts/build-info.json` 中补一条记录。

### 编辑器插件 —— `code/IC10/plugins/vscode/ic10-language-support`

一个由四个包组成的 pnpm 工作区：

| 包 | 职责 |
|---|---|
| `client` | 扩展入口与贡献点（语言、语法、配置、调试器） |
| `server` | LSP 服务端 —— 诊断、补全、悬停、语义标记、格式化、签名帮助 |
| `debugger` | 通过 `ic10r-node` 驱动运行时的 DAP 适配器 |
| `common` | 打包时分别复制进 `client` 与 `server` 的共享类型与工具 |

### 元数据 —— `code/IC10/assets/ic10-matedata`

`@ic10/metadata` 发布 `./locals`、`./std`、`./stationpedia/{CN,EN}`，由 `script/build.mjs` 从 YAML/JSON 源生成。VS Code 服务端消费的是已发布的包，编译器则通过 `.ic` 资源读取同一批数据。改动元数据后，先重新构建该包，再构建插件。

---

## 工具链

下表版本是 CI 与开发容器实际使用的版本。

| 工具 | 版本 | 备注 |
|---|---|---|
| CMake | 4.2.1 | `cmake_minimum_required(VERSION 3.28.1)` |
| C++ | C++23 | Linux CI 用 GCC 15，Windows CI 用 MSVC |
| Ninja / ccache / lld | 发行版提供 | 可选；可用时默认启用 ccache |
| Node.js | 26.7.0 | 各包声明 `engines.node >= 16` |
| pnpm | 11.20.0 | |
| Python | 3.13 | |
| JDK | 21 | Gradle 工具链为 21 |
| GoogleTest | v1.17.0 | 拉取到 `code/third-party` |
| fbjni | v0.7.0 | Java 绑定 |
| node-addon-api / pybind11 | 不固定（`latest`） | 配置阶段解析 —— 需要可复现构建请自行固定版本 |
| `jq` | — | bash 构建脚本依赖 |

---

## 构建

### 省事的方式

构建脚本会替你完成配置、构建，并且（多数目标）顺带跑测试。脚本自己定位仓库，因此在任何目录下都能调用：

```bash
# Linux / macOS
code/scripts/bashShell/buildIC10CompilerCore.sh
```

```powershell
# Windows
pwsh code/scripts/powerShell/BuildIC10CompilerCore.ps1
```

附加参数会原样透传给 CMake —— CI 就是靠这个把产物目录指过去的：

```bash
code/scripts/bashShell/buildIC10CompilerNode.sh -DSTATIONEERS_ARTIFACTS_DIR=/path/to/artifacts
```

| 脚本（`bashShell/` · `powerShell/`） | 构建内容 | 跑测试 | 产物 |
|---|---|---|---|
| `buildIC10CompilerCLI.sh` · `BuildIC10CompilerCLI.ps1` | 命令行工具（bash 版带 ASan/UBSan） | — | `code/build/bin/Release/ic10c[.exe]` |
| `buildIC10CompilerCore.sh` · `BuildIC10CompilerCore.ps1` | 核心 + C++ 测试 | ✅ | `code/build/IC10/backend/compiler/tests/Release/IC10_Compiler_Tests[.exe]` |
| `buildIC10CompilerNode.sh` · `BuildIC10CompilerNode.ps1` | Node 原生模块 | ✅ Jest | `…/compiler/publish/node/src/ic10c-node.node` |
| `buildIC10CompilerPython.sh` · `BuildIC10CompilerPython.ps1` | Python 扩展 | ✅ pytest | `…/compiler/publish/python/ic10c_python.{pyd,so}` |
| `buildIC10CompilerJava.sh` · `BuildIC10CompilerJava.ps1` | JNI 库 | ✅ Gradle | `…/compiler/publish/java/src/main/resources/native/` |
| `buildIC10RuntimeNode.sh` · `BuildIC10RuntimeNode.ps1` | 运行时模块 | ✅ Jest | `…/runtime/publish/node/src/ic10r-node.node` |

同样的信息在 `code/scripts/build-info.json` 里有机器可读版本 —— 目标名、配置、测试可执行文件、发布目录都从那里读取；新增目标请改这个文件，而不是往脚本里硬编码路径。

### 直接用 CMake

```bash
cd code
cmake -B build -S .
cmake --build build --target IC10_Compiler_Tests --config Release
```

构建目录与配置同样来自 `build-info.json`：`build/` 与 `Release`。

**配置项。** 项目会解析一棵*特性树*：`BUILD_<路径>` 形式的开关（如 `BUILD_IC10_COMPILER_CORE`、`BUILD_IC10_COMPILER_TESTS`、`BUILD_IC10_COMPILER_EXPORTS_NODE`、`BUILD_IC10_COMPILER_EXPORTS_PYTHON`、`BUILD_IC10_COMPILER_EXPORTS_JAVA`、`BUILD_IC10_RUNTIME_EXPORTS_NODE`）打开对应子目录，并由此派生 `ANY_*` 聚合变量（`ANY_BUILD`、`ANY_TESTS`、`ANY_NODE` 等）。这棵树在 `code/CMakeLists.txt` 里声明（`BUILD_CHILDREN`、`IC10_CHILDREN` 等），所以新增组件只需把它加进树里，而不用再写新的 `if()` 分支。

基础设施开关（见 `code/cmake/modules/Functions.cmake` 的 `st_l_options`）：

| 开关 | 默认 | 作用 |
|---|---|---|
| `ENABLE_CCACHE` | ON | 可用时使用 ccache/sccache |
| `ENABLE_PAPALLEL` | ON | 设置 `CMAKE_BUILD_PARALLEL_LEVEL`（对，源码里就是这么拼的） |
| `ENABLE_ARTIFACTS` | ON | 基于哈希的产物缓存，根目录由 `STATIONEERS_ARTIFACTS_DIR` 指定 |
| `ENABLE_DEP_CACHE` | ON | 把拉取的依赖留在 `STATIONEERS_DEP_CACHE_DIR`（默认 `code/third-party`） |
| `ENABLE_AUTO_LINKER` | ON | 自动选择更快的链接器 |
| `LIMIT_LINKED_MEMORY` | OFF | 在内存较小的机器上限制链接占用 |
| `STATIONEERS_LANG` | — | 构建期默认语言（`en-us`、`zh-hans`） |

**没有** `CMakePresets.json`；脚本也不使用 `CMAKE_BUILD_TYPE` —— 配置固定为 `Release`，反正多配置生成器本就会忽略 `CMAKE_BUILD_TYPE`。

---

## 测试

```bash
# C++ 核心（GoogleTest）
code/build/IC10/backend/compiler/tests/Release/IC10_Compiler_Tests      # Windows: …\IC10_Compiler_Tests.exe

# Node 绑定（Jest + ts-jest）
cd code/IC10/backend/compiler && pnpm install --ignore-scripts && pnpm run test

# 运行时（Jest）
cd code/IC10/backend/runtime && pnpm install --ignore-scripts && pnpm run test

# Python 绑定（pytest）
cd code/IC10/backend/compiler/tests/python && python -m pytest

# Java 绑定（JUnit + Gradle）
cd code/IC10/backend/compiler/tests/java && ./../../publish/java/gradlew test --no-daemon --stacktrace
```

`ctest` **目前不可用**：`add_test()` 是有的，但 `enable_testing()` 在 `code/IC10/backend/compiler/CMakeLists.txt` 里被注释掉了，所以请直接运行测试可执行文件（或者干脆用构建脚本，它会替你跑）。想用 `ctest` 就把那一行取消注释 —— 测试已经注册好了。单配置生成器下路径里的 `Release/` 段会消失，这段兼容由 `code/scripts/bashShell/modules/common.sh` 的 `resolve_artifact_path` 处理。

测试分层是刻意的：C++ 测试覆盖编译器行为，绑定测试只验证绑定是否正确搬运数据 —— 不要把核心断言在四种语言里各抄一遍。

---

## 代码风格与静态分析

| 范围 | 工具 | 配置 |
|---|---|---|
| C++（`compiler/`、`runtime/`、`common/cpp/`） | clang-format | `.clang-format`（基于 LLVM，100 列，4 空格） |
| C++ | clang-tidy | 上述三个目录下的 `.clang-tidy` |
| TypeScript（插件） | prettier | `code/IC10/plugins/vscode/ic10-language-support/.prettierrc.json`（120 列、双引号） |

CI 目前**只跑构建与测试** —— 没有静态分析工作流，所以下面这些需要你自己在本地跑（针对 C++）：

```bash
# 格式化
clang-format -i <file>

# clang-tidy，直接使用构建已导出的编译数据库
cd code && cmake -B build -S . && clang-tidy -p build <file>

# ASan + UBSan：命令行构建脚本本就以 -fsanitize=address,undefined 编译
code/scripts/bashShell/buildIC10CompilerCLI.sh
```

```bash
# cppcheck（在仓库根目录执行）
cppcheck --enable=all --suppress=missingIncludeSystem --std=c++23 \
  -I code/common/cpp/core/include \
  -I code/IC10/backend/compiler/core/include \
  code/IC10/backend/compiler/core/src/
```

C++ 文档注释使用 Doxygen、双语（`@if zh` / `@else` / `@endif`）。

---

## 开发容器

`.devcontainer/` 提供开箱可用的工具链（Ubuntu 24.04、GCC 15、CMake 4.2.1、Node 26.7.0、pnpm 11.20.0、Python 3.13、JDK 21、ninja、ccache、lld），并挂载了 ccache/pnpm/npm 缓存卷。它只是本地开发的便利，CI 并不在 Docker 里运行。

---

## 发布

`VERSION` 是一份手工维护的类 YAML 文件：

```yaml
core:
    IC10: 3.0.0

plugins:
    vscode:
        IC10 Language Support: 1.0.2
```

清单见 [docs/releaseList.md](docs/releaseList.md)。简而言之：更新 README/CHANGELOG/VERSION 与各包版本号，确保 C++ 核心与各绑定能编译、测试通过，并重新生成类型声明（`ic10c-node` 的 `.d.ts`、Python 的 `.pyi`）。

| 标签形式 | 工作流 | 产出 |
|---|---|---|
| `v[0-9]*`（如 `v3.0.0`） | `release.yml` | **草稿** GitHub Release，按平台附 `ic10c`、`ic10c-node.node`、`ic10c-python.os`、`ic10c-java.os` |
| `vscode-ic10-language-support-v*` | `vscode-extension-release.yml` | 用 `vsce` 打包的 `.vsix` + 草稿 Release |

插件发布不需要本地工具链：它依赖已发布的 `ic10c-node` / `ic10r-node` 包，并把它们的预编译 `.node` 打包进 VSIX。

Python 绑定以 **`ic10c-python`** wheel 发布：元数据在 `publish/python/pyproject.toml`，而 `publish/python/setup.py` 扩展了 `build_py`，把 CMake 产出的 `ic10c_python.{pyd,so}` 与 `.pyi` 存根一并打包（setuptools 的 `py-modules` 只认 `.py` 源文件，光有元数据会打出空 wheel）。

> **注意：** `release.yml` 的参数来自任务矩阵（`matrix.os`、`matrix.script-dir` 等），脚本目录是 `code/scripts/…`。可复用工作流会先 `cd` 进 `script-dir` 再执行脚本，改动该文件时两者要保持一致。

---

## 已知的毛边

如实列出，免得别人再踩一遍：

- `ctest` 没接上 —— `enable_testing()` 被注释掉了（见[测试](#测试)）。测试本身已经用 `add_test()` 注册好，取消那一行注释即可。
- `node-addon-api` 与 `pybind11` 在配置阶段按 `latest` 解析，同一提交的两次构建可能不一致。
- `code/Aero/`、`code/IC10/frontend/`、`code/IC10/plugins/jetbrains/` 是占位目录或试验代码。

---

## 文档索引

| 文档 | 用途 |
|---|---|
| [README.zh.md](README.zh.md) | 项目是什么、怎么用 |
| [CONTRIBUTING.zh.md](CONTRIBUTING.zh.md) | Issue、PR、提交约定与代码规范 |
| [CHANGELOG.zh.md](CHANGELOG.zh.md) | 发布历史 |
| [docs/releaseList.md](docs/releaseList.md) | 各组件发布检查清单 |
| [docs/annotationExample.md](docs/annotationExample.md) | `#:` / `#>` 注解语法示例 |
| [docs/grammar/](docs/grammar/) | 拆分后的 ANTLR 语法（词法/语法 × 指令元数） |
| [docs/doxygen/](docs/doxygen/) | Doxygen HTML 主题（doxygen-awesome）与页头 |
| [docs/PROJECT_DEBT.md](docs/PROJECT_DEBT.md) | 已知欠债及其记录格式 |
