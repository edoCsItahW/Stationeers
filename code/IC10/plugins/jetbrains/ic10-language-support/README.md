# Ic10-language-support

[![Twitter Follow](https://img.shields.io/badge/follow-%40JBPlatform-1DA1F2?logo=twitter)](https://twitter.com/JBPlatform)
[![Developers Forum](https://img.shields.io/badge/JetBrains%20Platform-Join-blue)][jb:forum]

## 概述

此仓库实现了一个 IntelliJ 平台插件。

## 演示功能

示例插件添加了一个“我的工具窗口”工具窗口，具有简单的随机数打乱功能。

## 插件结构

生成的项目包含以下内容结构：

```
.
├── .run/                   预定义的运行/调试配置
├── gradle
│   ├── wrapper/            Gradle Wrapper
│   ├── libs.versions.toml  版本目录
├── src                     插件源码
│   └── main
│       ├── kotlin/         Kotlin 生产源码
│       └── resources/      插件资源
│           ├── META-INF/   插件配置文件和标志图
│           └── messages/   消息资源束
├── .gitignore              Git 忽略规则
├── build.gradle.kts        Gradle 构建配置
├── gradle.properties       Gradle 配置属性
├── gradlew                 *nix Gradle Wrapper 脚本
├── gradlew.bat             Windows Gradle Wrapper 脚本
├── README.md               本文件
└── settings.gradle.kts     Gradle 项目设置
```

除了配置文件外，最关键的部分是 `src` 目录，其中包含我们的实现以及插件的清单文件 – [plugin.xml][file:plugin.xml]。

> [!NOTE]
> 如需在插件中使用 Java，请创建 `/src/main/java` 目录。

插件标志图放在 `src/main/resources/META-INF/pluginIcon.svg`。有关更多信息和标志图要求，请参阅[插件标志图][docs:logo]。

## 构建脚本

[build.gradle.kts][file:build.gradle.kts] 是项目定义的核心。它应用了三个 Gradle 插件：

| 插件                               | 描述                                                                               |
|-----------------------------------|------------------------------------------------------------------------------------|
| `org.jetbrains.kotlin.jvm`        | 添加 Kotlin 支持                                                                  |
| `org.jetbrains.changelog`         | 简化对 [CHANGELOG.md][file:CHANGELOG.md] 文件的修补                               |
| `org.jetbrains.intellij.platform` | [IntelliJ 平台 Gradle 插件][docs:intellij-platform-gradle-plugin-docs]            |

`intellijPlatform` 依赖块选择要针对其编译的 IDE：

```kotlin
intellijIdea("2025.3.5")
```

有关更多信息，请参阅[目标版本][docs:target-version]。

`intellijPlatform` 依赖块还包含对平台测试框架的依赖：

```kotlin
testFramework(TestFrameworkType.Platform)
```

有关更多信息，请参阅[测试][docs:testing]。

## 插件配置文件

插件配置文件是位于 `src/main/resources/META-INF` 目录中的 [plugin.xml][file:plugin.xml] 文件。它提供有关插件的常规信息、依赖项、扩展点和监听器。

您可以在我们的文档中的[插件配置文件][docs:plugin.xml]一节中阅读更多关于此文件的内容。

### 插件 ID 和名称

生成的插件 ID 和名称可能需要调整。

这些值是基于 IDE 插件向导中提供的“组 ID”和“工件 ID”生成的。建议检查 plugin.xml 文件中的 `<id>` 和 `<name>` 元素，并在需要时进行调整。

请注意，Gradle 属性 `rootProject.name` 和 `project.group` 不需要与 `<id>` 和 `<name>` 元素匹配。由于它们服务于不同的功能，因此没有与 IntelliJ 平台相关的理由要求它们匹配。

## 预定义的运行/调试配置

在默认项目结构中，提供了 `.run` 目录，其中包含预定义的*运行/调试配置*，这些配置对应公开的 Gradle 任务：

| 配置名称          | 描述                                                                                                                                                    |
|-------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
| Run IDE with Plugin | 运行 [`:runIde`][docs:intellij-platform-gradle-plugin-runIde] IntelliJ 平台 Gradle 插件任务。使用*调试*图标进行插件调试。                               |
| Run Tests         | 运行 [`:check`][gradle:lifecycle-tasks] Gradle 任务。                                                                                                  |
| Run Verifications | 运行 [`:verifyPlugin`][docs:intellij-platform-gradle-plugin-verifyPlugin] IntelliJ 平台 Gradle 插件任务，以检查插件与指定 IntelliJ IDE 的兼容性。      |

> [!NOTE]
> 您可以在 `idea.log` 选项卡中找到正在运行的任务的日志。

## 发布插件

> [!TIP]
> 请确保遵循[发布插件][docs:publishing]中列出的所有推荐和必需步骤。

将插件发布到 [JetBrains Marketplace](https://plugins.jetbrains.com) 是一项直接的操作，它使用由 [intellij-platform-gradle-plugin][docs:intellij-platform-gradle-plugin-docs] 提供的 `publishPlugin` Gradle 任务。

您也可以手动通过 UI 将插件上传到 [JetBrains 插件仓库](https://plugins.jetbrains.com/plugin/upload)。

## 有用链接

- [IntelliJ 平台 SDK 插件开发文档][docs]
- [IntelliJ 平台 Gradle 插件文档][docs:intellij-platform-gradle-plugin-docs]
- [IntelliJ 平台探索器][jb:ipe]
- [JetBrains Marketplace 质量指南][jb:quality-guidelines]
- [IntelliJ 平台 UI 指南][jb:ui-guidelines]
- [JetBrains Marketplace 付费插件][jb:paid-plugins]
- [IntelliJ SDK 代码示例][gh:code-samples]

[docs]: https://plugins.jetbrains.com/docs/intellij
[docs:plugin.xml]: https://plugins.jetbrains.com/docs/intellij/plugin-configuration-file.html?from=IJPluginReadmeFile
[docs:publishing]: https://plugins.jetbrains.com/docs/intellij/publishing-plugin.html?from=IJPluginReadmeFile
[docs:intellij-platform-gradle-plugin-docs]: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin.html?from=IJPluginReadmeFile
[docs:intellij-platform-gradle-plugin-runIde]: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin-tasks.html?from=IJPluginReadmeFile#runIde
[docs:intellij-platform-gradle-plugin-verifyPlugin]: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin-tasks.html?from=IJPluginReadmeFile#verifyPlugin
[docs:logo]: https://plugins.jetbrains.com/docs/intellij/plugin-icon-file.html?from=IJPluginReadmeFile
[docs:target-version]: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin-dependencies-extension.html?from=IJPluginReadmeFile#target-versions
[docs:testing]: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin-dependencies-extension.html?from=IJPluginReadmeFile#testing

[file:build.gradle.kts]: ./build.gradle.kts
[file:CHANGELOG.md]: ./CHANGELOG.md
[file:gradle.properties]: ./gradle.properties
[file:plugin.xml]: ./src/main/resources/META-INF/plugin.xml

[gh:code-samples]: https://github.com/JetBrains/intellij-sdk-code-samples

[gradle:lifecycle-tasks]: https://docs.gradle.org/current/userguide/java_plugin.html#lifecycle_tasks

[jb:github]: https://github.com/JetBrains/.github/blob/main/profile/README.md
[jb:forum]: https://platform.jetbrains.com/
[jb:quality-guidelines]: https://plugins.jetbrains.com/docs/marketplace/quality-guidelines.html
[jb:paid-plugins]: https://plugins.jetbrains.com/docs/marketplace/paid-plugins-marketplace.html
[jb:ipe]: https://jb.gg/ipe
[jb:ui-guidelines]: https://jetbrains.github.io/ui