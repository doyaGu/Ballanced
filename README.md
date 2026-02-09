# Ballanced

This repository is a reverse-engineering project for the game **Ballance**.

It contains a source-level reimplementation of the game runtime components (player, engine libraries, building blocks, plugins, etc.) and builds them into a Ballance-compatible runtime directory layout.

## Status

The project builds a complete runtime layout from source. You still need the original Ballance game assets to run the game.

## Requirements

- Windows (this project currently targets Win32 builds)
- Visual Studio 2022 (or another CMake-supported MSVC toolchain)
- CMake 3.16+

## Quick Start (Build)

### Configure

PowerShell example (Visual Studio 2022, Win32):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
```

### Build

```powershell
cmake --build build --config Release
```

Build-tree outputs are placed in:

- `build/Ballance/`

### Stage (Install) a runnable layout

```powershell
cmake --build build --config Release --target stage
```

Staged outputs are placed in:

- `build/stage/`

### Run

After staging, the primary entry point is:

- `build/stage/Bin/Player.exe`

## Tests

The repo includes CTest checks that validate the staged runtime layout:

```powershell
ctest --test-dir build -C Release
```

## Game Assets (Original Ballance)

**Ballanced** requires game assets from the original Ballance installation to function.

Important notes:

- This repository does **NOT** ship Ballance assets.
- This repository does **NOT** ship Virtools runtime DLLs.
- Obtain the original game legally and use files from your own copy.

### Asset staging (optional)

If you point CMake at an existing Ballance install directory, the `stage` target can copy assets into the staged layout:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 `
  -DBALLANCE_ASSETS_ROOT=C:/path/to/your/Ballance
cmake --build build --config Release --target stage
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
