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

    $os = Get-OSKey
    $artifactPath = Join-Path $Config.ArtifactPath $Config.ArtifactName.$os
    $resolved = Resolve-ArtifactPath $artifactPath
    Copy-Artifact -Source $resolved -Destination $Config.PublishDir

    Write-ST-Phase (__ "Java.Test")

    Set-Location $Config.TestDir

    $gradlewPath = "../../publish/java/$($Config.TestScript.$os)"
    & $gradlewPath test --no-daemon --stacktrace
} catch {
    Write-ST-Error (__ "Java.Error" -Arguments $_.Exception.Message)

    exit 1
}
