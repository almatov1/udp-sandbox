# UDP Sandbox

A small C++ game prototype built with [raylib](https://www.raylib.com/) and CMake.

## Features

- Fixed 800 × 600 window
- Player movement with **WASD** or **arrow keys**
- Player is kept inside the game boundaries
- FPS and frame-time debug information

> This is an early prototype, not a finished game.

## Requirements

- C++17-compatible compiler
- CMake 3.20 or newer
- raylib installed on your system

## Build on macOS

These instructions target macOS with Homebrew.

1. Install the required tools:

   ```bash
   xcode-select --install
   brew install cmake raylib
   ```

2. Clone the repository and enter its directory:

   ```bash
   git clone <REPOSITORY_URL>
   cd udp-sandbox-client
   ```

   Replace `<REPOSITORY_URL>` with the URL of your GitHub repository.

3. Configure and build:

   ```bash
   cmake -S . -B build
   cmake --build build
   ```

4. Run the game:

   ```bash
   ./build/udp_sandbox
   ```

If you change the CMake configuration or encounter stale build files, rebuild from a clean build directory:

```bash
rm -rf build
cmake -S . -B build
cmake --build build
```

## Controls

| Key | Action |
| --- | --- |
| W / Up arrow | Move up |
| A / Left arrow | Move left |
| S / Down arrow | Move down |
| D / Right arrow | Move right |
| Esc | Close the game |

## Project structure

```text
udp-sandbox-client/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── assets/
│   ├── textures/
│   ├── sounds/
│   ├── music/
│   └── fonts/
└── src/
    ├── main.cpp
    ├── game/
    │   ├── player.cpp
    │   ├── player.h
    │   ├── renderer.cpp
    │   └── renderer.h
    └── network/
```

The `assets/` folders are optional; create only the folders you need. Make sure the game loads assets using paths that work from the directory where it is launched.

## Download a prebuilt version

For ready-to-run builds, publish binaries on the repository's **GitHub Releases** page rather than committing build output to the source repository.

Use a separate archive for each supported platform, for example:

- `udp-sandbox-client-macos-arm64.zip` — Apple Silicon Macs
- `udp-sandbox-client-macos-x86_64.zip` — Intel Macs
- `udp-sandbox-client-windows-x86_64.zip` — Windows 64-bit
- `udp-sandbox-client-linux-x86_64.tar.gz` — Linux 64-bit

Only list platforms for which you have actually built and tested the game. A macOS build does not run natively on Windows or Linux. Include any required runtime libraries and the `assets/` directory in each archive.

## License

The source code is licensed under the MIT License if a `LICENSE` file is included in the repository. Third-party libraries and game assets may have their own licenses; check and preserve those notices. Do not assume that the MIT License applies to third-party assets unless you have the rights to license them that way.
