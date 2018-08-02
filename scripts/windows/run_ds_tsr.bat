@echo off
setlocal
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"
set "ProjectDir=%~dp0..\.."
set "BuildDir=%ProjectDir%\products\%Platform%-windows"
set "VcpkgBinDir=%USERPROFILE%\.vcpkg\vcpkg\installed\%Platform%-windows\bin"
set "Path=%Path%;%VcpkgBinDir%"
set "default_input=-s %~dp0..\..\data\Garching_LoopClosure-5.yaml -v %~dp0..\..\data\Garching_LoopClosure-5.mov -t %~dp0..\..\data\Garching_LoopClosure-5.txt"
cd %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set ds_tsr_app="%BuildDir%\%BuildType%\bin\run_ds_tsr.exe"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"

if "%~1"=="" call %ds_tsr_app% %default_input% -o %VocabularyFile%
if NOT "%~1"=="" call %ds_tsr_app% %* -o %VocabularyFile% 
endlocal