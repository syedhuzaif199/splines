# Spline Editor

A simple spline editor built with Raylib.

## Quick Start

1. Prerequisites:
    - Install [GNU Make](https://www.gnu.org/software/make/).
    - The raylib library is included in the libs directory.
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
<!-- say that switching is not implemented yet -->
- **Left Click**: Add control points.
- **Left Click + Drag**: Move control points.
- **KEY J**: Remove the last added control point.
- **KEY K**: Add back the last removed control point.
- **KEY C**: Toggle control points visibility.
- **KEY T**: Toggle connecting lines visibility.

## TODO
- Implement check for determining if a curve is closed.
- Implement select mode.
### Editor Modes
- **Select**: Select and move existing control points.
- **Pen Tool**: Auto generate spline handles.
- **Draw Spline Manual**: Manually place control points for the spline.