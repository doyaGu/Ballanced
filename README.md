# Ballanced

## Introduction

This is the WIP reversed source code for Ballance. The final purpose of the project is to recreate the game from scratch, based on the decompilation of the original.

The recompiled player and building blocks have been tested on Windows XP and newer versions of Windows.

The current progress is shown as follows.

| Filename                                  | Completion Status |
| ----------------------------------------- | :---------------- |
| Bin/Player.exe                            | Completed         |
| Bin/ResDll.dll                            | Completed         |
| BuildingBlocks/physics_RT.dll             | In Progress       |
| BuildingBlocks/TT_DatabaseManager_RT.dll  | Completed         |
| BuildingBlocks/TT_Gravity_RT.dll          | In Progress       |
| BuildingBlocks/TT_InterfaceManager_RT.dll | Completed         |
| BuildingBlocks/TT_ParticleSystems_RT.dll  | In Progress       |
| BuildingBlocks/TT_Toolbox_RT.dll          | In Progress       |

**Note: The repository is at the early stage. There is still a lot of work that needs to be done. Please be aware that the codebase can drastically change at any time.**

## Installation

- Ballanced requires game assets and unfinished parts of the game program.

- Build it yourself or download the latest build.

## Building from Source

This project uses CMake as the build tool, and Visual Studio 6.0 is also supported separately.

Clone the repository with `git clone https://github.com/doyaGu/Ballanced.git`. Then `cd Ballanced` into the cloned repository.

If you use CMake, run the command `cmake -B build -G "Visual Studio 16 2019" -A Win32` to generate Visual Studio projects and solutions. Then open the solution file under the `build` directory and compile the projects.

If you use Visual Studio 6.0, open `Ballance.dsw`, and compile the projects.

When using Visual Studio 6.0, you need [Windows Server 2003 SP1 SDK](http://www.microsoft.com/en-us/download/details.aspx?id=15656) to compile the **Player** project due to the use of `GlobalMemoryStatusEx`.

## Improvements

There are serval changes and improvements implemented to the original game.

- Fixed incomplete displaying in windowed mode
- Fixed memory checking problem
- Support in-game resizing in windowed mode 
- Replace registry operation with using initialization file
- No dependency on `Dsetup`
- Disabled Exception Handling in **Player**
- Support command-line configuration
- Support 32-bit color mode
- ResDll is integrated into Player
- ...

## To-Do

The following works will be done in the future.

- Reverse remaining classes and functions
- Reimplement all game-specific Building Blocks
- Migrate to the Virtools Dev 3.5
- ...

## Details

To be documented.

