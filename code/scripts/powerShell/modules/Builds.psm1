Import-Module "$PSScriptRoot\Locale.psm1"
Import-Module "$PSScriptRoot\Debug.psm1" -Force

function Find-FileExists {
    param(
        [string]$Path,
        [string]$FileName
    )
    return [bool](Get-ChildItem -Path $Path -Recurse -Filter $FileName -File -ErrorAction SilentlyContinue)
}


function Invoke-CMakeConfigure {
    param(
        [string]$Target,
        [string]$BuildDir = "build",
        [string]$SourceDir = ".",
        [string[]]$ExtraArgs = @()
    )

    Write-ST-Phase (__ "Build.Configure.Head")

    if ((Test-Path $BuildDir) -and (Find-FileExists -Path $BuildDir -FileName "$Target.vcxproj")) {
        Write-ST-Info (__ "Build.Configure.Skip")
    }

    else {
        if ((Test-Path $BuildDir) -and (Test-Path "$BuildDir/CMakeCache.txt")) {
            Remove-Item "$BuildDir/CMakeCache.txt" -Force

            Write-ST-Info (__ "Build.Configure.RemoveCache")
        }

        $cmakeArgs = @("-B", $BuildDir, "-S", $SourceDir)

        if ($ExtraArgs.Count -gt 0) {
            $cmakeArgs += $ExtraArgs
        }

        cmake @cmakeArgs

        if ($LASTEXITCODE -ne 0) {
            throw (__ "Build.Configure.Error")
        }

    }
}


function Invoke-CMakeBuild {
    param(
        [string]$BuildDir,
        [string]$Target,
        [string]$Config = "Release"
    )

    Write-ST-Phase (__ "Build.Build.Head")

    Write-ST-Info (__ "Build.Build.Target" -Arguments $Target)

    Write-ST-Info (__ "Build.Build.Config" -Arguments $Config)

    # 检查目标是否为 IMPORTED（缓存命中，不生成 vcxproj）
    $vcxproj = Get-ChildItem -Path $BuildDir -Recurse -Filter "$Target.vcxproj" -ErrorAction SilentlyContinue | Select-Object -First 1
    if (-not $vcxproj) {
        Write-ST-Info (__ "Build.Build.Cache" -Arguments $Target)
        return
    }

    cmake --build $BuildDir --target $Target --config $Config

    if ($LASTEXITCODE -ne 0) {
        throw (__ "Build.Build.Error" -Arguments $LASTEXITCODE)
    }
}


function Copy-Artifact {
    param(
        [string]$Source,
        [string]$Destination,
        [switch]$Force
    )

    if (-not (Test-Path $Source)) {
        throw (__ "Build.Copy.SourceNotFound" -Arguments $Source)
    }

    $parent = Split-Path $Destination -Parent

    if (-not (Test-Path $parent)) {
        New-Item -ItemType Directory -Path $parent -Force | Out-Null
    }

    Copy-Item -Path $Source -Destination $Destination -Force:$Force

    if ($LASTEXITCODE -ne 0) {
        throw (__ "Build.Copy.Error" -Arguments $LASTEXITCODE)
    }
    else {
        Write-ST-Info (__ "Build.Copy.Success" -Arguments $Source, $Destination)
    }
}


Export-ModuleMember -Function Invoke-CMakeConfigure, Invoke-CMakeBuild, Copy-Artifact
