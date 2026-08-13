Import-Module "$PSScriptRoot\Debug.psm1" -Force


$Script:CurrentLang = "en-us"
$Script:FallbackLang = "en-us"
$Script:Translations = @{}

function Initialize-Locale {
    param(
        [string]$Lang = (Get-Culture).Name,
        [string]$Fallback = "en-us",
        [string]$Path = "$PSScriptRoot\languages"
    )

    $Script:FallbackLang = $Fallback
    $Script:CurrentLang = $Lang

    if (-not (Load-LanguageFile -Lang $Lang -Path $Path)) {
        Write-ST-Warning "Failed to load language file for $Lang. Using fallback language $Fallback."
        $Script:CurrentLang = $Fallback
        [void](Load-LanguageFile -Lang $Fallback -Path $Path)
    }
}


function Load-LanguageFile {
    param(
        [string]$Lang,
        [string]$Path
    )

    $file = Join-Path -Path $Path -ChildPath "${Lang}.psd1"

    if (Test-Path $file) {
        $Script:Translations = Import-PowerShellDataFile -Path $file

        return $true
    }

    return $false
}


function Get-Text {
    param(
        [string]$Key,
        [string]$FallbackText = $Key,
        [Object[]]$Arguments,
        [int]$Count = $null
    )

    $value = $Script:Translations
    $Key -split '\.' | ForEach-Object {
        if ($value -is [hashtable] -and $value.ContainsKey($_)) {
            $value = $value[$_]
        }
        else {
            $value = $null
        }
    }

    if ($value -eq $null -or ($value -is [string] -and $value -eq '')) {
        return $FallbackText
    }

    if ($Count -ne $null -and $value -is [hashtable] -and $value.ContainsKey('Other')) {
        if ($Count -eq 0 -and $value.ContainsKey('Zero')) {
            $value = $value['Zero']
        }
        elseif ($Count -eq 1 -and $value.ContainsKey('One')) {
            $value = $value['One']
        }
        else {
            $value = $value['Other']
        }
    }

    if ($Arguments -and $value -is [string]) {
        return $value -f $Arguments
    }

    return $value
}

Set-Alias -Name __ -Value Get-Text -Scope Global
Export-ModuleMember -Function Initialize-Locale, Get-Text