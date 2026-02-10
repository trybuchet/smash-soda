@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Smash Soda Updater (Preview)

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
        -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    ) else (
      %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -ExecutionPolicy Bypass ^
        -Command "Start-Process -FilePath '%~f0' -Verb RunAs -ArgumentList '%*'"
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
echo Smash Soda Updater

:: --------- Config ----------
set "BRANCH=preview"
set "REPO_URL=https://github.com/trybuchet/smash-soda.git"
set "OVERLAY_REPO=https://github.com/trybuchet/smash-soda-overlay"

:: VS Build Tools 2026 bootstrapper (fallback if winget is missing)
set "VS_BUILD_TOOLS_URL=https://download.visualstudio.microsoft.com/download/pr/6efb3484-905b-485c-8b5f-9d3a5f39e731/07908cd6d91e75b8ea4339d8f2cfa6e8d8bb4fd706af7b918ae391cd6fc2a066/vs_BuildTools.exe"

:: Git for Windows (fallback if winget is missing)
set "GIT_SETUP_URL=https://github.com/git-for-windows/git/releases/latest/download/Git-64-bit.exe"

:: CMake (latest stable)
set "CMAKE_VERSION=4.2.3"
set "CMAKE_MSI_URL=https://cmake.org/files/v4.2/cmake-%CMAKE_VERSION%-windows-x86_64.msi"

:: CMake generator for VS 2026
set "VS_GENERATOR=Visual Studio 18 2026"

:: Optional overlay version argument
set "OVERLAY_VERSION=%~1"

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
set "VS_BOOTSTRAP=%WORK%\vs_BuildTools.exe"
set "CMAKE_MSI=%WORK%\cmake-%CMAKE_VERSION%.msi"
set "GIT_SETUP=%WORK%\Git-64-bit.exe"

if exist "%WORK%" rmdir /s /q "%WORK%"
if exist "%BUILD%" rmdir /s /q "%BUILD%"
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
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "$u='%URL%'; $d='%DEST%'; $l='%LABEL%'; " ^
  "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; " ^
  "try { " ^
  "  Write-Progress -Activity $l -Status 'Downloading...' -PercentComplete 0; " ^
  "  Invoke-WebRequest -Uri $u -OutFile $d -UseBasicParsing; " ^
  "  Write-Progress -Activity $l -Completed; " ^
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
echo [1/7] Installing Git...
where git >nul 2>&1
if %errorlevel% neq 0 (
  where winget >nul 2>&1
  if %errorlevel% equ 0 (
    winget install -e --id Git.Git --silent --accept-package-agreements --accept-source-agreements
  ) else (
    call :download "%GIT_SETUP_URL%" "%GIT_SETUP%" "Git for Windows"
    "%GIT_SETUP%" /VERYSILENT /NORESTART /SP- /SUPPRESSMSGBOXES
  )
)
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
  exit /b 1
)

:: --------- Install Build Tools ----------
echo.
echo [2/7] Installing Visual Studio Build Tools (C++ toolchain)...
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
)
if defined VSINSTALL (
  echo Visual Studio Build Tools already installed at:
  echo "%VSINSTALL%"
) else (
  where cl >nul 2>&1
  if %errorlevel% neq 0 (
    where winget >nul 2>&1
    if %errorlevel% equ 0 (
      winget install -e --id Microsoft.VisualStudio.BuildTools --silent --accept-package-agreements --accept-source-agreements
    ) else (
      call :download "%VS_BUILD_TOOLS_URL%" "%VS_BOOTSTRAP%" "Visual Studio Build Tools"
      if not exist "%VS_BOOTSTRAP%" (
        echo ERROR: Build Tools installer was not downloaded.
        exit /b 1
      )
      "%VS_BOOTSTRAP%" --quiet --wait --norestart --nocache --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended
    )
  )
)

:: --------- Install CMake ----------
echo.
echo [3/7] Installing CMake %CMAKE_VERSION%...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
  call :download "%CMAKE_MSI_URL%" "%CMAKE_MSI%" "CMake %CMAKE_VERSION%"
  if not exist "%CMAKE_MSI%" (
    echo ERROR: CMake installer was not downloaded.
    exit /b 1
  )
  msiexec /i "%CMAKE_MSI%" /qn /norestart ADD_CMAKE_TO_PATH=System
)

:: --------- Pull source ----------
echo.
echo [4/6] Pulling Smash Soda (%BRANCH% branch)...
set "SRC_DIR=%SRC%\smash-soda"
if exist "%SRC_DIR%" rmdir /s /q "%SRC_DIR%"
git clone --branch "%BRANCH%" --single-branch "%REPO_URL%" "%SRC_DIR%"
if %errorlevel% neq 0 exit /b 1

:: --------- Set VS environment (if available) ----------
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
  if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
)

:: --------- Build ----------
echo.
echo [5/6] Configuring CMake...
cmake -S "%SRC_DIR%" -B "%BUILD%" -G "%VS_GENERATOR%" -A x64
if %errorlevel% neq 0 exit /b 1

echo.
echo [6/6] Building Release...
mkdir "%SRC_DIR%\x64\release" 2>nul
cmake --build "%BUILD%" --config Release
if %errorlevel% neq 0 exit /b 1

:: --------- Install ----------
set "RELEASE_DIR=%SRC_DIR%\x64\release"
if not exist "%RELEASE_DIR%" (
  echo ERROR: Release output not found: "%RELEASE_DIR%"
  exit /b 1
)

echo.
echo Replacing install folder contents...
if exist "%INSTALL_DIR%" (
  for /f "delims=" %%i in ('dir /b "%INSTALL_DIR%" 2^>nul') do (
    if /I "%%i"=="%~nx0" (
      rem Skip deleting the updater itself while it is running
    ) else (
      if exist "%INSTALL_DIR%\%%i\" (
        rmdir /s /q "%INSTALL_DIR%\%%i"
      ) else (
        del /f /q "%INSTALL_DIR%\%%i" >nul 2>&1
      )
    )
  )
) else (
  mkdir "%INSTALL_DIR%"
)
echo Installing updated Smash Soda...
xcopy "%RELEASE_DIR%\*" "%INSTALL_DIR%\" /E /I /Y >nul

:: --------- Optional overlay update ----------
if not "%OVERLAY_VERSION%"=="" (
  echo.
  echo Updating Smash Glass overlay to version %OVERLAY_VERSION%...
  set "SMASH_GLASS_URL=%OVERLAY_REPO%/releases/download/v%OVERLAY_VERSION%/smash-glass.zip"
  set "OVERLAY_DIR=%INSTALL_DIR%\overlay"
  mkdir "%OVERLAY_DIR%" 2>nul
  call :download "%SMASH_GLASS_URL%" "%OVERLAY_ZIP%" "Smash Glass"
  %PS% -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%OVERLAY_ZIP%' -DestinationPath '%OVERLAY_DIR%' -Force"
  del /f /q "%OVERLAY_ZIP%" >nul 2>&1
  %PS% -NoProfile -ExecutionPolicy Bypass -Command ^
    "Get-ChildItem -Path '%OVERLAY_DIR%' -Recurse | Unblock-File"
)

echo.
echo DONE.
echo Smash Soda updated at: "%INSTALL_DIR%"
if not "%OVERLAY_VERSION%"=="" echo Overlay updated to v%OVERLAY_VERSION% at: "%INSTALL_DIR%\overlay"

echo Cleaning up updater files...
rmdir /s /q "%WORK%"
exit /b 0
