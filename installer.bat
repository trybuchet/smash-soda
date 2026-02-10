@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Smash Soda Installer (Preview)

:: --------- Admin check ----------
net session >nul 2>&1
if %errorlevel% neq 0 (
  echo.
  echo ERROR: This installer must be run as Administrator.
  echo.
  echo Attempting to relaunch with admin permissions...
  echo.
  if not defined SS_ELEVATED (
    set "SS_ELEVATED=1"
    %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe ^
      -NoProfile -ExecutionPolicy Bypass ^
      -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b 1
  ) else (
    echo UAC was cancelled.
    pause
    exit /b 1
  )
)

:: --------- Intro ----------
echo ==========================================================
echo Smash Soda Installer (Preview)
echo ==========================================================
echo This installer will download build tools and compile
echo Smash Soda from source.
echo ==========================================================
pause

:: --------- Config ----------
set "BRANCH=preview"
set "REPO_URL=https://github.com/trybuchet/smash-soda.git"
set "SMASH_GLASS_URL=https://github.com/trybuchet/smash-soda-overlay/releases/download/4.0.0/smash-glass.zip"
set "VIGEMBUS_URL=https://github.com/nefarius/ViGEmBus/releases/download/v1.22.0/ViGEmBus_1.22.0_x64_x86_arm64.exe"
set "VS_BUILD_TOOLS_URL=https://download.visualstudio.microsoft.com/download/pr/6efb3484-905b-485c-8b5f-9d3a5f39e731/07908cd6d91e75b8ea4339d8f2cfa6e8d8bb4fd706af7b918ae391cd6fc2a066/vs_BuildTools.exe"
set "GIT_SETUP_URL=https://github.com/git-for-windows/git/releases/latest/download/Git-64-bit.exe"
set "CMAKE_VERSION=3.28.3"
set "CMAKE_MSI_URL=https://cmake.org/files/v3.28/cmake-%CMAKE_VERSION%-windows-x86_64.msi"

:: --------- Install folder ----------
set "DEFAULT_INSTALL=%ProgramFiles%\Smash Soda"
for /f "usebackq delims=" %%i in (`powershell -STA -Command ^
  "Add-Type -AssemblyName System.Windows.Forms; $d=New-Object System.Windows.Forms.FolderBrowserDialog; $d.SelectedPath='%DEFAULT_INSTALL%'; if($d.ShowDialog() -eq 'OK'){ $d.SelectedPath }"`) do set "INSTALL_DIR=%%i"
if not defined INSTALL_DIR set "INSTALL_DIR=%DEFAULT_INSTALL%"

:: --------- Work dirs ----------
set "WORK=%LOCALAPPDATA%\SmashSodaInstaller"
set "SRC=%WORK%\src"
set "BUILD=%WORK%\build"
set "OVERLAY_ZIP=%WORK%\smash-glass.zip"
set "VIGEMBUS_SETUP=%WORK%\ViGEmBus.exe"
set "VS_BOOTSTRAP=%WORK%\vs_BuildTools.exe"
set "CMAKE_MSI=%WORK%\cmake.msi"
set "GIT_SETUP=%WORK%\Git.exe"
set "PS=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"

if exist "%WORK%" rmdir /s /q "%WORK%"
mkdir "%WORK%" "%SRC%" "%BUILD%"

:: --------- Download helper ----------
:download
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "Invoke-WebRequest -Uri '%~1' -OutFile '%~2'"
exit /b 0

:: --------- Git ----------
where git >nul 2>&1 || (
  where winget >nul 2>&1 && winget install -e --id Git.Git --silent ^
  || (call :download "%GIT_SETUP_URL%" "%GIT_SETUP%" "Git" & "%GIT_SETUP%" /VERYSILENT)
)
where git >nul || exit /b 1

:: --------- Build Tools ----------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" call :download "%VS_BUILD_TOOLS_URL%" "%VS_BOOTSTRAP%" "VS Build Tools" & "%VS_BOOTSTRAP%" --quiet --wait --add Microsoft.VisualStudio.Workload.VCTools

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64

:: --------- CMake ----------
where cmake >nul || (
  call :download "%CMAKE_MSI_URL%" "%CMAKE_MSI%" "CMake"
  msiexec /i "%CMAKE_MSI%" /qn ADD_CMAKE_TO_PATH=System
  set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
)
where cmake >nul || exit /b 1

:: --------- ViGEmBus ----------
call :download "%VIGEMBUS_URL%" "%VIGEMBUS_SETUP%" "ViGEmBus"
"%VIGEMBUS_SETUP%" /qn

:: --------- Clone ----------
set "SRC_DIR=%SRC%\smash-soda"
git clone --branch "%BRANCH%" --single-branch "%REPO_URL%" "%SRC_DIR%" || exit /b 1

:: --------- Configure / Build ----------
cmake -S "%SRC_DIR%" -B "%BUILD%" -A x64 || exit /b 1
cmake --build "%BUILD%" --config Release || exit /b 1

:: --------- Locate output (check both case variations) ----------
set "RELEASE_DIR="
if exist "%SRC_DIR%\x64\Release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\x64\Release"
if exist "%SRC_DIR%\x64\release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\x64\release"
if exist "%SRC_DIR%\Win32\Release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\Win32\Release"
if exist "%SRC_DIR%\Win32\release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\Win32\release"
if not defined RELEASE_DIR (
  echo ERROR: SmashSoda.exe not found.
  echo Searched locations:
  echo   - %SRC_DIR%\x64\Release
  echo   - %SRC_DIR%\x64\release
  echo   - %SRC_DIR%\Win32\Release
  echo   - %SRC_DIR%\Win32\release
  pause
  exit /b 1
)

echo Found SmashSoda.exe in: %RELEASE_DIR%

:: --------- Install ----------
mkdir "%INSTALL_DIR%" 2>nul
xcopy "%RELEASE_DIR%\*" "%INSTALL_DIR%\" /E /I /Y >nul

:: --------- Smash Glass ----------
set "OVERLAY_DIR=%INSTALL_DIR%\overlay"
call :download "%SMASH_GLASS_URL%" "%OVERLAY_ZIP%" "Smash Glass"
%PS% -Command "Expand-Archive '%OVERLAY_ZIP%' '%OVERLAY_DIR%' -Force"

:: --------- Shortcuts ----------
call :shortcuts

rmdir /s /q "%WORK%"
echo.
echo ==========================================================
echo Installation Complete!
echo ==========================================================
echo Smash Soda has been installed to:
echo %INSTALL_DIR%
echo.
echo A desktop shortcut has been created.
echo ==========================================================
pause
exit /b 0

:shortcuts
set "APP_EXE=%INSTALL_DIR%\SmashSoda.exe"
if not exist "%APP_EXE%" exit /b 0
%PS% -Command ^
  "$w=New-Object -ComObject WScript.Shell; $s=$w.CreateShortcut($env:USERPROFILE+'\Desktop\Smash Soda.lnk'); $s.TargetPath='%APP_EXE%'; $s.Save()"
exit /b 0