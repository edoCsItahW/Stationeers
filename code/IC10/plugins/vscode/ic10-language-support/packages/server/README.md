# 打包语言服务端可执行文件

<details>
<summary>中文</summary>

# 打包语言服务端可执行文件

> 这份说明只解决一件事：把 `packages/server` 打包成一个**独立可执行文件**（`ic10-lsp.exe`），供
> **JetBrains 插件**通过标准 LSP（`--stdio`）启动。它**不是** VS Code 扩展的打包说明 ——
> 扩展的产物是 VSIX，构建链是 `pnpm run compile` + `pnpm run bundle`（同一个 `dist/server.js`），
> 两者互不依赖。
>
> JetBrains 侧对应 `code/IC10/plugins/jetbrains/ic10-language-support` 里的
> `IC10LspClientDescriptor#createCommandLine()`，它执行的就是 `ic10-lsp --stdio`。

`--stdio` 由 `vscode-languageserver` 自己识别（`createConnection()` 会读 `process.argv`：`--stdio`、`--node-ipc`、`--socket=`、`--pipe=`），所以可执行文件**不需要**自己解析参数，只要能被这样启动即可。

不过**呈现方式**是另一回事：服务器默认按 VS Code 的渲染能力出内容（SVG 悬停卡片、行尾双空格的 Markdown
换行、行尾等宽对齐的伪代码内联提示），而 JetBrains 的 LSP 客户端这些都用不出来，且没有 VS Code 那条
`workspace/configuration` 通道来推设置。所以这些差异由服务器自己的开关声明，**启动方**（JetBrains 插件的
`createCommandLine()`）负责传：

| 开关 | 作用 |
| --- | --- |
| `--hover-renderer=markdown` | 悬停出原生 Markdown；默认 `svg`，即一张 data-URL 图片 |
| `--hover-breaks=html` | Markdown 的硬换行写成行内 `<br />`；默认行尾双空格。IntelliJ 的 quick doc 会逐行 `trimEnd`，双空格到不了解析器（两行会并成一段），而它放行 `<br />`；VS Code 的 markdown-it 反过来（关了行内 HTML） |
| `--inlay-hints=off` | 不向客户端声明 `inlayHintProvider`；默认声明 |

不传开关时行为与从前完全一致——VS Code 走的正是这条路，因此**它不应传这两个开关**：命令行声明会盖过
`ic10.hoverRenderer` 配置。取值非法时不会静默：写一条 stderr 警告，并保持默认值。

## 0. 先构建工作空间里的 bundle

包间引用已是 pnpm 工作空间的 `workspace:*`（旧的 verdaccio + `file:../common` 已废弃），所以直接：

```bash
pnpm install
pnpm run compile        # tsc -b
pnpm run bundle         # esbuild → packages/server/dist/server.js
```

产物 `packages/server/dist/server.js` 约 **895 KB**、**相对 `require` 为 0 处**（已自包含），但仍保留了
20 处外部 `require`：

| 外部依赖 | 出现次数 | 为什么保留 |
| --- | --- | --- |
| `@ic10/compiler` | 12 | 主入口就是原生模块 `src/ic10c-node.node`，esbuild 无法内联 `.node` |
| `@ic10/metadata` | 7 | 源码用拼接出来的 specifier 动态 `require`，esbuild 无法静态解析 |
| `@ic10/runtime` | 1 | 原生模块 `src/ic10r-node.node`，服务端 hover 用它算 `HASH` / `STR` |

## 1. 两种方案对这三类依赖的处理方式不同

| 依赖 | 方案 A：SEA + VFS（推荐） | 方案 B：SEA + assets / pkg |
| --- | --- | --- |
| `@ic10/metadata`（拼接出来的 `require`） | 把 `node_modules/@ic10/metadata/**` 放进 VFS，**运行时正常解析** | esbuild 静态分析不了它，只能把用到的子路径当 asset 嵌入后**手工 patch**，或把源码那处拼接改成静态映射 |
| `@ic10/compiler` / `@ic10/runtime`（原生模块） | VFS 里的 `.node` 不能直接 dlopen，仍需**写到临时文件再 `process.dlopen`**（文档明确此限制） | 同左 |
| `@ic10/compiler/src/stdLib.ic`（被当文件读） | VFS 内是真实路径，`fs.readFileSync` 直接可读 | 解包成临时文件后 patch `require.resolve` 的结果 |

定位那处拼接引用：`grep -n '_DescriptionSolver.ROOT' packages/server/bundle.js`。

## 2. 必须随可执行文件提供的运行时资产

| 资产 | 体积 | 用途 | 在仓库中的路径 |
| --- | --- | --- | --- |
| `ic10c-node.node` | 约 2.7 MB | 编译器原生模块（`@ic10/compiler` 的主入口） | `node_modules/@ic10/compiler/src/ic10c-node.node` |
| `ic10r-node.node` | 约 1.7 MB | 运行时原生模块（hover 的 `HASH` / `STR`） | `node_modules/@ic10/runtime/src/ic10r-node.node` |
| `stdLib.ic` | 约 929 KB | 标准库源文件，运行时经 `require.resolve` + 读取文件 | `node_modules/@ic10/compiler/src/stdLib.ic` |
| `@ic10/metadata/dist/**` | 按需（整包约 6.2 MB） | 供那处拼接出来的动态 `require` 解析 | `node_modules/@ic10/metadata/dist/**` |

## 3. 方案 A：Node SEA + VFS（推荐）

`useVfs` 是 Node **≥ 26.9** 的能力：资源作为**只读虚拟文件系统**暴露给 `node:fs`，而且**注入的主脚本本身也从 VFS 根目录执行** —— 于是 `__dirname` 落在 VFS 内，`require()` 支持相对路径与 **`node_modules` 包查找**。这正好消掉了方案 B 里的两处手工 patch。

内置的 `node --build-sea` 需要 Node **≥ 25.5**（不再需要 postject）。

### 3.1 生成 assets（仓库已备好脚本）

`assets` 需要逐条列出文件（metadata 一棵目录就有 49 个），所以交给脚本扫描生成，别手写：

| 文件 | 作用 |
| --- | --- |
| `scripts/sea-config.mjs` | 扫描三个运行时外部包，生成 `packages/server/sea-config.json`（当前 54 个 assets） |
| `packages/server/sea-entry.cjs` | SEA 入口：解包两个 `.node` → `process.dlopen` → 重定向包名 → 加载 `dist/server.js` |
| `packages/server/sea-config.json` | 生成物（已提交，改动可 review）：`main` / `output` / `useVfs` / `assets` |

```bash
pnpm run sea:config      # 生成 sea-config.json（依赖变化后重跑）
pnpm run sea:bundle      # esbuild：sea-entry.cjs → packages/server/bundle.js
pnpm run sea:build       # node --build-sea packages/server/sea-config.json → ic10-lsp.exe
```

三条命令都在**扩展根目录**执行（配置里的相对路径按当前工作目录解析）。生成的配置形如：

```json
{
    "main": "./packages/server/bundle.js",
    "output": "./packages/server/ic10-lsp.exe",
    "disableExperimentalSEAWarning": true,
    "useVfs": true,
    "assets": {
        "node_modules/@ic10/compiler/src/stdLib.ic": "node_modules/@ic10/compiler/src/stdLib.ic",
        "node_modules/@ic10/metadata/dist/locals/enums.js": "node_modules/@ic10/metadata/dist/locals/enums.js"
    }
}
```

### 3.2 入口与原生模块 shim

入口是仓库里的 `packages/server/sea-entry.cjs`（**不要手工改 bundle**）：从 VFS 读出两个 `.node`、写到临时文件、`process.dlopen`，再用 `Module._load` 钩子把这两个包名指向载入出来的 API，最后 `require("./dist/server.js")`。

```js
const compiler = loadAddon("node_modules/@ic10/compiler/src/ic10c-node.node");
const runtime = loadAddon("node_modules/@ic10/runtime/src/ic10r-node.node");

const load = Module._load;
Module._load = function (request, parent, isMain) {
    if (request === "@ic10/compiler") return compiler;
    if (request === "@ic10/runtime") return runtime;
    return load.apply(this, arguments);
};

require("./dist/server.js");
```

`pnpm run sea:bundle` 会把它打成单文件（`require("./dist/server.js")` 被内联），并保留两个原生模块 external。

### 3.3 构建

`packages/server/sea-config.json` 由 `pnpm run sea:config` 生成（三条命令见 §3.1），最终：

```bash
pnpm run sea:build       # 等价于 node --build-sea packages/server/sea-config.json
```

实测到的体积：`bundle.js` 约 **3.8 MB**（`@ic10/metadata` 的字面量 `require` 已被内联），assets 约
11 MB（两个原生模块 4.4 MB + metadata 6.4 MB）；但 **exe 实测 121,432,552 字节（约 116 MiB）**
—— SEA 会把整个 Node 运行时一起嵌入，体积主要由它决定，别按 assets 总量估算产物大小。

约束与注意：

- **先确认 Node 版本**：`node --version` 需 **≥ 26.9** 才有 `useVfs`，`--build-sea` 需 **≥ 25.5**（`node --help` 里能搜到即支持）；低于 26.9 就用 §4 的方案 B；
- `"useVfs": true` **不能**与 `"useSnapshot"` / `"useCodeCache"` 同开（因此没有启动加速，属已知取舍）；
- `"vfsArchive"` 要求 `"useVfs": true`，且不能与 `"assets"` 同时使用；
- 生成跨平台 exe 时必须关掉 `useCodeCache` / `useSnapshot`；
- 注入的 Node 版本必须与生成 blob 的 Node 版本一致；
- **已在实机验证（Node 26.11.1 生成的 exe）**：VFS 下 `require.resolve("@ic10/compiler/src/stdLib.ic")`
  正常解析，metadata 那处拼接出来的 `require` 也能解析（`initialize` 后请求 hover / 补全 / 伪代码提示
  都有正常响应），所以当前实现**没有**用 `Module._resolveFilename` 钩子。若将来失效，退路是补该钩子，
  或按 `node_modules/@ic10/compiler/src/stdLib.ic` 在 VFS 内的路径直接读取。

## 4. 方案 B：SEA + assets（或 pkg）——退路

Node < 26.9 拿不到 VFS，就退回老办法：逐条列 `"assets"`，并承担两处 patch。

```json
{
    "main": "./bundle.js",
    "output": "ic10-lsp.exe",
    "disableExperimentalSEAWarning": true,
    "assets": {
        "ic10c-node.node": "./node_modules/@ic10/compiler/src/ic10c-node.node",
        "ic10r-node.node": "./node_modules/@ic10/runtime/src/ic10r-node.node",
        "stdLib.ic": "./node_modules/@ic10/compiler/src/stdLib.ic"
    }
}
```

- 先按 §1 打一遍只留原生模块 external 的 bundle（把能内联的纯 JS 依赖内联掉）；
- 用 `sea.getRawAsset()` 取出两个 `.node` 写到临时文件后 `process.dlopen`，并把 bundle 里的
  `require("@ic10/compiler")` / `require("@ic10/runtime")` 换成载入出来的 API；
- 把 `stdLib.ic` 解包到临时文件，再把 `require.resolve(...)` 的结果指过去（注意是**新包名与新文件名**
  `stdLib.ic`，不再是旧包名 `ic10c_node/static/stdLib.ic.json`）；
- 那处拼接的 metadata 引用同样要作为 asset 嵌入后指过去，或改成静态映射。

不想用 SEA 时，`@yao-pkg/pkg` 依然可用（把三个文件放在 exe 同目录，`--external` 保留原生模块），
代价是 pkg 需要下载 Node 运行时，实测以小时计：

```bash
pnpm exec pkg packages/server/bundle.js --targets node26-win-x64 --output ic10-lsp.exe
```

## 5. 验证

用一条最小 `initialize` 请求喂给它，能收到响应就说明 stdio 通了（Content-Length 是字节数）：

```bash
printf 'Content-Length: 58\r\n\r\n{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | ic10-lsp.exe --stdio
```

实测结果（Node 26.11.1 生成的 exe）：stdout 先出一条 `window/logMessage`（`Server is running...`），
随后 `initialize` 返回完整能力表（含 `"inlayHintProvider":true`）；再发 `textDocument/didOpen` 后，
`textDocument/inlayHint` / `hover` / `completion` 均返回正常结果 —— 说明编译器原生模块、`stdLib.ic`
与 metadata 的那处动态引用在 VFS 内都已就位。

手工喂协议时有两个坑，别误判成 exe 的问题：

- **不要把 `exit` 和 `initialize` 一次灌进管道**：`exit` 会立即 `process.exit(0)`，异步的 `initialize`
  还没算完就退了，表现是"只回了 logMessage、没有 initialize 响应"。要按顺序发、中间留出间隔；
- 服务端在 `initialized` 之后会主动发 `client/registerCapability` 与 `workspace/workspaceFolders`
  请求，假客户端若不回应，进程不会自行退出，测试脚本需要自己结束它。

再把它指给 JetBrains 插件（命令名与参数需与 `createCommandLine()` 一致：`ic10-lsp` + `--stdio`），
打开 `.ic` 文件确认 hover / 补全有响应。

## 6. 与扩展打包的关系

同一个 `packages/server/dist/server.js` 服务两条产物线：

| 产物 | 消费者 | 做法 |
| --- | --- | --- |
| VSIX（含 bundle） | VS Code | `compile` + `bundle` + `vsce package`；`@ic10/compiler`/`@ic10/runtime`/`@ic10/metadata` 随 VSIX 发布（vsce 只收集根依赖） |
| `ic10-lsp.exe` | JetBrains | 本说明：§3（推荐）或 §4；JetBrains 插件的 Gradle 构建再把它复制进该插件分发包的 `bin/`，运行期不做任何下载 |

### 6.1 改完服务器代码必须重建，否则静默失效

服务器源码变了，上面两条产物线**都不会自动跟着变**：

| 产物 | 刷新命令 | 忘了的后果 |
| --- | --- | --- |
| `dist/server.js`（VS Code） | `pnpm run compile && pnpm run bundle` | 扩展跑的仍是旧逻辑 |
| `ic10-lsp.exe`（JetBrains） | `pnpm run sea:bundle && pnpm run sea:build` | **静默失效**：旧 exe 不认识新的命令行开关（如 `--hover-renderer=`），新功能看起来"没生效" |

`sea:bundle` 不能省——`sea:build` 只把**已有的** `bundle.js` 封进 exe。另外，JetBrains 侧因为 exe 随插件
分发，服务器改动必须发一个新的插件版本才能到达用户（VS Code 侧同理要发新的 VSIX）。

## 7. 历史做法（已废弃）

- **verdaccio + `file:../common`**：包间引用改由 pnpm 工作空间承担，不再需要本地 registry；
- **手工注入 blob（postject）**：Node ≥ 25.5 起 `node --build-sea` 已内置；
- **旧 esbuild 命令与旧包名**：以前是对 tsc 输出 `./out/server/server.js` 打 `--external:ic10c_node`；现在 `dist/server.js` 已自包含，包名与文件名都变了。

需要旧版全文时：`git log --follow -- packages/server/README.md`。

</details>

# Packaging the Language Server as an Executable

> This document solves exactly one problem: turning `packages/server` into a **standalone executable**
> (`ic10-lsp.exe`) that the **JetBrains plugin** launches over standard LSP (`--stdio`). It is **not** the
> packaging guide for the VS Code extension — that ships a VSIX built with `pnpm run compile` + `pnpm run bundle`
> (the same `dist/server.js`); the two are independent.
>
> On the JetBrains side this corresponds to `IC10LspClientDescriptor#createCommandLine()` in
> `code/IC10/plugins/jetbrains/ic10-language-support`, which runs `ic10-lsp --stdio`.

`--stdio` is handled by `vscode-languageserver` itself (`createConnection()` reads `process.argv` for `--stdio`,
`--node-ipc`, `--socket=`, `--pipe=`), so the executable does not have to parse arguments at all.

**Presentation** is a different matter. By default the server renders for VS Code's capabilities (an SVG hover
card, two-trailing-space Markdown breaks and monospace-aligned pseudocode inlay hints at the line end), but the
JetBrains LSP client can use none of them, and it has no `workspace/configuration` channel to push settings
through. Those differences are declared by the server's own switches, passed by whoever starts it (the JetBrains
plugin's `createCommandLine()`):

| Switch | Effect |
| --- | --- |
| `--hover-renderer=markdown` | Hovers become native Markdown; the default `svg` produces a data-URL image |
| `--hover-breaks=html` | Writes Markdown hard breaks as inline `<br />`; the default is two trailing spaces. IntelliJ's quick doc trims trailing whitespace from every line, so the spaces never reach the parser (the lines merge) while `<br />` passes through — and VS Code's markdown-it is the other way round, having inline HTML disabled |
| `--inlay-hints=off` | `inlayHintProvider` is not advertised; it is by default |

With no switch the behaviour is exactly what it was — the path VS Code takes — so **VS Code must not pass
them**: a command-line declaration overrides the `ic10.hoverRenderer` setting. An invalid value is not
swallowed: it is reported on stderr and the default is kept.

## 0. Build the workspace bundle first

Packages reference each other through the pnpm workspace (`workspace:*`; the old verdaccio + `file:../common`
dance is obsolete), so:

```bash
pnpm install
pnpm run compile        # tsc -b
pnpm run bundle         # esbuild → packages/server/dist/server.js
```

`packages/server/dist/server.js` is about **895 KB** with **zero relative `require`s** (self-contained), but it
still keeps 20 external `require`s:

| External | Occurrences | Why it stays external |
| --- | --- | --- |
| `@ic10/compiler` | 12 | its main entry *is* the native addon `src/ic10c-node.node`; esbuild cannot inline `.node` |
| `@ic10/metadata` | 7 | the source dynamically `require`s computed specifiers esbuild cannot resolve statically |
| `@ic10/runtime` | 1 | native addon `src/ic10r-node.node`, used by the server's hover to evaluate `HASH` / `STR` |

## 1. The two options treat these three kinds of dependency differently

| Dependency | Option A: SEA + VFS (recommended) | Option B: SEA + assets / pkg |
| --- | --- | --- |
| `@ic10/metadata` (a computed `require`) | put `node_modules/@ic10/metadata/**` into the VFS and it **resolves at runtime** | esbuild cannot analyse it: embed the referenced subpaths as assets and **patch by hand**, or rewrite that computed specifier into a static lookup |
| `@ic10/compiler` / `@ic10/runtime` (native addons) | a `.node` inside the VFS still cannot be `dlopen`ed, so it must be **written to a temp file and `process.dlopen`ed** (a documented limitation) | same |
| `@ic10/compiler/src/stdLib.ic` (read as a file) | inside the VFS it is a real path, so `fs.readFileSync` just works | unpack it to a temp file and patch the result of `require.resolve` |

To locate the computed reference: `grep -n '_DescriptionSolver.ROOT' packages/server/bundle.js`.

## 2. Runtime assets the executable must be given

| Asset | Size | Purpose | Path in the repo |
| --- | --- | --- | --- |
| `ic10c-node.node` | ~2.7 MB | compiler native addon (the main entry of `@ic10/compiler`) | `node_modules/@ic10/compiler/src/ic10c-node.node` |
| `ic10r-node.node` | ~1.7 MB | runtime native addon (`HASH` / `STR` in hover) | `node_modules/@ic10/runtime/src/ic10r-node.node` |
| `stdLib.ic` | ~929 KB | standard library source, read at runtime via `require.resolve` | `node_modules/@ic10/compiler/src/stdLib.ic` |
| `@ic10/metadata/dist/**` | as needed (whole package ~6.2 MB) | resolves that computed dynamic `require` | `node_modules/@ic10/metadata/dist/**` |

## 3. Option A: Node SEA + VFS (recommended)

`useVfs` arrived in Node **≥ 26.9**: the bundled assets are exposed as a read-only virtual file system to
`node:fs`, and **the injected main script itself executes from the VFS root** — so `__dirname` points inside the
VFS and `require()` supports relative paths as well as **`node_modules` package lookups**. That removes both
manual patches of option B.

The built-in `node --build-sea` needs Node **≥ 25.5** (postject is no longer required).

### 3.1 Build the assets (the scripts are already in the repo)

`assets` lists files one by one (a single `metadata` directory holds 49 of them), so a script scans and writes
them rather than a human:

| File | Role |
| --- | --- |
| `scripts/sea-config.mjs` | scans the three runtime packages and writes `packages/server/sea-config.json` (54 assets today) |
| `packages/server/sea-entry.cjs` | the SEA entry: unpack both `.node` files → `process.dlopen` → redirect the package names → load `dist/server.js` |
| `packages/server/sea-config.json` | generated (committed, so changes are reviewable): `main` / `output` / `useVfs` / `assets` |

```bash
pnpm run sea:config      # writes sea-config.json (re-run when dependencies change)
pnpm run sea:bundle      # esbuild: sea-entry.cjs → packages/server/bundle.js
pnpm run sea:build       # node --build-sea packages/server/sea-config.json → ic10-lsp.exe
```

All three run from the **extension root** (the relative paths in the config resolve against the current working
directory). The generated config looks like:

```json
{
    "main": "./packages/server/bundle.js",
    "output": "./packages/server/ic10-lsp.exe",
    "disableExperimentalSEAWarning": true,
    "useVfs": true,
    "assets": {
        "node_modules/@ic10/compiler/src/stdLib.ic": "node_modules/@ic10/compiler/src/stdLib.ic",
        "node_modules/@ic10/metadata/dist/locals/enums.js": "node_modules/@ic10/metadata/dist/locals/enums.js"
    }
}
```

### 3.2 Entry point and native-addon shim

The entry lives in the repo at `packages/server/sea-entry.cjs` (**do not hand-edit the bundle**): it reads both
`.node` files from the VFS, writes them to temp files, `process.dlopen`s them, redirects those two package names
through a `Module._load` hook, and finally `require("./dist/server.js")`.

```js
const compiler = loadAddon("node_modules/@ic10/compiler/src/ic10c-node.node");
const runtime = loadAddon("node_modules/@ic10/runtime/src/ic10r-node.node");

const load = Module._load;
Module._load = function (request, parent, isMain) {
    if (request === "@ic10/compiler") return compiler;
    if (request === "@ic10/runtime") return runtime;
    return load.apply(this, arguments);
};

require("./dist/server.js");
```

`pnpm run sea:bundle` bundles it into a single file (inlining `require("./dist/server.js")`) while keeping the
two native addons external.

### 3.3 Build

`packages/server/sea-config.json` is produced by `pnpm run sea:config` (all three commands are in §3.1), so:

```bash
pnpm run sea:build       # equivalent to node --build-sea packages/server/sea-config.json
```

Sizes observed here: `bundle.js` is about **3.8 MB** (the literal `require`s inside `@ic10/metadata` are
inlined), the assets are about 11 MB (4.4 MB of native addons + 6.4 MB of metadata) — but the executable
itself measured **121,432,552 bytes (~116 MiB)**, because SEA embeds the whole Node runtime and that
dominates the size. Do not estimate the artifact from the assets alone.

Constraints and caveats:

- **Check the Node version first**: `node --version` must be **≥ 26.9** for `useVfs`, and `--build-sea` needs
  **≥ 25.5** (it shows up in `node --help`); below 26.9, use option B in §4;
- `"useVfs": true` **cannot** be combined with `"useSnapshot"` / `"useCodeCache"` (so there is no startup
  speed-up — a known trade-off);
- `"vfsArchive"` requires `"useVfs": true` and cannot be combined with `"assets"`;
- when generating a cross-platform executable, `useCodeCache` / `useSnapshot` must be off;
- the injected Node version must match the Node version that generated the blob;
- **verified on a real machine (executable built by Node 26.11.1)**: under the VFS
  `require.resolve("@ic10/compiler/src/stdLib.ic")` resolves normally, and so does the computed metadata
  `require` (hover, completion and the pseudocode inlay hints all answer after `initialize`), so the current
  implementation does **not** need a `Module._resolveFilename` hook. If that ever breaks, the fallback is to
  add the hook or read the file by its VFS path (`node_modules/@ic10/compiler/src/stdLib.ic`) directly.

## 4. Option B: SEA + assets (or pkg) — the fallback

On Node < 26.9 there is no VFS, so fall back to listing `"assets"` and paying for both patches.

```json
{
    "main": "./bundle.js",
    "output": "ic10-lsp.exe",
    "disableExperimentalSEAWarning": true,
    "assets": {
        "ic10c-node.node": "./node_modules/@ic10/compiler/src/ic10c-node.node",
        "ic10r-node.node": "./node_modules/@ic10/runtime/src/ic10r-node.node",
        "stdLib.ic": "./node_modules/@ic10/compiler/src/stdLib.ic"
    }
}
```

- First run the §1 bundle pass that keeps only the native addons external (inlining the pure-JS dependencies);
- Use `sea.getRawAsset()` to write both `.node` files to temp files and `process.dlopen` them, replacing
  `require("@ic10/compiler")` / `require("@ic10/runtime")` in the bundle with the loaded APIs;
- Unpack `stdLib.ic` to a temp file and point the `require.resolve(...)` result at it (note the **new package and
  file name** `stdLib.ic`, not the old `ic10c_node/static/stdLib.ic.json`);
- Embed the subpaths behind that computed metadata reference as assets and point the path there, or rewrite the
  computed specifier into a static lookup.

If SEA is not an option, `@yao-pkg/pkg` still works (put the three files next to the exe, keeping the native
addons external) — at the cost of downloading Node runtimes, which in practice takes hours:

```bash
pnpm exec pkg packages/server/bundle.js --targets node26-win-x64 --output ic10-lsp.exe
```

## 5. Verifying

Feed it a minimal `initialize` request — a response means stdio is wired up (`Content-Length` is in bytes):

```bash
printf 'Content-Length: 58\r\n\r\n{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | ic10-lsp.exe --stdio
```

Observed on the executable built by Node 26.11.1: stdout first carries a `window/logMessage`
(`Server is running...`), then `initialize` answers with the full capability table (including
`"inlayHintProvider":true`); after a `textDocument/didOpen`, `textDocument/inlayHint` / `hover` /
`completion` all answer normally — which shows the compiler native addon, `stdLib.ic` and that computed
metadata reference are all in place inside the VFS.

Two traps when feeding the protocol by hand (neither is an executable bug):

- **Do not push `exit` and `initialize` into the pipe at once**: `exit` calls `process.exit(0)`
  immediately, before the asynchronous `initialize` has finished, so all you see is the log message and
  no initialize response. Send them in order with pauses in between;
- after `initialized`, the server itself sends `client/registerCapability` and
  `workspace/workspaceFolders` requests to the client; a fake client that never answers them leaves the
  process alive, so the test script has to terminate it.

Then point the JetBrains plugin at it (command and arguments must match `createCommandLine()`: `ic10-lsp`
plus `--stdio`) and open a `.ic` file to confirm hover and completion answer.

## 6. Relation to the extension package

The same `packages/server/dist/server.js` feeds two artifact lines:

| Artifact | Consumer | How |
| --- | --- | --- |
| VSIX (containing the bundle) | VS Code | `compile` + `bundle` + `vsce package`; `@ic10/compiler` / `@ic10/runtime` / `@ic10/metadata` ship with the VSIX (vsce only collects root dependencies) |
| `ic10-lsp.exe` | JetBrains | this document: §3 (recommended) or §4; the plugin's Gradle build then copies it into that plugin's distribution under `bin/`, with no download at runtime |

### 6.1 Rebuild after touching the server, or the change fails silently

Once the server sources change, neither artifact follows on its own:

| Artifact | Command to refresh it | What happens if you forget |
| --- | --- | --- |
| `dist/server.js` (VS Code) | `pnpm run compile && pnpm run bundle` | the extension keeps running the old logic |
| `ic10-lsp.exe` (JetBrains) | `pnpm run sea:bundle && pnpm run sea:build` | **silent failure**: the old executable does not know the new command-line switches (such as `--hover-renderer=`) and the new behaviour simply looks absent |

`sea:bundle` cannot be skipped — `sea:build` only seals the *existing* `bundle.js` into the executable. Also,
because the JetBrains plugin ships that executable, a server change only reaches its users through a new
plugin release (just as VS Code needs a new VSIX).

## 7. Obsolete practices

- **verdaccio + `file:../common`**: package references now go through the pnpm workspace, so no local registry is
  needed;
- **injecting the blob by hand (postject)**: `node --build-sea` has been built in since Node 25.5;
- **the old esbuild command and package names**: it used to bundle the tsc output `./out/server/server.js` with
  `--external:ic10c_node`; `dist/server.js` is self-contained now, and the package and file names have changed.

For the old text: `git log --follow -- packages/server/README.md`.
