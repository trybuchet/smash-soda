@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Smash Soda Updater

:: --------- Admin check ----------
net session >nul 2>&1
if %errorlevel% neq 0 (
  echo.
  echo ERROR: This updater must be run as Administrator.
  echo Attempting to relaunch with admin permissions...
  echo If you see a User Account Control prompt, click Yes.
  echo.
  if not defined SS_ELEVATED (
    set "SS_ELEVATED=1"
    if "%~1"=="" (
      %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -ExecutionPolicy Bypass ^
        -Command "Start-Process -FilePath 'cmd.exe' -Verb RunAs -ArgumentList '/c """"%~f0""""'"
    ) else (
      %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -ExecutionPolicy Bypass ^
        -Command "Start-Process -FilePath 'cmd.exe' -Verb RunAs -ArgumentList '/c """"%~f0"""" %*'"
    )
    timeout /t 5 >nul
    exit /b 1
  ) else (
    echo UAC was cancelled or elevation failed.
    echo Please right-click the updater and choose "Run as administrator".
    echo.
    pause >nul
  )
)

echo.
echo ==========================================================
echo Smash Soda Updater
echo ==========================================================
echo This updater will download build tools if needed and
echo compile Smash Soda from source.
echo ==========================================================
echo.

:: --------- Config ----------
set "BRANCH=preview"
set "REPO_URL=https://github.com/trybuchet/smash-soda.git"
set "SMASH_GLASS_VERSION=%~1"
if not defined SMASH_GLASS_VERSION (
  set "CMDLINE_LAST="
  for %%A in (%CMDCMDLINE%) do set "CMDLINE_LAST=%%~A"
  if defined CMDLINE_LAST (
    echo(!CMDLINE_LAST! | findstr /r "^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$" >nul && set "SMASH_GLASS_VERSION=!CMDLINE_LAST!"
  )
)
set "SMASH_GLASS_URL="
if defined SMASH_GLASS_VERSION (
  set "SMASH_GLASS_URL=https://github.com/trybuchet/smash-soda-overlay/releases/download/%SMASH_GLASS_VERSION%/smash-glass.zip"
)

:: VS Build Tools 2026 (use aka.ms link which always points to latest stable)
set "VS_BUILD_TOOLS_URL=https://aka.ms/vs/18/stable/vs_buildtools.exe"

:: --------- Install location (script folder) ----------
set "INSTALL_DIR=%~dp0"
if "%INSTALL_DIR:~-1%"=="\" set "INSTALL_DIR=%INSTALL_DIR:~0,-1%"
echo.
echo Install folder (script location): "%INSTALL_DIR%"

:: --------- Work dirs ----------
set "WORK=%LOCALAPPDATA%\SmashSodaInstaller"
set "SRC=%WORK%\src"
set "BUILD=%WORK%\build"
set "OVERLAY_ZIP=%WORK%\smash-glass.zip"
set "OVERLAY_EXTRACT=%WORK%\overlay-extract"
set "VS_BOOTSTRAP=%WORK%\vs_BuildTools.exe"

if exist "%WORK%" rmdir /s /q "%WORK%"
mkdir "%WORK%" "%SRC%" "%BUILD%" 2>nul

set "PS=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"

goto :main

:: --------- Helper: download with progress ----------
:download
set "URL=%~1"
set "DEST=%~2"
set "LABEL=%~3"
if "%URL%"=="" (
  echo ERROR: Download URL is empty for "%LABEL%".
  exit /b 1
)
echo Downloading %LABEL%...
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "$u='%URL%'; $d='%DEST%'; $l='%LABEL%'; " ^
  "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; " ^
  "try { " ^
  "  Write-Progress -Activity $l -Status 'Downloading...' -PercentComplete 0; " ^
  "  Invoke-WebRequest -Uri $u -OutFile $d -UseBasicParsing; " ^
  "  Write-Progress -Activity $l -Completed; " ^
  "  Write-Host 'Downloaded: ' $l; " ^
  "} catch { " ^
  "  try { " ^
  "    Write-Host 'Invoke-WebRequest failed. Trying BITS...'; " ^
  "    Start-BitsTransfer -Source $u -Destination $d -DisplayName $l; " ^
  "  } catch { " ^
  "    Write-Host 'Download failed:'; " ^
  "    Write-Host $_.Exception.Message; " ^
  "    exit 1; " ^
  "  } " ^
  "}"
if errorlevel 1 exit /b 1
exit /b 0

:main

@REM :: --------- Install Git ----------
@REM echo.
@REM echo [1/7] Installing Git...
@REM where git >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM   where winget >nul 2>&1
@REM   if %errorlevel% equ 0 (
@REM     echo Using winget to install Git...
@REM     winget install -e --id Git.Git --silent --accept-package-agreements --accept-source-agreements
@REM   ) else (
@REM     echo ERROR: winget is required to install Git automatically.
@REM     echo Please install winget ^(App Installer^) and rerun this updater.
@REM     pause
@REM     exit /b 1
@REM   )
@REM )

@REM :: Re-check git availability, try common install path if PATH is stale
@REM where git >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM   if exist "%ProgramFiles%\Git\bin\git.exe" (
@REM     set "PATH=%ProgramFiles%\Git\bin;%PATH%"
@REM   ) else if exist "%ProgramFiles(x86)%\Git\bin\git.exe" (
@REM     set "PATH=%ProgramFiles(x86)%\Git\bin;%PATH%"
@REM   )
@REM )
@REM where git >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM   echo ERROR: Git installation failed or Git not found in PATH.
@REM   echo Please install Git manually from https://git-scm.com/download/win
@REM   pause
@REM   exit /b 1
@REM )
@REM echo Git is ready.

@REM :: --------- Install Build Tools ----------
@REM echo.
@REM echo [2/7] Installing Visual Studio Build Tools (C++ toolchain)...

@REM set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
@REM set "VSINSTALL="

@REM :: Initial detection
@REM if exist "%VSWHERE%" (
@REM   for /f "usebackq tokens=*" %%i in (`
@REM     "%VSWHERE%" -latest -products * ^
@REM     -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
@REM     -property installationPath
@REM   `) do set "VSINSTALL=%%i"
@REM )

@REM if not defined VSINSTALL (
@REM   echo Visual Studio Build Tools not found. Installing...
@REM   echo This will take several minutes. Please wait...
@REM   echo.
  
@REM   where winget >nul 2>&1
@REM   if %errorlevel% equ 0 (
@REM     echo Using winget to install Visual Studio Build Tools 2026 with ATL support...
@REM     winget install -e --id Microsoft.VisualStudio.BuildTools ^
@REM       --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.VC.ATL --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --includeRecommended"
@REM   ) else (
@REM     echo Downloading Visual Studio Build Tools installer...
@REM     call :download "%VS_BUILD_TOOLS_URL%" "%VS_BOOTSTRAP%" "Visual Studio Build Tools"
    
@REM     echo Running Visual Studio Build Tools installer with ATL support...
@REM     "%VS_BOOTSTRAP%" --wait --quiet --norestart ^
@REM       --add Microsoft.VisualStudio.Workload.VCTools ^
@REM       --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
@REM       --add Microsoft.VisualStudio.Component.VC.ATL ^
@REM       --add Microsoft.VisualStudio.Component.Windows11SDK.22621 ^
@REM       --includeRecommended
@REM   )

@REM   echo Waiting for installation to complete...
@REM   timeout /t 10 >nul

@REM   :: Wait for vswhere to appear and detect installation (max 5 minutes)
@REM   set WAIT_COUNT=0
@REM   :wait_vs
@REM   if !WAIT_COUNT! GEQ 60 (
@REM     echo.
@REM     echo ERROR: Visual Studio Build Tools installation timed out or failed.
@REM     echo Please install manually from: 
@REM     echo https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2026
@REM     echo.
@REM     echo Required components:
@REM     echo - Desktop development with C++
@REM     echo - MSVC v145 - VS 2026 C++ x64/x86 build tools
@REM     echo - Windows 11 SDK
@REM     echo - ATL for latest v145 build tools ^(x86 and x64^)
@REM     pause
@REM     exit /b 1
@REM   )
  
@REM   timeout /t 5 >nul
@REM   set /a WAIT_COUNT+=1
@REM   echo Checking installation status... (!WAIT_COUNT!/60)
  
@REM   if exist "%VSWHERE%" (
@REM     for /f "usebackq tokens=*" %%i in (`
@REM       "%VSWHERE%" -latest -products * ^
@REM       -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
@REM       -property installationPath
@REM     `) do set "VSINSTALL=%%i"
@REM   )

@REM   if not defined VSINSTALL goto wait_vs
@REM )

@REM echo.
@REM echo Visual Studio Build Tools detected at:
@REM echo "%VSINSTALL%"

@REM :: CRITICAL: Initialize the VS environment before running CMake
@REM echo.
@REM echo Initializing Visual Studio development environment...
@REM if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" (
@REM   call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
@REM   if !errorlevel! neq 0 (
@REM     echo ERROR: Failed to initialize Visual Studio environment
@REM     pause
@REM     exit /b 1
@REM   )
@REM ) else (
@REM   echo ERROR: VsDevCmd.bat not found at expected location:
@REM   echo "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
@REM   pause
@REM   exit /b 1
@REM )

@REM echo C++ build environment is ready.

@REM :: --------- Install CMake ----------
@REM echo.
@REM echo [3/7] Installing CMake...
@REM where cmake >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM   where winget >nul 2>&1
@REM   if %errorlevel% neq 0 (
@REM     echo ERROR: winget is required to install CMake automatically.
@REM     echo Please install winget ^(App Installer^) and rerun this updater.
@REM     pause
@REM     exit /b 1
@REM   )
@REM   echo Using winget to install CMake...
@REM   winget install -e --id Kitware.CMake --silent --accept-package-agreements --accept-source-agreements
@REM   timeout /t 5 >nul
@REM )

@REM :: Add CMake to PATH manually if needed
@REM if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
@REM   set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
@REM )

@REM :: Verify CMake
@REM where cmake >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM   echo ERROR: CMake not found after installation
@REM   pause
@REM   exit /b 1
@REM )
@REM echo CMake is ready.

@REM :: --------- Clone source ----------
@REM echo.
@REM echo [4/7] Cloning Smash Soda (%BRANCH% branch)...
@REM set "SRC_DIR=%SRC%\smash-soda"
@REM if exist "%SRC_DIR%" rmdir /s /q "%SRC_DIR%"
@REM git clone --branch "%BRANCH%" --single-branch "%REPO_URL%" "%SRC_DIR%"
@REM if %errorlevel% neq 0 (
@REM   echo ERROR: Git clone failed
@REM   pause
@REM   exit /b 1
@REM )
@REM set "DEPS_DIR=%SRC_DIR%\dependencies"
@REM if not exist "%DEPS_DIR%" set "DEPS_DIR=%SRC_DIR%\Dependencies"
@REM echo Source code cloned successfully.

@REM :: --------- Build ----------
@REM echo.
@REM echo [5/7] Configuring CMake...
@REM cmake -S "%SRC_DIR%" -B "%BUILD%" -A x64
@REM if %errorlevel% neq 0 (
@REM   echo ERROR: CMake configuration failed
@REM   pause
@REM   exit /b 1
@REM )

@REM echo.
@REM echo CMake generator info:
@REM cmake -LA -N "%BUILD%" | findstr /C:"CMAKE_GENERATOR"

@REM echo.
@REM echo [6/7] Building Release...
@REM mkdir "%SRC_DIR%\x64\release" 2>nul
@REM cmake --build "%BUILD%" --config Release
@REM if %errorlevel% neq 0 (
@REM   echo ERROR: Build failed
@REM   pause
@REM   exit /b 1
@REM )

@REM :: --------- Install ----------
@REM set "RELEASE_DIR="
@REM if exist "%SRC_DIR%\x64\release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\x64\release"
@REM if not defined RELEASE_DIR if exist "%BUILD%\Release\SmashSoda.exe" set "RELEASE_DIR=%BUILD%\Release"
@REM if not defined RELEASE_DIR if exist "%BUILD%\SmashSoda\Release\SmashSoda.exe" set "RELEASE_DIR=%BUILD%\SmashSoda\Release"
@REM if not defined RELEASE_DIR (
@REM   for /f "delims=" %%F in ('dir /b /s "%BUILD%\SmashSoda.exe" 2^>nul') do (
@REM     if not defined RELEASE_DIR set "RELEASE_DIR=%%~dpF"
@REM   )
@REM )
@REM if not defined RELEASE_DIR (
@REM   echo ERROR: Release output not found. Checked:
@REM   echo   "%SRC_DIR%\x64\release\SmashSoda.exe"
@REM   echo   "%BUILD%\Release\SmashSoda.exe"
@REM   echo   "%BUILD%\SmashSoda\Release\SmashSoda.exe"
@REM   pause
@REM   exit /b 1
@REM )

@REM echo.
@REM echo [7/7] Installing Smash Soda...
@REM if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
@REM xcopy "%RELEASE_DIR%\*" "%INSTALL_DIR%\" /E /I /Y
@REM if %errorlevel% neq 0 (
@REM   echo ERROR: Failed to copy files to install directory
@REM   pause
@REM   exit /b 1
@REM )

:: --------- Smash Glass Overlay ----------
echo.
set "OVERLAY_STATUS=Overlay update skipped - no version argument provided."
if defined SMASH_GLASS_VERSION (
  echo Installing Smash Glass overlay version %SMASH_GLASS_VERSION%...
  set "OVERLAY_DIR=%INSTALL_DIR%\overlay"
  mkdir "!OVERLAY_DIR!" 2>nul
  if exist "!OVERLAY_EXTRACT!" rmdir /s /q "!OVERLAY_EXTRACT!"
  mkdir "!OVERLAY_EXTRACT!" 2>nul
  call :download "%SMASH_GLASS_URL%" "%OVERLAY_ZIP%" "Smash Glass %SMASH_GLASS_VERSION%"
  %PS% -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%OVERLAY_ZIP%' -DestinationPath '!OVERLAY_EXTRACT!' -Force"
  del /f /q "%OVERLAY_ZIP%" >nul 2>&1

  :: Copy overlay files but preserve user's plugins and themes folders
  robocopy "!OVERLAY_EXTRACT!" "!OVERLAY_DIR!" /E /XO /NFL /NDL /NJH /NJS /NP /XD "plugins" "themes" >nul
  if %errorlevel% GEQ 8 (
    echo ERROR: Failed to update overlay files.
    pause
    exit /b 1
  )
  if exist "!OVERLAY_EXTRACT!" rmdir /s /q "!OVERLAY_EXTRACT!"

  :: Unblock overlay files
  %PS% -NoProfile -ExecutionPolicy Bypass -Command ^
    "Get-ChildItem -Path '!OVERLAY_DIR!' -Recurse | Where-Object { $_.FullName -notlike '*\\plugins\\*' -and $_.FullName -notlike '*\\themes\\*' } | Unblock-File"
  set "OVERLAY_STATUS=Overlay installed at: !OVERLAY_DIR!"
)

echo.
echo ============================================================
echo UPDATE COMPLETE!
echo ============================================================
echo Smash Soda updated at: "%INSTALL_DIR%"
echo(!OVERLAY_STATUS!
echo ============================================================
echo.

echo Cleaning up updater files...
rmdir /s /q "%WORK%"

echo.
echo Smash Soda updated! Press any key to exit...
pause >nul
exit /b 0
