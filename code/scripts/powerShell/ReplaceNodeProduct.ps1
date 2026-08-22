Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Builds.psm1"
Import-Module -Force -Scope Global -Name "$PSScriptRoot\modules\Locale.psm1"

Initialize-Locale -Lang "zh-hans"

Copy-Artifact -Source "$PSScriptRoot/../../IC10/backend/compiler/publish/node/src/ic10c-node.node" -Destination "$PSScriptRoot/../../IC10/plugins/vscode/ic10-language-support/node_modules/ic10c-node/src/ic10c-node.node" -Force
Copy-Artifact -Source "$PSScriptRoot/../../IC10/backend/runtime/publish/node/src/ic10r-node.node" -Destination "$PSScriptRoot/../../IC10/plugins/vscode/ic10-language-support/node_modules/ic10r-node/src/ic10r-node.node" -Force