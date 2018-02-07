@echo off

setlocal

set "Config=Release"
set "VcpkgDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows"
set "ProjectDir=%~dp0..\.."
set "Path=%VcpkgDir%\bin;%VcpkgDir%\debug\bin;C:\Program Files\7-Zip;%Path%"
set "ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64.v141"
set "Path=%ProductsDir%\ext_g2o\%Config%;%Path%"

cd /d "%ProjectDir%"

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set "App=%ProductsDir%\experiments\kitti-runner\%Config%\kitti-runner.exe"
set "Vocabulary=%ProjectDir%\Vocabulary\ORBvoc.bin"

REM set "Settings=%ProjectDir%\Examples\Monocular\using-gps\KITTI00-02.yaml"
REM set "Settings=%ProjectDir%\Examples\Monocular\using-gps\KITTI03.yaml"
set "Settings=%ProjectDir%\Examples\Monocular\using-gps\KITTI04-12.yaml"

REM set "Mode=gt"
set "Mode=gt"

REM set ImagesDir=Z:/C/data/synced/00/2011_10_03/2011_10_03_drive_0027_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/00/image_0
REM set SensorSource=Z:/C/data/synced/00/2011_10_03/2011_10_03_drive_0027_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/00.txt

REM set ImagesDir=Z:/C/data/synced/01/2011_10_03/2011_10_03_drive_0042_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/01/image_0
REM set SensorSource=Z:/C/data/synced/01/2011_10_03/2011_10_03_drive_0042_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/01.txt

REM set ImagesDir=Z:/C/data/synced/02/2011_10_03/2011_10_03_drive_0034_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/02/image_0
REM set SensorSource=Z:/C/data/synced/02/2011_10_03/2011_10_03_drive_0034_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/02.txt

REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/03/image_0
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/03.txt

REM set ImagesDir=Z:/C/data/synced/04/2011_09_30/2011_09_30_drive_0016_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/04/image_0
REM set SensorSource=Z:/C/data/synced/04/2011_09_30/2011_09_30_drive_0016_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/04.txt

REM set ImagesDir=Z:/C/data/synced/05/2011_09_30/2011_09_30_drive_0018_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/05/image_0
REM set SensorSource=Z:/C/data/synced/05/2011_09_30/2011_09_30_drive_0018_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/05.txt

REM set ImagesDir=Z:/C/data/synced/06/2011_09_30/2011_09_30_drive_0020_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/06/image_0
REM set SensorSource=Z:/C/data/synced/06/2011_09_30/2011_09_30_drive_0020_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/06.txt

REM set ImagesDir=Z:/C/data/synced/07/2011_09_30/2011_09_30_drive_0027_sync/image_00/data
set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/07/image_0
REM set SensorSource=Z:/C/data/synced/07/2011_09_30/2011_09_30_drive_0027_sync/oxts/data
set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/07.txt

REM set ImagesDir=Z:/C/data/synced/08/2011_09_30/2011_09_30_drive_0028_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/08/image_0
REM set SensorSource=Z:/C/data/synced/08/2011_09_30/2011_09_30_drive_0028_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/08.txt

REM set ImagesDir=Z:/C/data/synced/09/2011_09_30/2011_09_30_drive_0033_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/09/image_0
REM set SensorSource=Z:/C/data/synced/09/2011_09_30/2011_09_30_drive_0033_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/09.txt

REM set ImagesDir=Z:/C/data/synced/10/2011_09_30/2011_09_30_drive_0034_sync/image_00/data
REM set ImagesDir=Z:/C/data/data_odometry_gray/dataset/sequences/10/image_0
REM set SensorSource=Z:/C/data/synced/10/2011_09_30/2011_09_30_drive_0034_sync/oxts/data
REM set SensorSource=Z:/C/data/gt/data_odometry_poses/dataset/poses/10.txt

%App% %Mode% %Vocabulary% %Settings% %ImagesDir% %SensorSource%

endlocal
