Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Locale.psm1"
Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Builds.psm1"
Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Debug.psm1"

Initialize-Locale -Lang "zh-hans"

$Project = Get-Content "$PSScriptRoot\..\build-info.json" | ConvertFrom-Json
$Config = $Project.IC10_Compiler_CLI

$ExtraArgs = @(
    "-DSTATIONEERS_LANG=zh-hans"
    "-DBUILD_IC10_COMPILER_CORE=ON"
)

try {
    Set-Location $Project.rootDir

    Invoke-CMakeConfigure -Target $Config.Target -BuildDir $Config.BuildDir -SourceDir $Config.SourceDir -ExtraArgs $ExtraArgs

    Invoke-CMakeBuild -BuildDir $Config.BuildDir -Target $Config.Target -Config $Config.Config

    Write-ST-Phase (__ "Core.Test")

    $os = Get-OSKey
    $testExe = Join-Path $Config.TestExeDir $Config.TestExeName.$os
    $actualTestExe = Resolve-ArtifactPath $testExe

    if (-not (Test-Path $actualTestExe)) {
        throw (__ "Build.Copy.SourceNotFound" -Arguments $actualTestExe)
    }

    Set-Location $Config.TestDir

    . $actualTestExe --version
} catch {
    Write-ST-Error (__ "Core.Error" -Arguments $_.Exception.Message)

    exit 1
}