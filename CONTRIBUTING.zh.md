# 为 IC10 编译器做贡献

[English](CONTRIBUTING.md)

感谢您对本项目的兴趣！本文档讲的是**协作方式** —— Issue、拉取请求、提交信息与版本管理。

> 想了解环境搭建、构建与测试、或整体架构？那些都在[开发者指引](DEVELOPER_GUIDE.zh.md)里。

## 行为准则

参与本项目即表示您同意营造一个尊重和包容的环境。请向项目维护者举报不当行为。

## 贡献方式

1. **报告 Bug** – 使用 GitHub Issues，描述问题、重现步骤和环境信息。
2. **提出增强建议** – 新建标记为 `enhancement` 的 Issue，清晰描述建议的功能。
3. **提交拉取请求** – 遵循以下流程。

## 开发工作流

1. **复刻（Fork）** 仓库。
2. **从 `develop` 分支创建功能分支**（紧急修复可从 `main` 创建）：
   ```bash
   git checkout -b feature/你的功能名
   ```
3. **进行修改** – 保持提交逻辑清晰，撰写符合[约定式提交](#提交信息规范)的提交信息。
4. **构建并测试你的改动** – 工具链版本、构建脚本与各语言的测试命令见[开发者指引](DEVELOPER_GUIDE.zh.md#构建)。下面这一条脚本会构建 C++ 核心并运行其测试：
   ```bash
   code/scripts/bashShell/buildIC10CompilerCore.sh   # Windows: pwsh code/scripts/powerShell/BuildIC10CompilerCore.ps1
   ```
5. **格式化你的改动** – 项目使用 `clang-format`（4 空格缩进、100 列）：
   ```bash
   clang-format -i <文件>
   ```
6. **推送**并创建指向 `develop` 的 Pull Request。

## 提交信息规范

本项目遵循[约定式提交 1.0.0](https://www.conventionalcommits.org/zh-hans/v1.0.0/)。所有提交信息必须使用以下格式：

```
<类型>[可选作用域]: <描述>

[可选正文]

[可选脚注]
```

### 提交类型

| 类型 | 说明 |
|:-----|:-----|
| `feat` | 新功能 |
| `fix` | Bug 修复 |
| `docs` | 仅文档变更 |
| `style` | 不影响代码含义的变更（格式化等） |
| `refactor` | 既不修复 Bug 也不添加功能的代码变更 |
| `test` | 添加缺失的测试或修正现有测试 |
| `ci` | CI 配置文件和脚本的变更 |
| `chore` | 其他不修改源码或测试文件的变更 |
| `build` | 影响构建系统或外部依赖的变更 |

### 破坏性变更

对于破坏性变更，在脚注中添加 `BREAKING-CHANGE:` 或在类型/作用域后添加 `!`：

```
feat!: 伴随游戏更新的全面语法更新

BREAKING-CHANGE: 语法已变更，语言使用方式受到影响
```

### 示例

```
feat: 实现链接器以支持多单元符号合并
fix(lexer): 修复未闭合字符串吞没后续输入的问题
docs: 更新开发者指引
test(ic10): 添加链接器单元测试
ci: 添加 Python 工作流
```

## 代码规范

- **语言**：C++23（合理使用协程、概念、范围库）。
- **风格**：遵循所在组件的 `.clang-format` —— [compiler](code/IC10/backend/compiler/.clang-format)、[runtime](code/IC10/backend/runtime/.clang-format)、[common](code/common/cpp/.clang-format)（4 空格缩进，花括号不换行，100 列限制）。VS Code 插件的 TypeScript 使用 prettier（120 列、双引号）。
- **命名**：
    - 类型：`PascalCase`（如 `Lexer`、`SymbolTable`、`IncCompiler`）
    - 函数/变量：`camelCase`（如 `extractHexNumber`、`pos_`）
    - 成员变量：`camelCase` 加尾下划线（如 `source_`、`pos_`）
    - 宏：`UPPER_SNAKE_CASE`（尽量少用宏）
    - 枚举值：`PascalCase`（如 `TokenType::KEYWORD_ADD`）
- **错误处理**：项目使用 `std::expected<T, Error>` 处理可恢复错误，解析错误使用自定义 `Error` 层次结构和异常。请遵循 `parser.cpp` 和 `main.cpp` 中的模式。
- **国际化**：所有面向用户的字符串必须通过本地化框架处理：
    ```cpp
    // 无参数消息
    ILoc::msgStr<IMsgId::IWL1>()

    // 带参数消息
    ILoc::msgFormat<IMsgId::IWL1>(charValue)
    ```
- **测试**：为新功能添加单元测试：
    - C++ 测试：`code/IC10/backend/compiler/tests/cpp/`（运行时：`code/IC10/backend/runtime/tests/`）
    - Node.js 测试：`code/IC10/backend/compiler/tests/node/`
    - Python 测试：`code/IC10/backend/compiler/tests/python/`
    - Java 测试：`code/IC10/backend/compiler/tests/java/`

    行为请在 C++ 核心中测试；绑定测试只验证绑定是否正确搬运数据，不要把核心断言再抄一遍。
- **文档**：公共 API 使用 Doxygen 风格的双语注释，标签为 `@if zh` / `@else` / `@endif`。

## 模块概览

| 模块 | 位置 | 说明 |
|:-----|:-----|:-----|
| 词法分析器 | `compiler/core/include/ic10_compiler/lexer/` | 基于状态机的词法分析 |
| 语法分析器 | `compiler/core/include/ic10_compiler/parser/` | 递归下降解析器，AST 构建 |
| 语义分析 | `compiler/core/include/ic10_compiler/semantic/` | 符号表、类型推断、类型检查 |
| 链接器 | `compiler/core/include/ic10_compiler/link/` | 多单元符号合并、跨单元解析 |
| 增量编译 | `compiler/core/include/ic10_compiler/incremental/` | 行/语句级缓存，快速重新编译 |
| 本地化 | `compiler/core/include/ic10_compiler/locals/` | 诊断消息本地化（en-us、zh-hans） |
| 运行时 | `runtime/core/` | IC10 执行引擎：内存/栈、设备、指令执行器 |
| 元数据 | `assets/ic10-matedata/` | `@ic10/metadata` —— 指令、设备与 Stationpedia 数据 |
| 公共库 | `code/common/cpp/core/` | 共享工具（异步协程、异常诊断、国际化、工具函数） |

编译器、运行时与元数据路径相对于 `code/IC10/backend/`。

## 拉取请求指南

- 目标分支：`develop`。
- 包含清晰的描述，如果有关联的 Issue 请注明。
- 确保 CI 通过 —— CI 会构建并测试所有目标。格式与 clang-tidy **不在** CI 中运行，请自行检查（见[代码风格与静态分析](DEVELOPER_GUIDE.zh.md#代码风格与静态分析)）。
- 保持更改聚焦 – 每个 PR 只解决一个功能或 Bug。
- 必要时更新文档（README、行内 Doxygen 注释）。
- 遵循[提交信息规范](#提交信息规范)。

## 版本管理

本项目遵循[语义化版本 2.0.0](https://semver.org/lang/zh-CN/)。版本号记录在 [VERSION](VERSION) 中，每个组件各有一条：

```yaml
core:
    IC10: 3.0.0

plugins:
    vscode:
        IC10 Language Support: 1.0.2
```

- **主版本号**：破坏性语法变更或 API 不兼容
- **次版本号**：新功能
- **修订号**：Bug 修复和小改进

标签与组件对应：`v<版本号>` 发布核心，`vscode-ic10-language-support-v<版本号>` 发布 VS Code 插件。每次发布都要更新 [CHANGELOG.zh.md](CHANGELOG.zh.md)，各组件的检查清单见 [docs/releaseList.md](docs/releaseList.md)。

## 有问题？

欢迎发起讨论 Issue，或联系维护者：[edocsitahw@qq.com](mailto:edocsitahw@qq.com)。
