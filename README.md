# Ballanced

This repository is a reverse-engineering project for the game **Ballance**.

It contains a source-level reimplementation of the game runtime components (player, engine libraries, building blocks, plugins, etc.) and builds them into a Ballance-compatible runtime directory layout.

## Status

The project builds a complete runtime layout from source. You still need the original Ballance game assets to run the game.

## Requirements

- Windows
- Visual Studio 2022 (MSVC)
- CMake 3.25+ for presets

Supported preset architectures are `Win32` and `x64`.

## Quick Start

```powershell
cmake --preset ballance-runtime-msvc-win32
cmake --build --preset ballance-runtime-win32-stage-release
ctest --preset ballance-runtime-win32-stage-release
```

The staged runtime is written to:

```text
build/ballance-runtime-msvc-win32/stage/
```

Run:

```text
build/ballance-runtime-msvc-win32/stage/Bin/Player.exe
```

See [BUILD.md](BUILD.md) for the full preset matrix, static Player builds, RenderEngine standalone builds, and manual fallback commands.

## Game Assets (Original Ballance)

**Ballanced** requires game assets from the original Ballance installation to function.

Important notes:

- This repository does **NOT** ship Ballance assets.
- Obtain the original game legally and use files from your own copy.

### Asset staging (optional)

If you point CMake at an existing Ballance install directory, the `stage` target can copy assets into the staged layout:

```powershell
cmake --preset ballance-runtime-msvc-win32 `
  -DBALLANCE_ASSETS_ROOT=C:/path/to/your/Ballance
cmake --build --preset ballance-runtime-win32-stage-release
```

## Releases

Pre-built binaries are published on GitHub releases:

- https://github.com/doyaGu/Ballanced/releases

## Contributing

Contributions are welcome. If you're interested in helping with decompilation, bug fixing, or improving compatibility, please open an issue or submit a pull request.

## Acknowledgments

This project would not be possible without the following:
- The original **Ballance** developers for their incredible game.
- The open-source community for tools and resources used in this project.
- Contributors who have helped with code, testing, and debugging.

## Contact

If you have any questions, feel free to open an issue on GitHub.
