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
    exit /b 1
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
set "BRANCH=master"
set "REPO_URL=https://github.com/trybuchet/smash-soda.git"
set "SMASH_GLASS_VERSION=%~1"
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
set "GIT_SETUP=%WORK%\Git-64-bit.exe"
set "CMAKE_SETUP=%WORK%\cmake-windows-x86_64.msi"

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

:: --------- Helper: download latest GitHub release asset by regex ----------
:download_latest_github_asset
set "GH_REPO=%~1"
set "GH_REGEX=%~2"
set "GH_DEST=%~3"
set "GH_LABEL=%~4"
if "%GH_REPO%"=="" (
  echo ERROR: GitHub repo is empty for "%GH_LABEL%".
  exit /b 1
)
if "%GH_REGEX%"=="" (
  echo ERROR: GitHub asset regex is empty for "%GH_LABEL%".
  exit /b 1
)
if "%GH_DEST%"=="" (
  echo ERROR: Destination is empty for "%GH_LABEL%".
  exit /b 1
)
echo Downloading %GH_LABEL%...
%PS% -NoProfile -ExecutionPolicy Bypass -Command ^
  "$repo='%GH_REPO%'; $regex='%GH_REGEX%'; $dest='%GH_DEST%'; " ^
  "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; " ^
  "try { " ^
  "  $api = 'https://api.github.com/repos/' + $repo + '/releases/latest'; " ^
  "  $rel = Invoke-RestMethod -Uri $api -Headers @{ 'User-Agent'='SmashSodaInstaller' }; " ^
  "  $asset = $rel.assets | Where-Object { $_.name -match $regex } | Select-Object -First 1; " ^
  "  if (-not $asset) { throw 'No matching asset found in latest release.' } " ^
  "  Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $dest -UseBasicParsing; " ^
  "} catch { " ^
  "  Write-Host 'Download failed:'; " ^
  "  Write-Host $_.Exception.Message; " ^
  "  exit 1; " ^
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
  if !errorlevel! equ 0 (
    echo Using winget to install Git...
    winget install -e --id Git.Git --silent --accept-package-agreements --accept-source-agreements
    set "WINGET_GIT_RC=!errorlevel!"
    if not "!WINGET_GIT_RC!"=="0" if not "!WINGET_GIT_RC!"=="3010" (
      echo WARNING: winget Git install failed with exit code !WINGET_GIT_RC!.
      echo Falling back to direct Git installer download...
      call :download_latest_github_asset "git-for-windows/git" "^Git-.*-64-bit\.exe$" "%GIT_SETUP%" "Git for Windows"
      if errorlevel 1 (
        echo ERROR: Failed to download Git installer.
        pause
        exit /b 1
      )
      start "" /wait "%GIT_SETUP%" /VERYSILENT /NORESTART /NOCANCEL /SP-
      if errorlevel 1 (
        echo ERROR: Git installer failed.
        pause
        exit /b 1
      )
    )
  ) else (
    echo winget not found. Falling back to direct Git installer download...
    call :download_latest_github_asset "git-for-windows/git" "^Git-.*-64-bit\.exe$" "%GIT_SETUP%" "Git for Windows"
    if errorlevel 1 (
      echo ERROR: Failed to download Git installer.
      pause
      exit /b 1
    )
    start "" /wait "%GIT_SETUP%" /VERYSILENT /NORESTART /NOCANCEL /SP-
    if errorlevel 1 (
      echo ERROR: Git installer failed.
      pause
      exit /b 1
    )
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

:: --------- Install Build Tools ----------
echo.
echo [2/7] Installing Visual Studio Build Tools (C++ toolchain)...

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
  if !errorlevel! equ 0 (
    echo Using winget to install Visual Studio Build Tools 2026 with ATL support...
    winget install -e --id Microsoft.VisualStudio.BuildTools ^
      --override "--wait --quiet --norestart --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.VC.ATL --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --includeRecommended"
    set "WINGET_RC=!errorlevel!"
    if not "!WINGET_RC!"=="0" if not "!WINGET_RC!"=="3010" (
      echo WARNING: winget install failed with exit code !WINGET_RC!.
      echo Falling back to direct Visual Studio Build Tools installer...
      call :download "%VS_BUILD_TOOLS_URL%" "%VS_BOOTSTRAP%" "Visual Studio Build Tools"
      if errorlevel 1 (
        echo ERROR: Failed to download Visual Studio Build Tools installer.
        pause
        exit /b 1
      )

      echo Running Visual Studio Build Tools installer with ATL support...
      "%VS_BOOTSTRAP%" --wait --quiet --norestart ^
        --add Microsoft.VisualStudio.Workload.VCTools ^
        --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
        --add Microsoft.VisualStudio.Component.VC.ATL ^
        --add Microsoft.VisualStudio.Component.Windows11SDK.22621 ^
        --includeRecommended
      set "VSBT_RC=!errorlevel!"
      if not "!VSBT_RC!"=="0" if not "!VSBT_RC!"=="3010" (
        echo ERROR: Visual Studio Build Tools installer failed with exit code !VSBT_RC!.
        pause
        exit /b 1
      )
    )
  ) else (
    echo Downloading Visual Studio Build Tools installer...
    call :download "%VS_BUILD_TOOLS_URL%" "%VS_BOOTSTRAP%" "Visual Studio Build Tools"
    if errorlevel 1 (
      echo ERROR: Failed to download Visual Studio Build Tools installer.
      pause
      exit /b 1
    )
    
    echo Running Visual Studio Build Tools installer with ATL support...
    "%VS_BOOTSTRAP%" --wait --quiet --norestart ^
      --add Microsoft.VisualStudio.Workload.VCTools ^
      --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
      --add Microsoft.VisualStudio.Component.VC.ATL ^
      --add Microsoft.VisualStudio.Component.Windows11SDK.22621 ^
      --includeRecommended
    set "VSBT_RC=!errorlevel!"
    if not "!VSBT_RC!"=="0" if not "!VSBT_RC!"=="3010" (
      echo ERROR: Visual Studio Build Tools installer failed with exit code !VSBT_RC!.
      pause
      exit /b 1
    )
  )

  echo Waiting for installation to complete...
  timeout /t 10 >nul

  :: Wait for vswhere
  set WAIT_COUNT=0
  :wait_vs
  if !WAIT_COUNT! GEQ 60 (
    echo.
    echo WARNING: Visual Studio Build Tools detection timed out.
    echo Continuing anyway. If build fails, reboot and rerun updater.
    goto :after_install_build_tools
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

:: Initialize the VS environment
echo.
echo Initializing Visual Studio development environment...
if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" (
  call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
  if !errorlevel! neq 0 (
    echo WARNING: Failed to initialize Visual Studio environment.
    echo Continuing anyway. If build fails, reboot and rerun updater.
    goto :after_install_build_tools
  )
) else (
  echo WARNING: VsDevCmd.bat not found at expected location:
  echo "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
  echo Continuing anyway. If build fails, reboot and rerun updater.
  goto :after_install_build_tools
)

echo C++ build environment is ready.
:after_install_build_tools

:: --------- Install CMake ----------
echo.
echo [3/7] Installing CMake...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
  where winget >nul 2>&1
  if !errorlevel! equ 0 (
    echo Using winget to install CMake...
    winget install -e --id Kitware.CMake --silent --accept-package-agreements --accept-source-agreements
    set "WINGET_CMAKE_RC=!errorlevel!"
    if not "!WINGET_CMAKE_RC!"=="0" if not "!WINGET_CMAKE_RC!"=="3010" (
      echo WARNING: winget CMake install failed with exit code !WINGET_CMAKE_RC!.
      echo Falling back to direct CMake installer download...
      call :download_latest_github_asset "Kitware/CMake" "^cmake-.*-windows-x86_64\.msi$" "%CMAKE_SETUP%" "CMake"
      if errorlevel 1 (
        echo ERROR: Failed to download CMake installer.
        pause
        exit /b 1
      )
      msiexec /i "%CMAKE_SETUP%" /qn /norestart ADD_CMAKE_TO_PATH=System
      if errorlevel 1 (
        echo ERROR: CMake installer failed.
        pause
        exit /b 1
      )
    )
    timeout /t 5 >nul
  ) else (
    echo winget not found. Falling back to direct CMake installer download...
    call :download_latest_github_asset "Kitware/CMake" "^cmake-.*-windows-x86_64\.msi$" "%CMAKE_SETUP%" "CMake"
    if errorlevel 1 (
      echo ERROR: Failed to download CMake installer.
      pause
      exit /b 1
    )
    msiexec /i "%CMAKE_SETUP%" /qn /norestart ADD_CMAKE_TO_PATH=System
    if errorlevel 1 (
      echo ERROR: CMake installer failed.
      pause
      exit /b 1
    )
    timeout /t 5 >nul
  )
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

:: --------- Clone source ----------
echo.
echo [4/7] Cloning Smash Soda (%BRANCH% branch)...
set "SRC_DIR=%SRC%\smash-soda"
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

:: --------- Build ----------
echo.
echo [5/7] Configuring CMake...
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
echo [6/7] Building Release...
mkdir "%SRC_DIR%\x64\release" 2>nul
cmake --build "%BUILD%" --config Release
if %errorlevel% neq 0 (
  echo ERROR: Build failed
  pause
  exit /b 1
)

:: --------- Install ----------
set "RELEASE_DIR="
if exist "%SRC_DIR%\x64\release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\x64\release"
if not defined RELEASE_DIR if exist "%SRC_DIR%\x64\Release\SmashSoda.exe" set "RELEASE_DIR=%SRC_DIR%\x64\Release"
if not defined RELEASE_DIR if exist "%BUILD%\Release\SmashSoda.exe" set "RELEASE_DIR=%BUILD%\Release"
if not defined RELEASE_DIR if exist "%BUILD%\SmashSoda\Release\SmashSoda.exe" set "RELEASE_DIR=%BUILD%\SmashSoda\Release"
if not defined RELEASE_DIR (
  for /f "delims=" %%F in ('dir /b /s "%BUILD%\SmashSoda.exe" 2^>nul') do (
    if not defined RELEASE_DIR set "RELEASE_DIR=%%~dpF"
  )
)
if not defined RELEASE_DIR (
  echo ERROR: Release output not found. Checked:
  echo   "%SRC_DIR%\x64\release\SmashSoda.exe"
  echo   "%SRC_DIR%\x64\Release\SmashSoda.exe"
  echo   "%BUILD%\Release\SmashSoda.exe"
  echo   "%BUILD%\SmashSoda\Release\SmashSoda.exe"
  pause
  exit /b 1
)

echo.
echo [7/7] Installing Smash Soda...
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
xcopy "%RELEASE_DIR%\*" "%INSTALL_DIR%\" /E /I /Y
if %errorlevel% neq 0 (
  echo ERROR: Failed to copy files to install directory
  pause
  exit /b 1
)

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
  if errorlevel 1 (
    echo ERROR: Failed to download Smash Glass overlay.
    pause
    exit /b 1
  )
  if not exist "%OVERLAY_ZIP%" (
    echo ERROR: Overlay archive not found after download: "%OVERLAY_ZIP%"
    pause
    exit /b 1
  )
  %PS% -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%OVERLAY_ZIP%' -DestinationPath '!OVERLAY_EXTRACT!' -Force"
  if errorlevel 1 (
    echo ERROR: Failed to extract Smash Glass overlay archive.
    pause
    exit /b 1
  )
  del /f /q "%OVERLAY_ZIP%" >nul 2>&1

  :: Copy overlay files but preserve user's plugins and themes folders
  robocopy "!OVERLAY_EXTRACT!" "!OVERLAY_DIR!" /E /XO /NFL /NDL /NJH /NJS /NP /XD "plugins" "themes" >nul
  if !errorlevel! GEQ 8 (
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
