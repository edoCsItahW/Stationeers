# IC10

[Stationeers](https://store.steampowered.com/app/544550/Stationeers/) 游戏内脚本语言 IC10 的语言支持。

<details>
<summary>中文</summary>

## 功能

- **悬停**：指令、寄存器、设备、别名、常量、类型与字段的说明；
- **补全**：指令关键字、设备名称与型号名、标签名，以及 `@type` 类型注解的取值；
- **诊断**：越权访问、槽位不适用等编译期问题；
- **语义着色**：与 VS Code 扩展同一套配色，寄存器、设备、标签、类型各有颜色。

> 伪代码内嵌提示暂未在本插件启用。它依赖行尾的等宽对齐，而 IntelliJ 的行内提示渲染不保证这一点
> ——与其显示一堆对不齐的提示，不如先不显示。VS Code 扩展仍然可以用。

## 安装

在 IDE 中打开 **Settings | Plugins | Marketplace**，搜索 **IC10** 安装并重启，然后打开 `.ic` 文件即可。

语言服务器随插件一起分发（可执行文件约 116 MB，下载压缩后约 40 MB），**不需要**另外安装 Node、
Python 或任何运行时，也**不需要**联网下载任何东西。

## 设置

**Settings | Tools | IC10** 只有一项：语言服务器可执行文件路径。

留空即使用插件自带的那一个，绝大多数情况下都不需要填。只有你自己构建了服务器、或想指定别处的版本时，
才需要指向那个文件。

## 已知限制

- 语言服务器目前**只有 Windows 版**：它依赖的编译器原生模块只提供 Windows 动态库。其他平台上会给出
  提示，可用上面的设置项指向你自己构建的可执行文件；
- 目前只识别 `.ic` 扩展名。

## 反馈

问题与建议请使用仓库的 Issue 模板：<https://github.com/edoCsItahW/Stationeers/issues>

</details>

# IC10

Language support for IC10, the in-game scripting language of
[Stationeers](https://store.steampowered.com/app/544550/Stationeers/).

## Features

- **Hover**: instructions, registers, devices, aliases, constants, types and fields;
- **Completion**: instruction keywords, device names and model names, labels, and the values of a `@type`
  hint;
- **Diagnostics**: compile-time problems such as accesses the device does not allow or slots that do not
  apply;
- **Semantic highlighting**: the same palette as the VS Code extension, so registers, devices, labels and
  types each get their own colour.

> The pseudocode inlay hints are not enabled in this plugin yet. They rely on monospace alignment at the end
> of the line, which IntelliJ's inline hint rendering does not guarantee — a screenful of hints that do not
> line up is worse than none. The VS Code extension still offers them.

## Installation

Open **Settings | Plugins | Marketplace**, search for **IC10**, install, and restart. Then open a `.ic` file.

The language server ships with the plugin (a ~116 MB executable that arrives compressed to roughly 40 MB).
Nothing else is required — no Node, no Python, and no download at runtime.

## Settings

**Settings | Tools | IC10** has a single field: the path to the language server executable.

Leave it empty to use the one bundled with the plugin, which is what almost everyone wants. Fill it in only
if you built the server yourself or want to point the plugin at a different build.

## Known limitations

- The language server is currently **Windows only**, because the compiler native modules it depends on are
  built for Windows. Other platforms get a notification, and the setting above can point at an executable you
  built yourself;
- only the `.ic` extension is recognised today.

## Feedback

Please use the repository's issue templates: <https://github.com/edoCsItahW/Stationeers/issues>

---

<details>
<summary>维护者 / Maintainers</summary>

构建配置：Java 25 工具链、IntelliJ Platform 2026.2.0.1、`sinceBuild 253.0`；源码在 `src/main/java`，
插件描述与扩展点注册在 `src/main/resources/META-INF/plugin.xml`。

**语言服务器可执行文件不入库**（GitHub 单文件上限 100 MB），由 vscode 插件那边的 SEA 构建产出，本工程
构建时复制进分发包的 `bin/`：

```bash
# 1. 产出 exe（在 code/IC10/plugins/vscode/ic10-language-support，需 Node ≥ 26.9）
pnpm run sea:bundle && pnpm run sea:build

# 2. 开发：沙箱里会自动带上 exe（prepareSandbox → plugins/ic10/bin/）
./gradlew runIde

# 3. 打包 / 发布（产物约 43 MB，内含 exe）
./gradlew buildPlugin publishPlugin
```

用 `-Pic10.lspExecutable=<路径>` 可指定别处的 exe。exe 缺失时构建会**明确失败**并提示先跑哪条命令。

两处容易踩的坑：

- **服务器源码改了必须重建 exe**（见 `packages/server/README.md` §6.1）。本插件按 IntelliJ 的渲染能力
  给服务器传 `--hover-renderer=markdown --hover-breaks=html --inlay-hints=off`，而**旧 exe 会静默忽略未知
  开关**，表现就是"新功能没生效"；
- `buildSearchableOptions` 只在 IDE 处于默认界面语言时才能成功，中文环境下会以
  `Locale must be default` 失败；`build.gradle` 里用 `ignoreExitValue = true` 容忍它，代价是本插件的
  设置页不出现在 IDE 的设置搜索结果里（设置页本身仍在 **Settings | Tools | IC10**）。

本插件没有自动化测试：端到端测试在 VS Code 侧（`pnpm test`），IntelliJ 侧靠 `./gradlew runIde` 手工验证
（打开 `.ic` 文件，确认悬停、补全、诊断与语义着色）。`./gradlew verifyPlugin` 需要联网访问 Marketplace。

</details>
