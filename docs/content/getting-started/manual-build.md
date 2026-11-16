# Manual Build (Windows)

If the installer script does not work for your system, you can build Smash Soda manually.

## Requirements

- Git for Windows
- CMake (3.18 or newer), added to `PATH`
- Visual Studio Build Tools with:
  - Desktop development with C++
  - MSVC C++ toolset
  - Windows 10/11 SDK

## 1) Clone the repository

```powershell
git clone https://github.com/soda-arcade/smash-soda.git
cd smash-soda
```

## 2) Configure local credentials

Create your own client ID and secret on Soda Arcade first. [Soda Arcade Developer Credentials](developer-credentials.md)
explains how, including which scopes to select.
Run `SetupPrivateConfig.ps1` and enter those credentials. The script creates an ignored
`SmashSoda/PrivateConfig.h` and generates local encryption values. The API host,
port, transport, route prefix, and Smash Soda version can be changed in that
private header before building.

## 3) Configure CMake

```powershell
cmake -S . -B build
```

## 4) Build Release

```powershell
cmake --build build --config Release --parallel
```

Output executable:

- `x64/Release/SmashSoda.exe`

## 5) Optional: build a debuggable version

```powershell
cmake --build build --config RelWithDebInfo --parallel
```

Output executable:

- `x64/RelWithDebInfo/SmashSoda.exe`
