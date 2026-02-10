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
    echo Relaunch requested. This window will close in 10 seconds.
    timeout /t 10 >nul
    exit /b 1
  ) else (
    echo UAC was cancelled or elevation failed.
    echo Please right-click the installer and choose "Run as administrator".
    echo.
    pause >nul
  )
)

echo.
echo ==========================================================
echo Smash Soda Installer (Preview)
echo ==========================================================
echo Smash Soda is an unofficial tool for hosting rooms on the
echo Parsec service, with greater control over guests and
echo gamepad management. It is NOT affiliated with Parsec.
echo.
echo This installer will download build tools and compile
echo Smash Soda from source (required due to SDK licensing).
echo.
echo Recommended free disk space: at least 15 GB.
echo ==========================================================
echo.
echo Press any key to continue...
pause >nul

:: --------- Config ----------
set "BRANCH=preview"
set "REPO_URL=https://github.com/trybuchet/smash-soda.git"
set "SMASH_GLASS_URL=https://github.com/trybuchet/smash-soda-overlay/releases/download/4.0.0/smash-glass.zip"

:: VS Build Tools 2026 bootstrapper (fallback if winget is missing)
set "VS_BUILD_TOOLS_URL=https://download.visualstudio.microsoft.com/download/pr/6efb3484-905b-485c-8b5f-9d3a5f39e731/07908cd6d91e75b8ea4339d8f2cfa6e8d8bb4fd706af7b918ae391cd6fc2a066/vs_BuildTools.exe"

:: Git for Windows (fallback if winget is missing)
set "GIT_SETUP_URL=https://github.com/git-for-windows/git/releases/latest/download/Git-64-bit.exe"

:: CMake (latest stable)
set "CMAKE_VERSION=4.2.3"
set "CMAKE_MSI_URL=https://cmake.org/files/v4.2/cmake-%CMAKE_VERSION%-windows-x86_64.msi"

:: CMake generator for VS 2026
set "VS_GENERATOR=Visual Studio 18 2026"

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

:: --------- Clone source ----------
echo.
echo [4/7] Cloning Smash Soda (%BRANCH% branch)...
set "SRC_DIR=%SRC%\smash-soda"
if exist "%SRC_DIR%" rmdir /s /q "%SRC_DIR%"
git clone --branch "%BRANCH%" --single-branch "%REPO_URL%" "%SRC_DIR%"
if %errorlevel% neq 0 exit /b 1
set "DEPS_DIR=%SRC_DIR%\dependencies"
if not exist "%DEPS_DIR%" set "DEPS_DIR=%SRC_DIR%\Dependencies"

:: --------- Set VS environment (if available) ----------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
  if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
)

:: --------- Build ----------
echo.
echo [5/7] Configuring CMake...
cmake -S "%SRC_DIR%" -B "%BUILD%" -G "%VS_GENERATOR%" -A x64
if %errorlevel% neq 0 exit /b 1

echo.
echo [6/7] Building Release...
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
echo [7/7] Installing Smash Soda...
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
xcopy "%RELEASE_DIR%\*" "%INSTALL_DIR%\" /E /I /Y >nul

:: --------- Smash Glass Overlay ----------
echo.
echo Installing Smash Glass overlay...
set "OVERLAY_DIR=%INSTALL_DIR%\overlay"
mkdir "%OVERLAY_DIR%" 2>nul
call :download "%SMASH_GLASS_URL%" "%OVERLAY_ZIP%" "Smash Glass"
%PS% -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%OVERLAY_ZIP%' -DestinationPath '%OVERLAY_DIR%' -Force"
del /f /q "%OVERLAY_ZIP%" >nul 2>&1

:: Unblock overlay files
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "Get-ChildItem -Path '%OVERLAY_DIR%' -Recurse | Unblock-File"

echo.
echo DONE.
echo Smash Soda installed at: "%INSTALL_DIR%"
echo Overlay installed at: "%OVERLAY_DIR%"
echo.

:: --------- Optional shortcut ----------
set "APP_NAME=Smash Soda"
set "APP_EXE=%INSTALL_DIR%\SmashSoda.exe"
if not exist "%APP_EXE%" set "APP_EXE=%RELEASE_DIR%\SmashSoda.exe"

if exist "%APP_EXE%" (
  set /p "MAKE_SHORTCUT=Create desktop shortcut and Start Menu entry? [Y/N]: "
  if /i "%MAKE_SHORTCUT%"=="Y" (
    %PS% -NoProfile -ExecutionPolicy Bypass -Command ^
      "$w=New-Object -ComObject WScript.Shell;" ^
      "$d=$env:USERPROFILE+'\Desktop\%APP_NAME%.lnk';" ^
      "$s=$w.CreateShortcut($d);" ^
      "$s.TargetPath='%APP_EXE%';" ^
      "$s.WorkingDirectory='%INSTALL_DIR%';" ^
      "$s.IconLocation='%APP_EXE%';" ^
      "$s.Save();"
    %PS% -NoProfile -ExecutionPolicy Bypass -Command ^
      "$w=New-Object -ComObject WScript.Shell;" ^
      "$folder=$env:ProgramData+'\Microsoft\Windows\Start Menu\Programs\Trybuchet';" ^
      "New-Item -ItemType Directory -Path $folder -Force | Out-Null;" ^
      "$p=$folder+'\%APP_NAME%.lnk';" ^
      "$s=$w.CreateShortcut($p);" ^
      "$s.TargetPath='%APP_EXE%';" ^
      "$s.WorkingDirectory='%INSTALL_DIR%';" ^
      "$s.IconLocation='%APP_EXE%';" ^
      "$s.Save();"
    echo Shortcuts created on Desktop and Start Menu (Trybuchet).
  ) else (
    echo Skipped shortcut creation.
  )
) else (
  echo WARNING: Could not find SmashSoda.exe to create a shortcut.
)

exit /b 0
