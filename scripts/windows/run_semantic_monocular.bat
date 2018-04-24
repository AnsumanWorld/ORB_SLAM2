@echo off

setlocal
set "OrbSlam2_Platform=x64"
set "OrbSlam2_Toolset=v141"
set "OrbSlam2_VcpkgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\%OrbSlam2_Platform%-windows\bin"
set "OrbSlam2_ProjectDir=%~dp0..\.."
set "Path=%Path%;%OrbSlam2_VcpkgBinDir%"
set "OrbSlam2_ProductsDir=%OrbSlam2_ProjectDir%\products\cmake.msbuild.windows.%OrbSlam2_Platform%.%OrbSlam2_Toolset%"

pushd "%OrbSlam2_ProjectDir%"

if not exist "%OrbSlam2_ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%OrbSlam2_ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%OrbSlam2_ProjectDir%\Vocabulary"
    7z x "%OrbSlam2_ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%OrbSlam2_ProjectDir%\Vocabulary"
    del /f /q "%OrbSlam2_ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set "OrbSlam2_App=%OrbSlam2_ProductsDir%\Release\semantic_monocular.exe"
set "OrbSlam2_Vocabulary=Vocabulary\ORBvoc.bin"
set "OrbSlam2_Settings=Examples\Monocular\Garching-Test-Drive.yaml"
set "OrbSlam2_Images=data\Garching_LoopClosure-3-images"
set "OrbSlam2_SensorReadings=data\Garching_LoopClosure-3-features.json"
set "OrbSlam2_Use_org_local_maaping=OFF"
call "%OrbSlam2_App%" "%OrbSlam2_Vocabulary%" "%OrbSlam2_Settings%" "%OrbSlam2_Images%" "%OrbSlam2_SensorReadings%"  "%OrbSlam2_Use_org_local_maaping%"

popd

endlocal

