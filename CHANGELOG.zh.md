# 变更日志

## 2026/09/26

- feat: 放宽 IC10 语法的操作数规则
- feat: 适配 IC10 v3 更新 VS Code 插件
- feat: 发布 IC10 VS Code 插件 v2（client 与 server 改用 esbuild 打包，重构打包流程）

## 2026/09/25

- feat: 发布 ic10c-node v3
- feat: 适配 IC10 v3 更新 VS Code 插件
- fix: 修复 v3.0.0 发布 CI（产物命名、重复产物与发布权限）

## 2026/09/24

- refactor: 重构 IC10 元数据（独立为 `@ic10/metadata` 包）

## 2026/09/23

- refactor: 重构 IC10 VS Code 插件包结构（client / common / debugger / server 分包）
- feat: 适配 IC10 v3 调整 VS Code 插件
- fix: 修复部分 AST 位置错误
- fix: 修复协程链断裂问题

## 2026/09/22

- perf: 优化 IC10 核心的编译时间与内存占用

## 2026/09/21

- feat: 支持 Docker 容器
- fix: 修复协程链断裂问题

## 2026/09/20

- feat: 调整 IC10 语法注解（annotation）规则（@logic / @logic-slot 需带引脚序号，新增 @slot 标签）
- fix: 修补元数据与标准库

## 2026/09/19

- feat: 适配 IC10 v3 调整 VS Code 插件

## 2026/09/18

- feat: 调整 IC10 语法链接（link）规则
- feat: 适配 IC10 v3 调整 VS Code 插件

## 2026/09/12

- feat: 更新 IC10 标准库
- feat: 适配 IC10 v3 调整 VS Code 插件
- fix: 解决预定义常量问题
- fix: 修复 DefineDirectiveNode 字段缺失错误

## 2026/09/05

- feat: 实现 IC10 v3（扁平化指令变体，删除各语言绑定中重复的 AST 定义）
- build: 更新 CMake 缓存系统

## 2026/08/28

- feat: 实现 IC10 v3（重写词法分析与语法分析，统一按元数生成的 AST 节点）

## 2026/08/26

- docs: 重构 IC10 v3 语法文件（拆分 lexer / parser 模块并新增语法生成工具）

## 2026/08/22

- feat: 实现 VS Code 插件运行时（适配调试会话）

## 2026/08/21

- feat: 实现 VS Code 插件运行时（调试器，两轮）
- feat: 发布 IC10 运行时 Node.js 包（ic10r-node）
- feat: 实现 VS Code 插件悬停渲染（svg / markdown 可切换）
- fix: 修复 ic10r-node 栈错误
- fix: 修复 ic10r-node 类型定义
- fix: 修复 VS Code 崩溃问题

## 2026/08/20

- refactor: 重构项目 CI/CD 框架
- chore: 删除 code/package.json

## 2026/08/19

- refactor: 重构项目 CMake 框架

## 2026/08/14

- refactor: 重构项目 CMake 框架

## 2026/08/07

- refactor: 重构项目目录结构（统一到 code/IC10/** 分层）
- feat: 实现 IC10 运行时（C++ 执行引擎：内存栈、设备管理与 I/O）

## 2026/08/06

- feat: 支持将 VS Code LSP 打包为可执行文件（ic10-lsp.exe）
- feat: 更新 IC10 Java 包装文件

## 2026/08/04

- feat: 实现 IC10 Java JSON 帧（基于 Jackson 反序列化 AST）
- chore: 补充 AST 字段并简化 Java AST

## 2026/08/03

- chore: 重命名 Java 包（com.stationeers.ic10 → io.github.stationeers.ic10）

## 2026/08/02

- feat: 实现 IC10 Java 支持（基于 fbjni 的 JNI 绑定）
- fix: 修复标签推送 CI（三轮）

## 2026/08/01

- chore: main 与 develop 分支已同步，并重新变基到 main

## 2026/07/31

- release: IC10 Language Support v1.0.2 — 已在 VS Code Marketplace 正式发布
- fix: 修复若干插件错误（补全提供器、签名帮助、AST 辅助）
- chore: 修正扩展打包流程

## 2026/07/30

- release: 准备发布（扩展打包）
- fix: 修复注解截断错误（`#:` 类型提示解析）
- fix: 修复跳转目标范围错误
- docs: 优化 IC10 语言支持插件性能（解析流水线、补全提供器、语义标记、通用工具）
- docs: 完善 IC10 语言支持插件文档（服务端结构、设置管理器、处理器、本地化资源、主题、SVG 构建器）

## 2026/07/29

- feat: 增强错误恢复（两轮）
- feat: 补充 LSP 配置
- fix: 修复转义错误（词法字符串处理）
- fix: 修复增量解析中刷新缓存令牌位置导致的错位
- fix: 修复同步点错误

## 2026/07/28

- feat: 实现 IC10 LSP 格式化功能（连续语句列对齐、行尾注释对齐、缩进、连续空行压缩、激进操作数对齐）
- feat: 实现 IC10 LSP 签名帮助（指令参数签名与当前参数高亮）

## 2026/07/27

- feat: 实现 IC10 LSP 代码补全功能（三次迭代 — 关键字补全、操作数类型驱动补全、设备上下文感知过滤）
- feat: 从链接器导出类型表
- fix: 修复符号读取错误

## 2026/07/26

- feat: 实现 IC10 LSP 代码补全功能
- feat: 导出类型表（供 LSP 语义分析使用）
- feat: 更新 LSP 悬停功能

## 2026/07/25

- feat: 更新 LSP 悬停功能
- feat: 实现 IC10 LSP 语义标记（两轮 — 基于编译器符号表的语义着色）
- fix: 修复误触发的错误恢复问题

## 2026/07/24

- feat: 实现 IC10 LSP 语义标记

## 2026/07/23

- feat: 实现 IC10 LSP 诊断报告功能（实时增量重分析 + 问题面板集成）
- feat: 实现 IC10 LSP 悬停功能
- fix: 修复单词读取错误

## 2026/07/22

- feat: 实现 IC10 LSP 悬停功能（别名、标签、常量、指令关键字）
- feat: 更新 IC10 Node.js 包（ic10-node-api）

## 2026/07/21

- chore: 添加 Issue 模板
- docs: 更新文档
- feat: 导出 Linker 到 Node.js 和 Python
- feat: 导出链接器

## 2026/07/20

- feat: 实现标准库
- feat: 完善 IC10 C++ 测试
- feat: 完善 IC10 Node.js 和 Python 测试

## 2026/07/19

- feat: 实现类型推导
- feat: 实现链接器（Linker）
- feat: 优化本地化输出
- feat: 更精确的语义分析

## 2026/07/18

- feat: 实现注解语法
- feat: 更新元数据
- feat: 更精确的语义分析

## 2026/07/17

- refactor: 更新元数据

## 2026/07/15

- feat: 编写和组织元数据

## 2026/07/14

- feat: 编写和组织元数据

## 2026/07/12

- feat: 伴随游戏更新的全面语法更新
- test: 完善 IC10 Node.js 测试
- ci: 更新 CI/CD，推送标签时自动上传工件
- ci: 完善 IC10 Python 测试

## 2026/07/11

- fix: 修复未闭合字符串吞掉后续输入的问题
- fix: 修复一些词法和语法分析器问题
- test: 完善 IC10 C++ 测试（集成与系统测试）
- test: 完善 IC10 C++ 测试（词法改进）
- ci: 添加 Python 工作流
- build: 更新构建框架

## 2026/07/10

- feat: 支持 IC10 编译器命令行模式（ic10c CLI）

## 2026/07/09

- feat: 实现增量编译器（行级词法缓存 + 语句级语法缓存）
- chore: 调整导出目录结构
- chore: 优化编译速度

## 2026/07/03

- feat: 统一 Node.js 使用方式
- feat: 优化编译器（两轮）

## 2026/07/01

- feat: 完成 IC10 的 Python 绑定与导出
- chore: WIP 中间检查点

## 2026/06/29

- feat: 实现 LSP 悬停提示功能（两轮 — 初始原型）
- fix: 修复 VSCode 导入模块时崩溃的问题

## 2026/06/28

- docs: 添加 ANTLR4 语法文件（IC10.g4）
- chore: 更新版权信息

## 2026/06/25

- docs: 支持 Doxygen 文档生成
- docs: 完善 index 文件中的 TypeScript 类型定义

## 2026/06/24

- fix: 解决 Lexer 适配器生命周期问题（#22）
- docs: 更新 README.md
- docs: 更新 README 和 CHANGELOG 文件
- docs: 完善代码的 Doxygen 文档
- ci: 重构 CI 流程

## 2026/06/23

- feat: IC10 全面支持 Node.js API（11 个适配器：Lexer、Token、Parser、Program/AST、Analyser、SymbolTable、Linker、IC10Local、IncLexer、IncParser、IncCompiler）
- ci: 解决 CI/CI 问题（卡死作业）
- ci: 解决 CI/CI 卡死问题

## 2026/06/18

- feat: IC10 支持 Node.js 接口

## 2026/06/08

- 引入 Google Test 框架
- 添加 MSVC 编译支持
- 添加 GitHub 工作流
- 完善 README 和 CONTRIBUTING 文件
- 提交 v1.0.0 版本

## 2026/06/07

- 实现 IC10 编译器

## 2026/06/02

- 初始化项目结构和基本文件。
- 添加后端编译相关的 cmake 配置
