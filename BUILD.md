# Ballanced build system

CMake presets are the primary entry point. The root project assembles a Ballance-compatible runtime from the submodules under `Source/`.

Use manual `cmake -S ... -B ...` commands only when a preset does not cover your generator or architecture.

## Prerequisites

- Windows
- Visual Studio 2022 (MSVC)
- CMake 3.25+

Supported architectures: Win32 and x64.

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
cmake --preset ballance-runtime-msvc-win32
```

Build and stage:

```powershell
cmake --build --preset ballance-runtime-win32-stage-release
```

Run layout checks:

```powershell
ctest --preset ballance-runtime-win32-stage-release
```

Output path: `build/<configure-preset>/stage/`. For `ballance-runtime-msvc-win32`:

```
build/ballance-runtime-msvc-win32/stage/Bin/Player.exe
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
| `ballance-runtime-msvc-win32` | Win32 | Shared DLL runtime |
| `ballance-runtime-msvc-x64` | x64 | Shared DLL runtime |
| `ballance-static-player-msvc-win32` | Win32 | Modules linked statically |
| `ballance-static-player-msvc-x64` | x64 | Modules linked statically |
| `ballance-runtime-tests-msvc-win32` | Win32 | Shared runtime + module tests |
| `ballance-runtime-tests-msvc-x64` | x64 | Shared runtime + module tests |

### Build presets

| Build preset | Effect |
|---|---|
| `ballance-runtime-win32-release` | Build Win32 runtime in Release |
| `ballance-runtime-x64-release` | Build x64 runtime in Release |
| `ballance-runtime-win32-stage-release` | Build and stage Win32 runtime |
| `ballance-runtime-x64-stage-release` | Build and stage x64 runtime |
| `ballance-static-player-win32-release` | Build Win32 static player in Release |
| `ballance-static-player-x64-release` | Build x64 static player in Release |
| `ballance-static-player-win32-stage-release` | Build and stage Win32 static player |
| `ballance-static-player-x64-stage-release` | Build and stage x64 static player |
| `ballance-runtime-tests-win32-release` | Build Win32 module tests in Release |
| `ballance-runtime-tests-x64-release` | Build x64 module tests in Release |

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
cmake --preset ballance-runtime-msvc-win32
cmake --build --preset ballance-runtime-win32-stage-release
```

### Static player

All Virtools modules linked into `Player.exe`. No separate module DLLs.

```powershell
cmake --preset ballance-static-player-msvc-win32
cmake --build --preset ballance-static-player-win32-stage-release
```

### Tests

Module unit tests for CK2, VxMath, RenderEngine, and Plugins.

```powershell
cmake --preset ballance-runtime-tests-msvc-win32
cmake --build --preset ballance-runtime-tests-win32-release
ctest --preset ballance-runtime-tests-win32-release
```

## Custom configuration

Stage assets from an existing Ballance installation:

```powershell
cmake --preset ballance-runtime-msvc-win32 -DBALLANCE_ASSETS_ROOT=C:/path/to/Ballance
cmake --build --preset ballance-runtime-win32-stage-release
```

Use a custom stage directory:

```powershell
cmake --preset ballance-runtime-msvc-win32 -DCMAKE_INSTALL_PREFIX=C:/BallanceStage
cmake --build --preset ballance-runtime-win32-stage-release
```

Build a single target:

```powershell
cmake --build --preset ballance-runtime-win32-release --target Player
cmake --build --preset ballance-runtime-win32-release --target CK2
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
cmake --preset ballance-runtime-msvc-win32
cmake --build --preset ballance-runtime-win32-stage-release
```

### Stage layout verification fails

CTest output shows the missing file or directory:

```powershell
ctest --preset ballance-runtime-win32-stage-release
```

### Missing runtime DLLs

The default build target does not run the install step. Use the stage preset:

```powershell
cmake --build --preset ballance-runtime-win32-stage-release
```

## Notes

- CMake presets require CMake 3.25+. The `cmake_minimum_required` in project files is lower to support manual builds.
- Set `BALLANCE_DIR` to a live Ballance installation for direct deployment.
