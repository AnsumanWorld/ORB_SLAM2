@echo off

setlocal

REM pip install evo --upgrade

set "Config=Release"
set "VcpkgDir=C:\Shanmukha\Shared\vcpkg-orb-slam2"
set "ProjectDir=%~dp0..\..\.."
set "Path=%VcpkgDir%\bin;%VcpkgDir%\debug\bin;C:\Program Files\7-Zip;%Path%"
set "ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64"
set "Path=%ProductsDir%\%Config%\bin;%Path%"

cd /d "%ProjectDir%"

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set "App=%ProductsDir%\%Config%\bin\monod-runner.exe"
set "Vocabulary=%ProjectDir%\Vocabulary\ORBvoc.bin"

set "Settings=%ProjectDir%\experiments\cpp\monod-runner\monod.yml"

set ImagesDir=C:\Shanmukha\Shared\Dataset\KITTI\data_odometry_gray\dataset\sequences\00\image_0
set DepthDir=C:\Shanmukha\Shared\Dataset\Depth\00
set SlamTraj=Trajectory.txt

%App% ^
    --frame_interval_ms=33 ^
    --images_dir=%ImagesDir% ^
    --depth_dir=%DepthDir% ^
    --output_traj_path=%SlamTraj% ^
    --settings_path=%Settings% ^
    --vocabulary_path=%Vocabulary% ^
    --wait_for_stdin=false

endlocal
