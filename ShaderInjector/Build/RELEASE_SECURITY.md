# Release Security Checklist

Shader Injector is a `dsound.dll` proxy that installs runtime D3D12 hooks. Those behaviors are
also used by malicious software, so an unsigned build can attract heuristic antivirus detections.
Version metadata and checksums improve transparency, but they do not establish publisher identity.

## Production Signing

1. Obtain an Authenticode code-signing certificate issued by a publicly trusted certificate
   authority. A self-signed development certificate does not establish trust on user machines.
   Microsoft currently recommends Azure Artifact Signing for applications distributed outside
   the Microsoft Store. Qualifying open-source projects can also apply to SignPath Foundation for
   free code signing:

   - Microsoft code-signing options: https://learn.microsoft.com/windows/apps/package-and-deploy/code-signing-options
   - SignPath Foundation: https://signpath.org/
2. Install the certificate and private key in the signing account's Personal certificate store.
   Hardware-backed certificates may expose the key through the same store.
3. Find the certificate thumbprint:

   ```powershell
   Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert |
       Select-Object Subject, Thumbprint, NotAfter, HasPrivateKey
   ```

4. Set the signing environment for the release build:

   ```powershell
   $env:SHADER_INJECTOR_SIGNING_CERT_SHA1 = "CERTIFICATE_THUMBPRINT"
   $env:SHADER_INJECTOR_REQUIRE_SIGNING = "1"
   $env:SHADER_INJECTOR_TIMESTAMP_URL = "https://timestamp.digicert.com"
   ```

5. Build `Release|x64`. The post-build step signs with SHA-256, obtains an RFC 3161 SHA-256
   timestamp, verifies the Authenticode chain, and refuses to produce a releasable artifact if
   signing is required but unavailable.
6. Confirm `x64/Release/dsound.dll.release.json` reports `authenticode.valid` as `true`.

Do not publish an unsigned fallback after a signing failure. Do not publish the PFX, private key,
token credentials, or certificate password. Keep using the same publisher certificate identity
across releases so security products can accumulate reputation for the publisher.

## Antivirus Submission

Submit the final signed `dsound.dll`, not an earlier unsigned build, through the Microsoft Security
Intelligence file submission portal as a software developer. Select the applicable Defender product
and `Incorrectly detected as malware/malicious`, include the exact detection name, GitHub source URL,
release URL, and SHA-256 from `dsound.dll.sha256`, then retain the submission ID.

- Microsoft submission portal: https://www.microsoft.com/wdsi/filesubmission
- Microsoft software developer FAQ: https://learn.microsoft.com/en-us/defender-xdr/developer-faq
- SignTool documentation: https://learn.microsoft.com/en-us/windows/win32/seccrypto/signtool

Each changed DLL has a new hash. Submit the exact release artifact when a new version is detected.
Other antivirus vendors have their own false-positive submission channels and may require separate
submissions.

## Distribution

- Publish `dsound.dll`, `dsound.dll.sha256`, and `dsound.dll.release.json` together.
- Publish releases from one canonical HTTPS location.
- Keep source and release tags public and aligned with the embedded product version.
- Do not pack, encrypt, or obfuscate the DLL. Those techniques make inspection harder and can
  increase heuristic detections.
- Do not instruct users to disable antivirus protection. Resolve false positives with signed builds
  and vendor submissions.

## Development-only behavior

The release DLL intentionally avoids enumerating running RenderDoc processes. RenderDoc remains
discoverable when it is already injected, through its registered installation, through the standard
installation directory, or through `SHADER_INJECTOR_RENDERDOC_PATH`. This keeps a disabled-by-default
developer feature from adding process-enumeration APIs to the production DLL import surface.
