# Ballanced

## Introduction

This is the WIP reversed source code for Ballance. The final purpose of the project is to recreate the game from scratch, based on the decompilation of the original.

The recompiled player and building blocks have been tested on Windows XP and newer versions of Windows.

The current progress is shown as follows.

| Filename                                  | Completion Status |
| ----------------------------------------- | :---------------- |
| Bin/Player.exe                            | Completed         |
| Bin/ResDll.dll                            | Completed         |
| BuildingBlocks/physics_RT.dll             | Completed         |
| BuildingBlocks/TT_DatabaseManager_RT.dll  | Completed         |
| BuildingBlocks/TT_Gravity_RT.dll          | Almost Done       |
| BuildingBlocks/TT_InterfaceManager_RT.dll | Completed         |
| BuildingBlocks/TT_ParticleSystems_RT.dll  | Almost Done       |
| BuildingBlocks/TT_Toolbox_RT.dll          | Functional        |

## Installation

- Ballanced requires game assets and unfinished parts of the game program.

- Build it yourself or download the latest build.

## Building from Source

This project uses CMake as the build tool, and Visual Studio 6.0 is also supported separately.

Clone the repository with `git clone https://github.com/doyaGu/Ballanced.git`. Then `cd Ballanced` into the cloned repository.

If you use CMake, run the command `cmake -B build -G "Visual Studio 16 2022" -A Win32` to generate Visual Studio projects and solutions. Then open the solution file under the `build` directory and compile the projects.

If you use Visual Studio 6.0, open `Ballance.dsw`, and compile the projects.

## To-Do

The following works will be done in the future.

- Reverse remaining classes and functions
- Reimplement all game-specific Building Blocks
- Migrate to the Virtools Dev 3.5
- ...

## Details

To be documented.

