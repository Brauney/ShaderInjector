# Release Signing

Release builds always generate `dsound.dll.sha256` and `dsound.dll.release.json`
beside the DLL. Signing occurs before these files are generated.

## Certificate-store signing

Install a trusted code-signing certificate in the current user's Windows
certificate store, then set its SHA-1 thumbprint before building:

```powershell
$env:SHADER_INJECTOR_SIGNING_CERT_SHA1 = "CERTIFICATE_THUMBPRINT"
$env:SHADER_INJECTOR_REQUIRE_SIGNING = "1"
```

Optional configuration:

```powershell
$env:SHADER_INJECTOR_TIMESTAMP_URL = "https://timestamp.digicert.com"
$env:SHADER_INJECTOR_SIGNTOOL_PATH = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\signtool.exe"
```

Build `Release|x64`. `FinalizeRelease.ps1` signs `dsound.dll`, verifies its
Authenticode signature, and writes the checksum and release manifest. With
`SHADER_INJECTOR_REQUIRE_SIGNING=1`, a missing certificate, signing failure, or
verification failure stops the build rather than producing a publishable artifact.

Without that setting, an unsigned artifact may still be produced for local testing,
but the build emits a warning and the release manifest records `valid: false`.

Never commit a PFX file, certificate password, private key, or signing-service
credential. CI should obtain signing credentials from its secret store.
