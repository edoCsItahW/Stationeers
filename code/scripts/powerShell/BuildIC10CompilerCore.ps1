param(
    [string[]]$ExtraArgs = @()
)

$Project = Get-Content "$PSScriptRoot\build-info.json" | ConvertFrom-Json
$Config = $Project.IC10_Compiler_Core

Set-Location $Project.rootDir

$ExtraArgs += @(
    "-DSTATIONEERS_LANG=zh-hans"
    "-DBUILD_IC10_COMPILER_TESTS=ON"
)

. "$PSScriptRoot\modules\BuildCore.ps1" -Config $Config -ExtraArgs $ExtraArgs

Exit $LASTEXITCODE
