# =============================================================================
# 构建 IC10 Node.js 原生模块（仅构建必需依赖，支持增量编译）
# =============================================================================

# 切换到项目根目录
Push-Location (Join-Path $PSScriptRoot "..")

# 配置构建类型（可修改为 Debug）
$BuildConfig = "Release"

Write-Output "当前工作目录: $PWD"

# ============================================================================
# 1. 配置（仅在 build 目录不存在时执行）
# ============================================================================
if (-not (Test-Path "build")) {
    Write-Output "加载CMake目标: 首次配置构建 (生成器自动选择)"
    cmake -B build -S .
} else {
    Write-Output "构建目录已存在，跳过 CMake 配置（如需重新配置请删除 build 目录）"
}

# ============================================================================
# 2. 构建（仅构建 IC10_Node_Build 及其依赖）
# ============================================================================
Write-Output "`n编译目标: 仅构建 IC10_Node_API 模块 (配置: $BuildConfig)"
cmake --build build --target IC10_Node_API --config $BuildConfig

# ============================================================================
# 3. 复制生成的 .node 文件到 test/ 目录
# ============================================================================
# Visual Studio 多配置生成器输出路径为：
#   build/exports/IC10/node/<配置名>/IC10_Node_Build.node
$nodeFile = "build/exports/IC10/node/$BuildConfig/ic10-node-api.node"
$testDir = "tests/ic10/node"
$moduleDir = "$testDir/ic10/src"
Write-Output "`n复制目标: 从 $nodeFile 复制到 $testDir 目录"
if (Test-Path $nodeFile) {
    # 确保 test 目录存在
    if (-not (Test-Path $moduleDir)) { New-Item -ItemType Directory -Path $moduleDir | Out-Null }
    Move-Item -Path $nodeFile -Destination $moduleDir -Force
    Write-Output "复制成功"
} else {
    Write-Error "未找到生成的文件: $nodeFile"
    Pop-Location
    exit 1
}

# ============================================================================
# 4. 运行测试
# ============================================================================
Write-Output "`n测试目标: 在 Node.js 环境下测试原生模块"
# 若 test.ts 使用 ES Module，可以用 ts-node 或 node --loader ts-node/esm
node $testDir\test.js

ts-node $testDir\test.ts

Write-Output "进程结束: 退出码 $LASTEXITCODE"

Pop-Location