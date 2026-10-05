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

    Write-ST-Phase (__ "Build.StdLib.Head")

    # 标准库只有声明了 StdLibDir 的目标才有；这里放进 jar 的 resources 根（.dll 在 native/ 下）
    if ([string]::IsNullOrEmpty($Config.StdLibDir)) {
        Write-ST-Info (__ "Build.StdLib.Skipped" -Arguments $Config.Target)
    }
    else {
        Sync-StdLib -Source $Config.StdLibPath -DestinationDir $Config.StdLibDir
    }

    Write-ST-Phase (__ "Java.Test")

    Set-Location $Config.TestDir

    $gradlewPath = "../../publish/java/$($Config.TestScript.$os)"
    & $gradlewPath test --no-daemon --stacktrace
} catch {
    Write-ST-Error (__ "Java.Error" -Arguments $_.Exception.Message)

    exit 1
}
