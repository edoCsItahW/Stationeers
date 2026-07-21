# 为 IC10 编译器做贡献

[English](CONTRIBUTING.md)

感谢您对本项目的兴趣！本文档概述了贡献的流程和指南。

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
4. **配置构建环境**（用于 Node.js 和 Python 绑定）：
   ```bash
   # Node.js 绑定：安装 node-gyp 并下载 Node.js 头文件
   pnpm add -g node-gyp          # 或: npm install -g node-gyp
   pnpm exec node-gyp install    # 或: npm exec node-gyp install

   # Python 绑定：确保 Python 3.13 在 PATH 中
   python3 --version             # 应显示 3.13.x
   ```

5. **本地运行测试**：
   ```bash
   # C++ 测试
   cmake -B build -S code/backend/compiler -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build --parallel
   cd build && ctest --output-on-failure

   # Node.js 测试
   cd code/backend/compiler && pnpm test

   # Python 测试
   cd code/backend/compiler/tests/ic10/python && pytest -v
   ```
6. **确保代码风格** – 项目使用 `clang-format`（4 空格缩进）。运行：
   ```bash
   clang-format -i <文件>
   ```
7. **推送**并创建指向 `develop` 的 Pull Request。

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
docs: 更新 README 至 v2.0.0
test(ic10): 添加链接器单元测试
ci: 添加 Python 工作流
```

## 代码规范

- **语言**：C++23（合理使用协程、概念、范围库）。
- **风格**：遵循 `.clang-format`（4 空格缩进，花括号不换行，100 列限制）。完整配置见 [`.clang-format`](code/backend/compiler/.clang-format)。
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
    - C++ 测试：`code/backend/compiler/tests/ic10/`
    - Node.js 测试：`code/backend/compiler/tests/ic10/node/__tests__/`
    - Python 测试：`code/backend/compiler/tests/ic10/python/`
- **文档**：公共 API 使用 Doxygen 风格注释，支持双语（`@if zh / @elseif en`）标签。

## 模块概览

| 模块 | 位置 | 说明 |
|:-----|:-----|:-----|
| 词法分析器 | `ic10/lexer/` | 基于状态机的词法分析 |
| 语法分析器 | `ic10/parser/` | 递归下降解析器，AST 构建 |
| 语义分析 | `ic10/semantic/` | 符号表、类型推断、类型检查 |
| 链接器 | `ic10/link/` | 多单元符号合并、跨单元解析 |
| 增量编译 | `ic10/incremental/` | 行/语句级缓存，快速重新编译 |
| 本地化 | `ic10/locals/` | 诊断消息本地化（en-us、zh-hans） |
| 公共库 | `common/` | 共享工具（异步、异常、工具） |

## 拉取请求指南

- 目标分支：`develop`。
- 包含清晰的描述，如果有关联的 Issue 请注明。
- 确保 CI 通过（构建、测试、静态分析）。
- 保持更改聚焦 – 每个 PR 只解决一个功能或 Bug。
- 必要时更新文档（README、行内 Doxygen 注释）。
- 遵循[提交信息规范](#提交信息规范)。

## 使用额外检查构建

- **静态分析**（clang-tidy）：
  ```bash
  cmake -B build -S code/backend/compiler -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_CLANG_TIDY="clang-tidy;--config-file=code/backend/compiler/.clang-tidy"
  cmake --build build
  ```

- **cppcheck**：
  ```bash
  cppcheck --enable=all --suppress=missingIncludeSystem --std=c++23 \
    -I code/backend/compiler/common/include \
    -I code/backend/compiler/IC10/include \
    code/backend/compiler/IC10/src/
  ```

- **地址消毒器**（Linux）：
  ```bash
  cmake -B build-san -S code/backend/compiler -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"
  ```

## 版本管理

本项目遵循[语义化版本 2.0.0](https://semver.org/lang/zh-CN/)。版本号记录在 [VERSION](VERSION) 中。

- **主版本号**：破坏性语法变更或 API 不兼容
- **次版本号**：新功能（链接器、增量编译器、Python 绑定等）
- **修订号**：Bug 修复和小改进

## 有问题？

欢迎发起讨论 Issue，或联系维护者：[edocsitahw@qq.com](mailto:edocsitahw@qq.com)。
