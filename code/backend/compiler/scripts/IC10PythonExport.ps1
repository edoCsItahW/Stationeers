Import-Module -Force -Scope Global -Name "$PSScriptRoot\Build.psm1"

$CONFIG = Get-Content (Join-Path $PSScriptRoot "product-info.json") | ConvertFrom-Json
$PROJECT = $CONFIG.IC10Python

Push-Location (Join-Path $PSScriptRoot "..")

try {
    # 1. CMake 配置（仅首次或需要重新配置时）
    # CMake configure (first run or when reconfigure needed)
    $needsConfigure = -not (Test-Path "build")
    
    # 如果build目录存在但CMakeCache.txt中没有启用Python绑定，也需要重新配置
    # If build dir exists but Python bindings not enabled in CMakeCache.txt, also needs reconfigure
    if (Test-Path "build/CMakeCache.txt") {
        $cacheContent = Get-Content "build/CMakeCache.txt" -Raw
        if ($cacheContent -notmatch "BUILD_PYTHON_BINDINGS:BOOL=ON") {
            $needsConfigure = $true
            Write-Info "BUILD_PYTHON_BINDINGS未启用，需要重新配置CMake"
        }
    }
    
    if ($needsConfigure) {
        Write-Header "配置CMake"
        cmake -B build -S . -DBUILD_PYTHON_BINDINGS=ON
    }

    # 2. 构建
    # Build
    Invoke-CMakeBuild -BuildDir "build" -Target $PROJECT.CMakeTarget -Config $PROJECT.BuildConfig

    # 3. 复制工件
    # Copy artifact
    Write-Header "复制工件"

    Copy-Artifact -Source $PROJECT.ArtifactSource -Destination "$($PROJECT.PackageRoot)/$($PROJECT.PackageName).pyd" -Force

    # 4. 测试
    # Test
    Write-Header "测试"

    python -m pytest $PROJECT.TestDir

} catch {
    Write-Error $_.Exception.Message
    exit 1

} finally {
    Pop-Location
}
