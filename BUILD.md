# Ballanced Build System

## Overview

Ballanced uses CMake presets as the primary build entry point. The root project builds a Ballance-compatible runtime layout from the component submodules under `Source/`.

Use the handwritten `cmake -S ... -B ...` form only when you need a custom generator, architecture, or cache setting that is not covered by a preset.

## Recommended Commands

Configure the standard shared-DLL runtime:

```powershell
cmake --preset ballance-runtime-msvc-win32
```

Build and stage the runtime layout:

```powershell
cmake --build --preset ballance-runtime-win32-stage-release
```

Run stage/layout checks:

```powershell
ctest --preset ballance-runtime-win32-stage-release
```

The staged runtime is written to:

```text
build/ballance-runtime-msvc-win32/stage/
```

Run the player from:

```text
build/ballance-runtime-msvc-win32/stage/Bin/Player.exe
```

## Presets

Root presets are defined in `CMakePresets.json`.

| Preset | Kind | Purpose |
|--------|------|---------|
| `ballance-runtime-msvc-win32` | configure | Standard Win32 runtime with shared DLL modules |
| `ballance-runtime-msvc-x64` | configure | Standard x64 runtime with shared DLL modules |
| `ballance-static-player-msvc-win32` | configure | Win32 Player with Virtools modules linked statically |
| `ballance-static-player-msvc-x64` | configure | x64 Player with Virtools modules linked statically |
| `ballance-runtime-tests-msvc-win32` | configure | Shared runtime plus module test targets |
| `ballance-runtime-tests-msvc-x64` | configure | x64 shared runtime plus module test targets |
| `ballance-runtime-win32-release` | build | Build the Win32 runtime in Release |
| `ballance-runtime-x64-release` | build | Build the x64 runtime in Release |
| `ballance-runtime-win32-stage-release` | build/test | Build, install, and test the Win32 staged runtime |
| `ballance-runtime-x64-stage-release` | build/test | Build, install, and test the x64 staged runtime |
| `ballance-static-player-win32-release` | build | Build Win32 static Player in Release |
| `ballance-static-player-x64-release` | build | Build x64 static Player in Release |
| `ballance-static-player-win32-stage-release` | build | Build and install Win32 static Player staging outputs |
| `ballance-static-player-x64-stage-release` | build | Build and install x64 static Player staging outputs |
| `ballance-runtime-tests-win32-release` | build/test | Build or test the Win32 test-enabled preset |
| `ballance-runtime-tests-x64-release` | build/test | Build or test the x64 test-enabled preset |

List available configure presets:

```powershell
cmake --list-presets
```

List build presets:

```powershell
cmake --build --list-presets
```

List test presets:

```powershell
ctest --list-presets
```

## Runtime Layout

The `stage` target creates a Ballance-compatible directory layout:

```text
stage/
  Bin/
    Player.exe
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
    Collisions.dll
    ...
```

If `BALLANCE_ASSETS_ROOT` is set, staging can also copy legal game assets from an existing Ballance installation:

```text
Textures/
Sounds/
Text/
3D Entities/
base.cmo
Database.tdb
```

This repository does not ship proprietary Ballance assets.

## Build Matrix

### Root Project

| Mode | Preset | Main effect |
|------|--------|-------------|
| Runtime | `ballance-runtime-msvc-win32`, `ballance-runtime-msvc-x64` | Builds shared module DLLs and a normal `Player.exe` |
| Static Player | `ballance-static-player-msvc-win32`, `ballance-static-player-msvc-x64` | Builds static module libraries and links them into `Player.exe` |
| Runtime Tests | `ballance-runtime-tests-msvc-win32`, `ballance-runtime-tests-msvc-x64` | Enables module tests where supported |

The root project currently targets the RenderEngine `main` branch behavior: DX9 rasterizer runtime targets.

## RenderEngine Standalone

RenderEngine has its own presets in `Source/RenderEngine/CMakePresets.json`.

From `Source/RenderEngine`:

```powershell
cmake --preset renderengine-dx9-runtime-msvc-win32
cmake --build --preset renderengine-dx9-runtime-win32-release
```

Available standalone presets:

| Preset | Kind | Purpose |
|--------|------|---------|
| `renderengine-dx9-runtime-msvc-win32` | configure | Shared DX9 RenderEngine runtime |
| `renderengine-dx9-runtime-msvc-x64` | configure | x64 shared DX9 RenderEngine runtime |
| `renderengine-dx9-static-msvc-win32` | configure | Static DX9 RenderEngine libraries |
| `renderengine-dx9-static-msvc-x64` | configure | x64 static DX9 RenderEngine libraries |
| `renderengine-dx9-tests-msvc-win32` | configure | DX9 RenderEngine tests |
| `renderengine-dx9-tests-msvc-x64` | configure | x64 DX9 RenderEngine tests |
| `renderengine-dx9-runtime-win32-release` | build | Build Win32 shared DX9 runtime in Release |
| `renderengine-dx9-runtime-x64-release` | build | Build x64 shared DX9 runtime in Release |
| `renderengine-dx9-static-win32-release` | build | Build Win32 static DX9 libraries in Release |
| `renderengine-dx9-static-x64-release` | build | Build x64 static DX9 libraries in Release |
| `renderengine-dx9-tests-win32-release` | build/test | Build or test Win32 DX9 test configuration |
| `renderengine-dx9-tests-x64-release` | build/test | Build or test x64 DX9 test configuration |

## Custom Configuration

Presets can still accept extra cache variables.

Stage assets from a legal game installation:

```powershell
cmake --preset ballance-runtime-msvc-win32 `
  -DBALLANCE_ASSETS_ROOT=C:/path/to/Ballance
cmake --build --preset ballance-runtime-win32-stage-release
```

Use a custom stage directory:

```powershell
cmake --preset ballance-runtime-msvc-win32 `
  -DCMAKE_INSTALL_PREFIX=C:/BallanceStage
cmake --build --preset ballance-runtime-win32-stage-release
```

Build one target from an existing preset build tree:

```powershell
cmake --build --preset ballance-runtime-win32-release --target Player
cmake --build --preset ballance-runtime-win32-release --target CK2
cmake --build --preset ballance-runtime-win32-release --target Cameras
```

## Manual Fallback

Use manual CMake commands when presets do not cover a local experiment:

```powershell
cmake -S . -B build/manual-msvc-win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build/manual-msvc-win32 --config Release --target stage
ctest --test-dir build/manual-msvc-win32 -C Release --output-on-failure
```

## Troubleshooting

### `stage` Fails After CMake File Changes

Re-run configure before building `stage`:

```powershell
cmake --preset ballance-runtime-msvc-win32
cmake --build --preset ballance-runtime-win32-stage-release
```

### Stage Layout Verification Fails

Run CTest with failure output:

```powershell
ctest --preset ballance-runtime-win32-stage-release
```

The failing test reports the missing runtime file or directory.

### Missing Runtime DLLs

Build the stage target, not only the default build:

```powershell
cmake --build --preset ballance-runtime-win32-stage-release
```

## Notes

- Platform: Windows only.
- Primary compiler: Visual Studio 2022 MSVC.
- Supported preset architectures: Win32 and x64.
- CMake presets require CMake 3.25 or newer. The project CMake files still keep a lower `cmake_minimum_required` for manual builds.
