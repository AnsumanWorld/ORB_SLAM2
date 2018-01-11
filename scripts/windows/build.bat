@echo off

setlocal

call "%~dp0configure.bat" %*
if errorlevel 1 echo configure error, bailing out & exit /b 1

echo OrbSlamBuildDir = %OrbSlamBuildDir%
echo OrbSlamBuildType= %OrbSlamBuildType%

call cmake.exe --build "%OrbSlamBuildDir%" --config %OrbSlamBuildType%

endlocal
