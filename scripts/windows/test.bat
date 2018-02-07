@echo off

setlocal
set "VcpkgBinDir=%~d0\git\conan4vcpkg\installed\x64-windows-v141\bin"
set "ProjectDir=%~dp0..\.."
set "Path=%Path%;%VcpkgBinDir%;C:\Program Files\7-Zip"
set "OrbSlam2ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64.v141"

cd %ProjectDir%

set TestApp="%OrbSlam2ProductsDir%\tests\Release\test-orb-slam2.exe"

%TestApp%

endlocal

