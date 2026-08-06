Import-Module -Force -Scope Global -Name "$PSScriptRoot\Build.psm1"

$CONFIG = Get-Content (Join-Path $PSScriptRoot "product-info.json") | ConvertFrom-Json
$PROJECT = $CONFIG.IC10Java   # 对应 product-info.json 中的新条目

Push-Location (Join-Path $PSScriptRoot "..")

try {
    $coreArtifactDir = ""
    if (Test-CoreArtifactCache -Config $PROJECT.BuildConfig) {
        $coreArtifactDir = Get-CoreArtifactCacheDir
        Write-Info "使用缓存的核心构件: $coreArtifactDir"
    } else {
        Write-Info "核心构件缓存不存在，将从源码编译"
    }

    # 1. CMake 配置（确保开启 Java 绑定）
    Invoke-CMakeConfigure -BuildDir "build" -SourceDir "." -CoreArtifactDir $coreArtifactDir -ExtraArgs @("-DBUILD_JAVA_BINDINGS=ON")

    Invoke-CMakeBuild -BuildDir "build" -Target $PROJECT.CMakeTarget -Config $PROJECT.BuildConfig

    # 4. 复制工件到 publish/java/src/main/resources/native/
    Write-Header "复制工件"
    $destPath = Join-Path $PROJECT.PackageRoot "src" "main" "resources" "native" "ic10c_java.dll"
    Copy-Artifact -Source $PROJECT.ArtifactSource -Destination $destPath -Force

    # 5. 运行 Java 测试（Gradle）
    Write-Header "运行 Java 测试"
    Push-Location $PROJECT.TestDir
    try {
        # 如果项目有 gradlew，用 ./gradlew test；否则用系统 gradle
        if (Test-Path "gradlew.bat") {
            .\gradlew.bat test
        } else {
            gradle test
        }
        if ($LASTEXITCODE -ne 0) {
            throw "Gradle 测试失败，退出码 $LASTEXITCODE"
        }
    } finally {
        Pop-Location
    }

} catch {
    Write-Error $_.Exception.Message
    exit 1
} finally {
    Pop-Location
}