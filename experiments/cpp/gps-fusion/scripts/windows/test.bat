@echo off

setlocal

set "conan_dir=%~d0\git\conan4vcpkg"
set "project_dir=%~dp0..\.."

set "build_type=Release"
cd /d "%project_dir%"

set "install_dir=%conan_dir%\installed\x64-windows-v141"
set "build_dir=products\windows"

set "proj_dll_path=%build_dir%\g2o_custom\%build_type%"
set "path=%install_dir%\bin;%install_dir%\debug\bin;%proj_dll_path%;%path%"

set "app=%build_dir%\tests\%build_type%\tests-fuser.exe"

%app%

endlocal
