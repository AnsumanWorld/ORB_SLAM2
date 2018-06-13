@echo off

setlocal

set "vcpkg_dir=%~d0\git\conan4vcpkg"
set "project_dir=%~dp0..\.."

set "build_type=Release"
cd /d "%project_dir%"

set "install_dir=%vcpkg_dir%\installed\x64-windows-v141"
set "build_dir=products\windows"

set "gps_fusion_dir=%project_dir%\%build_dir%"
set "proj_dll_path=%gps_fusion_dir%\g2o_custom\%build_type%"
set "path=%install_dir%\bin;%install_dir%\debug\bin;%proj_dll_path%;%path%"

set "app=%gps_fusion_dir%\apps\%build_type%\app-gps-ba.exe"
set "cfg=%project_dir%\cfg\config.json"

%app% %cfg%

endlocal
