@echo off

set "OrbSlamPlatform=x64"
set "OrbSlamToolset=v141"

if NOT "%~1"=="" set "OrbSlamPlatform=%~1"
if NOT "%~2"=="" set "OrbSlamToolset=%~2"

set "VcPkgGitUrl=https://github.com/apattnaik0721013/conan4vcpkg.git"
set "VcPkgDir=%~d0\Software\vcpkgbin"

if defined VCPKG_ROOT_DIR if /i not "%VCPKG_ROOT_DIR%"=="" set "VcPkgDir=%VCPKG_ROOT_DIR%"
set "VcPkgTriplet=%OrbSlamPlatform%-windows-%OrbSlamToolset%"
if defined VCPKG_DEFAULT_TRIPLET if /i not "%VCPKG_DEFAULT_TRIPLET%"=="" set "VcpkgTriplet=%VCPKG_DEFAULT_TRIPLET%"
set "VcPkgBinPath=%VcPkgDir%\vcpkgbin.bat"

echo. & echo Installing/updating vcpkg...
call :BootstrapVcPkg "%VcPkgGitUrl%" "%VcPkgDir%"
if %errorlevel% neq 0 echo An error occured in %~n0, bailing out & exit /b %errorlevel%
echo. & echo vcpkg %VcPkgTag% successfully installed...

rem ==============================
rem Update and Install packages.
rem ==============================
set "cur_path=%~dp0"
cd %VcPkgDir%
call "%VcPkgBinPath%" download vcpkg/0.0.81-6@had/vcpkg "eigen3 opencv pangolin" %VcPkgTriplet%
if %errorlevel% neq 0 echo An error occured in %~n0, bailing out & exit /b %errorlevel%
cd %cur_path%

set "VcPkgTripletDir=%VcPkgDir%\installed\%VcPkgTriplet%"
if not exist "%VcPkgTripletDir%" echo %VcPkgTripletDir% does not exist, bailing out & exit /b 1

set "CMAKE_PREFIX_PATH=%VcPkgTripletDir%;%CMAKE_PREFIX_PATH%"

echo. & echo Bootstrapping successful for triplet: %VcPkgTriplet% & echo CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% & echo.

goto :eof

:BootstrapVcPkg
setlocal
    set "LocErrorState=0"
    set "LocVcPkgGit=%~1"
    set "LocVcPkgDir=%~2"

    echo Checking for existence of "%LocVcPkgDir%"...
    if not exist "%LocVcPkgDir%" (
        echo Attempt to: git clone "%LocVcPkgGit%" "%LocVcPkgDir%"...
        git clone "%LocVcPkgGit%" "%LocVcPkgDir%"
    )
    if %errorlevel% neq 0 echo An error occured in %~n0, bailing out & exit /b %errorlevel%

    pushd "%LocVcPkgDir%"
    if %errorlevel% neq 0 echo An error occured in %~n0, bailing out & exit /b %errorlevel%

    echo Attempt to: git fetch --all --tags --prune...
    git fetch --all --tags --prune
    if %errorlevel% neq 0 echo An error occured in %~n0, bailing out & exit /b %errorlevel

    popd
endlocal & exit /b %errorlevel%

goto :eof
