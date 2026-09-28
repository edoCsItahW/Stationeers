# 变更日志

[English](CHANGELOG.md)

> 本包以 Node.js 原生扩展的形式封装 IC10 运行时 C++ 核心，因此条目既可能是**绑定层**的变更
> （导出的适配器、类型声明、打包内容），也可能是该扩展所暴露的**执行行为**变更。

## 2026/08/12

- [1.0.0]: 发布 1.0.0

## 2026/08/21

- [1.0.1]: 修复 `ic10r-node` 的类型声明
- [1.0.4]: 重新打包：运行时不再重复发布编译器的声明文件——词法、语法、AST 与指令类型一律取自编译器包，本包只保留 `config` / `context` / `device` / `engine` / `manager` / `memory` / `value`

## 2026/08/22

- [1.0.5]: 为 VS Code 插件提供运行时支持：消息语言切换（`IC10RuntimeLocale.setLanguage`）、`Engine.step()` 与 `Engine.diagnostics`、`allowErrorStatements` 配置项，并把诊断改由 reporter 上报（不再打印到 stderr）

## 2026/09/28

- [2.0.0]: **包名由 `ic10r-node` 改为 `@ic10/runtime`**。适配 IC10 v3：执行器移植到统一按元数生成的 AST（`Statement` / `ExecutableInstruction` 变为句柄，旧文档注释节点由 `TypeAnnotation` 取代）
- [2.0.0]: v3 语法接受的各类操作数现在都能执行：写入目标可为寄存器别名（`alias tmp r3`），设备引用可为别名、动态寄存器（`rr0`）或动态端口（`dr0`、`drr0`——端口号存于寄存器，`-1` 表示自身引用），`sb` / `lb` / `lbn` / `lbs` / `lbns` 的设备哈希与名称哈希也可存放在寄存器中
- [2.0.0]: `s` / `sb` / `ss` / `sbn` / `sbs` 的写入值可为字面量、常量别名、寄存器或枚举常量，与编译器放宽后的操作数集合一致
- [2.0.0]: 枚举操作数据类型表求值（`Color.Green`，以及逻辑属性 / 聚合模式 / 试剂模式位置上的裸成员名 `Pressure`、`Sum`、`Contents` 等）；`Engine` 与 `Context` 新增第 4 个可选参数 `TypeTable`
- [2.0.0]: 旧语法的数字逻辑属性按十进制文本传递，`$` 十六进制与 `%` 二进制文本在操作数取值和常量中都能解析，无法解析的寄存器 / 设备 / 操作数改为上报 `IEM2_1`（不再静默跳过）
