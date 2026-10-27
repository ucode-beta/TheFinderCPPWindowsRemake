# TheFinder

Windows version of TheFinder. The game starts in a main menu, then loads `res/maps/map1.json` with the original waste-collection and enemy gameplay. It is a single-player game; the upstream repository does not contain networking or multiplayer code.

## Requirements

- CodeLLDB VS Code extension
- MinGW-w64 with `g++`, `gendef`, and `dlltool` available on `PATH`

The build task automatically downloads the official 64-bit SDL2 2.32.10 and SDL2_image 2.8.12 development archives, verifies their SHA-256 hashes, and places them under the ignored `build/deps` directory. It creates MinGW-compatible import libraries from the verified SDL DLLs in the ignored `build` directory.

## Build and run

1. Press **F5** and select **TheFinder (CodeLLDB)**. VS Code prepares the SDL2 files, builds `finder.exe`, then launches it from the repository root so the `res/` paths resolve.

You can run **Build TheFinder** from **Terminal: Run Build Task** (or press **Ctrl+Shift+B**) without launching the app.

Alternatively, run `powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\ps1\build-and-run.ps1` to prepare dependencies, build, and launch the game. Add `-BuildOnly` to prepare and build without launching.

## Play

- Click **Play** or press **Enter** at the menu.
- Move with **WASD** or the arrow keys.
- Collect the four waste items and deliver the two organic items to the first bin and the two metal items to the second bin.
- Avoid the enemy as it navigates around walls and follows you. If caught, press **Enter** or **R** to restart.
- Press **Esc** to return to the menu or close the game window to quit.
