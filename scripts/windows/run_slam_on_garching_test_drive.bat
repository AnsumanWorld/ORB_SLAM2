@echo off

setlocal
set "VcpkgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\bin"
set "ProjectDir=%~dp0..\.."
set "Path=%Path%;%VcpkgBinDir%;C:\Program Files\7-Zip"
set "OrbSlam2ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64.v141"

cd %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set MonoVideoApp="%OrbSlam2ProductsDir%\Release\mono_video.exe"
set VideoFile="%~1"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"
set SettingsFile="%ProjectDir%\Examples\Monocular\Garching-Test-Drive.yaml"

%MonoVideoApp% %VideoFile% %VocabularyFile% %SettingsFile%

endlocal
