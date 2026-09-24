# IC10

[English](README.md)

![Version](https://img.shields.io/badge/version-3.0.0-blue)
![C%2B%2B](https://img.shields.io/badge/C%2B%2B-23-blue)
![License](https://img.shields.io/badge/license-CC%20BY--NC--SA%204.0-lightgrey)
![VS Code](https://img.shields.io/badge/VS%20Code-edocsitahw.ic10-007ACC)

<!--
  ===== 图片位 =====
  在此插入横幅图与功能截图（HTML 注释不会渲染，替换成下面这种写法即可）：

  <p align="center">
    <img src="code/IC10/plugins/vscode/ic10-language-support/static/你的图.png" alt="IC10" width="720">
  </p>
  ==================
-->

**IC10** 是你在 [Stationeers](https://store.steampowered.com/app/544550/Stationeers/) 里给基地编程用的汇编级语言 —— 寄存器、设备、没有类型、没有补全，游戏内的报错基本只有一个红叉。

这个项目就是让写 IC10 变得舒服的那套工具链：一个真正读懂这门语言的 C++23 编译器、一个能单步调试的运行时，以及一个把诊断、补全、悬停和格式化直接摆在你代码旁边的编辑器扩展。

```ic10
alias sensor d0
define TARGET 300

main:
    l r0 sensor Temperature
    blt r0 TARGET heating_off
    s sensor On 1
    j main

heating_off:
    s sensor On 0
    j main
```

---

## 你能得到什么

- **一个懂 IC10 的编辑器** —— 装上 [IC10 扩展](https://marketplace.visualstudio.com/items?itemName=edocsitahw.ic10)，打开 `.ic` 文件就行。高亮、实时诊断、悬停、补全、签名帮助、格式化，还能调试，完全不需要本地工具链。
- **一个会告诉你哪里错了的编译器** —— 词法 → 语法 → 语义 → 链接，带真实源码位置、分级诊断和双语消息，另有增量模式让你边打字边得到反馈。
- **能接进你自己工具的绑定** —— 用 **Node.js**、**Python** 或 **Java** 调用同一套编译器，或者用 `ic10c` 命令行把 token、AST、符号表导成 JSON。
- **一个可以对着调试的运行时** —— `ic10r-node` 能带着模拟设备执行 IC10，这也是扩展能单步运行你程序、而不只是静态检查的原因。

---

## 特性

**编译器**

- 覆盖全部 IC10 指令元数（0 元到 6 元）、寄存器 `r0`–`r15`、设备 `d0`–`d5`、多进制数字（`$hex`、`%bin`、十进制、浮点）、字符串、`#`/`//` 注释、`alias`/`define` 预处理指令以及文档注释注解（`#:`、`#>`）。
- 语义分析包含符号解析、类型推导与类型检查，符号表基于异步协程，前向引用无需额外处理。
- 完整的链接器：多编译单元合并、跨单元前向引用、符号可见性、循环依赖检测。
- 增量编译：行级词法缓存 + 语句级语法缓存，只重新处理改动的部分。
- 诊断支持英文与简体中文，所有面向用户的消息都走本地化层。

**编辑器扩展**

| | |
|---|---|
| 语法与语义高亮 | 着色直接来自编译器符号表，因此别名与裸寄存器一眼可辨 |
| 实时诊断 | 每次编辑增量重分析，在问题面板中分类展示 |
| 悬停与补全 | 悬停显示类型、值与描述；指令与操作数补全按设备上下文过滤 |
| 签名帮助与格式化 | 高亮当前参数；格式化可通过 `.ic.yaml` / `.ic.yml` / `.ic.json` 配置 |
| 调试器 | 以 `program` 启动、`stopOnEntry` 停住，可下断点、查看模拟机器状态 |
| 类型注解 | `#:` 类型提示与 `#>` 文档注释，用于声明设备与枚举类型 |

**运行时、绑定与元数据**

- `ic10r-node` —— 执行引擎，自带 N-API 绑定。
- `ic10c-node`（Node.js）、`ic10c_python`（Python）、`io.github.stationeers:ic10`（Java）暴露同一套编译阶段。
- [`@ic10/metadata`](https://www.npmjs.com/package/@ic10/metadata) —— 指令、枚举、设备与 Stationpedia 文本做成的带类型包，让工具链始终跟得上游戏。

---

## 快速开始

### 在 VS Code 里（推荐）

```bash
code --install-extension edocsitahw.ic10
```

然后打开任意 `.ic` 或 `.ic10` 文件即可。扩展已内置预编译的编译器与运行时原生模块 —— 不需要安装 C++ 工具链、CMake 或编译器。

### 在终端里

从 [Releases](https://github.com/edoCsItahW/Stationeers/releases) 下载 `ic10c`，然后：

```bash
ic10c main.ic                # 编译，输出符号表 JSON
ic10c --emit-ast main.ic     # 查看语法树
ic10c --link a.ic b.ic       # 链接多个单元
```

退出码在脚本里很好用：`0` 成功，`1` 命令行或文件错误，`2` 编译器产生了警告或错误。

### 在你自己的代码里

```typescript
import { Lexer, Parser, Analyser, IC10Local } from 'ic10c-node';

IC10Local.setLanguage('zh-hans');            // 可选，默认英文

const tokens = Lexer.tokenize(source);
const program = new Parser(tokens, false).parse();

const analyser = new Analyser();
await analyser.visit(program);               // 异步：前向引用由协程解析

console.log(analyser.symbolTable.toJSON(), analyser.diagnostics);
```

```python
from ic10c_python import Lexer, Parser, Analyser

tokens = Lexer.tokenize(source)
program = Parser(tokens, False).parse()

analyser = Analyser()
analyser.visit(program)
print(analyser.symbolTable.toJSON())
```

链接与增量编译也是同样的暴露方式 —— 参见 [Node.js 包](code/IC10/backend/compiler/publish/node)里的 `Linker`、`IncCompiler`，或
[Python 存根](code/IC10/backend/compiler/publish/python/ic10c_python.pyi)。

---

## 文档

| 位置 | 内容 |
|---|---|
| [IC10 扩展 README](code/IC10/plugins/vscode/ic10-language-support/README.md) | 编辑器扩展的每一项设置与功能 |
| [DEVELOPER_GUIDE.zh.md](DEVELOPER_GUIDE.zh.md) | 架构、构建、测试、发布 —— 给改这个仓库的人看 |
| [CONTRIBUTING.zh.md](CONTRIBUTING.zh.md) | Issue、PR 与提交约定 |
| [CHANGELOG.zh.md](CHANGELOG.zh.md) | 逐个版本记录改了什么 |
| [docs/annotationExample.md](docs/annotationExample.md) | `#:` / `#>` 注解语法示例 |
| [docs/grammar/](docs/grammar/) | IC10 语法，按词法/语法与指令元数拆分 |

---

## 从源码构建

你需要 CMake ≥ 3.28.1 和一个 C++23 编译器；如果用 bash 脚本还需要 `jq`。其余交给脚本：

```bash
git clone https://github.com/edoCsItahW/Stationeers.git && cd Stationeers
code/scripts/bashShell/buildIC10CompilerCore.sh      # Windows: pwsh code/scripts/powerShell/BuildIC10CompilerCore.ps1
```

这一条命令会完成配置、构建并跑完 C++ 测试。Node.js、Python、Java、运行时与命令行目标，以及工具链版本、CMake 选项和发布流程，都在
**[开发者指引](DEVELOPER_GUIDE.zh.md)** 里。

---

## 国际化

编译器消息提供英文（`en-us`，默认）与简体中文（`zh-hans`）：

```cpp
IC10Local::setLanguage("zh-hans");     // C++
```
```typescript
IC10Local.setLanguage('zh-hans');      // Node.js
```

---

## 版本管理

本项目遵循[语义化版本](https://semver.org/lang/zh-CN/)。`VERSION` 当前为 IC10 核心 **3.0.0**、VS Code 扩展 **1.0.2**；最近的标签是 `v2.6.6`。

- **v3.0.0** —— IC10 v3 重构：重写词法与语法分析、AST 节点按元数统一，调整 `annotation` 与 `link` 语法规则，新增可调试的执行运行时、Java 绑定，元数据独立为 `@ic10/metadata`。
- **v2.x** —— 链接器、增量编译器、类型推导、注解语法、Python 绑定，以及随游戏更新而来的语法变更。
- **v1.0.x** —— 首次发布：词法分析器、语法分析器、语义分析与 Node.js 绑定。

完整历史见 [CHANGELOG.zh.md](CHANGELOG.zh.md)。

---

## 许可证

**CC BY-NC-SA 4.0** —— 详见 [LICENSE](LICENSE)。如需商用请先联系作者。

---

## 作者与致谢

**edocsitahw** —— [edocsitahw@qq.com](mailto:edocsitahw@qq.com)

感谢 [Stationeers](https://stationeers.com/) 提供了值得为它写编译器的这门语言；也感谢
[node-addon-api](https://github.com/nodejs/node-addon-api)、[pybind11](https://github.com/pybind/pybind11) 与
[fbjni](https://github.com/facebookincubator/fbjni)，让这些语言绑定成为可能。
