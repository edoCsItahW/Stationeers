param(
    [Object]$Config,
    [string[]]$ExtraArgs = @()
)

Import-Module -Force -Scope Global -Name "$PSScriptRoot\Locale.psm1"
Import-Module -Force -Scope Global -Name "$PSScriptRoot\Builds.psm1"
Import-Module -Force -Scope Global -Name "$PSScriptRoot\Debug.psm1"

Initialize-Locale -Lang "zh-hans"

try {
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

    . $actualTestExe
} catch {
    Write-ST-Error (__ "Core.Error" -Arguments $_.Exception.Message)

    exit 1
}