param(
    [Parameter(Mandatory = $true)]
    [string]$TargetPath
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $TargetPath -PathType Leaf)) {
    throw "Release artifact does not exist: $TargetPath"
}

function Find-SignTool {
    if ($env:SHADER_INJECTOR_SIGNTOOL_PATH) {
        if (-not (Test-Path -LiteralPath $env:SHADER_INJECTOR_SIGNTOOL_PATH -PathType Leaf)) {
            throw "SHADER_INJECTOR_SIGNTOOL_PATH does not point to a file."
        }

        return $env:SHADER_INJECTOR_SIGNTOOL_PATH
    }

    $windowsKitsBin = Join-Path ${env:ProgramFiles(x86)} "Windows Kits\10\bin"
    $candidate = Get-ChildItem -LiteralPath $windowsKitsBin -Filter signtool.exe -File -Recurse -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match "\\x64\\signtool\.exe$" } |
        Sort-Object FullName -Descending |
        Select-Object -First 1

    if (-not $candidate) {
        throw "SignTool was not found. Install the Windows SDK or set SHADER_INJECTOR_SIGNTOOL_PATH."
    }

    return $candidate.FullName
}

$certificateThumbprint = $env:SHADER_INJECTOR_SIGNING_CERT_SHA1
if ($certificateThumbprint) {
    $signTool = Find-SignTool
    $timestampUrl = if ($env:SHADER_INJECTOR_TIMESTAMP_URL) {
        $env:SHADER_INJECTOR_TIMESTAMP_URL
    } else {
        "http://timestamp.digicert.com"
    }

    Write-Host "Signing $TargetPath"
    & $signTool sign `
        /sha1 $certificateThumbprint `
        /fd SHA256 `
        /tr $timestampUrl `
        /td SHA256 `
        /d "Shader Injector D3D12 Mod" `
        /du "https://github.com/frostbone25/ShaderInjector" `
        $TargetPath

    if ($LASTEXITCODE -ne 0) {
        throw "SignTool failed with exit code $LASTEXITCODE."
    }

    & $signTool verify /pa /v $TargetPath
    if ($LASTEXITCODE -ne 0) {
        throw "Signature verification failed with exit code $LASTEXITCODE."
    }
} else {
    Write-Host "Signing skipped: SHADER_INJECTOR_SIGNING_CERT_SHA1 is not configured."
}

$hash = (Get-FileHash -LiteralPath $TargetPath -Algorithm SHA256).Hash.ToLowerInvariant()
$hashPath = "$TargetPath.sha256"
$hashLine = "$hash  $([IO.Path]::GetFileName($TargetPath))"
[IO.File]::WriteAllText($hashPath, "$hashLine`r`n", [Text.UTF8Encoding]::new($false))
Write-Host "SHA-256: $hash"
Write-Host "Checksum: $hashPath"
