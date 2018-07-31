@echo off
setlocal
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"
set "ProjectDir=%~dp0..\.."
set "BuildDir=%ProjectDir%\products\cmake.msbuild.windows.%Platform%.%Toolset%"
set "VcpkgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\x64-windows\bin"
set "Path=%Path%;%VcpkgBinDir%"

cd %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set ds_traficsign_app="%BuildDir%\%BuildType%\bin\run_ds_traficsign.exe"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"

if /i not "%~1"=="" set "settings_file_path=%~1"
if /i not "%~2"=="" set "primary_input_src_path=%~2"
if /i not "%~3"=="" set "secondary_input_src_path=%~3"

if /i "%primary_input_src_path%"=="" (
    echo "invalid arguments provided !!!"
    echo ".\%~n0 <setting-path> <path-to-image-or-video> <path-to-gps-or-tsr-file>"
    echo An error occured in %~n0, bailing out & exit /b %errorlevel%
)


call %ds_traficsign_app% %VocabularyFile% %settings_file_path% %primary_input_src_path% %secondary_input_src_path%
endlocal