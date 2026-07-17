# Release Signing

Release builds always generate `dsound.dll.sha256` beside the DLL. Signing is
optional and occurs before the checksum is calculated.

## Certificate-store signing

Install a trusted code-signing certificate in the current user's Windows
certificate store, then set its SHA-1 thumbprint before building:

```powershell
$env:SHADER_INJECTOR_SIGNING_CERT_SHA1 = "CERTIFICATE_THUMBPRINT"
```

Optional configuration:

```powershell
$env:SHADER_INJECTOR_TIMESTAMP_URL = "http://timestamp.digicert.com"
$env:SHADER_INJECTOR_SIGNTOOL_PATH = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\signtool.exe"
```

Build `Release|x64`. `FinalizeRelease.ps1` signs `dsound.dll`, verifies its
Authenticode signature, and writes the checksum. A signing or verification
failure fails the build rather than publishing an unsigned artifact by mistake.

Never commit a PFX file, certificate password, private key, or signing-service
credential. CI should obtain signing credentials from its secret store.
