@echo off

setlocal
set "BuildType=RelWithDebInfo"

set "ExtToolsBinDir=C:\Program Files\7-Zip"

set "VcpkgRelBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\bin"
set "VcpkgDbgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\debug\bin"

set "ProjectDir=%~dp0..\..\.."
set "ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64"
set "BinaryDir=%ProductsDir%\%BuildType%\bin"

set "Path=%VcpkgRelBinDir%;%VcpkgDbgBinDir%;%ExtToolsBinDir%;%BinaryDir%;%Path%"

cd /d %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set "SettingsPath=%ProjectDir%\Examples\Monocular\Garching-Test-Drive.yaml"
REM set "SettingsPath=%ProjectDir%\Examples\Monocular\KITTI04-12.yaml"
set "VocabularyPath=%ProjectDir%\Vocabulary\ORBvoc.bin"
REM set "ImageSource=C:\Shanmukha\Dataset\Garching\Garching_LoopClosure-4.mp4"
set "ImageSource=C:\Shanmukha\Dataset\Garching\Garching-5\images"
REM set "ImageSource=C:\Shanmukha\Dataset\KITTI\data_odometry_gray\dataset\sequences\07\image_0"
REM set ImageSource=0

 %BinaryDir%\slam-runner ^
    --image_source="%ImageSource%" ^
    --frame_interval_ms=1000 ^
    --settings_path="%SettingsPath%" ^
    --vocabulary_path="%VocabularyPath%" ^
    --wait_for_stdin=false ^
    --show_viewer=true 

endlocal
