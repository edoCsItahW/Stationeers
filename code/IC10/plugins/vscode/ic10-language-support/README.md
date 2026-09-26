<img src="https://img.remit.ee/api/file/BQACAgUAAyEGAASHRsPbAAEYRcBqbImDpkFUNZuN-IR1ksGO1tMv0AACNyUAAhvBaVf8vAVm0ki34T0E.png" />

# IC10 Language Support

[![License](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc-sa/4.0/)

<details>
   <summary>中文版</summary>

写 IC10，最花时间的往往不是想逻辑，而是在游戏里改一遍、试一遍、再改一遍。这个扩展就是为了把这部分省下来——让你能在 VS Code 里像写普通代码那样写 IC10。

它提供完整的 IC10 语言支持：语法与语义高亮、实时诊断、悬停提示、智能补全、签名帮助、代码格式化，以及中英双语界面。背后是一套原生编译器核心，几千行的脚本写起来也依然跟手。

装好就能用，不需要额外准备任何东西。

## 特性

### 1. 语义高亮
这里的颜色不只是"看起来像代码"，而是由编译器的符号表决定的：

- 寄存器别名与原生寄存器区分着色
- 设备别名与原生设备引用区分着色
- `define` 常量与原始数字字面量区分着色
- 标签、宏调用（`HASH`/`STR`）、类型注解各有独立颜色
- 同一个名字，在声明处和引用处也会有不同的呈现

<img alt="语义高亮" src="https://img.remit.ee/i/xjf8G4KTmBPk" style="padding: 20px;">

> [!TIP]
> 语义高亮的颜色跟随 VS Code 的语义令牌主题。想换成自己喜欢的配色，可以在设置里配置 `editor.semanticTokenColorCustomizations`，比如单独为寄存器别名、设备别名指定前景色或样式。具体写法参考 VS Code 官方文档。

### 2. 实时诊断
每次编辑后立即增量分析，把词法、语法与语义问题分类报在问题面板里——不用等保存，也不用切回游戏。

<img alt="实时诊断" src="https://img.remit.ee/i/WxEIz5IP26cG" style="padding: 20px;">

> [!NOTE]
> IC10 没有公开的官方语言规范，所以编译器的行为是拿 Stationeers 游戏里的实际运行结果一条条比对出来的。如果你遇到了我们还没覆盖到的情况，欢迎到 [Issues](https://github.com/edoCsItahW/Stationeers/issues) 说一声。

### 3. 悬停提示
把鼠标停在任意符号上（别名、标签、常量、指令关键字），就能看到它的类型、值、描述等信息。

<video src="https://img.remit.ee/i/zgXZzeNCjl65"  style="padding: 20px;"></video>


### 4. 智能补全
- 指令关键字补全（支持前缀匹配）
- 操作数补全：按当前指令的操作数类型约束（`typeN`），自动筛选寄存器、设备引用、枚举值、跳转标签
- 设备上下文感知：根据前置设备节点过滤对应的 `LogicType`/`LogicSlot`/`BatchMode`/`SlotIndex`
- `alias` / `define` 专用补全

<video src="https://img.remit.ee/i/3FWX8xvn5i0d" style="padding: 20px;"></video>

### 5. 函数签名帮助
输入空格或逗号后，显示当前指令的参数签名与各操作数位置，并标出你正在输入的那一个。

<video src="https://img.remit.ee/api/file/BAACAgUAAyEGAASHRsPbAAEYRZtqbIbBxWIdlw-xCLLbN2zHE87lbQACDiUAAhvBaVe996a-GINPfj0E.mp4"  style="padding: 20px;"> </video>

### 6. 代码格式化
格式化配置可以从项目根目录的 `.ic.yaml`、`.ic.yml` 或 `.ic.json` 读取。支持的规则：

- 连续 `alias` / `define` 按列对齐
- 行尾注释同组内对齐，与代码之间的间隔可配置
- 标签下指令统一缩进（默认 4 空格或 Tab）
- 连续空行压缩到设定的上限
- 标签前保留的最小空行数可配置
- 激进模式：同一标签作用域内，指令操作数按列对齐

<img alt="格式化" src="https://img.remit.ee/api/file/BQACAgUAAyEGAASHRsPbAAEYRYVqbIXjgdYWiFEGaKZ6r1DcqEZPHwAC-CQAAhvBaVe71pql3gViLj0E.png" style="padding: 20px;">

> [!TIP]
> 如果代码里有 Unicode 字符（中文注释、特殊符号等），建议用等宽字体打开，否则格式化后的对齐位置看起来可能会偏。

### 7. 大文件也跟手
每次编辑只重新分析受影响的部分，几千行的脚本也不会一敲就卡。缓存还会跨会话保留，重新打开同一个文件时不必从头再来一遍。

### 8. 双语界面
支持英语（en-us）和简体中文（zh-hans），诊断信息、悬停提示、补全文档等界面文本都在覆盖范围内，切换后即时生效。

<img alt="本地化" src="https://img.remit.ee/api/file/BQACAgUAAyEGAASHRsPbAAEYRYhqbIXub5wcp_zox88AAQUFM65H80wAAvskAAIbwWlX2n0Lf7hzuKw9BA.png" style="padding: 20px;">

### 9. 类型注解语法
用法是给别名加一条特殊的尾随注释：

```
alias myDevice d0  #: @type DeviceType
```

编译器据此推断操作数的类型，补全和诊断都会更准确。类型提示以 `#:` 开头，后面可以跟 `@type`、`@desc` 等标签。

> [!NOTE]
> 编辑器内置了一份标准库，覆盖常见设备的类型、逻辑槽位与批处理模式。不过游戏里的设备实在太多，目前只收录了一部分。如果你的项目用到了尚未收录的设备，欢迎到 [Issues](https://github.com/edoCsItahW/Stationeers/issues) 提出来，或者直接向[标准库](https://github.com/edoCsItahW/Stationeers/blob/develop/code/IC10/assets/ic/stdLib.ic)提交改动。

## 调试器：下一个版本才有

> [!IMPORTANT]
> 断点、单步执行、查看变量这些调试能力**还没有包含在这个版本里**，它们计划随下一个版本一起提供。

## 依赖

- **VS Code** `1.125.0` 或更高版本
- 不需要再装别的

## 扩展设置

设置位置：`文件 > 首选项 > 设置 > 扩展 > IC10 Language Support`

- `ic10.language`：界面语言，默认 `"en-us"`，可选 `"zh-hans"`。切换后即时生效。

- `ic10.hoverRenderer`：悬停信息的渲染方式，默认 `"svg"`，可选 `"markdown"`。两种风格各有特点，按自己的喜好挑就行。

- `ic10.format.useTab`：使用 Tab 缩进，默认 `false`。为 `true` 时忽略 `indentWidth`。

- `ic10.format.indentWidth`：每级缩进空格数，默认 `4`。

- `ic10.format.spacesBeforeTrailingComments`：行尾注释前的空格数，默认 `2`。

- `ic10.format.maxEmptyLinesToKeep`：保留的最大连续空行数，默认 `1`。

- `ic10.format.minEmptyLinesBeforeLabels`：标签前的最小空行数，`0` 表示无限制，默认 `0`。

- `ic10.format.alignConsecutiveStatements`：连续同类语句按列对齐，默认 `true`。

- `ic10.format.alignTrailingComments`：同组内行尾注释对齐，默认 `true`。

> [!TIP]
> 格式化配置也可以直接写在项目根目录的 `.ic.json`、`.ic.yaml` 或 `.ic.yml` 里，不必去改全局设置。

## 已知问题

1. **超大文件的首次打开**：增量分析让日常编辑很流畅，但第一次打开几千行的文件时，仍然需要一点时间做完整解析。

## 待办事项

- 编译器
    - [ ] 完善标准库，覆盖更多设备类型
    - [ ] 链接器增量支持，进一步降低声明式编辑的延迟

- IDE
    - [ ] 调试器：断点、单步执行与变量查看（下个版本）
    - [ ] 跳转定义 / 查找引用
    - [ ] 重命名符号

## 发布说明

### 2.0.0

跟随 IC10 v3 一起发布。编译器核心升级到 v3，指令与设备元数据也重新整理过，补全、悬停和诊断因此覆盖到更多设备与指令；扩展自身的打包方式同样重做了一遍，装好即用。

完整的版本历史见 [CHANGELOG.md](./CHANGELOG.md)。

</details>

---

IC10 is the assembly-style language behind the programmable logic chips in [Stationeers](https://store.steampowered.com/app/544550/Stationeers/). Writing it usually means a lot of trial and error inside the game — this extension exists to shorten that loop, so you can write IC10 in VS Code the way you'd write any other code.

It brings a complete IC10 language service to VS Code: syntax and semantic highlighting, real-time diagnostics, hover tooltips, intelligent completion, signature help, code formatting, and a bilingual interface. A native compiler core does the heavy lifting, so even scripts thousands of lines long stay responsive.

Everything works out of the box — there's nothing extra to install.

## Features

### 1. Semantic Highlighting
The colors here aren't just code-shaped — they come from the compiler's symbol table:

- Register aliases are colored apart from native registers
- Device aliases are colored apart from native device references
- `define` constants are colored apart from raw numeric literals
- Labels, macro calls (`HASH`/`STR`), and type annotations each get their own color
- The same name renders differently where it's declared and where it's used

<img alt="semantic" src="https://img.remit.ee/i/xjf8G4KTmBPk" style="padding: 20px;">

> [!TIP]
> Semantic colors follow VS Code's semantic token theming. To use your own palette, configure `editor.semanticTokenColorCustomizations` — for example to give register or device aliases a custom foreground color or style. See the VS Code documentation for the exact syntax.

### 2. Real-time Diagnostics
Every edit is re-analyzed straight away, with lexical, syntax, and semantic problems sorted into the Problems panel — no save required, no trip back to the game.

<img alt="diagnostic" src="https://img.remit.ee/i/WxEIz5IP26cG" style="padding: 20px;">

> [!NOTE]
> IC10 has no public official language specification, so the compiler's behavior is validated case by case against how Stationeers actually runs it in game. If you run into something we haven't covered, tell us in [Issues](https://github.com/edoCsItahW/Stationeers/issues).

### 3. Hover Tooltips
Hover any symbol — alias, label, constant, instruction keyword — to see its type, value, description, and more.

<video src="https://img.remit.ee/i/zgXZzeNCjl65"  style="padding: 20px;"></video>

### 4. Intelligent Completion
- Instruction keyword completion with prefix matching
- Operand completion: registers, device references, enum values, and jump targets filtered by the current instruction's operand type constraints (`typeN`)
- Device context awareness: `LogicType`/`LogicSlot`/`BatchMode`/`SlotIndex` filtered by the preceding device node
- Dedicated `alias` / `define` directive completion

<video src="https://img.remit.ee/i/3FWX8xvn5i0d" style="padding: 20px;"></video>

### 5. Function Signature Help
Type a space or a comma and you get the current instruction's parameter signature with every operand position, highlighting the one you're on.

<video src="https://img.remit.ee/api/file/BAACAgUAAyEGAASHRsPbAAEYRZtqbIbBxWIdlw-xCLLbN2zHE87lbQACDiUAAhvBaVe996a-GINPfj0E.mp4"  style="padding: 20px;"> </video>

### 6. Code Formatting
Formatting options can be read from `.ic.yaml`, `.ic.yml`, or `.ic.json` in the project root. Available rules:

- Consecutive `alias` / `define` column alignment
- Trailing comments aligned within a group, with configurable spacing from the code
- Uniform indentation for instructions under a label (4 spaces or Tab by default)
- Runs of blank lines collapsed to a configurable maximum
- Configurable minimum blank lines before labels
- Aggressive mode: instruction operands aligned by column within the same label scope

<img alt="formatting" src="https://img.remit.ee/api/file/BQACAgUAAyEGAASHRsPbAAEYRYVqbIXjgdYWiFEGaKZ6r1DcqEZPHwAC-CQAAhvBaVe71pql3gViLj0E.png" style="padding: 20px;">

> [!TIP]
> If your code contains Unicode characters (Chinese comments, special symbols), use a monospaced font — otherwise the alignment formatting produces can look off.

### 7. Stays Responsive on Large Files
Only the parts touched by your edit get re-analyzed, so multi-thousand-line scripts don't stutter as you type. The cache also carries over between sessions, so reopening a file doesn't start from scratch.

### 8. Bilingual Interface
English (en-us) and Simplified Chinese (zh-hans), covering diagnostics, hover tooltips, completion documentation, and the rest of the interface text. Switching takes effect immediately.

<img alt="locale" src="https://img.remit.ee/api/file/BQACAgUAAyEGAASHRsPbAAEYRYhqbIXub5wcp_zox88AAQUFM65H80wAAvskAAIbwWlX2n0Lf7hzuKw9BA.png" style="padding: 20px;">

### 9. Type Annotation Syntax
Annotate an alias with a special trailing comment:

```
alias myDevice d0  #: @type DeviceType
```

The compiler infers the operand's type from it, which makes completion and diagnostics noticeably more precise. Type hints start with `#:` and accept tags such as `@type` and `@desc`.

> [!NOTE]
> A standard library ships built in, covering common device types, logic slots, and batch modes. Stationeers has rather a lot of devices, though, and only some of them are covered so far. If your project uses one that isn't, bring it up in [Issues](https://github.com/edoCsItahW/Stationeers/issues) or send the change straight to the [standard library](https://github.com/edoCsItahW/Stationeers/blob/develop/code/IC10/assets/ic/stdLib.ic).

## Debugger: Coming in the Next Version

> [!IMPORTANT]
> Breakpoints, stepping, and variable inspection are **not part of this release**. They're planned for the next version.

## Requirements

- **VS Code** `1.125.0` or newer
- Nothing else to install

## Extension Settings

Location: `File > Preferences > Settings > Extensions > IC10 Language Support`

- `ic10.language`: interface language, default `"en-us"`, also accepts `"zh-hans"`. Takes effect immediately.

- `ic10.hoverRenderer`: how hover information is rendered, default `"svg"`, also accepts `"markdown"`. Each style has its own character — pick whichever you prefer.

- `ic10.format.useTab`: use Tab for indentation, default `false`. When `true`, `indentWidth` is ignored.

- `ic10.format.indentWidth`: spaces per indentation level, default `4`.

- `ic10.format.spacesBeforeTrailingComments`: spaces before trailing comments, default `2`.

- `ic10.format.maxEmptyLinesToKeep`: maximum consecutive empty lines to keep, default `1`.

- `ic10.format.minEmptyLinesBeforeLabels`: minimum empty lines before labels, `0` means no restriction, default `0`.

- `ic10.format.alignConsecutiveStatements`: align consecutive statements of the same type by column, default `true`.

- `ic10.format.alignTrailingComments`: align trailing comments within the same group, default `true`.

> [!TIP]
> Formatting can also be configured from a `.ic.json`, `.ic.yaml`, or `.ic.yml` file at the project root, instead of changing global settings.

## Known Issues

1. **Opening very large files for the first time**: incremental analysis keeps day-to-day editing smooth, but the initial full parse of a multi-thousand-line file still takes a moment.

## Todo

- Compiler
    - [ ] Expand standard library coverage for more device types
    - [ ] Incremental linker support to further reduce latency for declarative edits

- IDE
    - [ ] Debugger: breakpoints, stepping, and variable inspection (next version)
    - [ ] Go to definition / find references
    - [ ] Rename symbol

## Release Notes

### 2.0.0

Released alongside IC10 v3. The compiler core is now v3 and the instruction and device metadata has been reorganized, so completion, hover, and diagnostics reach more devices and instructions than before. The extension's own packaging was reworked as well — it installs and just works.

Full version history is in [CHANGELOG.md](./CHANGELOG.md).
