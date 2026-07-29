<#
.SYNOPSIS
    从 npm 迁移到 pnpm：删除所有 package-lock.json 和 node_modules，并在每个 package.json 所在目录运行 pnpm i。
.DESCRIPTION
    该脚本会递归遍历指定根目录（默认为当前目录），删除所有 package-lock.json 文件和 node_modules 文件夹，
    然后查找所有包含 package.json 的目录，并依次执行 pnpm i 安装依赖。
.PARAMETER Root
    项目根目录路径，默认为当前目录。
.PARAMETER Force
    强制执行，跳过确认提示。
.EXAMPLE
    .\Convert-NpmToPnpm.ps1
    在当前目录执行迁移。
.EXAMPLE
    .\Convert-NpmToPnpm.ps1 -Root "C:\MyProject" -Force
    在指定路径执行迁移，无需确认。
#>

param(
    [string]$Root = ".",
    [switch]$Force
)

# 解析根目录为绝对路径
$Root = Resolve-Path $Root

Write-Host "Converting from npm to pnpm in '$Root'" -ForegroundColor Cyan

# 检查 pnpm 是否已安装
if (-not (Get-Command pnpm -ErrorAction SilentlyContinue)) {
    Write-Error "pnpm is not installed. Please install pnpm first (npm install -g pnpm)."
    exit 1
}

# 确认操作（除非使用 -Force）
if (-not $Force) {
    $confirmation = Read-Host "This will delete ALL package-lock.json and node_modules folders recursively. Are you sure? (y/n)"
    if ($confirmation -ne 'y') {
        Write-Host "Aborted."
        exit 0
    }
}

# ---------- 1. 删除所有 package-lock.json ----------
Write-Host "`nDeleting package-lock.json files..." -ForegroundColor Yellow
$lockFiles = Get-ChildItem -Path $Root -Filter "package-lock.json" -Recurse -File
$lockCount = 0
foreach ($file in $lockFiles) {
    Remove-Item $file.FullName -Force -ErrorAction SilentlyContinue
    if (-not $?) {
        Write-Warning "Failed to delete $($file.FullName)"
    } else {
        $lockCount++
        Write-Host "Deleted $($file.FullName)"
    }
}
Write-Host "Deleted $lockCount package-lock.json file(s)." -ForegroundColor Green

# ---------- 2. 删除所有 node_modules（从最深层开始） ----------
Write-Host "`nDeleting node_modules directories..." -ForegroundColor Yellow
$nodeModulesDirs = Get-ChildItem -Path $Root -Directory -Recurse | Where-Object { $_.Name -eq "node_modules" }
# 按路径长度降序排序，确保先删除最深层的目录，避免因父目录被删导致子目录找不到的错误
$sortedDirs = $nodeModulesDirs | Sort-Object -Property { $_.FullName.Length } -Descending
$dirCount = 0
foreach ($dir in $sortedDirs) {
    Remove-Item $dir.FullName -Recurse -Force -ErrorAction SilentlyContinue
    if (-not $?) {
        Write-Warning "Failed to delete $($dir.FullName)"
    } else {
        $dirCount++
        Write-Host "Deleted $($dir.FullName)"
    }
}
Write-Host "Deleted $dirCount node_modules folder(s)." -ForegroundColor Green

# ---------- 3. 在每个包含 package.json 的目录中运行 pnpm i ----------
Write-Host "`nInstalling dependencies with pnpm in each package..." -ForegroundColor Yellow
$packageJsonFiles = Get-ChildItem -Path $Root -Filter "package.json" -Recurse -File
$pkgCount = 0
foreach ($file in $packageJsonFiles) {
    $dir = $file.Directory
    Write-Host "Installing in '$($dir.FullName)'" -ForegroundColor Green
    Push-Location $dir.FullName
    try {
        pnpm i
        if ($LASTEXITCODE -eq 0) {
            $pkgCount++
            Write-Host "Successfully installed in '$($dir.FullName)'" -ForegroundColor Green
        } else {
            Write-Warning "pnpm i failed in '$($dir.FullName)' (exit code: $LASTEXITCODE)"
        }
    } catch {
        Write-Warning "Error installing in '$($dir.FullName)': $_"
    } finally {
        Pop-Location
    }
}
Write-Host "`nConversion completed. Installed dependencies in $pkgCount package(s)." -ForegroundColor Cyan