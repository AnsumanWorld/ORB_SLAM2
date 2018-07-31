@echo off
setlocal
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"
set "ProjectDir=%~dp0..\.."
set "BuildDir=%ProjectDir%\products\%Platform%-windows"
set "VcpkgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\%Platform%-windows\bin"
set "Path=%Path%;%VcpkgBinDir%"

cd %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set ds_tsr_app="%BuildDir%\%BuildType%\bin\run_ds_tsr.exe"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"

set "Settings=%ProjectDir%\Examples\Monocular\Garching-Test-Drive.yaml"
set "VideoOrImages=%ProjectDir%\data\Garching_LoopClosure-5-images"
set "TSRData=%ProjectDir%\data\Garching_LoopClosure-5-features.json"

if /i not "%~1"=="" set "Settings=%~1"
if /i not "%~2"=="" set "VideoOrImages=%~2"
if /i not "%~3"=="" set "TSRData=%~3"

if /i "%VideoOrImages%"=="" (
    echo "invalid arguments provided !!!"
    echo ".\%~n0 <setting-path> <path-to-image-or-video> <path-to-gps-or-tsr-file>"
    echo An error occured in %~n0, bailing out & exit /b %errorlevel%
)
call %ds_tsr_app% %VocabularyFile% %Settings% %VideoOrImages% %TSRData%
endlocal