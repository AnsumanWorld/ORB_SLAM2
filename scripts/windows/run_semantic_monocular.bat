@echo off

setlocal
set "VcpkgBinDir=%~d0\Software\vcpkg\vcpkg\installed\x86-windows-v141\bin"
set "ProjectDir=%~dp0..\.."
set "Path=%Path%;%VcpkgBinDir%;C:\Program Files\7-Zip"
set "OrbSlam2ProductsDir=%ProjectDir%\products\cmake.msbuild.windows.x86.v141"

cd %ProjectDir%

if not exist "%ProjectDir%\Vocabulary\ORBvoc.bin" (
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar.gz" -o"%ProjectDir%\Vocabulary"
    7z x "%ProjectDir%\Vocabulary\ORBvoc.bin.tar" -o"%ProjectDir%\Vocabulary"
    del /f /q "%ProjectDir%\Vocabulary\ORBvoc.bin.tar"
)

set SemanticMonoApp="%OrbSlam2ProductsDir%\Release\semantic_monocular.exe"
set VocabularyFile="%ProjectDir%\Vocabulary\ORBvoc.bin"
set SettingsFile="%ProjectDir%\Examples\Monocular\Garching-Test-Drive.yaml"
set ImagesFolder="%ProjectDir%\data\Garching_LoopClosure-4-images"
set FeaturesJson="%ProjectDir%\data\Garching_LoopClosure-4-features.json"

%SemanticMonoApp% %VocabularyFile% %SettingsFile% %ImagesFolder% %FeaturesJson%

endlocal

