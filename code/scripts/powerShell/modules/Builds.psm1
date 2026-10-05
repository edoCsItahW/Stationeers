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

    $isSourceDir = Test-Path $Source -PathType Container

    if ($isSourceDir) {
        # 目录：把源目录的“内容”复制到目标目录
        if (-not (Test-Path $Destination)) {
            New-Item -ItemType Directory -Path $Destination -Force | Out-Null
        }
        Copy-Item -Path (Join-Path $Source '*') -Destination $Destination -Recurse -Force:$Force

        Write-ST-Info (__ "Build.Copy.Success" -Arguments $Source, $Destination)
        return
    }

    # 单文件：保持原有逻辑
    $isDir = $Destination.EndsWith('\') -or $Destination.EndsWith('/') -or (Test-Path -Path $Destination -PathType Container)

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


# 读标准库的文件头日期标志（`# stdLib.ic YYYY-MM-DD`）。
# 标准库以 assets/ic/stdLib.ic 为**唯一来源**（见 script/genStdLib.py 的文件头说明），
# 发布副本是构建产物（见 publish/node/.gitignore），因此用这一行确认副本与源是同一份。
function Get-StdLibMarker {
    param([string]$Path)

    if ([string]::IsNullOrEmpty($Path) -or -not (Test-Path $Path -PathType Leaf)) {
        return $null
    }

    $first = Get-Content -Path $Path -TotalCount 1

    if ($first -match '\d{4}-\d{2}-\d{2}') {
        return $Matches[0]
    }

    return $null
}


# 把标准库从 assets 同步到发布目录，并比对日期标志——
# 复制后再比对，既完成同步，也防止"复制没生效/副本是旧的"却照样发布。
function Sync-StdLib {
    param(
        [string]$Source,
        [string]$PublishDir
    )

    if ([string]::IsNullOrEmpty($Source) -or -not (Test-Path $Source -PathType Leaf)) {
        throw (__ "Build.StdLib.SourceNotFound" -Arguments $Source)
    }

    $sourceMarker = Get-StdLibMarker $Source

    if (-not $sourceMarker) {
        throw (__ "Build.StdLib.NoMarker" -Arguments $Source)
    }

    Copy-Artifact -Source $Source -Destination $PublishDir -Force

    $copied = Join-Path $PublishDir (Split-Path $Source -Leaf)
    $copiedMarker = Get-StdLibMarker $copied

    if ($copiedMarker -ne $sourceMarker) {
        throw (__ "Build.StdLib.Stale" -Arguments $sourceMarker, "$copiedMarker")
    }

    Write-ST-Info (__ "Build.StdLib.Synced" -Arguments $sourceMarker, $copied)
}


Export-ModuleMember -Function Invoke-CMakeConfigure, Invoke-CMakeBuild, Copy-Artifact, Resolve-ArtifactPath, Get-OSKey, Get-StdLibMarker, Sync-StdLib
