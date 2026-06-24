function Write-Info {
    param([string]$Msg)

    Write-Host "[Info] $Msg"
}


#function Console-Debug {
#    param([string]$Msg)
#
#    Write-Debug "[Debug] $Msg"
#}


#function Console-Error {
#    param([string]$Msg)
#
#    Write-Error "[Error] $Msg"
#}


function Write-Header {
    param([string]$Msg)

    $delimiter = "========================="

    Write-Host "$delimiter $Msg $delimiter"
}


function Invoke-CMakeConfigure {
    param([string]$BuildDir = "build", [string]$SourceDir = ".")

    if (-not (Test-Path $BuildDir)) {
        Write-Info "首次配置 CMake (生成器自动选择)"
        cmake -B $BuildDir -S $SourceDir

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


Export-ModuleMember -Function Write-Info, Write-Header, Invoke-CMakeConfigure, Invoke-CMakeBuild, Copy-Artifact
