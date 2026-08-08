# Ballanced

Source-level reimplementation of the **Ballance** game runtime. Rebuilds the player, engine libraries, building blocks, and plugins from source into a Ballance-compatible directory layout.

Original game assets are not included - you need a copy of the original game to run it.

This document describes the cross-platform SDL3 runtime on the default `sdl` branch.

## For players

Download the latest pre-built release:

**https://github.com/doyaGu/Ballanced/releases**

### Installation

1. Get a legal copy of the original Ballance game.
2. Extract the release package into the **game root**, alongside the original asset directories. The archive already contains `Bin/`, `RenderEngines/`, `Managers/`, `Plugins/`, and `BuildingBlocks/`.
3. Run `Bin\Player.exe` on Windows or `Bin/Player` on Linux and macOS.

Windows does not require compatibility mode or registry edits.

### Flat layout (alternative)

Put `Player` (`Player.exe` on Windows), `base.cmo`, `Database.tdb`, the asset folders, and all runtime modules in the same directory. When `base.cmo` is next to the executable, the player treats that directory as its root.

## For developers

### Requirements

| Platform | Toolchain | CMake |
|----------|-----------|-------|
| Windows (x86 / x64 / ARM64) | Visual Studio 2022 (MSVC) | 3.25+ |
| Linux (x64 / ARM64) | GCC or Clang + Ninja | 3.25+ |
| macOS (x64 / ARM64) | Apple Clang + Ninja | 3.25+ |

SDL3 must be on the CMake package path on all platforms (`find_package(SDL3 CONFIG REQUIRED)`).
The macOS stage layout supports SDL3 shared-library packages with a relocatable
`@rpath` install name; SDL3 framework packages are not part of the supported layout.

### Quick start (Windows x86)

The repository has nested Git submodules. Clone with `--recurse-submodules`; after pulling, run `git submodule update --init --recursive`. See [BUILD.md](BUILD.md) for details.

```powershell
git clone --recurse-submodules https://github.com/doyaGu/Ballanced.git
cd Ballanced
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
ctest --preset windows-x86-runtime-stage-release
```

Output goes to:

```
build/windows-x86-runtime/stage/Bin/Player.exe
```

See [BUILD.md](BUILD.md) for the full preset matrix, static builds, RenderEngine standalone builds, and troubleshooting.

### Asset staging (optional)

Point CMake at an existing Ballance installation to copy assets into the stage:

```powershell
cmake --preset windows-x86-runtime -DBALLANCE_ASSETS_ROOT=C:/path/to/your/Ballance
cmake --build --preset windows-x86-runtime-stage-release
```

For an explicit local-development shortcut, place the assets under `assets/`
and configure with `-DBALLANCE_AUTO_DETECT_ASSETS=ON`. Canonical presets do not
read this ignored directory by default.

### Source of truth and versioning

- `CMakePresets.json` defines the supported full-runtime platform, architecture, and linkage matrix.
- `.github/workflows/build.yml` defines the configurations continuously built by CI.
- CI runs the complete component test preset on Linux x64 in addition to the platform/linkage build matrix.
- `BUILD.md` documents the supported superproject workflow. Component READMEs describe standalone workflows only.
- Ballanced and its Git submodules are independently versioned. The root project version identifies the assembled runtime; Player and the engine/module projects keep their own versions. A component version should not be inferred from the root version or release tag.
- Every staged runtime contains `Bin/BallancedBuildManifest.json`, recording the exact component revisions used for that build.

## Repository structure

Components under `Source/` are Git submodules:

| Submodule | Repository | Description |
|-----------|-----------|-------------|
| `Source/Player` | [BallancePlayer](https://github.com/doyaGu/BallancePlayer) | Game player executable |
| `Source/CK2` | [CK2](https://github.com/doyaGu/CK2) | Virtools behavioral engine |
| `Source/VxMath` | [VxMath](https://github.com/doyaGu/VxMath) | Math and platform utilities |
| `Source/RenderEngine` | [CKRenderEngine](https://github.com/doyaGu/CKRenderEngine) | Render engine (bgfx) |
| `Source/BuildingBlocks` | [CKBuildingBlocks](https://github.com/doyaGu/CKBuildingBlocks) | Virtools building block modules |
| `Source/Plugins` | [CKPlugins](https://github.com/doyaGu/CKPlugins) | File format plugins (image, audio, CMO) |
| `Source/Managers/ParameterOperations` | [CKParameterOperations](https://github.com/doyaGu/CKParameterOperations) | Parameter operations manager |
| `Source/Managers/SdlInputManager` | [SdlInputManager](https://github.com/doyaGu/SdlInputManager) | SDL3 input manager |
| `Source/Managers/SdlSoundManager` | [SdlSoundManager](https://github.com/doyaGu/SdlSoundManager) | SDL3 sound manager |

## Contributing

Open an issue or pull request. Decompilation, bug fixes, and compatibility work are all useful.

## Acknowledgments

- The original **Ballance** developers for the game.
- The open-source projects this builds on.
- Everyone who has contributed code, tests, or bug reports.
