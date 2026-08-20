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

    Write-ST-Phase (__ "Node.Copy")

    $resolved = Resolve-ArtifactPath $Config.ArtifactPath
    Copy-Artifact -Source $resolved -Destination $Config.PublishDir

    Write-ST-Phase (__ "Node.Test")

    Set-Location $Config.TestDir

    pnpm install --ignore-scripts

    pnpm run test
} catch {
    Write-ST-Error (__ "Node.Error" -Arguments $_.Exception.Message)

    exit 1
}
