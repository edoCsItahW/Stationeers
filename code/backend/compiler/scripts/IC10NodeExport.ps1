Import-Module -Force -Scope Global -Name "$PSScriptRoot\Build.psm1"

$CONFIG = Get-Content (Join-Path $PSScriptRoot "product-info.json") | ConvertFrom-Json
$PROJECT = $CONFIG.IC10Node

Push-Location (Join-Path $PSScriptRoot "..")

try {
    # 1. CMake 配置（仅首次）
    if (-not (Test-Path "build")) {
        Write-Header "配置CMake"
        cmake -B build -S .
    }

    # 2. 构建
    Invoke-CMakeBuild -BuildDir "build" -Target $PROJECT.CMakeTarget -Config $PROJECT.BuildConfig

    # 3. 复制工件
    Write-Header "复制工件"

    Copy-Artifact -Source $PROJECT.ArtifactSource -Destination "$($PROJECT.PackageRoot)/src" -Force

    # 4. 测试
    Write-Header "测试"

    pnpm run test

} catch {
    Write-Error $_.Exception.Message
    exit 1
    
} finally {
    Pop-Location
}
