Import-Module "$PSScriptRoot\Locale.psm1"
Import-Module "$PSScriptRoot\Debug.psm1" -Force

# Detect OS key for build-info.json field lookup (windows / linux)
function Get-OSKey {
    if ($IsWindows -or $env:OS -eq "Windows_NT") { "windows" } else { "linux" }
}

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
    # 仅对 Visual Studio 生成器有意义：VS 生成器会把 cmake --build 翻译成
    # msbuild <target>.vcxproj，若目标为 IMPORTED 则 .vcxproj 不存在，msbuild 报 MSB1009。
    # Ninja/Makefile 等单配置生成器对 IMPORTED 目标会直接成功，无需此检测。
    $generator = ""
    if (Test-Path "$BuildDir/CMakeCache.txt") {
        $line = Select-String -Path "$BuildDir/CMakeCache.txt" -Pattern "^CMAKE_GENERATOR:INTERNAL=(.+)$" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($line) { $generator = $line.Matches[0].Groups[1].Value }
    }
    if ($generator -like "Visual Studio*") {
        $vcxproj = Get-ChildItem -Path $BuildDir -Recurse -Filter "$Target.vcxproj" -ErrorAction SilentlyContinue | Select-Object -First 1
        if (-not $vcxproj) {
            Write-ST-Info (__ "Build.Build.Cache" -Arguments $Target)
            return
        }
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

    $isDir = $Destination.EndsWith('\') -or $Destination.EndsWith('/') -or (Test-Path -Path $Destination -PathType Container)

    # If source is a file and destination doesn't exist yet, treat as directory
    # so the file is copied INTO it (e.g. .../native/).
    if (-not $isDir -and (Test-Path $Source -PathType Leaf) -and -not (Test-Path $Destination)) {
        $isDir = $true
    }

    if ($isDir) {
        if (-not (Test-Path $Destination)) {
            New-Item -ItemType Directory -Path $Destination -Force | Out-Null
        }
    }
    else {
        $parent = Split-Path $Destination -Parent
        if (-not [string]::IsNullOrEmpty($parent) -and -not (Test-Path $parent)) {
            New-Item -ItemType Directory -Path $parent -Force | Out-Null
        }
    }

    Copy-Item -Path $Source -Destination $Destination -Force:$Force

    if ($LASTEXITCODE -ne 0) {
        throw (__ "Build.Copy.Error" -Arguments $LASTEXITCODE)
    }
    else {
        Write-ST-Info (__ "Build.Copy.Success" -Arguments $Source, $Destination)
    }
}

# Resolve artifact path across platforms.
# Multi-config generators (Visual Studio) create Release/ subdirectories;
# single-config generators (Ninja/Makefile on Linux) do not.
# This function tries the path as-is, then without /Release/.
function Resolve-ArtifactPath {
    param([string]$ArtifactPath)

    if (Test-Path $ArtifactPath) {
        return $ArtifactPath
    }
    $stripped = $ArtifactPath -replace '/Release/', '/'
    if (Test-Path $stripped) {
        return $stripped
    }
    return $ArtifactPath
}


Export-ModuleMember -Function Invoke-CMakeConfigure, Invoke-CMakeBuild, Copy-Artifact, Resolve-ArtifactPath, Get-OSKey
