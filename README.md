# Robust 3D Cloth Simulator
Final project  
COMP-559 (Fundamentals of Computer Animation)  
McGill University

## Project Summary
Cloth simulator based on Bridson2002 (Robust Treatment of Collisions, Contact and Friction for Cloth Animation).

## Controls
|Control           | Action                   |
|:-----------------|:--------------------------|
| Mouse      | Camera                         |
| Arrow keys | Move                           |
| ESC        | Quit                           |
| 1          | Unpin cloth up left corner     |
| 2          | Unpin cloth up right corner    |
| D          | Unpin cloth up corners         |
| W          | Toggle wind                    |
| A          | Toggle 3D axis                 |
| L          | Toggle light                   |
| P          | Toggle cloth particles         |
| H          | Toggle wireframe mode          |
| K          | Toggle collisions detection    |
| R          | Reset simulation               |
| V          | Toggle record screen           |
| C          | Print camera position & target |

## Build
These instructions are for Microsoft Visual Studio 2017 but should work with earlier versions.

1. Extract the latest source code of the Irrlicht engine into the `irrlicht` directory
2. Open `irrlicht/examples/BuildAllExamples_vc12.sln`
3. Build the `Irrlicht` project
4. Copy `irrlicht/bin/Win64-VisualStudio/irrlicht.dll` into `bin/win64/`
5. Open `src/clothsim.sln`
6. Build the `clothsim` project
