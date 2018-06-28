@echo off

setlocal

REM pip install evo --upgrade

set "Config=Release"
set "VcpkgDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows"
set "ProjectDir=%~dp0..\..\.."
set "Path=%VcpkgDir%\bin;%VcpkgDir%\debug\bin;C:\Program Files\7-Zip;%Path%"
set "ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64.v141"
set "Path=%ProductsDir%\%Config%\bin;%Path%"

cd /d "%ProjectDir%"

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set "App=%ProductsDir%\%Config%\bin\kitti-runner.exe"
set "Vocabulary=%ProjectDir%\Vocabulary\ORBvoc.bin"

REM set "Settings=%ProjectDir%\Examples\Monocular\using-gps\KITTI00-02.yaml"
REM set "Settings=%ProjectDir%\Examples\Monocular\using-gps\KITTI03.yaml"
set "Settings=%ProjectDir%\Examples\Monocular\using-gps\KITTI04-12.yaml"

set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/04/image_0
set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/04.txt

set "Option=gps"
set "Output=KeyFrameTrajectoryKITTI.txt"
%App% %Option% %Vocabulary% %Settings% %ImagesDir% %SensorSource% %Output% 99999 99999

endlocal