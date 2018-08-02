@echo off

setlocal
    call "%~dp0bootstrap.bat" %* || exit /b 1
    
    set "CMakeGeneratorName=Visual Studio 15 2017 Win64"
    
    if "%Platform%"=="x86" (
        if "%Toolset%"=="v140" set "CMakeGeneratorName=Visual Studio 14 2015"
        if "%Toolset%"=="v141" set "CMakeGeneratorName=Visual Studio 15 2017"
    )
    
    if "%Platform%"=="x64" (
        if "%Toolset%"=="v140" set "CMakeGeneratorName=Visual Studio 14 2015 Win64"
        if "%Toolset%"=="v141" set "CMakeGeneratorName=Visual Studio 15 2017 Win64"
    )

    set "BuildDir=%~dp0..\..\products\%Platform%-windows"
    set "InstallDir=%~dp0..\..\installed\%Platform%-windows"

    if not exist "%BuildDir%" mkdir "%BuildDir%

    ( pushd "%BuildDir%" ^
   && call cmake.exe -G "%CMakeGeneratorName%" ^
    -DCMAKE_INSTALL_PREFIX="%InstallDir%" ^
    -DVCPKG_TARGET_TRIPLET=%VcPkgTriplet% ^
    -DCMAKE_TOOLCHAIN_FILE="%VcPkgDir%\scripts\buildsystems\vcpkg.cmake" ^
    -DG2O_ROOT="%VcPkgDir%\installed\%VcPkgTriplet%" ^
    -DBUILD_ALL_EXAMPLES=OFF ^
    -DWARNINGS=ON ^
    -DBUILD_EXAMPLES=ON ^
    -DBUILD_TOOLS=OFF ^
    -DBUILD_EXPERIMENTS=OFF ^
    -DPRINT_CMAKE_VARIABLES=OFF ^
    -DCOTIRE_MINIMUM_NUMBER_OF_TARGET_SOURCES=1 ^
    "%~dp0..\.." ^
   && popd ) || exit /b 1
endlocal & set "BuildDir=%BuildDir%" & set "BuildType=%BuildType%" & exit /b %errorlevel%