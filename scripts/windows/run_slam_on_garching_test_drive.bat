@echo off

setlocal
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"
set "ProjectDir=%~dp0..\.."
set "BuildDir=%ProjectDir%\products\%Platform%-windows"
set "VcpkgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\bin"
set "Path=%Path%;%VcpkgBinDir%"

cd %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set MonoVideoApp="%BinaryDir%\mono_video.exe"
set VideoFile="%~1"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"
set SettingsFile="%ProjectDir%\Examples\Monocular\Garching-Test-Drive.yaml"

%MonoVideoApp% %VideoFile% %VocabularyFile% %SettingsFile%

endlocal
