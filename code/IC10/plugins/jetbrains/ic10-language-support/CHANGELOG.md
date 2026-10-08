<!-- Keep a Changelog guide -> https://keepachangelog.com -->

# Ic10-language-support Changelog

## [Unreleased]

### Added

- 语言服务器（约 116 MB 的可执行文件）随本插件一起分发：构建时把 vscode 插件那边的 SEA 产物复制进分发包的
  `bin/`（下载约 40 MB），一次安装即得全部功能；`Settings | Tools | IC10` 仍可指定自建的可执行文件覆盖它 /
  The language server (a ~116 MB executable) ships inside this plugin: the build copies the SEA artifact from
  the vscode plugin into the distribution's `bin/` (~40 MB to download), so one install brings everything. A
  self-built executable can still override it under `Settings | Tools | IC10`.

### Changed

- 没有可用的语言服务器时不再静默启动客户端，改为通知用户该装什么或该配哪一项 / The plugin no longer starts
  a client without an executable available; it notifies the user what to install or configure.
- 按 IntelliJ 实际能渲染的内容启动服务器：悬停改用原生 Markdown（换行用行内 `<br />`，因为它会逐行
  `trimEnd` 掉行尾双空格），内联提示不再声明（服务端开关
  `--hover-renderer=markdown --hover-breaks=html --inlay-hints=off`）/ The server is started with switches
  matching what IntelliJ can render: native Markdown hovers (with inline `<br />` breaks, since trailing
  whitespace is trimmed per line) and no inlay hints
  (`--hover-renderer=markdown --hover-breaks=html --inlay-hints=off`).
