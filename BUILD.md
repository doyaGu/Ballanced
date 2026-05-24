# Ballanced build system

CMake presets are the primary entry point. The root project assembles a Ballance-compatible runtime from the submodules under `Source/`.

Use manual `cmake -S ... -B ...` commands only when a preset does not cover your generator or architecture.

## Prerequisites

- Windows
- Visual Studio 2022 (MSVC)
- CMake 3.25+

Supported architectures: x86, x64, and ARM64.

## Getting the source

The repository has two levels of Git submodules. Top-level submodules are the components under `Source/`. Several have their own nested submodules (RenderEngine -> bgfx, VxMath -> stb/simde, CK2 -> miniz, BuildingBlocks -> ivp/qhull).

### Fresh clone

```powershell
git clone --recurse-submodules https://github.com/doyaGu/Ballanced.git
```

### Updating an existing clone

```powershell
git pull
git submodule update --init --recursive
```

If a submodule URL changed (e.g. after a `.gitmodules` edit):

```powershell
git submodule sync --recursive
git submodule update --init --recursive
```

### Checking submodule state

```powershell
git submodule status --recursive
```

A line prefixed with `-` means that submodule is not initialized. Run `git submodule update --init --recursive` to fix it.

## Recommended commands

Configure:

```powershell
cmake --preset windows-x86-runtime
```

Build and stage:

```powershell
cmake --build --preset windows-x86-runtime-stage-release
```

Run layout checks:

```powershell
ctest --preset windows-x86-runtime-stage-release
```

Output path: `build/<configure-preset>/stage/`. For `windows-x86-runtime`:

```
build/windows-x86-runtime/stage/Bin/Player.exe
```

## Preset matrix

Defined in `CMakePresets.json`. To list all available presets:

```powershell
cmake --list-presets
cmake --build --list-presets
ctest --list-presets
```

### Configure presets

| Preset | Arch | Mode |
|--------|------|------|
| `windows-x86-runtime` | x86 | Shared DLL runtime |
| `windows-x64-runtime` | x64 | Shared DLL runtime |
| `windows-arm64-runtime` | ARM64 | Shared DLL runtime |
| `windows-x86-static` | x86 | Modules linked statically |
| `windows-x64-static` | x64 | Modules linked statically |
| `windows-arm64-static` | ARM64 | Modules linked statically |
| `windows-x86-tests` | x86 | Shared runtime + module tests |
| `windows-x64-tests` | x64 | Shared runtime + module tests |
| `windows-arm64-tests` | ARM64 | Shared runtime + module tests |

### Build presets

| Build preset | Effect |
|---|---|
| `windows-x86-runtime-release` | Build x86 runtime in Release |
| `windows-x64-runtime-release` | Build x64 runtime in Release |
| `windows-arm64-runtime-release` | Build ARM64 runtime in Release |
| `windows-x86-runtime-stage-release` | Build and stage x86 runtime |
| `windows-x64-runtime-stage-release` | Build and stage x64 runtime |
| `windows-arm64-runtime-stage-release` | Build and stage ARM64 runtime |
| `windows-x86-static-release` | Build x86 static runtime in Release |
| `windows-x64-static-release` | Build x64 static runtime in Release |
| `windows-arm64-static-release` | Build ARM64 static runtime in Release |
| `windows-x86-static-stage-release` | Build and stage x86 static runtime |
| `windows-x64-static-stage-release` | Build and stage x64 static runtime |
| `windows-arm64-static-stage-release` | Build and stage ARM64 static runtime |
| `windows-x86-tests-release` | Build x86 module tests in Release |
| `windows-x64-tests-release` | Build x64 module tests in Release |
| `windows-arm64-tests-release` | Build ARM64 module tests in Release |
| `windows-x86-tests-stage-release` | Build and stage x86 module tests |
| `windows-x64-tests-stage-release` | Build and stage x64 module tests |
| `windows-arm64-tests-stage-release` | Build and stage ARM64 module tests |

## Runtime layout

The `stage` target installs under `build/<preset>/stage/`:

```
stage/
  Bin/
    Player.exe
    ConfigTool.exe
    CK2.dll
    VxMath.dll
  RenderEngines/
    CK2_3D.dll
    CKDX9Rasterizer.dll
  Managers/
    Dx8InputManager.dll
    Dx8SoundManager.dll
    ParameterOperations.dll
  Plugins/
    AVIReader.dll
    ImageReader.dll
    WavReader.dll
    VirtoolsLoader.dll
  BuildingBlocks/
    3DTransfo.dll
    Cameras.dll
    Collision.dll
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

```powershell
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
```

### Static

All Virtools modules linked into `Player.exe`. No separate module DLLs.
The static presets set `BALLANCE_BUILD_STATIC=ON`.

```powershell
cmake --preset windows-x86-static
cmake --build --preset windows-x86-static-stage-release
```

### Tests

Module unit tests for CK2, VxMath, RenderEngine, and Plugins.

```powershell
cmake --preset windows-x86-tests
cmake --build --preset windows-x86-tests-release
ctest --preset windows-x86-tests-release
```

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

```powershell
cmake -S . -B build/manual-win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build/manual-win32 --config Release --target stage
ctest --test-dir build/manual-win32 -C Release --output-on-failure
```

## Troubleshooting

### `stage` fails after CMake file changes

Re-run configure first:

```powershell
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
```

### Stage layout verification fails

CTest output shows the missing file or directory:

```powershell
ctest --preset windows-x86-runtime-stage-release
```

### Missing runtime DLLs

The default build target does not run the install step. Use the stage preset:

```powershell
cmake --build --preset windows-x86-runtime-stage-release
```

## Notes

- CMake presets require CMake 3.25+. The `cmake_minimum_required` in project files is lower to support manual builds.
- Set `BALLANCE_DIR` to a live Ballance installation for direct deployment.
