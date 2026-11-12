# Ballanced build system

CMake presets are the primary entry point. The root project combines `Source/Player` with the engine provided by `Source/NeMo2`.

Use manual `cmake -S ... -B ...` commands only when a preset does not cover your generator or architecture.

`CMakePresets.json` is the source of truth for the supported full-runtime matrix. Component-level presets and READMEs cover standalone builds and may intentionally support a narrower matrix.

This guide applies to the default `sdl` branch.

## Prerequisites

| Platform | Toolchain | Generator | CMake |
|----------|-----------|-----------|-------|
| Windows (x86 / x64 / ARM64) | Visual Studio 2022 (MSVC) | Visual Studio 17 2022 | 3.25+ |
| Linux (x64 / ARM64) | GCC or Clang | Ninja | 3.25+ |
| macOS (x64 / ARM64) | Apple Clang | Ninja | 3.25+ |

SDL3 must be locatable by CMake (`find_package(SDL3 CONFIG REQUIRED)`). Install via your package manager, vcpkg, or build from source and set `CMAKE_PREFIX_PATH`.

## Getting the source

Ballanced directly tracks Player and NeMo2. NeMo2 tracks the engine repositories, which retain their own nested dependencies (VxMath -> stb/simde/libx87, CK2 -> miniz, BuildingBlocks -> ivp/qhull). Every level must be initialized.

### Fresh clone

```bash
git clone --recurse-submodules https://github.com/doyaGu/Ballanced.git
```

### Updating an existing clone

Clones created before `sdl` became the default branch should switch to it before updating:

```bash
git switch sdl
git pull --ff-only
git submodule update --init --recursive
```

If a submodule URL changed (e.g. after a `.gitmodules` edit):

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

### Upgrading from the direct-module layout

Use a fresh recursive clone and a new build directory when switching to the
NeMo2 layout. Existing CMake caches contain the old source paths and cannot be
reused. Preserve local module changes before updating an existing checkout.
The former NeMo2 implementation is retained at its
`legacy/pre-ballanced-split` tag.

The engine's module list comes from the `NEMO_RUNTIME_COMPONENT_MANIFEST` global
property. Ballanced adds Player and checks its static declarations against that
manifest. `BALLANCE_BUILD_STATIC` selects `NEMO_BUILD_STATIC`; individual engine
options such as `CKRE_BUILD_TESTS` continue to work. Ballanced only overrides
Player's version; engine modules use their own versions.

### Checking submodule state

```bash
git submodule status --recursive
```

A line prefixed with `-` means that submodule is not initialized. Run `git submodule update --init --recursive` to fix it.

## Recommended commands

Configure:

```bash
# Windows x86
cmake --preset windows-x86-runtime

# Windows x64
cmake --preset windows-x64-runtime

# Linux x64
cmake --preset linux-x64-runtime

# macOS ARM64
cmake --preset macos-arm64-runtime
```

Build and stage:

```bash
cmake --build --preset windows-x86-runtime-stage-release
```

Run layout checks:

```bash
ctest --preset windows-x86-runtime-stage-release
```

Output path: `build/<configure-preset>/stage/`. For `windows-x86-runtime`:

```
build/windows-x86-runtime/stage/Bin/Player.exe
```

Package the stage into a release archive (`.zip`, `.tar.gz`, `.tar.bz2`, or `.tar.xz`) and verify it:

```bash
cmake -DSTAGE_ROOT=build/windows-x86-runtime/stage -DARCHIVE_PATH=dist/ballance-sdl-windows-x86-runtime.zip -P cmake/PackageStage.cmake
cmake -DARCHIVE_PATH=dist/ballance-sdl-windows-x86-runtime.zip -DEXTRACT_ROOT=build/package-check -DBALLANCE_COMPONENT_MANIFEST=build/windows-x86-runtime/RuntimeComponents.cmake -P cmake/VerifyReleaseArchive.cmake
```

This archive packaging path is shared by local builds and CI. Signed macOS
releases use the disk-image workflow below.

## macOS release signing

`.github/workflows/macos-release.yml` builds x64 and ARM64, each with runtime and
static linkage. It signs `Player` and every bundled Mach-O library with Developer
ID, enables Hardened Runtime for Player, submits a DMG to Apple, and staples the
accepted notarization ticket. The DMG must pass Gatekeeper assessment and a
startup test with download quarantine attributes before it can be published.

Create a **Developer ID Application** certificate using an Apple Developer Program
account with permission to create Developer ID certificates. Install the downloaded
certificate on the Mac that generated its certificate signing request. In Keychain
Access, export the identity from **My Certificates**, including its private key,
as a password-protected `.p12` file. The same identity signs all four packages.
This DMG workflow does not need a Developer ID Installer certificate or a
provisioning profile.

Configure these repository Actions Secrets at
<https://github.com/doyaGu/Ballanced/settings/secrets/actions>:

| Secret | Value |
| --- | --- |
| `MACOS_CERTIFICATE_P12_BASE64` | Base64 of the exported `.p12`, including the private key |
| `MACOS_CERTIFICATE_PASSWORD` | Password selected when exporting that `.p12` |
| `APPLE_ID` | Apple Account email used for notarization |
| `APPLE_TEAM_ID` | Ten-character Developer Program Team ID matching the certificate |
| `APPLE_APP_SPECIFIC_PASSWORD` | App-specific password generated for that Apple Account |

Upload the P12 through standard input so it is not printed or stored in shell
history:

```bash
base64 -i /path/to/DeveloperID.p12 | gh secret set MACOS_CERTIFICATE_P12_BASE64 --repo doyaGu/Ballanced
```

Set the other four values in the repository Secrets UI, or use `gh secret set
SECRET_NAME --repo doyaGu/Ballanced` and enter the value at its prompt. Keep the
P12, private key, and app-specific password out of commits, issue comments, and
chat messages. CI generates its temporary keychain password and removes that
keychain after each job.

To replace the macOS assets of an existing release after configuring the Secrets:

```bash
gh workflow run macos-release.yml --repo doyaGu/Ballanced --ref sdl -f tag=v0.2.0
```

The workflow takes tooling from the selected workflow revision and builds runtime
code from the existing release tag's immutable commit. It publishes only after all
four images have passed. After verifying the uploaded DMGs and their SHA-256
digests, it removes the four old macOS `.tar.gz` assets and updates their filenames
in the release notes. Windows and Linux assets are not part of this operation.
The release tag and its draft/published state are preserved.

This workflow is manual and is not invoked by the existing tag release workflow.
It shares the tag release concurrency group so it waits for any ordinary release
build to finish before replacing macOS assets. Missing Secrets, invalid identities,
rejected notarization, a changed release tag, or failed distribution checks stop
publication.

Certificate and service documentation:
[Developer ID certificates](https://developer.apple.com/help/account/certificates/create-developer-id-certificates/),
[exporting signing identities](https://developer.apple.com/documentation/xcode/sharing-your-teams-signing-certificates),
[notarization workflow](https://developer.apple.com/documentation/security/customizing-the-notarization-workflow),
[app-specific passwords](https://support.apple.com/102654).

## Preset matrix

Defined in `CMakePresets.json`. To list all available presets:

```bash
cmake --list-presets
cmake --build --list-presets
ctest --list-presets
```

### Configure presets

| Preset | Platform | Arch | Mode |
|--------|----------|------|------|
| `windows-x86-runtime` | Windows | x86 | Shared-module runtime |
| `windows-x64-runtime` | Windows | x64 | Shared-module runtime |
| `windows-arm64-runtime` | Windows | ARM64 | Shared-module runtime |
| `linux-x64-runtime` | Linux | x64 | Shared runtime |
| `linux-arm64-runtime` | Linux | ARM64 | Shared runtime |
| `macos-x64-runtime` | macOS | x64 | Shared runtime |
| `macos-arm64-runtime` | macOS | ARM64 | Shared runtime |
| `windows-x86-static` | Windows | x86 | Modules linked statically |
| `windows-x64-static` | Windows | x64 | Modules linked statically |
| `windows-arm64-static` | Windows | ARM64 | Modules linked statically |
| `linux-x64-static` | Linux | x64 | Modules linked statically |
| `linux-arm64-static` | Linux | ARM64 | Modules linked statically |
| `macos-x64-static` | macOS | x64 | Modules linked statically |
| `macos-arm64-static` | macOS | ARM64 | Modules linked statically |
| `windows-x86-tests` | Windows | x86 | Shared runtime + module tests |
| `windows-x64-tests` | Windows | x64 | Shared runtime + module tests |
| `windows-arm64-tests` | Windows | ARM64 | Shared runtime + module tests |
| `linux-x64-tests` | Linux | x64 | Shared runtime + module tests |
| `linux-arm64-tests` | Linux | ARM64 | Shared runtime + module tests |
| `macos-x64-tests` | macOS | x64 | Shared runtime + module tests |
| `macos-arm64-tests` | macOS | ARM64 | Shared runtime + module tests |

### Build presets

Build presets follow `<configure-preset>-release` (plain build) or `<configure-preset>-stage-release` (build + stage install).

| Build preset | Effect |
|---|---|
| `windows-x86-runtime-release` | Build Win32 runtime in Release |
| `windows-x86-runtime-stage-release` | Build and stage Win32 runtime |
| `windows-x86-static-stage-release` | Build and stage Win32 static build |
| `linux-x64-runtime-stage-release` | Build and stage Linux x64 runtime |
| `macos-arm64-runtime-stage-release` | Build and stage macOS ARM64 runtime |

## Runtime layout

The `stage` target installs under `build/<preset>/stage/`. Names below use Windows extensions; Linux and macOS use the platform-native executable and shared-library names (`Player`, `lib*.so`, or `lib*.dylib`).

```
stage/
  Bin/
    Player.exe
    CK2.dll
    VxMath.dll
    SDL3.dll
  RenderEngines/
    CK2_3D.dll
    CKSdlGpuRasterizer.dll
  Managers/
    SdlInputManager.dll
    SdlSoundManager.dll
    ParameterOperations.dll
  Plugins/
    AVIReader.dll
    ImageReader.dll
    WavReader.dll
    VirtoolsLoader.dll
  BuildingBlocks/
    3DTransfo.dll
    Cameras.dll
    Collisions.dll
    ...
```

If `BALLANCE_ASSETS_ROOT` is set, staging also copies assets from an existing installation:

```
Textures/
Sounds/
Text/
3D Entities/
base.cmo
Database.tdb
```

A local `assets/` directory can be used explicitly with
`-DBALLANCE_AUTO_DETECT_ASSETS=ON`. It is disabled by default so ignored local
files cannot silently change a canonical stage.

## Build modes

### Runtime (default)

Shared modules plus the Player executable.

```bash
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
```

### Static build

All Virtools modules linked into the Player executable. No separate module shared libraries.

```bash
cmake --preset windows-x86-static
cmake --build --preset windows-x86-static-stage-release
```

### Tests

Tests for Player utilities, CK2, VxMath, RenderEngine, Plugins, and selected Building Blocks.

```bash
cmake --preset windows-x86-tests
cmake --build --preset windows-x86-tests-release
ctest --preset windows-x86-tests-release
```

CI has **Build** (`build.yml`) and **Test** (`test.yml`) workflows.
Build compiles and packages the platform/linkage matrix with `BUILD_TESTING=OFF`.
Test builds and runs Player tests, engine/application assembly checks, and stage
checks across the same matrix. NeMo2 owns the full engine test matrix. Test publishes JUnit
reports and failure logs; Build publishes runtime packages. Both workflows share
incremental build caches for each platform and linkage. Cached source timestamps
are restored by content hash, so a fresh checkout, Player-only change, or test
script update does not recompile unchanged engine targets. Configure still runs;
stage layouts and test reports are regenerated.

The nightly Build runs when `sdl` has new commits. After the entire Build matrix
succeeds and saves its caches, it calls Test at the same commit. Each workflow
can also be dispatched independently with its own `target` selector:

```bash
gh workflow run build.yml --ref sdl -f target=sdl-linux-x64-runtime
gh workflow run test.yml --ref sdl -f target=sdl-linux-x64-runtime
```

Cache compatibility includes the runner image, architecture, linkage, SDL version,
presets, and compiler/linker policy. Source revisions select snapshots within that
cache family; workflow and test-script edits do not change the family. Existing
pre-sharing caches are accepted as fallback inputs. NeMo2's standalone CI caches
stay in its own repository because CMake build trees contain absolute source paths.

For local incremental builds, keep using the same configured build directory and
run `cmake --build --preset ...`; unchanged NeMo2 targets are reused automatically.
Use a fresh directory for the initial repository migration or a generator/toolchain
change, as described above.

GPU validation captures and replay diagnostics run in the NeMo2 repository.
Dispatch its `test.yml` with `gpu-debug=true`, then pass that run ID to its
**GPU replay diagnostics** workflow. Player JIT acceptance scripts and the level
route remain in Ballanced under `tests/player/`.

### Migration verification (2026-11-12)

The pre-migration macOS log `build/logs/libx87-integrated-tests.log` recorded
13 failing CTest entries: `ConfigurationSectionReferenceTest` and 12 rendering
checks (`SdlGpuPixelTests`, three scene-capture checks, four FFP/sampler/clipping/
readback pixel checks, and four render-manager/context checks). The first
standalone NeMo2 run reproduced the same 13 failures. The configuration test now
uses a private writable directory; all 13 passed when rerun in a macOS GUI
session, and the full local engine suite passed 1,007 CTest entries / 13,111
GoogleTest cases with no skips. The older `build/logs/ci-speed-full-ctest.log`
also recorded `SceneGraphRootTraversalTest`; it passed in that full rerun.

A fresh recursive NeMo2 clone configured runtime, static, and full-test presets
without Player or game assets. Its static build and aggregator checks passed;
standalone CI subsequently built and ran the complete engine suite from fresh
recursive checkouts. Ballanced's Windows, Linux, and macOS build and application
test matrices passed all 14 platform/linkage combinations at `4d63ec1`:
[Build](https://github.com/doyaGu/Ballanced/actions/runs/43161143793) and
[Test](https://github.com/doyaGu/Ballanced/actions/runs/43161147097).
These checks include nested numerical isolation, the component registry,
staging, archive verification, and macOS runtime relocation.

The shared-cache round trip at Ballanced `26a099a` restored the previous Test
snapshot into Build, then restored that Build snapshot into Test. Both stages
compiled zero C/C++ files; all 16 macOS application/assembly checks passed
([Build](https://github.com/doyaGu/Ballanced/actions/runs/43163838259),
[Test](https://github.com/doyaGu/Ballanced/actions/runs/43164148493)).
NeMo2 `1439a4a` passed the same round trip with 998 CTest entries / 13,094
GoogleTest cases ([Test](https://github.com/doyaGu/NeMo2/actions/runs/43164150349)).
Switching its renderer static libraries between release and test-hook settings
recompiled 64 source files; these configurations use different compile definitions.

With local game assets, the migrated Player loaded the menu and Level 01,
accepted keyboard input, paused, and exited normally. This smoke used a private
runtime/save directory. A subsequent session changed the music-volume setting,
exited normally, restarted, and read the saved value back. The isolated run
changed its private `Database.tdb` while preserving the source database's hash.
Audible output was not separately measured.

Player test sessions copy `base.cmo`, `Database.tdb`, and `Player.ini`, and link
the remaining asset directories. Copying the composition is required because
game scripts save relative to its directory through the `Gravity` environment
variable; `--data-path` alone does not redirect those writes. The manual JIT
package includes the session helper alongside `launch.py`. Regression checks
exercise composition-relative writes for both manual and authored sessions on
Linux and Windows, without compiling or launching the engine.

## RenderEngine standalone

Build independently from `Source/NeMo2/Source/RenderEngine`:

```powershell
cd Source/NeMo2/Source/RenderEngine
cmake --preset renderengine-runtime-msvc-win32
cmake --build --preset renderengine-runtime-win32-release
```

| Configure preset | Purpose |
|---|---|
| `renderengine-runtime-msvc-win32` | Shared RenderEngine (Win32) |
| `renderengine-runtime-msvc-x64` | Shared RenderEngine (x64) |
| `renderengine-static-msvc-win32` | Static RenderEngine (Win32) |
| `renderengine-static-msvc-x64` | Static RenderEngine (x64) |
| `renderengine-tests-msvc-win32` | RenderEngine tests (Win32) |
| `renderengine-tests-msvc-x64` | RenderEngine tests (x64) |

Build presets follow `renderengine-<mode>-<arch>-release`.

## Custom configuration

Stage assets from an existing Ballance installation:

```powershell
cmake --preset windows-x86-runtime -DBALLANCE_ASSETS_ROOT=C:/path/to/Ballance
cmake --build --preset windows-x86-runtime-stage-release
```

Or opt into the ignored repository-local `assets/` directory:

```powershell
cmake --preset windows-x86-runtime -DBALLANCE_AUTO_DETECT_ASSETS=ON
```

Use a custom stage directory:

```powershell
cmake --preset windows-x86-runtime -DCMAKE_INSTALL_PREFIX=C:/BallanceStage
cmake --build --preset windows-x86-runtime-stage-release
```

Build a single target:

```powershell
cmake --build --preset windows-x86-runtime-release --target Player
cmake --build --preset windows-x86-runtime-release --target CK2
```

## Manual fallback

**Windows:**

```powershell
cmake -S . -B build/manual-win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build/manual-win32 --config Release --target stage
ctest --test-dir build/manual-win32 -C Release --output-on-failure
```

**Linux / macOS:**

```bash
cmake -S . -B build/manual -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/manual --target stage
ctest --test-dir build/manual --output-on-failure
```

## Troubleshooting

### `stage` fails after CMake file changes

Re-run configure first:

```bash
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
```

### Stage layout verification fails

CTest output shows the missing file or directory:

```bash
ctest --preset windows-x86-runtime-stage-release
```

### Missing runtime DLLs

The default build target does not run the install step. Use the stage preset:

```bash
cmake --build --preset windows-x86-runtime-stage-release
```

### SDL3 not found

```bash
cmake --preset windows-x86-runtime -DCMAKE_PREFIX_PATH=C:/SDL3
```

## Notes

- CMake presets require CMake 3.25+. The `cmake_minimum_required` in project files is lower to support manual builds.
- `MidiManager` is disabled automatically on non-Windows platforms.
- `BALLANCE_DIR` identifies and validates a live Ballance installation as a manual deployment destination; staging does not copy files there automatically.
- Ballanced and its submodules are independently versioned. Root releases pin exact component commits; component version numbers do not need to match the root project version.
