@echo off

setlocal
set "BuildType=Debug"

set "ExtToolsBinDir=C:\Program Files\7-Zip"

set "VcpkgRelBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\bin"
set "VcpkgDbgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\debug\bin"

set "ProjectDir=%~dp0..\..\.."
set "ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x64.v141"
set "BinaryDir=%ProjectDir%\products\cmake.msbuild.windows.x64.v141\%BuildType%\bin"

set "Path=%VcpkgRelBinDir%;%VcpkgDbgBinDir%;%ExtToolsBinDir%;%BinaryDir%;%Path%"

cd /d %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set App="%BinaryDir%\slam-runner.exe"
set VideoFile="%~1"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"
set SettingsFile="%ProjectDir%\Examples\Monocular\Garching-Test-Drive.yaml"

%App% %VideoFile% %VocabularyFile% %SettingsFile%

endlocal
