# Ballanced

This repository is a reverse-engineering project for the game **Ballance**.

It contains a source-level reimplementation of the game runtime components (player, engine libraries, building blocks, plugins, etc.) and builds them into a Ballance-compatible runtime directory layout.

## Status

The project builds a complete runtime layout from source. You still need the original Ballance game assets to run the game.

## Requirements

- Windows
- Visual Studio 2022 (MSVC) or MinGW-w64
- CMake 3.16+

Supported Windows architectures:

- `Win32` (x86)
- `x64`
- `ARM64`

## Quick Start (Build)

### Configure

PowerShell examples (Visual Studio 2022):

```powershell
# Win32 (x86)
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32

# x64
cmake -S . -B build-x64 -G "Visual Studio 17 2022" -A x64

# ARM64
cmake -S . -B build-arm64 -G "Visual Studio 17 2022" -A ARM64
```

PowerShell example (MinGW-w64):

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles"
```

### Build

```powershell
# Multi-config generators (Visual Studio)
cmake --build build --config Release

# Single-config generators (MinGW Makefiles)
cmake --build build-mingw
```

Build-tree outputs are placed in:

- `build/Ballance/`

### Stage (Install) a runnable layout

```powershell
# Multi-config generators (Visual Studio)
cmake --build build --config Release --target stage

# Single-config generators (MinGW Makefiles)
cmake --build build-mingw --target stage
```

Staged outputs are placed in:

- `build/stage/`

### Run

After staging, the primary entry point is:

- `build/stage/Bin/Player.exe`

## Tests

The repo includes CTest checks that validate the staged runtime layout:

```powershell
# Multi-config generators (Visual Studio)
ctest --test-dir build -C Release

# Single-config generators (MinGW Makefiles)
ctest --test-dir build-mingw
```

## Game Assets (Original Ballance)

**Ballanced** requires game assets from the original Ballance installation to function.

Important notes:

- This repository does **NOT** ship Ballance assets.
- Obtain the original game legally and use files from your own copy.

### Asset staging (optional)

If you point CMake at an existing Ballance install directory, the `stage` target can copy assets into the staged layout:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DBALLANCE_ASSETS_ROOT=C:/path/to/your/Ballance
cmake --build build --config Release --target stage

cmake -S . -B build-mingw -G "MinGW Makefiles" `
  -DBALLANCE_ASSETS_ROOT=C:/path/to/your/Ballance
cmake --build build-mingw --target stage
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
