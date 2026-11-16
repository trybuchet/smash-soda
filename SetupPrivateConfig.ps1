$ErrorActionPreference = 'Stop'

$configPath = Join-Path $PSScriptRoot 'SmashSoda\PrivateConfig.h'
$templatePath = Join-Path $PSScriptRoot 'SmashSoda\PrivateConfig.example.h'

if (-not (Test-Path -LiteralPath $configPath)) {
    Copy-Item -LiteralPath $templatePath -Destination $configPath
}

$content = Get-Content -LiteralPath $configPath -Raw
$rng = [System.Security.Cryptography.RandomNumberGenerator]::Create()

function Set-PrivateConfigString([string]$Text, [string]$Name, [string]$Value) {
    $pattern = '(inline constexpr char ' + $Name + '\[\] = )"([^"]*)";'
    $match = [regex]::Match($Text, $pattern)
    if (-not $match.Success) {
        throw "PrivateConfig.h is missing $Name. Restore it from PrivateConfig.example.h."
    }
    $replacement = $match.Groups[1].Value + '"' + $Value + '";'
    return [regex]::Replace(
        $Text,
        $pattern,
        [System.Text.RegularExpressions.MatchEvaluator]{ param($current) $replacement },
        1
    )
}

try {
    foreach ($entry in @(
        @{ Name = 'SessionEncryptionKey'; Length = 32 },
        @{ Name = 'SessionEncryptionNonce'; Length = 12 },
        @{ Name = 'ArcadeCredentialsEncryptionKey'; Length = 32 }
    )) {
        $pattern = '(inline constexpr char ' + $entry.Name + '\[\] = )"([^"]*)";'
        $match = [regex]::Match($content, $pattern)
        if (-not $match.Success) {
            throw "PrivateConfig.h is missing $($entry.Name). Restore it from PrivateConfig.example.h."
        }
        if ($match.Groups[2].Value.Length -eq 0) {
            $bytes = New-Object byte[] $entry.Length
            $rng.GetBytes($bytes)
            $hex = -join ($bytes | ForEach-Object { $_.ToString('x2') })
            $replacement = $match.Groups[1].Value + '"' + $hex + '";'
            $content = [regex]::Replace(
                $content,
                $pattern,
                [System.Text.RegularExpressions.MatchEvaluator]{ param($current) $replacement },
                1
            )
        }
    }
}
finally {
    $rng.Dispose()
}

$clientIdMatch = [regex]::Match($content, 'inline constexpr char ClientAppId\[\] = "([^"]*)";')
$clientSecretMatch = [regex]::Match($content, 'inline constexpr char ClientAppSecret\[\] = "([^"]*)";')
if (-not $clientIdMatch.Success -or -not $clientSecretMatch.Success) {
    throw 'PrivateConfig.h is missing client credential settings. Restore it from PrivateConfig.example.h.'
}

if ([string]::IsNullOrWhiteSpace($clientIdMatch.Groups[1].Value)) {
    $clientId = Read-Host 'Enter the client ID issued to your app in Soda Arcade'
    if ([string]::IsNullOrWhiteSpace($clientId) -or $clientId.Contains('"') -or $clientId.Contains("`r") -or $clientId.Contains("`n")) {
        throw 'The client ID is empty or contains unsupported characters.'
    }
    $content = Set-PrivateConfigString $content 'ClientAppId' $clientId
}

if ([string]::IsNullOrWhiteSpace($clientSecretMatch.Groups[1].Value)) {
    $secureSecret = Read-Host 'Enter the client secret issued to your app in Soda Arcade' -AsSecureString
    $secretPointer = [System.Runtime.InteropServices.Marshal]::SecureStringToBSTR($secureSecret)
    try {
        $clientSecret = [System.Runtime.InteropServices.Marshal]::PtrToStringBSTR($secretPointer)
    }
    finally {
        [System.Runtime.InteropServices.Marshal]::ZeroFreeBSTR($secretPointer)
    }
    if ([string]::IsNullOrWhiteSpace($clientSecret) -or $clientSecret.Contains('"') -or $clientSecret.Contains("`r") -or $clientSecret.Contains("`n")) {
        throw 'The client secret is empty or contains unsupported characters.'
    }
    $content = Set-PrivateConfigString $content 'ClientAppSecret' $clientSecret
}

$utf8 = New-Object -TypeName System.Text.UTF8Encoding -ArgumentList $false
[System.IO.File]::WriteAllText($configPath, $content, $utf8)
Write-Host "Prepared ignored local config: $configPath"
Write-Host 'Your Soda Arcade API credentials are stored only in the ignored local header and compiled into this build.'
