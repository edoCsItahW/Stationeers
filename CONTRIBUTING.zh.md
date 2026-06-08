## 为 IC10 编译器做贡献

感谢您对本项目的兴趣！本文档概述了贡献的流程和指南。

### 行为准则

参与本项目即表示您同意营造一个尊重和包容的环境。请向项目维护者举报不当行为。

### 贡献方式

1. **报告 Bug** – 使用 GitHub Issues，描述问题、重现步骤和环境信息。
2. **提出增强建议** – 新建标记为 `enhancement` 的 Issue，清晰描述建议的功能。
3. **提交拉取请求** – 遵循以下流程。

### 开发工作流

1. **复刻（Fork）** 仓库。
2. **从 `develop` 分支创建功能分支**（紧急修复可从 `main` 创建）：
   ```bash
   git checkout -b feature/你的功能名
   ```
3. **进行修改** – 保持提交逻辑清晰，撰写明确的提交信息。
4. **本地运行测试**：
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   cd build && ctest --output-on-failure
   ```
5. **确保代码风格** – 项目使用 `clang-format`。运行：
   ```bash
   clang-format -i <文件>
   ```
6. **推送**并创建指向 `develop` 的 Pull Request。

### 代码规范

- **语言**：C++23（合理使用协程、概念、范围库）。
- **风格**：遵循现有模式（缩进 2 个空格，花括号不换行）。参见 `.clang-format`（如有）。
- **命名**：
    - 类型：`PascalCase`
    - 函数/变量：`camelCase` 或 `snake_case` – 与周围代码保持一致。
    - 宏：`UPPER_SNAKE_CASE`（尽量少用宏）。
- **错误处理**：使用 `std::expected` 还是异常？本项目在解析中使用自定义 `Error` 层次结构和异常。请遵循 `parser.cpp` 中的模式。
- **国际化**：所有面向用户的字符串必须通过 `Loc::msg<MsgId::...>` 或 `Loc::msgFormat<...>` 处理。
- **测试**：在 `tests/ic10/` 中为新功能添加单元测试。

### 拉取请求指南

- 目标分支：`develop`。
- 包含清晰的描述，如果有关联的 Issue 请注明。
- 确保 CI 通过（构建、测试、静态分析）。
- 保持更改聚焦 – 每个 PR 只解决一个功能或 Bug。
- 必要时更新文档（README、行内注释）。

### 使用额外检查构建

- **静态分析**：
  ```bash
  cppcheck --enable=all --suppress=missingIncludeSystem --std=c++23 -I common/include -I IC10/include src/
  ```
- **地址消毒器**（Linux）：
  ```bash
  cmake -B build-san -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address"
  ```

### 有问题？

欢迎发起讨论 Issue，或联系维护者：[2207150234@st.sziit.edu.cn](mailto:2207150234@st.sziit.edu.cn)。