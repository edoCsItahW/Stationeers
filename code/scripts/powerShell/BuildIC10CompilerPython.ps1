param(
    [string[]]$ExtraArgs = @()
)

$Project = Get-Content "$PSScriptRoot\..\build-info.json" | ConvertFrom-Json
$Config = $Project.IC10_Compiler_Python

Set-Location $Project.rootDir

$ExtraArgs += @(
    "-DSTATIONEERS_LANG=zh-hans"
    "-DBUILD_IC10_COMPILER_EXPORTS_PYTHON=ON"
)

. "$PSScriptRoot\modules\BuildPython.ps1" -Config $Config -ExtraArgs $ExtraArgs

Exit $LASTEXITCODE