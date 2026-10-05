Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Builds.psm1"
Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Locale.psm1"

Initialize-Locale -Lang "zh-hans"

# ---- compiler ----
Copy-Artifact `
    -Source      "$PSScriptRoot/../../IC10/backend/compiler/publish/node/src/ic10c-node.node" `
    -Destination "$PSScriptRoot/../../IC10/plugins/vscode/ic10-language-support/node_modules/@ic10/compiler/src/ic10c-node.node" `
    -Force

Copy-Artifact `
    -Source      "$PSScriptRoot/../../IC10/backend/compiler/publish/node/types" `
    -Destination "$PSScriptRoot/../../IC10/plugins/vscode/ic10-language-support/node_modules/@ic10/compiler/types" `
    -Force

# ---- runtime ----
Copy-Artifact `
    -Source      "$PSScriptRoot/../../IC10/backend/runtime/publish/node/src/ic10r-node.node" `
    -Destination "$PSScriptRoot/../../IC10/plugins/vscode/ic10-language-support/node_modules/@ic10/runtime/src/ic10r-node.node" `
    -Force

Copy-Artifact `
    -Source      "$PSScriptRoot/../../IC10/backend/runtime/publish/node/types" `
    -Destination "$PSScriptRoot/../../IC10/plugins/vscode/ic10-language-support/node_modules/@ic10/runtime/types" `
    -Force