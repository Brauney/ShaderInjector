# Cross-Version Shader Discovery Tests

This folder is a small offline test program for the ShaderInjector mod. The mod swaps game shaders at runtime; to do that safely it must recognize “the same logical shader” even when the game’s compiled bytecode (DXIL — the GPU shader binary format) changes between patches.

These tests exercise that recognition logic using **real captured ModifiedShader packages** from FF7 Rebirth 1.004 and 1.005.

## What problem this catches

When FF7 Rebirth updates (for example 1.004 → 1.005), shader bytes often change even though the pass is still “the local light pixel shader.” The mod fingerprints shaders instead of trusting raw bytes alone, but today’s fingerprints can still miss across patches. The parent document [`CrossVersionDiscovery.md`](../../CrossVersionDiscovery.md) explains why.

This suite runs focused scenarios and prints a human-readable report for each: which fingerprint fields match, the similarity score, and whether the current discovery code would accept or reject the match.

## Fixture data

Real data lives under `data/case01-real-captures/`. It is **not** from the `Dumps` folder — it is full **ModifiedShader packages** copied from a live Windows install.

Layout:

```
data/case01-real-captures/
  local-light/
    v1.004/   package.json, modified-shader.hlsl, modified-shader.blob
    v1.005/   package.json, modified-shader.hlsl, modified-shader.blob
  directional-light/
    v1.004/   …
    v1.005/   …
```

Each `package.json` is a complete `ShaderInjector.ModifiedShader` document. The test reads `targets[0]` for the **original game shader** fingerprint (hash, byte length, analysis). The `.hlsl` / `.blob` files are the mod’s replacement shader source and compiled output; they are kept for completeness but the tests do not need them for matching math.

#### What the three real file types are

On a real install these files live under:

`<Game>/ShaderInjector/ModifiedShaders/<PackageName>/`

| File | What it is |
|------|------------|
| `<PackageName>.json` | Package metadata plus `targets[]` — each target stores the **original game shader** hash, byte length, and full analyzer fingerprint (`shaderAnalysis`). |
| `<PackageName>.hlsl` | Human-editable HLSL source for the **mod replacement** shader. |
| `<PackageName>.blob` | DXIL bytecode produced by compiling that HLSL with the bundled `dxc` tool — **not** the game’s original shader. |

A package is normally created from the in-game UI (“Create Modified Shader template”) while viewing a captured pipeline shader. The folder name embeds `Hash::HashMemory` of the **original game bytecode** at capture time (for example `ModifiedPixelShader_C7097C5B2ACFE4AA`). That hash differs per game patch even when the mod’s `.hlsl` / `.blob` stay identical.

#### Optional: raw bytecode dumps

The GUI “Dump Bytecode” button writes to `<Game>/ShaderInjector/Dumps/` as `<ShaderType>_<HASH>.bin` plus a `.dxil` disassembly — a separate, optional debug path. Shader-target replacements under `ShaderTargets/` use `OriginalShaderBytecode.bin` inside each target folder. These tests do not use either of those paths; they only need the package JSON fingerprints.

#### Capturing more real packages yourself

1. Install the mod on the game version you care about.
2. Open the injector menu, find the shader pass, and use **Create Modified Shader template** (or copy an existing package folder after editing).
3. Copy the whole package folder from `ShaderInjector/ModifiedShaders/` (`.json`, `.hlsl`, `.blob` — skip Windows `*:Zone.Identifier` sidecars).
4. Drop copies into `data/case01-real-captures/<shader-name>/v<version>/` using the neutral names above (`package.json`, etc.), or add a new case folder following the same pattern.

Repeat on a second game patch to get paired cross-version data.

## Test cases

### Case 1 — cross-patch fingerprint match (real captures)

Scores a 1.004 runtime shader fingerprint against the paired 1.005 ModifiedShader package for Local Light and Directional Light. Both passes currently **accept** discovery at ~0.946 similarity with matching `crossVersionIdentityHash`. This confirms the one-candidate-vs-one-package path works for these two shaders.

For **ShaderTarget replacement** discovery (`DiscoverEnabledReplacement`), these real pairs also share identical `crossVersionIdentityHash`, so they still match on the **exact-identity fast path** — the fuzzy fallback is not needed for this specific capture data.

### Case 2 — ambiguity rejection (multi-candidate)

Production `DiscoverEnabledModifiedShader` does not just ask “is there a good match?” — it also asks “is the best match clearly better than the runner-up?” If two enabled packages score within `0.02` of each other (`SHADER_INJECTOR_DISCOVERY_SIMILARITY_AMBIGUITY_MARGIN`), the mod **rejects both** rather than risk picking the wrong replacement.

In plain terms: the mod finds two shaders that look almost equally similar and gives up instead of guessing wrong.

This matters for the real-world “auto discovery doesn’t work on 1.004” complaint because a live install has **many** ModifiedShader packages enabled at once, not just one. A legitimately correct match can be rejected purely because something else scores almost as high.

Case 2 runs three sub-scenarios:

| Sub-scenario | Data | What it shows |
|--------------|------|---------------|
| **Baseline** | Fully real: 1.004 Local Light candidate vs 1.005 Local Light + Directional Light packages | Different shaders score far apart (~0.965 vs ~0.780); discovery accepts the correct winner. |
| **Dual-version probe** | Fully real: 1.004 candidate vs both 1.004 and 1.005 Local Light packages enabled | Same-patch package wins (~1.0 vs ~0.965); margin clears 0.02 and discovery accepts. |
| **Constructed near-tie** | Real 1.005 Local Light package + exact in-memory clone | Both score identically (~0.965); margin is zero; discovery rejects despite both being above the 0.90 minimum. |

The constructed near-tie is synthetic in the sense that it duplicates one real package in memory — real data alone did not produce a natural near-tie between two *different* packages. It demonstrates the rejection mechanism exists and can fire in realistic multi-package installs.

### Case 3 — replacement fuzzy fallback (ShaderTarget path)

`DiscoverEnabledReplacement` applies enabled **ShaderTarget** replacements at runtime. It used to accept only an exact `crossVersionIdentityHash` match. It now keeps that fast path and adds a **fuzzy fallback**: among replacements that pass the ±5% length gate and share the same `portableReflectionIdentityHash` (interface/resources/CBs/stage hard gate), pick the best weighted similarity score if it clears the same 0.90 minimum and 0.02 ambiguity margin used elsewhere.

Real 1.004/1.005 Local Light captures share identical identity hashes, so Case 3 uses a **constructed identity drift** on otherwise-real 1.005 data: flip `crossVersionIdentityHash` while leaving portable reflection aligned. That shows:

| Sub-scenario | What it shows |
|--------------|---------------|
| **Identity drift accept** | Exact path would reject; fuzzy path accepts the correct 1.005 Local Light ShaderTarget (~0.907 score when only identity hash differs). |
| **Cross-patch near-miss** | Same drift applied to the real 1.004-vs-1.005 pair scores ~0.846 (below 0.90) — documents a remaining gap when compile-unstable fields already differ. |
| **Reflection gate** | Drifted Local Light candidate with two enabled replacements (Local + Directional Light) — Directional Light never enters the fuzzy set despite being present. |
| **Fuzzy ambiguity reject** | Two identical Local Light ShaderTarget clones score the same; fuzzy matching rejects instead of guessing. |

## How to build and run

Verified on WSL2 Ubuntu with `g++` 9.3. Requires `build-essential` (`g++`, `make`) and CMake 3.16+.

```bash
cd Tests/CrossVersionDiscovery
rm -rf build && mkdir build && cd build
cmake ..
cmake --build .
./cross_version_discovery_tests
```

Or via CTest: `ctest --output-on-failure`

Filter by case group:

```bash
./cross_version_discovery_tests -tc="case01*"
./cross_version_discovery_tests -tc="case02*"
./cross_version_discovery_tests -tc="case03*"
```

**Bootstrapping CMake without sudo:** if `apt install cmake` is unavailable, download a portable binary once:

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.29.6/cmake-3.29.6-linux-x86_64.tar.gz
tar xzf cmake-3.29.6-linux-x86_64.tar.gz
/tmp/cmake-3.29.6-linux-x86_64/bin/cmake ..
/tmp/cmake-3.29.6-linux-x86_64/bin/cmake --build .
```

A portable CMake may already exist at `/tmp/cmake-3.29.6-linux-x86_64/bin/cmake` from a prior bootstrap.

## How to read the output

Each case prints a block like:

```
raw hash equal? yes/no
portable reflection equal? yes/no
semantic instruction set equal? yes/no
crossVersionIdentity equal? yes/no
similarity score: 0.XX
sub-scores that hurt: ...
DiscoverEnabledModifiedShader accept? yes/no
DiscoverEnabledReplacement accept? yes/no
length gate enqueue (±15%)? yes/no
length gate replacement (±5%)? yes/no
```

Replacement fuzzy cases (Case 3) additionally print whether the match used exact identity or fuzzy similarity, plus best/second-best fuzzy scores and margin.

Ambiguity cases additionally print per-package scores, winner, runner-up, margin, and whether the margin clears the 0.02 threshold.

| Result | Meaning |
|--------|---------|
| **PASS** | Behavior matches expectations for that case |
| **FAIL** | Unexpected regression |

## What code this reuses

The build compiles real production sources where possible (`Hash.cpp`, `ShaderAnalysis.cpp`). Discovery decision math that depends on Windows-only analyzer/IO code lives in mirrored test files (`DiscoveryLogicMirror.*`, `ModifiedShaderMatching.*`) kept in sync with `ShaderDiscovery.cpp` and `ModifiedShader.cpp` by comment links to the original line ranges.

Real package JSON deserializes through production `ModifiedShader::PackageDisk` / `ShaderAnalysis::ShaderAnalysisDisk` types via `LoadPackageTargetCapture` in `FixtureLoader.h`.
