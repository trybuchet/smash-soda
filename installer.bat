@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Smash Soda Installer

:: --------- Admin check ----------
set "SKIP_ADMIN_CHECK=1"
if not "%SKIP_ADMIN_CHECK%"=="1" (
  net session >nul 2>&1
  if %errorlevel% neq 0 (
    echo.
    echo ERROR: This installer must be run as Administrator.
    echo Attempting to relaunch with admin permissions...
    echo If you see a User Account Control prompt, click Yes.
    echo.
    if not defined SS_ELEVATED (
      set "SS_ELEVATED=1"
      if "%~1"=="" (
        %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -ExecutionPolicy Bypass ^
          -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
      ) else (
        %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -ExecutionPolicy Bypass ^
          -Command "Start-Process -FilePath '%~f0' -Verb RunAs -ArgumentList '%*'"
      )
      timeout /t 5 >nul
      exit /b 1
    ) else (
      echo UAC was cancelled or elevation failed.
      echo Please right-click the installer and choose "Run as administrator".
      echo.
      pause >nul
    )
  )
)

echo.
echo ==========================================================
echo Smash Soda Installer
echo ==========================================================
echo Smash Soda is an unofficial tool for hosting rooms on the
echo Parsec service, with greater control over guests and
echo gamepad management. It is NOT affiliated with Parsec.
echo.
echo This installer will download build tools and compile
echo Smash Soda from source (required due to SDK licensing).
echo.
echo Recommended free disk space: at least 15 GB.
echo.
echo Sometimes the installer can go to sleep, click the window
echo or press a key if it shows no activity.
echo ==========================================================
echo.
echo Press any key to continue...
pause >nul

:: --------- Config ----------
set "BRANCH=master"
set "REPO_URL=https://github.com/trybuchet/smash-soda.git"
set "SMASH_GLASS_URL=https://github.com/trybuchet/smash-glass/releases/download/1.0.0/smash-glass-v1.00.zip"
set "VIGEMBUS_URL=https://github.com/nefarius/ViGEmBus/releases/download/v1.22.0/ViGEmBus_1.22.0_x64_x86_arm64.exe"

:: --------- Step toggles (1=run, 0=skip) ----------
:: Use these for quick testing of installer sections.
set "STEP_INSTALL_GIT=1"
set "STEP_INSTALL_BUILD_TOOLS=1"
set "STEP_INSTALL_CMAKE=1"
set "STEP_INSTALL_VIGEMBUS=1"
set "STEP_CLONE_SOURCE=1"
set "STEP_BUILD=1"
set "STEP_INSTALL_APP=1"
set "STEP_MIGRATE_DATA=1"
set "STEP_INSTALL_OVERLAY=1"
set "STEP_CREATE_SHORTCUTS=1"
set "STEP_CLEANUP=1"

:: VS Build Tools 2026 (use aka.ms link which always points to latest stable)
set "VS_BUILD_TOOLS_URL=https://aka.ms/vs/18/stable/vs_buildtools.exe"

:: --------- Install location (folder picker) ----------
set "DEFAULT_INSTALL=%ProgramFiles%\Smash Soda"
echo.
echo Please choose where to install Smash Soda.
echo This folder will contain the app and its data.
echo.
for /f "usebackq delims=" %%i in (`%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -ExecutionPolicy Bypass -STA -Command ^
  "Add-Type -AssemblyName System.Windows.Forms; " ^
  "$dlg=New-Object System.Windows.Forms.FolderBrowserDialog; " ^
  "$dlg.Description='Select the folder to install Smash Soda (e.g. C:\Program Files\Smash Soda)'; " ^
  "$dlg.SelectedPath='%DEFAULT_INSTALL%'; " ^
  "$dlg.ShowNewFolderButton=$true; " ^
  "if($dlg.ShowDialog() -eq 'OK'){ $dlg.SelectedPath }"`) do set "INSTALL_DIR=%%i"
if "%INSTALL_DIR%"=="" set "INSTALL_DIR=%DEFAULT_INSTALL%"
echo.
echo Install folder: "%INSTALL_DIR%"

:: --------- Work dirs ----------
set "WORK=%LOCALAPPDATA%\SmashSodaInstaller"
set "SRC=%WORK%\src"
set "BUILD=%WORK%\build"
set "SRC_DIR=%SRC%\smash-soda"
set "DEPS_DIR=%SRC_DIR%\dependencies"
set "OVERLAY_ZIP=%WORK%\smash-glass.zip"
set "VIGEMBUS_SETUP=%WORK%\ViGEmBus_1.22.0_x64_x86_arm64.exe"
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

:: --------- Install Git ----------
echo.
echo [1/8] Installing Git...
if "%STEP_INSTALL_GIT%"=="0" (
  echo Skipped [1/8] Install Git.
  goto :after_install_git
)
where git >nul 2>&1
if %errorlevel% neq 0 (
  where winget >nul 2>&1
  if %errorlevel% equ 0 (
    echo Using winget to install Git...
    winget install -e --id Git.Git --silent --accept-package-agreements --accept-source-agreements
  ) else (
    echo ERROR: winget is required to install Git automatically.
    echo Please install winget ^(App Installer^) and rerun this installer.
    pause
    exit /b 1
  )
)

:: Re-check git availability, try common install path if PATH is stale
where git >nul 2>&1
if %errorlevel% neq 0 (
  if exist "%ProgramFiles%\Git\bin\git.exe" (
    set "PATH=%ProgramFiles%\Git\bin;%PATH%"
  ) else if exist "%ProgramFiles(x86)%\Git\bin\git.exe" (
    set "PATH=%ProgramFiles(x86)%\Git\bin;%PATH%"
  )
)
where git >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: Git installation failed or Git not found in PATH.
  echo Please install Git manually from https://git-scm.com/download/win
  pause
  exit /b 1
)
echo Git is ready.
:after_install_git

:: --------- Install Build Tools ----------
echo.
echo [2/8] Installing Visual Studio Build Tools (C++ toolchain)...
if "%STEP_INSTALL_BUILD_TOOLS%"=="0" (
  echo Skipped [2/8] Install Build Tools.
  goto :after_install_build_tools
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="

:: Initial detection
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`
    "%VSWHERE%" -latest -products * ^
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
    -property installationPath
  `) do set "VSINSTALL=%%i"
)

if not defined VSINSTALL (
  echo Visual Studio Build Tools not found. Installing...
  echo This will take several minutes. Please wait...
  echo.
  
  where winget >nul 2>&1
  if %errorlevel% equ 0 (
    echo Using winget to install Visual Studio Build Tools 2026 with ATL support...
    winget install -e --id Microsoft.VisualStudio.BuildTools ^
      --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.VC.ATL --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --includeRecommended"
  ) else (
    echo Downloading Visual Studio Build Tools installer...
    call :download "%VS_BUILD_TOOLS_URL%" "%VS_BOOTSTRAP%" "Visual Studio Build Tools"
    
    echo Running Visual Studio Build Tools installer with ATL support...
    "%VS_BOOTSTRAP%" --wait --quiet --norestart ^
      --add Microsoft.VisualStudio.Workload.VCTools ^
      --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
      --add Microsoft.VisualStudio.Component.VC.ATL ^
      --add Microsoft.VisualStudio.Component.Windows11SDK.22621 ^
      --includeRecommended
  )

  echo Waiting for installation to complete...
  timeout /t 10 >nul

  :: Wait for vswhere to appear and detect installation (max 5 minutes)
  set WAIT_COUNT=0
  :wait_vs
  if !WAIT_COUNT! GEQ 60 (
    echo.
    echo ERROR: Visual Studio Build Tools installation timed out or failed.
    echo Please install manually from: 
    echo https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2026
    echo.
    echo Required components:
    echo - Desktop development with C++
    echo - MSVC v145 - VS 2026 C++ x64/x86 build tools
    echo - Windows 11 SDK
    echo - ATL for latest v145 build tools (x86 and x64)
    pause
    exit /b 1
  )
  
  timeout /t 5 >nul
  set /a WAIT_COUNT+=1
  echo Checking installation status... (!WAIT_COUNT!/60)
  
  if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`
      "%VSWHERE%" -latest -products * ^
      -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
      -property installationPath
    `) do set "VSINSTALL=%%i"
  )

  if not defined VSINSTALL goto wait_vs
)

echo.
echo Visual Studio Build Tools detected at:
echo "%VSINSTALL%"

:: CRITICAL: Initialize the VS environment before running CMake
echo.
echo Initializing Visual Studio development environment...
if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" (
  call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
  if !errorlevel! neq 0 (
    echo ERROR: Failed to initialize Visual Studio environment
    pause
    exit /b 1
  )
) else (
  echo ERROR: VsDevCmd.bat not found at expected location:
  echo "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
  pause
  exit /b 1
)

echo C++ build environment is ready.
:after_install_build_tools

:: --------- Install CMake ----------
echo.
echo [3/8] Installing CMake...
if "%STEP_INSTALL_CMAKE%"=="0" (
  echo Skipped [3/8] Install CMake.
  goto :after_install_cmake
)
where cmake >nul 2>&1
if %errorlevel% neq 0 (
  where winget >nul 2>&1
  if %errorlevel% neq 0 (
    echo ERROR: winget is required to install CMake automatically.
    echo Please install winget ^(App Installer^) and rerun this installer.
    pause
    exit /b 1
  )
  echo Using winget to install CMake...
  winget install -e --id Kitware.CMake --silent --accept-package-agreements --accept-source-agreements
  timeout /t 5 >nul
)

:: Add CMake to PATH manually if needed
if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
  set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
)

:: Verify CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: CMake not found after installation
  pause
  exit /b 1
)
echo CMake is ready.
:after_install_cmake

:: --------- ViGEmBus driver ----------
echo.
echo [4/8] ViGEmBus driver 
if "%STEP_INSTALL_VIGEMBUS%"=="0" (
  echo Skipped [4/8] ViGEmBus driver.
  goto :after_install_vigembus
)
call :download "%VIGEMBUS_URL%" "%VIGEMBUS_SETUP%" "ViGEmBus Driver"
if not exist "%VIGEMBUS_SETUP%" (
  echo ERROR: ViGEmBus installer was not downloaded.
  exit /b 1
)
"%VIGEMBUS_SETUP%" /qn
:after_install_vigembus

:: --------- Clone source ----------
echo.
echo [5/8] Cloning Smash Soda (%BRANCH% branch)...
if "%STEP_CLONE_SOURCE%"=="0" (
  echo Skipped [5/8] Clone source.
  goto :after_clone_source
)
if exist "%SRC_DIR%" rmdir /s /q "%SRC_DIR%"
git clone --branch "%BRANCH%" --single-branch "%REPO_URL%" "%SRC_DIR%"
if %errorlevel% neq 0 (
  echo ERROR: Git clone failed
  pause
  exit /b 1
)
set "DEPS_DIR=%SRC_DIR%\dependencies"
if not exist "%DEPS_DIR%" set "DEPS_DIR=%SRC_DIR%\Dependencies"
echo Source code cloned successfully.
:after_clone_source

:: --------- Build ----------
echo.
echo [6/8] Configuring CMake...
if "%STEP_BUILD%"=="0" (
  echo Skipped [6/8] Configure and [7/8] Build.
  goto :after_build
)
cmake -S "%SRC_DIR%" -B "%BUILD%" -A x64
if %errorlevel% neq 0 (
  echo ERROR: CMake configuration failed
  pause
  exit /b 1
)

echo.
echo CMake generator info:
cmake -LA -N "%BUILD%" | findstr /C:"CMAKE_GENERATOR"

echo.
echo [7/8] Building Release...
mkdir "%SRC_DIR%\x64\release" 2>nul
cmake --build "%BUILD%" --config Release
if %errorlevel% neq 0 (
  echo ERROR: Build failed
  pause
  exit /b 1
)
:after_build

:: --------- Install ----------
if "%STEP_INSTALL_APP%"=="0" (
  echo.
  echo Skipped [8/8] Install app files.
  goto :after_install_app
)
set "RELEASE_DIR=%SRC_DIR%\x64\release"
if not exist "%RELEASE_DIR%" (
  set "RELEASE_DIR=%BUILD%\Release"
)
if not exist "%RELEASE_DIR%" (
  echo ERROR: Release output not found. Checked:
  echo   "%SRC_DIR%\x64\release"
  echo   "%BUILD%\Release"
  pause
  exit /b 1
)

echo.
echo [8/8] Installing Smash Soda...
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
xcopy "%RELEASE_DIR%\*" "%INSTALL_DIR%\" /E /I /Y
if %errorlevel% neq 0 (
  echo ERROR: Failed to copy files to install directory
  pause
  exit /b 1
)
:after_install_app

:: --------- Migrate user data ----------
if "%STEP_MIGRATE_DATA%"=="0" (
  echo.
  echo Skipped migrate user data.
  goto :after_migrate_user_data
)
echo.
echo Checking for legacy Smash Soda appdata files...
set "OLD_APPDATA_DIR=%APPDATA%\SmashSodaTwo"
set "NEW_APPDATA_DIR=%APPDATA%\Trybuchet\Smash Soda"
if not exist "%NEW_APPDATA_DIR%" mkdir "%NEW_APPDATA_DIR%"

if exist "%OLD_APPDATA_DIR%" (
  for %%F in (
    banned.json
    banned_ip_addresses.json
    games.json
    preferences.json
    mods.json
    profiles.json
    session.json
    tiers.json
    vip.json
  ) do (
    if exist "%OLD_APPDATA_DIR%\%%F" (
      copy /Y "%OLD_APPDATA_DIR%\%%F" "%NEW_APPDATA_DIR%\%%F" >nul
      if !errorlevel! equ 0 (
        echo Migrated %%F
      ) else (
        echo WARNING: Failed to migrate %%F
      )
    )
  )
) else (
  echo No legacy appdata folder found at "%OLD_APPDATA_DIR%". Skipping migration.
)
:after_migrate_user_data

:: --------- Smash Glass Overlay ----------
set "OVERLAY_DIR=%INSTALL_DIR%\overlay"
if "%STEP_INSTALL_OVERLAY%"=="0" (
  echo.
  echo Skipped Smash Glass overlay install.
  goto :after_install_overlay
)
echo.
echo Installing Smash Glass overlay...
mkdir "%OVERLAY_DIR%" 2>nul
call :download "%SMASH_GLASS_URL%" "%OVERLAY_ZIP%" "Smash Glass"
%PS% -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%OVERLAY_ZIP%' -DestinationPath '%OVERLAY_DIR%' -Force"
del /f /q "%OVERLAY_ZIP%" >nul 2>&1
if not exist "%OVERLAY_DIR%\plugins" mkdir "%OVERLAY_DIR%\plugins"
if not exist "%OVERLAY_DIR%\themes" mkdir "%OVERLAY_DIR%\themes"

:: Unblock overlay files
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "Get-ChildItem -Path '%OVERLAY_DIR%' -Recurse | Unblock-File"
:after_install_overlay

echo.
echo ============================================================
echo INSTALLATION COMPLETE!
echo ============================================================
echo Smash Soda installed at: "%INSTALL_DIR%"
echo Overlay installed at: "%OVERLAY_DIR%"
echo ============================================================
echo.

if "%STEP_CREATE_SHORTCUTS%"=="0" (
  echo Skipped shortcut creation step.
) else (
  call :shortcuts
)

echo.
echo Cleaning up installer files...
if "%STEP_CLEANUP%"=="0" (
  echo Skipped cleanup step.
) else (
  rmdir /s /q "%WORK%"
)

echo.
echo Press any key to exit...
pause >nul
exit /b 0

:: --------- Optional shortcut ----------
:shortcuts
setlocal EnableExtensions DisableDelayedExpansion
set "APP_NAME=Smash Soda"
set "APP_EXE=%INSTALL_DIR%\SmashSoda.exe"
if not exist "%APP_EXE%" set "APP_EXE=%RELEASE_DIR%\SmashSoda.exe"

if not exist "%APP_EXE%" (
  echo WARNING: Could not find SmashSoda.exe to create a shortcut.
  exit /b 0
)

choice /C YN /N /M "Create desktop shortcut? [Y/N]: "
if errorlevel 2 (
  echo Skipped shortcut creation.
  exit /b 0
)

:: Resolve the current user's Desktop folder properly
for /f "usebackq tokens=*" %%D in (`%PS% -NoProfile -Command "[Environment]::GetFolderPath('Desktop')"`) do set "DESKTOP_PATH=%%D"

if not exist "%DESKTOP_PATH%" mkdir "%DESKTOP_PATH%"

:: Create shortcut
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "$w=New-Object -ComObject WScript.Shell;" ^
  "$d='%DESKTOP_PATH%\%APP_NAME%.lnk';" ^
  "$s=$w.CreateShortcut($d);" ^
  "$s.TargetPath='%APP_EXE%';" ^
  "$s.WorkingDirectory='%INSTALL_DIR%';" ^
  "$s.IconLocation='%APP_EXE%';" ^
  "$s.Save();"

echo Desktop shortcut created.
exit /b 0
