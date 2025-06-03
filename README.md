# Ballanced

This repository is a work-in-progress reverse-engineering project for the game **Ballance**. The ultimate goal is to fully recreate the game from scratch based on a decompilation of the original code. We aim to preserve the spirit and mechanics of the original game while making it compatible with modern systems.

## Current Progress

The status of the project's main components is as follows:

| Filename                                   | Completion Status |
| -----------------------------------------  | :---------------- |
| `Bin/Player.exe`                           | Completed         |
| `Bin/ResDll.dll`                           | Completed         |
| `BuildingBlocks/3DTransfo.dll`             | Completed         |
| `BuildingBlocks/BuildingBlocksAddons1.dll` | Completed         |
| `BuildingBlocks/Cameras.dll`               | Completed         |
| `BuildingBlocks/Collision.dll`             | Completed         |
| `BuildingBlocks/Controllers.dll`           | Completed         |
| `BuildingBlocks/Interface.dll`             | Completed         |
| `BuildingBlocks/Lights.dll`                | Completed         |
| `BuildingBlocks/Logics.dll`                | Completed         |
| `BuildingBlocks/Materials.dll`             | Completed         |
| `BuildingBlocks/Narratives.dll`            | Completed         |
| `BuildingBlocks/physics_RT.dll`            | Completed         |
| `BuildingBlocks/Sounds.dll`                | Completed         |
| `BuildingBlocks/TT_DatabaseManager_RT.dll` | Completed         |
| `BuildingBlocks/TT_Gravity_RT.dll`         | Completed         |
| `BuildingBlocks/TT_InterfaceManager_RT.dll`| Completed         |
| `BuildingBlocks/TT_ParticleSystems_RT.dll` | Completed         |
| `BuildingBlocks/TT_Toolbox_RT.dll`         | Completed*        |
| `BuildingBlocks/Visuals.dll`               | Completed         |
| `BuildingBlocks/WorldEnvironments.dll`     | Completed         |

_\* Some unused building blocks are not implemented._

## Installation

To run **Ballanced**, you will need the original game assets and some components that are still under development. Here’s how to get started:

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

Once you have these, place them in the appropriate directories in your build folder.

## Roadmap

Our vision for **Ballanced** is to fully recreate the original **Ballance** game while extending its capabilities and ensuring long-term compatibility with modern systems. Below are the key milestones we aim to achieve:

1. **Complete Decompilation**  
   - Continue reverse-engineering and implementing missing components, particularly focusing on the remaining **Virtools DLLs**.
   - Refactor existing implementations for improved performance and stability.

2. **Cross-Platform Support**  
   - Expand support beyond Windows, with future versions planned for **Linux** and **macOS**.
   - Ensure compatibility with Wine for better support on Linux systems.

3. **Enhanced Modding Support**  
   - Extend the modding capabilities beyond the original game, providing tools for easier mod creation and integration.
   - Create documentation and modding guides for the community.

4. **Performance Optimizations**  
   - Analyze the current codebase for bottlenecks and optimize it to improve performance on both legacy and modern systems.
   - Implement multi-threading and other modern techniques to enhance gameplay smoothness on high-end hardware.

5. **Improved User Experience and Interface**  
   - Modernize the user interface where appropriate, maintaining the original aesthetic but enhancing usability on higher resolutions and widescreen displays.
   - Improve input methods, including better support for modern controllers and remappable keys.

## Acknowledgments

This project would not be possible without the following:
- The original **Ballance** developers for their incredible game.
- The open-source community for tools and resources used in this project.
- Contributors who have helped with code, testing, and debugging.

## Contributing

Contributions are welcome! If you're interested in helping with decompiling, debugging, or adding new features, feel free to fork the repository and submit a pull request.

## Contact

If you have any questions, feel free to open an issue on GitHub.
