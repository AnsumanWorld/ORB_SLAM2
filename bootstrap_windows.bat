@echo off

set "OrbSlamPlatform=x64"
set "OrbSlamToolset=v141"
set "OrbSlamBuildType=Debug"

if NOT "%~1"=="" set "OrbSlamPlatform=%~1"
if NOT "%~2"=="" set "OrbSlamToolset=%~2"
if NOT "%~3"=="" set "OrbSlamBuildType=%~3" 

rem Locate vcpkg:
if defined VCPKG_ROOT_DIR if /i not "%VCPKG_ROOT_DIR%"=="" set "VcPkgDir=%VCPKG_ROOT_DIR%"
if not exist "%VcPkgDir%" set "VcPkgDir=%USERPROFILE%\.vcpkg\vcpkg"
if not exist "%VcPkgDir%" set "VcPkgDir=%USERPROFILE%\Software\vcpkg\vcpkg"
if not exist "%VcPkgDir%" set "VcPkgDir=%~d0\Software\vcpkg\vcpkg"

if not exist "%VcPkgDir%" echo vcpkg not found in your system, bailing out & exit /b 1

set "VcPkgTriplet=%OrbSlamPlatform%-windows-%OrbSlamToolset%"
if defined VCPKG_DEFAULT_TRIPLET if /i not "%VCPKG_DEFAULT_TRIPLET%"=="" set "VcpkgTriplet=%VCPKG_DEFAULT_TRIPLET%"
set "VcPkgPath=%VcPkgDir%\vcpkg.exe"

echo. & echo Bootstrapping dependencies for triplet: %VcPkgTriplet% & echo.

rem ==============================
rem Update and Install packages.
rem ==============================
call "%VcPkgDir%\bootstrap-vcpkg.bat"
call "%VcPkgPath%" update
call "%VcPkgPath%" remove --outdated --recurse
call "%VcPkgPath%" install eigen3 opencv pangolin --triplet %VcPkgTriplet%

set "VcPkgTripletDir=%VcPkgDir%\installed\%VcPkgTriplet%"

if not exist "%VcPkgTripletDir%" echo %VcPkgTripletDir% does not exist, bailing out & exit /b 1

set "CMAKE_PREFIX_PATH=%VcPkgTripletDir%;%CMAKE_PREFIX_PATH%"

echo. & echo Bootstrapping successful for triplet: %VcPkgTriplet% & echo CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% & echo.
