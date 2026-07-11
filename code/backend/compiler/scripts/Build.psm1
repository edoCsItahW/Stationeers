function Write-Info {
    param([string]$Msg)

    Write-Host "[Info] $Msg"
}


function Write-Header {
    param([string]$Msg)

    $delimiter = "========================="

    Write-Host "$delimiter $Msg $delimiter"
}


function Invoke-CMakeConfigure {
    param([string]$BuildDir = "build", [string]$SourceDir = ".", [string]$CoreArtifactDir = "")

    $needsConfigure = -not (Test-Path $BuildDir)

    if (-not $needsConfigure -and $CoreArtifactDir -ne "") {
        $cachePath = Join-Path $BuildDir "CMakeCache.txt"
        if (Test-Path $cachePath) {
            $cacheContent = Get-Content $cachePath -Raw
            if ($cacheContent -notmatch "IC10_CORE_ARTIFACT_DIR") {
                Write-Info "缓存目录存在但未配置 IC10_CORE_ARTIFACT_DIR，需要重新配置"
                $needsConfigure = $true
            }
        }
    }

    if ($needsConfigure) {
        Write-Info "配置 CMake"
        $cmakeArgs = @("-B", $BuildDir, "-S", $SourceDir)
        if ($CoreArtifactDir -ne "") {
            $cmakeArgs += "-DIC10_CORE_ARTIFACT_DIR=$CoreArtifactDir"
            Write-Info "使用核心构件目录: $CoreArtifactDir"
        }
        cmake @cmakeArgs

        if ($LASTEXITCODE -ne 0) {
            throw "CMake 配置失败"
        }

    } else {
        Write-Info "构建目录已存在，跳过 CMake 配置"
    }
}


function Invoke-CMakeBuild {
    param(
        [string]$BuildDir,
        [string]$Target,
        [string]$Config = "Release"
    )

    Write-Header "构建目标"

    Write-Info "构建目标: $Target"
    Write-Info "配置: $Config"

    cmake --build $BuildDir --target $Target --config $Config

    if ($LASTEXITCODE -ne 0) {
        throw "CMake 构建失败，退出码 $LASTEXITCODE"
    }
}


function Copy-Artifact {
    param([string]$Source, [string]$Destination, [switch]$Force)

    if (-not (Test-Path $Source)) {
        throw "源文件不存在: $Source"
    }

    $parent = Split-Path $Destination -Parent

    if (-not (Test-Path $parent)) {
        New-Item -ItemType Directory -Path $parent -Force | Out-Null
    }

    Copy-Item -Path $Source -Destination $Destination -Force:$Force
    Write-Info "已复制 $Source -> $Destination"
}


function Get-CoreArtifactCacheDir {
    param([string]$RootDir = $PWD)
    return Join-Path $RootDir ".core-artifact"
}


function Update-CoreArtifactCache {
    param(
        [string]$BuildDir = "build",
        [string]$Config = "Release"
    )

    $cacheDir = Get-CoreArtifactCacheDir
    $cacheLibDir = Join-Path $cacheDir "lib"
    $cacheIncludeDir = Join-Path $cacheDir "include"

    Write-Header "更新核心构件缓存"

    New-Item -ItemType Directory -Path (Join-Path $cacheLibDir "common" $Config) -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $cacheLibDir "IC10" $Config) -Force | Out-Null
    New-Item -ItemType Directory -Path $cacheIncludeDir -Force | Out-Null

    $commonLib = Join-Path $BuildDir "common" $Config "common.lib"
    $ic10Lib = Join-Path $BuildDir "IC10" $Config "ic10_lib.lib"

    if (Test-Path $commonLib) {
        Copy-Item -Path $commonLib -Destination (Join-Path $cacheLibDir "common" $Config) -Force
        Write-Info "已缓存 common.lib"
    }

    if (Test-Path $ic10Lib) {
        Copy-Item -Path $ic10Lib -Destination (Join-Path $cacheLibDir "IC10" $Config) -Force
        Write-Info "已缓存 ic10_lib.lib"
    }

    Copy-Item -Path (Join-Path $PWD "common" "include" "*") -Destination (Join-Path $cacheIncludeDir "common") -Recurse -Force
    Copy-Item -Path (Join-Path $PWD "IC10" "include" "*") -Destination (Join-Path $cacheIncludeDir "IC10") -Recurse -Force
    Write-Info "已缓存头文件"

    return $cacheDir
}


function Test-CoreArtifactCache {
    param([string]$Config = "Release")

    $cacheDir = Get-CoreArtifactCacheDir
    if (-not (Test-Path $cacheDir)) {
        return $false
    }

    $commonLib = Join-Path $cacheDir "lib" "common" $Config "common.lib"
    $ic10Lib = Join-Path $cacheDir "lib" "IC10" $Config "ic10_lib.lib"

    return (Test-Path $commonLib) -and (Test-Path $ic10Lib)
}


Export-ModuleMember -Function Write-Info, Write-Header, Invoke-CMakeConfigure, Invoke-CMakeBuild, Copy-Artifact, Get-CoreArtifactCacheDir, Update-CoreArtifactCache, Test-CoreArtifactCache
