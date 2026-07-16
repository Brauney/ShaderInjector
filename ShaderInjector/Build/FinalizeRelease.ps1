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

$requireSigningValues = @("1", "true", "yes", "on")
$requireSigningSetting = [string]$env:SHADER_INJECTOR_REQUIRE_SIGNING
$requireSigning = $requireSigningValues -contains $requireSigningSetting.Trim().ToLowerInvariant()
$certificateThumbprint = ([string]$env:SHADER_INJECTOR_SIGNING_CERT_SHA1) -replace "\s", ""
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

    & $signTool verify /pa /all /v /tw $TargetPath
    if ($LASTEXITCODE -ne 0) {
        throw "Signature verification failed with exit code $LASTEXITCODE."
    }
} else {
    $unsignedMessage = "Signing skipped: SHADER_INJECTOR_SIGNING_CERT_SHA1 is not configured."

    if ($requireSigning) {
        throw "$unsignedMessage SHADER_INJECTOR_REQUIRE_SIGNING is enabled, so this artifact cannot be released."
    }

    Write-Warning $unsignedMessage
}

$signature = Get-AuthenticodeSignature -LiteralPath $TargetPath
$signatureIsValid = $signature.Status -eq [System.Management.Automation.SignatureStatus]::Valid

if ($certificateThumbprint -and -not $signatureIsValid) {
    throw "Authenticode verification did not report a valid signature. Status: $($signature.Status)."
}

if ($requireSigning -and -not $signatureIsValid) {
    throw "Release signing is required, but the artifact does not have a valid Authenticode signature."
}

$hash = (Get-FileHash -LiteralPath $TargetPath -Algorithm SHA256).Hash.ToLowerInvariant()
$hashPath = "$TargetPath.sha256"
$hashLine = "$hash  $([IO.Path]::GetFileName($TargetPath))"
[IO.File]::WriteAllText($hashPath, "$hashLine`r`n", [Text.UTF8Encoding]::new($false))

$targetFile = Get-Item -LiteralPath $TargetPath
$versionInfo = $targetFile.VersionInfo
$releaseManifest = [ordered]@{
    schemaVersion = 1
    productName = $versionInfo.ProductName
    productVersion = $versionInfo.ProductVersion
    fileName = $targetFile.Name
    fileVersion = $versionInfo.FileVersion
    sizeBytes = $targetFile.Length
    sha256 = $hash
    generatedUtc = [DateTime]::UtcNow.ToString("o")
    sourceRepository = "https://github.com/frostbone25/ShaderInjector"
    authenticode = [ordered]@{
        status = $signature.Status.ToString()
        valid = $signatureIsValid
        signerSubject = if ($signature.SignerCertificate) { $signature.SignerCertificate.Subject } else { $null }
        signerThumbprint = if ($signature.SignerCertificate) { $signature.SignerCertificate.Thumbprint } else { $null }
        certificateNotAfterUtc = if ($signature.SignerCertificate) { $signature.SignerCertificate.NotAfter.ToUniversalTime().ToString("o") } else { $null }
        timestampSubject = if ($signature.TimeStamperCertificate) { $signature.TimeStamperCertificate.Subject } else { $null }
    }
}
$manifestPath = "$TargetPath.release.json"
$releaseManifestJson = $releaseManifest | ConvertTo-Json -Depth 4
[IO.File]::WriteAllText($manifestPath, "$releaseManifestJson`r`n", [Text.UTF8Encoding]::new($false))

Write-Host "SHA-256: $hash"
Write-Host "Checksum: $hashPath"
Write-Host "Release manifest: $manifestPath"
Write-Host "Authenticode status: $($signature.Status)"
