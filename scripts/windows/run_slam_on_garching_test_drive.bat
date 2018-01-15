@echo off

setlocal
set "OrbSlamPlatform=x64"
set "OrbSlamToolset=v141"
set "OrbSlamBuildType=Release"

if NOT "%~1"=="" set "OrbSlamPlatform=%~1"
if NOT "%~2"=="" set "OrbSlamToolset=%~2"

set "VcPkgTriplet=%OrbSlamPlatform%-windows-%OrbSlamToolset%"
set "VcpkgBinDir=%~d0\Software\vcpkgbin\installed\%VcPkgTriplet%\bin"
echo "%VcpkgBinDir%"
set "ProjectDir=%~dp0..\.."
set "Path=%VcpkgBinDir%;C:\Program Files\7-Zip;%Path%"
set "OrbSlam2ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.%OrbSlamPlatform%.%OrbSlamToolset%.%OrbSlamBuildType%"

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

