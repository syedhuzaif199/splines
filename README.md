# Spline Editor

A simple spline editor built with Raylib.

## Quick Start

1. Prerequisites:
    - Install [GNU Make](https://www.gnu.org/software/make/).
    - The raylib library is included in the libs directory, but needs to be built using make.
    - Ensure you have a C compiler set up.

2. Build the project:
    - For linux/macOS:
      ```bash
      ./build.sh
      ```
    - For Windows:
      ```bash
      .\build.bat
      ```
3. Run the application:
    - For linux/macOS:
      ```bash
      ./build/main
      ```
    - For Windows:
      ```bash
      .\build\main.exe
      ```   

## Controls
- **Left Click**: Add control points.
- **Left Click + Drag**: Move control points.
- **KEY J**: Remove the last added control point.
- **KEY K**: Add back the last removed control point.
- **KEY C**: Toggle control points visibility.
- **KEY T**: Toggle connecting lines visibility.
### Editor Modes
- **KEY 1: Select**: (not implemented)
- **KEY 2: Pen Tool**: 
  - Add hard corners by clicking once.
  - Add smooth curves by clicking and dragging.
- **KEY 3: Draw Spline Manual**: Manually place control points for the spline.

## TODO
- Implement check for determining if a curve is closed.
### Editor Modes
- **Select**: Implement functionality to select and move existing control points.
- **Pen Tool**: Implement functionality to allow treatment of collapsed handles at hard corners as a single node.