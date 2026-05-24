# Ballanced build system

CMake presets are the primary entry point. The root project assembles a Ballance-compatible runtime from the submodules under `Source/`.

Use manual `cmake -S ... -B ...` commands only when a preset does not cover your generator or architecture.

## Prerequisites

| Platform | Toolchain | Generator | CMake |
|----------|-----------|-----------|-------|
| Windows (x86 / x64 / ARM64) | Visual Studio 2022 (MSVC) | Visual Studio 17 2022 | 3.25+ |
| Linux (x64 / ARM64) | GCC or Clang | Ninja | 3.25+ |
| macOS (x64 / ARM64) | Apple Clang | Ninja | 3.25+ |

SDL3 must be locatable by CMake (`find_package(SDL3 CONFIG REQUIRED)`). Install via your package manager, vcpkg, or build from source and set `CMAKE_PREFIX_PATH`.

## Getting the source

The repository has two levels of Git submodules. Top-level submodules are the components under `Source/`. Several have their own nested submodules (RenderEngine -> bgfx, VxMath -> stb/simde, CK2 -> miniz, BuildingBlocks -> ivp/qhull).

### Fresh clone

```bash
git clone --recurse-submodules https://github.com/doyaGu/Ballanced.git
```

### Updating an existing clone

```bash
git pull
git submodule update --init --recursive
```

If a submodule URL changed (e.g. after a `.gitmodules` edit):

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

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
| `windows-x86-runtime` | Windows | x86 | Shared DLL runtime |
| `windows-x64-runtime` | Windows | x64 | Shared DLL runtime |
| `windows-arm64-runtime` | Windows | ARM64 | Shared DLL runtime |
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

The `stage` target installs under `build/<preset>/stage/`:

```
stage/
  Bin/
    Player.exe          (Player on Linux/macOS)
    CK2.dll
    VxMath.dll
    SDL3.dll
  RenderEngines/
    CK2_3D.dll
    CKBgfxRasterizer.dll
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

A local `assets/` directory in the repo root is picked up automatically (`BALLANCE_AUTO_DETECT_ASSETS=ON` by default).

## Build modes

### Runtime (default)

Shared DLL modules plus a normal `Player.exe`.

```bash
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
```

### Static build

All Virtools modules linked into `Player.exe`. No separate module DLLs.

```bash
cmake --preset windows-x86-static
cmake --build --preset windows-x86-static-stage-release
```

### Tests

Module unit tests for CK2, VxMath, RenderEngine, and Plugins.

```bash
cmake --preset windows-x86-tests
cmake --build --preset windows-x86-tests-release
ctest --preset windows-x86-tests-release
```

## RenderEngine standalone

Build independently from `Source/RenderEngine`:

```powershell
cd Source/RenderEngine
cmake --preset renderengine-bgfx-runtime-msvc-win32
cmake --build --preset renderengine-bgfx-runtime-win32-release
```

| Configure preset | Purpose |
|---|---|
| `renderengine-bgfx-runtime-msvc-win32` | Shared bgfx RenderEngine (Win32) |
| `renderengine-bgfx-runtime-msvc-x64` | Shared bgfx RenderEngine (x64) |
| `renderengine-bgfx-static-msvc-win32` | Static bgfx RenderEngine (Win32) |
| `renderengine-bgfx-static-msvc-x64` | Static bgfx RenderEngine (x64) |
| `renderengine-bgfx-tests-msvc-win32` | bgfx tests (Win32) |
| `renderengine-bgfx-tests-msvc-x64` | bgfx tests (x64) |

Build presets follow `renderengine-bgfx-<mode>-<arch>-release`.

## Custom configuration

Stage assets from an existing Ballance installation:

```powershell
cmake --preset windows-x86-runtime -DBALLANCE_ASSETS_ROOT=C:/path/to/Ballance
cmake --build --preset windows-x86-runtime-stage-release
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
- Set `BALLANCE_DIR` to a live Ballance installation for direct deployment.
