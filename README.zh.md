# IC10 编译器

一个现代 C++23 编写的 **IC10** 编译器。IC10 是游戏《Stationeers》中使用的类汇编脚本语言。  
本项目提供词法分析、语法分析、语义分析，并内置了基于协程的异步基础设施。

---

## 特性

- **词法分析器** – 将 IC10 源码转换为 Token（支持寄存器、设备、数字、字符串、注释、关键字）。
- **语法分析器** – 构建抽象语法树（AST），覆盖所有 IC10 指令（0 元到 6 元指令）。
- **语义分析器** – 使用 `Promise`/`Future` 异步符号表进行符号解析和类型检查。
- **错误报告** – 带源码位置的详细错误信息，支持多语言（英文/简体中文）。
- **异步协程基础设施** – 自定义 `Task<T>`、`Promise<T>`、`Future<T>` 及协程状态管理，实现非阻塞符号解析。
- **跨平台** – Linux（GCC/Clang）和 Windows（MSVC）。
- **测试** – 基于 GoogleTest 的词法/语法分析单元测试。
- **CI/CD** – GitHub Actions 工作流：构建、测试、静态分析（cppcheck、clang‑tidy）。

---

## 项目结构

```
IC10_Compiler/
├── CMakeLists.txt                 # 顶层 CMake
├── common/                        # 公共基础库
│   ├── include/common/async/      # 协程任务、Promise、Future
│   ├── include/common/exception/  # 异常层次
│   ├── include/common/locals/     # 国际化框架
│   └── include/common/utils/      # FStr, BiMap, enum_to_str, 类型列表等
├── IC10/                          # IC10 编译器核心
│   ├── include/ic10/lexer/        # Token 和词法分析器
│   ├── include/ic10/parser/ast/   # AST 节点定义
│   ├── include/ic10/semantic/     # 符号表、语义分析器
│   ├── include/ic10/locals/       # 语言资源（英文、简体中文）
│   ├── src/                       # 实现文件
│   └── main.cpp                   # 命令行入口
├── tests/                         # 单元测试（GoogleTest）
│   ├── common/                    # 测试工具
│   └── ic10/                      # 词法/语法分析测试
├── .github/workflows/             # CI 脚本
```

---

## 构建要求

- **CMake** 3.28.1 或更高
- **C++23** 编译器：
    - Linux：GCC 13+ 或 Clang 16+
    - Windows：MSVC 2022（含 Visual Studio 2022 生成工具）
- **Ninja**（推荐）或 Make
- **Git**（用于获取 GoogleTest）

---

## 构建步骤

### 1. 克隆仓库

```bash
git clone https://github.com/yourusername/IC10_Compiler.git
cd IC10_Compiler
```

### 2. 使用 CMake 配置

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

在 Windows 上（MSVC）可能需要指定生成器：

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
```

### 3. 编译

```bash
cmake --build build --parallel
```

可执行文件 `IC10_Compiler` 将生成在 `build/bin/` 目录下。

### 4. 运行测试

```bash
cd build
ctest --output-on-failure
```

---

## 使用方法

```bash
./IC10_Compiler <源码.ic>
```

目前 `main.cpp` 中硬编码了源文件路径（指向一个测试文件）。  
你可以修改 `main.cpp` 以接受命令行参数。

IC10 程序示例：

```
alias counter r0
define PI 3.14159

start:
    move r0 10
    add r1 r0 PI
    hcf
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
