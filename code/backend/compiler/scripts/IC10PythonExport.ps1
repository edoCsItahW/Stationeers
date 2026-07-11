Import-Module -Force -Scope Global -Name "$PSScriptRoot\Build.psm1"

$CONFIG = Get-Content (Join-Path $PSScriptRoot "product-info.json") | ConvertFrom-Json
$PROJECT = $CONFIG.IC10Python

Push-Location (Join-Path $PSScriptRoot "..")

try {
    $coreArtifactDir = ""
    if (Test-CoreArtifactCache -Config $PROJECT.BuildConfig) {
        $coreArtifactDir = Get-CoreArtifactCacheDir
        Write-Info "使用缓存的核心构件: $coreArtifactDir"
    } else {
        Write-Info "核心构件缓存不存在，将从源码编译"
    }

    Invoke-CMakeConfigure -BuildDir "build" -SourceDir "." -CoreArtifactDir $coreArtifactDir

    if (Test-Path "build/CMakeCache.txt") {
        $cacheContent = Get-Content "build/CMakeCache.txt" -Raw
        if ($cacheContent -notmatch "BUILD_PYTHON_BINDINGS:BOOL=ON") {
            Write-Info "BUILD_PYTHON_BINDINGS未启用，重新配置CMake"
            cmake -B build -S . -DBUILD_PYTHON_BINDINGS=ON
            if ($coreArtifactDir -ne "") {
                cmake -B build -S . -DIC10_CORE_ARTIFACT_DIR=$coreArtifactDir
            }
        }
    }

    Invoke-CMakeBuild -BuildDir "build" -Target $PROJECT.CMakeTarget -Config $PROJECT.BuildConfig

    Write-Header "复制工件"

    Copy-Artifact -Source $PROJECT.ArtifactSource -Destination "$($PROJECT.PackageRoot)/$($PROJECT.PackageName).pyd" -Force

    Write-Header "测试"

    python -m pytest $PROJECT.TestDir

} catch {
    Write-Error $_.Exception.Message
    exit 1

} finally {
    Pop-Location
}
