# Ballanced

## Introduction

This is the WIP reversed source code for Ballance. The final purpose of the project is to recreate the game from scratch, based on the decompilation of the original. The finished part has been tested and works on Windows XP and newer versions of Windows. Only compilable code will be found in this repository.

The current progress is shown as follows.

| Filename                                  | Completion Status |
| ----------------------------------------- | :---------------- |
| Bin/Player.exe                            | Completed         |
| Bin/ResDll.dll                            | Completed         |
| BuildingBlocks/physics_RT.dll             | In Progress       |
| BuildingBlocks/TT_DatabaseManager_RT.dll  | Completed         |
| BuildingBlocks/TT_Gravity_RT.dll          | Startup           |
| BuildingBlocks/TT_InterfaceManager_RT.dll | Completed         |
| BuildingBlocks/TT_ParticleSystems_RT.dll  | Startup           |
| BuildingBlocks/TT_Toolbox_RT.dll          | Startup           |

**Note: The repository is at the early stage. There is still a lot of work that needs to be done. Please be aware that the codebase can drastically change at any time.**

## Installation

- Ballanced requires game assets and unfinished parts of the game program.

- Build it yourself or download the latest build.

## Building from Source

This project offers Visual Studio 6.0 project files and a Visual Studio 2019 solution at this stage.

Clone the repository with `git clone https://github.com/doyaGu/Ballanced.git`. Then `cd Ballanced` into the cloned repository.

Open `Ballance.dsw` with Visual Studio 6.0 or `Ballance.sln` with Visual Studio 2019, and compile the projects.

When using Visual Studio 6.0, you need [Windows Server 2003 SP1 SDK](http://www.microsoft.com/en-us/download/details.aspx?id=15656) to compile the **Player** project due to the use of `GlobalMemoryStatusEx`.

## Improvements

There are serval changes and improvements implemented to the original game.

- Fixed incomplete displaying in windowed mode
- Fixed memory checking problem
- In-game resizing in windowed mode support
- ...

## To-Do

The following works will be done in the future.

- Replace registry operation with using initialization file
- Remove dependency on `Dsetup`
- Improve Exception Handling in **Player**
- Add MinGW port
- Add debugging functions
- Add modding support
- Reverse remaining classes and functions
- Reimplement all game-specific Building Blocks
- Migrate to the Virtools Dev 3.5
- ...

## Details

To be documented.

