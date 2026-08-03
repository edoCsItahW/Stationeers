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
    Invoke-CMakeConfigure -BuildDir "build" -SourceDir "." -CoreArtifactDir $coreArtifactDir

    # 2. 强制检查并开启 BUILD_JAVA_BINDINGS（若未开启）
    if (Test-Path "build/CMakeCache.txt") {
        $cacheContent = Get-Content "build/CMakeCache.txt" -Raw
        if ($cacheContent -notmatch "BUILD_JAVA_BINDINGS:BOOL=ON") {
            Write-Info "BUILD_JAVA_BINDINGS 未启用，重新配置 CMake"
            cmake -B build -S . -DBUILD_JAVA_BINDINGS=ON
            if ($coreArtifactDir -ne "") {
                cmake -B build -S . -DIC10_CORE_ARTIFACT_DIR=$coreArtifactDir
            }
        }
    }

    # 3. 构建目标
    Invoke-CMakeBuild -BuildDir "build" -Target $PROJECT.CMakeTarget -Config $PROJECT.BuildConfig

    # 4. 复制工件到测试目录
    Write-Header "复制工件"
    # 注意：这里把 .dll 复制到 tests/java/ic10/libs/ 下，方便 Gradle 通过 java.library.path 加载
    $destDir = Join-Path $PROJECT.PackageRoot "libs"
    Copy-Artifact -Source $PROJECT.ArtifactSource -Destination $destDir -Force

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