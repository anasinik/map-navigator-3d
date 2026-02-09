# 🗺️ 3D Map Movement & Distance Measuring

C++ OpenGL application for moving on a textured map and measuring distances.  
The project has two modes: **Walking** and **Measuring**.

## ✨ Features
- 3D scene with a large textured map (ground plane)
- Perspective camera looking down at the map
- 💡 Light source affecting all objects
- Two modes:
  - 🚶 **Walking mode**: move a humanoid model and measure traveled distance
  - 📏 **Measuring mode**: place pins and measure total polyline distance
- 🖥️ Mode indicator + distance text rendered as a 2D overlay (fixed on screen)

## 🎮 Controls

| Key / Mouse | Action |
|------------|--------|
| `W A S D` | Move humanoid (Walking mode) |
| `Arrow keys` | Move camera over the map |
| `R` | Toggle Walking / Measuring mode |
| `Left click` | Add a pin (Measuring mode) |
| `Left click on pin` | Remove pin (Measuring mode) |

## 📝 Notes
- Movement is limited by map boundaries (X and Z axes).
- Camera Y changes depending on the active mode.
- 📌 Pins are represented as a thin cylinder with a red sphere on top.

## 🧩 Requirements
- C++
- OpenGL 3.x (programmable pipeline)

## ▶️ How to Run
Open the project in Visual Studio and build/run.
