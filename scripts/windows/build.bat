@echo off

setlocal

set "OrbSlamPlatform=x64"
set "OrbSlamToolset=v141"
set "OrbSlamBuildType=Release"

if NOT "%~1"=="" set "OrbSlamPlatform=%~1"
if NOT "%~2"=="" set "OrbSlamToolset=%~2"
if NOT "%~3"=="" set "OrbSlamBuildType=%~3" 

call "%~dp0bootstrap.bat" %*
if %errorlevel% neq 0 echo An error occured in %~n0, bailing out & exit /b %errorlevel%

set "OrbSlamCMakeGeneratorName=Visual Studio 14 2015"

if "%OrbSlamPlatform%"=="x86" (
    if "%OrbSlamToolset%"=="v140" set "OrbSlamCMakeGeneratorName=Visual Studio 14 2015"
    if "%OrbSlamToolset%"=="v141" set "OrbSlamCMakeGeneratorName=Visual Studio 15 2017"
)

if "%OrbSlamPlatform%"=="x64" (
    if "%OrbSlamToolset%"=="v140" set "OrbSlamCMakeGeneratorName=Visual Studio 14 2015 Win64"
    if "%OrbSlamToolset%"=="v141" set "OrbSlamCMakeGeneratorName=Visual Studio 15 2017 Win64"
)

set "OrbSlamBuildDir=%~dp0..\..\products\cmake.msbuild.windows.%OrbSlamPlatform%.%OrbSlamToolset%.%OrbSlamBuildType%"
if not exist "%OrbSlamBuildDir%" mkdir "%OrbSlamBuildDir%"
cd "%OrbSlamBuildDir%"

call cmake.exe -G "%OrbSlamCMakeGeneratorName%" -DBUILD_EXAMPLES=ON "%~dp0..\.."
call cmake.exe --build . --config %OrbSlamBuildType%

endlocal
