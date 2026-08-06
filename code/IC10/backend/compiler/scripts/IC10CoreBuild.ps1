Import-Module -Force -Scope Global -Name "$PSScriptRoot\Build.psm1"

$CONFIG = Get-Content (Join-Path $PSScriptRoot "product-info.json") | ConvertFrom-Json
$BUILD_CONFIG = "Release"

Push-Location (Join-Path $PSScriptRoot "..")

try {
    Write-Header "IC10 核心编译"

    Invoke-CMakeConfigure -BuildDir "build" -SourceDir "."

    Write-Info "编译核心库: common, ic10_lib"
    cmake --build build --config $BUILD_CONFIG --parallel 4 --target common ic10_lib

    if ($LASTEXITCODE -ne 0) {
        throw "CMake 构建失败，退出码 $LASTEXITCODE"
    }

    Update-CoreArtifactCache -BuildDir "build" -Config $BUILD_CONFIG

    Write-Header "核心编译完成"

} catch {
    Write-Error $_.Exception.Message
    exit 1

} finally {
    Pop-Location
}