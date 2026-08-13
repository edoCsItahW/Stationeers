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

    Write-ST-Phase (__ "Java.Copy")

    Copy-Artifact -Source $Config.ArtifactPath -Destination $Config.PublishDir

    Write-ST-Phase (__ "Java.Test")

    Set-Location $Config.TestDir

    if (Test-Path "gradlew.bat") {
        .\gradlew.bat test
    } else {
        gradle test
    }
} catch {
    Write-ST-Error (__ "Java.Error" -Arguments $_.Exception.Message)

    exit 1
}
