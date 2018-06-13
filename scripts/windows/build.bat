@echo off

setlocal
call "%~dp0configure.bat" %*
echo BuildType = %BuildType%
call cmake.exe --build "%BuildDir%" --config %BuildType%
endlocal & set "BuildDir=%BuildDir%" & set "BuildType=%BuildType%"

