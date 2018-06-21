@echo off

setlocal

set "CMakeGeneratorName=Visual Studio 15 2017 Win64"
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"
set "VcPkgDir=%USERPROFILE%\.vcpkg\vcpkg"
set "VcPkgTriplet=%Platform%-windows"
set "BuildDir=%~dp0..\..\products\cmake.msbuild.windows.%Platform%.%Toolset%"
set "InstallDir=%~dp0..\..\install\windows.%Platform%.%Toolset%"

if not exist "%BuildDir%" mkdir "%BuildDir%"
if errorlevel 1 echo unable to create "%BuildDir%", bailing out & exit /b 1
pushd "%BuildDir%"

cmake -G "%CMakeGeneratorName%" ^
    -DCMAKE_INSTALL_PREFIX="%InstallDir%" ^
    -DVCPKG_TARGET_TRIPLET=%VcPkgTriplet% ^
    -DCMAKE_TOOLCHAIN_FILE="%VcPkgDir%\scripts\buildsystems\vcpkg.cmake" ^
    -DBUILD_TESTING=ON ^
    "%~dp0..\.."

cmake --build "%BuildDir%" --config %BuildType%

popd
endlocal
