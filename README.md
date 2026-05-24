# Ballanced

Source-level reimplementation of the **Ballance** game runtime. Rebuilds the player, engine libraries, building blocks, and plugins from source into a Ballance-compatible directory layout.

Original game assets are not included - you need a copy of the original game to run it.

## For players

Download the latest pre-built release:

**https://github.com/doyaGu/Ballanced/releases**

### Installation

1. Get a legal copy of the original Ballance game.
2. Extract the release package into the `Bin` directory of your Ballance installation.
3. Run `Bin\Player.exe`.

No compatibility mode or registry edits needed.

### Flat layout (alternative)

Put `Player.exe`, `base.cmo`, `Database.tdb`, the asset folders, and all runtime DLLs in the same directory. When `base.cmo` is next to `Player.exe`, the player treats that directory as its root.

## For developers

### Requirements

- Windows
- Visual Studio 2022 (MSVC)
- CMake 3.25+

Supported architectures: x86, x64, and ARM64.

### Quick start (x86)

The repository has nested Git submodules. Clone with `--recurse-submodules`; after pulling, run `git submodule update --init --recursive`. See [BUILD.md](BUILD.md) for details.

```powershell
git clone --recurse-submodules https://github.com/doyaGu/Ballanced.git
cd Ballanced
cmake --preset windows-x86-runtime
cmake --build --preset windows-x86-runtime-stage-release
ctest --preset windows-x86-runtime-stage-release
```

Output:

```
build/windows-x86-runtime/stage/Bin/Player.exe
```

See [BUILD.md](BUILD.md) for the full preset matrix, static builds, and troubleshooting.

### Asset staging (optional)

Point CMake at an existing Ballance installation to copy assets into the stage:

```powershell
cmake --preset windows-x86-runtime -DBALLANCE_ASSETS_ROOT=C:/path/to/your/Ballance
cmake --build --preset windows-x86-runtime-stage-release
```

A local `assets/` directory in the repo root is picked up automatically.

## Repository structure

Components under `Source/` are Git submodules:

| Submodule | Repository | Description |
|-----------|-----------|-------------|
| `Source/Player` | [BallancePlayer](https://github.com/doyaGu/BallancePlayer) | Game player and config tool |
| `Source/CK2` | [CK2](https://github.com/doyaGu/CK2) | Virtools behavioral engine |
| `Source/VxMath` | [VxMath](https://github.com/doyaGu/VxMath) | Math and platform utilities |
| `Source/RenderEngine` | [CKRenderEngine](https://github.com/doyaGu/CKRenderEngine) | Render engine |
| `Source/BuildingBlocks` | [CKBuildingBlocks](https://github.com/doyaGu/CKBuildingBlocks) | Virtools building block modules |
| `Source/Plugins` | [CKPlugins](https://github.com/doyaGu/CKPlugins) | File format plugins (image, audio, CMO) |
| `Source/Managers/Dx8InputManager` | [Dx8InputManager](https://github.com/doyaGu/Dx8InputManager) | DirectX 8 input manager |
| `Source/Managers/Dx8SoundManager` | [Dx8SoundManager](https://github.com/doyaGu/Dx8SoundManager) | DirectX 8 sound manager |
| `Source/Managers/ParameterOperations` | [CKParameterOperations](https://github.com/doyaGu/CKParameterOperations) | Parameter operations manager |

## Contributing

Open an issue or pull request. Decompilation, bug fixes, and compatibility work are all useful.

## Acknowledgments

- The original **Ballance** developers for the game.
- The open-source projects this builds on.
- Everyone who has contributed code, tests, or bug reports.
