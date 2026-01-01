# BreakOut (Win32 / DirectX9)


A small Win32 / DirectX9 game engine project that implements the classic BreakOut game. This repository contains a minimal, hand-rolled engine demonstrating Win32 window handling, DirectX9 rendering, simple physics, input management and sound.

Prerequisites: see the [Prerequisites](#prerequisites) section below.

## Purpose

- Showcase: a compact game engine suitable for a GitHub portfolio.
- Learning: demonstrates practical Win32 + DirectX9 usage and classic game architecture.
- Components: rendering, physics, input, sound and basic game states.

## Repository layout

- `Breakout/` — Visual Studio project files and source (`.cpp`) for the game.
- `headers/` — Public header files used by the project.
- `assets/` — Art, textures and audio used by the game.

See the project root and folders: [Breakout/](Breakout/) • [headers/](Breakout/headers/) • [assets/](Breakout/assets/)

## Prerequisites

- Windows 10/11 (the project is Win32-focused).
- Visual Studio (2015, 2017, 2019 or 2022) with C++ workload installed.
- DirectX 9 libraries: If you get missing D3DX or other DirectX9 linker errors, install the DirectX SDK (June 2010). Modern Windows SDKs include many DirectX components, but legacy D3DX helpers often require the older SDK.
- DirectX 9 libraries: If you get missing D3DX or other DirectX9 linker errors, install the DirectX SDK (June 2010). Modern Windows SDKs include many DirectX components, but legacy D3DX helpers often require the older SDK.
- FMOD: The project uses FMOD for audio. Download the FMOD Studio API (Windows) and install or extract the SDK. Add the FMOD include/lib paths to the project properties (use the x86 libraries for Win32 builds).

Optional: run Visual Studio as Administrator if you need to register any dev components.

## How to open and build

1. Clone or copy the repository to your Windows machine.
2. Open the solution `Breakout.sln` in Visual Studio (File → Open → Project/Solution).
3. In Visual Studio, set the Solution Configuration to `Debug` or `Release` and Platform to `Win32`.
4. If you installed the DirectX SDK (June 2010), ensure project include/lib directories are set (Project → Properties → VC++ Directories).
   - Typical DirectX SDK paths to add: `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include` and `...\Lib\x86` for 32-bit builds.
5. FMOD setup: download the FMOD Studio API for Windows (https://fmod.com). Extract the SDK and add the FMOD `inc` folder to Project → Properties → C/C++ → Additional Include Directories, and the appropriate `lib` (x86) folder to Linker → Additional Library Directories. Link against the FMOD library (for example `fmod_vc.lib` or `fmodex_vc.lib` depending on the FMOD version used).
6. Build the solution (`Build → Build Solution`).
7. Run the built executable from Visual Studio (`Debug → Start Debugging`) or run the `.exe` from the build output directory.
5. Build the solution (`Build → Build Solution`).
6. Run the built executable from Visual Studio (`Debug → Start Debugging`) or run the `.exe` from the build output directory.

Notes:
- The project targets 32-bit Win32. Make sure Platform is `Win32` before building.
- If you encounter linker errors referencing `d3dx9*.lib`, install the June 2010 DirectX SDK and add its library paths.

## Running

- After a successful build, run the produced `.exe` (from the `Breakout/Debug` or `Breakout/Release` output). The game opens a Win32 window and runs using DirectX9 rendering.

## What you'll see / Features

- BreakOut gameplay (paddle, ball, bricks).
- Minimal game state management (menu, level, gameover, win states).
- Simple physics handling and collision response.
- Sound management for effects and basic audio playback.

## For your portfolio

- This project is ideal for showcasing low-level Windows game development skills and knowledge of legacy graphics APIs.
- Consider adding a short GIF or screenshot to the README to increase visual impact.

## Troubleshooting

- Missing D3DX functions or headers: install DirectX SDK (June 2010) and add include/lib paths to project settings.
- Wrong platform (x64) selected: switch to `Win32` in the toolbar.
- If assets fail to load at runtime, ensure the `assets` folder is copied or available next to the compiled executable.
- FMOD runtime errors: ensure FMOD DLLs are available next to the executable (copy the FMOD `dll` files from the SDK `bin` folder into the build output folder). Use x86 DLLs for Win32 builds.

