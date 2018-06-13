@echo off

setlocal

set "vcpkg_dir=%~d0\git\conan4vcpkg"
set "project_dir=%~dp0..\.."
set "build_type=Release"
set "build_dir=%project_dir%\products\windows"
set "install_dir=%vcpkg_dir%\installed\x64-windows-v141"

if not exist "%install_dir%" echo %install_dir% does not exist, bailing out & exit /b 1

set "CMAKE_PREFIX_PATH=%install_dir%\debug;%install_dir%;%CMAKE_PREFIX_PATH%"
set "CMAKE_LIBRARY_PATH=%install_dir%\debug\lib;%install_dir%\debug\lib\manual-link;%install_dir%\lib;%install_dir%\lib\manual-link;%CMAKE_LIBRARY_PATH%"
set "generator=Visual Studio 15 2017 Win64"

if not exist "%build_dir%" mkdir "%build_dir%"
cd "%build_dir%"

cmake -G "%generator%" -DCMAKE_BUILD_TYPE=%build_type% -DUSE_COTIRE=OFF "%project_dir%"
cd /d %build_dir%
cmake --build . --config %build_type%

endlocal
