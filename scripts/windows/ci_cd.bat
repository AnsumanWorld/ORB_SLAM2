@echo off

setlocal EnableDelayedExpansion

REM set "VcpkgUrl=https://www.dropbox.com/s/6b788vd6ox1f5iq/orb_slam2_vcpkg.7z?dl=1"
set "VcpkgUrl=http://localhost:12345/orb_slam2_vcpkg.7z"
set "VcpkgExport=C:\orb_slam2_vcpkg.7z"

if not exist "%VcpkgExport%" (
    curl --output "%VcpkgExport%" --location "%VcpkgUrl%"
)

set "ExtractTo=C:"
set "ExtToolsBinDir=C:\Program Files\7-Zip"
set "Path=%ExtToolsBinDir%;%Path%"
set "VcpkgDir=%ExtractTo%\orb_slam2_vcpkg"
if not exist "%VcpkgDir%" (
    7z x "%VcpkgExport%" -o"%ExtractTo%"
)

set "ProjectDir=%~dp0..\.."
cd /d "%ProjectDir%"

set "CMakeGeneratorName=Visual Studio 15 2017 Win64"
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"
set "VcPkgTriplet=%Platform%-windows"
set "BuildDir=%~dp0..\..\products\cmake.msbuild.windows.%Platform%.%Toolset%"
set "InstallDir=%~dp0..\..\install\windows.%Platform%.%Toolset%"

if not exist "%BuildDir%" mkdir "%BuildDir%"

rem ==============================
rem temporary hack to copy opencv_ffmpeg dlls until this is solved in vcpkg
rem ==============================
if not exist "%~dp0..\..\products" mkdir "%~dp0..\..\products"
echo xcopy "%VcPkgDir%\installed\%VcPkgTriplet%\bin\opencv_ffmpeg*.dll" "%%~1/" /y /f > "%~dp0..\..\products\copy_ffmpeg.bat"
echo xcopy "%VcPkgDir%\installed\%VcPkgTriplet%\debug\bin\opencv_ffmpeg*.dll" "%%~1/" /y /f > "%~dp0..\..\products\copy_ffmpegd.bat"

pushd "%BuildDir%"

call cmake.exe -G "%CMakeGeneratorName%" ^
-DCMAKE_INSTALL_PREFIX="%InstallDir%" ^
-DVCPKG_TARGET_TRIPLET="%VcPkgTriplet%" ^
-DG2O_ROOT="%VcPkgDir%\installed\%VcPkgTriplet%" ^
-DCMAKE_TOOLCHAIN_FILE="%VcPkgDir%\scripts\buildsystems\vcpkg.cmake" ^
-DBUILD_ALL_EXAMPLES=ON ^
-DWARNINGS=OFF ^
-DBUILD_EXAMPLES=ON ^
-DBUILD_TOOLS=ON ^
-DBUILD_EXPERIMENTS=ON ^
-DBUILD_TESTING=ON ^
-DPRINT_CMAKE_VARIABLES=OFF ^
"%~dp0..\.."

call cmake.exe --build "!BuildDir!" --config !BuildType! --target install

popd

endlocal
