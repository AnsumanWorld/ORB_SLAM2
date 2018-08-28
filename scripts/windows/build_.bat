@echo off

setlocal

set "VcPkgDir=C:\Shanmukha\Shared\vcpkg-export-20180816-183850"
set "BuildType=RelWithDebInfo"

set "CMakeGenerator=Visual Studio 15 2017 Win64"
set "Platform=x64"
set "VcPkgTriplet=x64-windows"

set "BuildDir=%~dp0..\..\products\cmake.msbuild.windows.%Platform%"
set "InstallDir=%~dp0..\..\install\windows.%Platform%"

if not exist "%BuildDir%" mkdir "%BuildDir%"

pushd "%BuildDir%"
echo xcopy "%VcPkgDir%\installed\%VcPkgTriplet%\bin\opencv_ffmpeg*.dll" "%%~1/" /y /f > "%~dp0..\..\products\copy_ffmpeg.bat"
echo xcopy "%VcPkgDir%\installed\%VcPkgTriplet%\debug\bin\opencv_ffmpeg*.dll" "%%~1/" /y /f > "%~dp0..\..\products\copy_ffmpegd.bat"

cmake -G "%CMakeGenerator%" ^
-DCMAKE_INSTALL_PREFIX="%InstallDir%" ^
-DVCPKG_TARGET_TRIPLET=%VcPkgTriplet% ^
-DG2O_ROOT="%VcPkgDir%\installed\%VcPkgTriplet%" ^
-DCMAKE_TOOLCHAIN_FILE="%VcPkgDir%\scripts\buildsystems\vcpkg.cmake" ^
-DBUILD_ALL_EXAMPLES=ON ^
-DWARNINGS=ON ^
-DBUILD_EXAMPLES=ON ^
-DBUILD_TOOLS=ON ^
-DBUILD_EXPERIMENTS=ON ^
-DBUILD_TESTING=ON ^
-DPRINT_CMAKE_VARIABLES=OFF ^
"%~dp0..\.."

cmake --build "%BuildDir%" --config %BuildType%

popd
endlocal
