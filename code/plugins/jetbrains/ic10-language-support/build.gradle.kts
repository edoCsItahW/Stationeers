import org.jetbrains.intellij.platform.gradle.TestFrameworkType

plugins {
    java
    id("org.jetbrains.changelog")
    id("org.jetbrains.intellij.platform")
}

java {
    toolchain {
        languageVersion = JavaLanguageVersion.of(21)
    }
}

// Read more: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin.html
repositories {
    mavenLocal()

    mavenCentral()

    intellijPlatform {
        defaultRepositories()
    }
}

dependencies {
    testImplementation(libs.junit)

    implementation("io.github.stationeers:ic10:1.0.0")

    // IntelliJ Platform Gradle Plugin Dependencies Extension - read more: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin-dependencies-extension.html
    intellijPlatform {
        intellijIdea("2025.3.5")
        testFramework(TestFrameworkType.Platform)

        // Add plugin dependencies for compilation here, for example:
        // bundledPlugin("com.intellij.java")
    }
}
