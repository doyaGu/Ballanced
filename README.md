# Ballanced

This repository is a work-in-progress reverse-engineering project for the game **Ballance**. The ultimate goal is to fully recreate the game from scratch based on a decompilation of the original code. We aim to preserve the spirit and mechanics of the original game while making it compatible with modern systems.

## Current Progress

The status of the project's main components is as follows:

| Filename                                   | Completion Status |
| -----------------------------------------  | :---------------- |
| `Bin/Player.exe`                           | Completed         |
| `Bin/ResDll.dll`                           | Completed         |
| `BuildingBlocks/physics_RT.dll`            | Completed         |
| `BuildingBlocks/TT_DatabaseManager_RT.dll` | Completed         |
| `BuildingBlocks/TT_Gravity_RT.dll`         | Completed         |
| `BuildingBlocks/TT_InterfaceManager_RT.dll`| Completed         |
| `BuildingBlocks/TT_ParticleSystems_RT.dll` | Completed         |
| `BuildingBlocks/TT_Toolbox_RT.dll`         | Completed*        |

_\* Some unused building blocks are not implemented._

## Installation

To run **Ballanced**, you will need the original game assets and some components that are still under development. Here's how to get started:

### Option 1: Download Pre-Built Binaries
- Download the latest build from the [releases](https://github.com/doyaGu/Ballanced/releases) section.
- Follow the instructions below for installing game assets.

### Option 2: Build It Yourself

If you prefer building from source, follow these steps:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/doyaGu/Ballanced.git
   cd Ballanced
   ```

2. **Build with CMake**:
   Ensure you have CMake installed and then run the following command:
   ```bash
   cmake -B build -G "Visual Studio 16 2022" -A Win32
   ```
   This will generate Visual Studio project files.

3. **Compile the Solution**:
   Open the generated solution file (`.sln`) in Visual Studio under the `build` directory and build the project.

## Game Assets

**Ballanced** requires assets and **Virtools** DLLs from the original Ballance game to function.

**Important:** This repository does **NOT** include Virtools itself or its DLLs. You must obtain the original Ballance game and extract the required Virtools DLLs from your own copy.

Once you have these, place them in the appropriate directories in your build folder.

## Acknowledgments

This project would not be possible without the following:
- The original **Ballance** developers for their incredible game.
- The open-source community for tools and resources used in this project.
- Contributors who have helped with code, testing, and debugging.

## Contributing

Contributions are welcome! If you're interested in helping with decompiling, debugging, or adding new features, feel free to fork the repository and submit a pull request.

## Contact

If you have any questions, feel free to open an issue on GitHub.
