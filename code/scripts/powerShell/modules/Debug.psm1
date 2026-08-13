enum Level {
    Debug
    Info
    Warning
    Error
}


function Write-ST-message {
    param(
        [Level]$MsgLevel,
        [string]$Message
    )

    switch ($MsgLevel) {
        Debug {
            Write-Debug "[STATIONEERS] - Debug: $Message"
        }

        Info {
            Write-Host "[STATIONEERS] - Info: $Message" -ForegroundColor Green
        }

        Warning {
            Write-Warning "[STATIONEERS] - Warning: $Message"
        }

        Error {
            Write-Error "[STATIONEERS] - Error: $Message"
        }
    }
}

function Write-ST-Debug {
    param(
        [string]$Message
    )

    Write-ST-message -MsgLevel Debug -Message $Message
}

function Write-ST-Info {
    param(
        [string]$Message
    )

    Write-ST-message -MsgLevel Info -Message $Message
}

function Write-ST-Warning {
    param(
        [string]$Message
    )

    Write-ST-message -MsgLevel Warning -Message $Message
}

function Write-ST-Error {
    param(
        [string]$Message
    )

    Write-ST-message -MsgLevel Error -Message $Message
}

function Write-ST-Phase {
    param(
        [string]$Message
    )

    $msgLen = $Message.Length

    $delim = '/' * ($msgLen + 42)

    Write-Host $delim
    Write-Host (('/' * 20) + " $Message " + ('/' * 20))
    Write-Host $delim
}


Export-ModuleMember -Function Write-ST-message, Write-ST-Debug, Write-ST-Info, Write-ST-Warning, Write-ST-Error, Write-ST-Phase
