@echo off

setlocal
    call "%~dp0configure.bat" %* || exit /b 1
    call cmake.exe --build "%BuildDir%" --config %BuildType% || exit /b 1
endlocal & set "BuildDir=%BuildDir%" & set "BuildType=%BuildType%"

