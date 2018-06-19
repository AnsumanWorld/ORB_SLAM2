@echo off
setlocal
set "Platform=x64"
set "Toolset=v141"
set "BuildType=Release"

if NOT "%~1"=="" set "Platform=%~1"
if NOT "%~2"=="" set "Toolset=%~2"
if NOT "%~3"=="" set "BuildType=%~3"

rem ----------------------------------
rem Locate vcpkg using environment variables
rem ----------------------------------
set "VcPkgDir=%USERPROFILE%\.vcpkg\vcpkg"
set "VcPkgTriplet=%Platform%-windows"
if defined VCPKG_ROOT_DIR if /i not "%VCPKG_ROOT_DIR%"=="" set "VcPkgDir=%VCPKG_ROOT_DIR%"
if defined VCPKG_DEFAULT_TRIPLET if /i not "%VCPKG_DEFAULT_TRIPLET%"=="" set "VcPkgTriplet=%VCPKG_DEFAULT_TRIPLET%"

rem ----------------------------------
rem Try to look for vcpkg at default locations
rem ----------------------------------
if not exist "%VcPkgDir%" set "VcPkgDir=%~d0\Software\vcpkg\vcpkg"
if not exist "%VcPkgDir%" set "VcPkgDir=%~d0\.vcpkg\vcpkg"
if not exist "%VcPkgDir%" set "VcPkgDir=C:\Software\vcpkg\vcpkg"
if not exist "%VcPkgDir%" set "VcPkgDir=C:\.vcpkg\vcpkg"
if not exist "%VcPkgDir%" set "VcPkgDir=%USERPROFILE%\.vcpkg\vcpkg"
if not exist "%VcPkgDir%" (
    echo vcpkg not found, installing at %VcPkgDir%...
    git clone --recursive https://github.com/Microsoft/vcpkg.git "%VcPkgDir%"
) 
    
echo vcpkg at %VcPkgDir%...

rem
rem Check whether we have a difference in the toolsrc folder. If non empty, %errorlevel% should be 0  
rem git --no-pager diff --name-only origin/HEAD remotes/origin/HEAD | find "toolsrc/" > NUL & echo %errorlevel%
rem Check whether changes was made between local commit an remotes/origin/HEAD in toolsrc\VERSION.txt
rem 
rem git --no-pager diff --name-only b07568da4100d8be5d54feb1b1ccf65d0a5c0c3f remotes/origin/HEAD -- toolsrc\VERSION.txt

rem ----------------------------------
rem Using version 0.0.113 (toolsrc/VERSION.txt)
rem ----------------------------------
set "VcPkgCommit=b07568da4100d8be5d54feb1b1ccf65d0a5c0c3f"
set "VcPkgCommitLock=bootstrap-vcpkg-%VcPkgCommit%"
call
pushd "%VcPkgDir%"
git fetch origin %VcPkgCommit%
git checkout FETCH_HEAD
popd

rem ==============================
rem Upgrade and Install packages.
rem ==============================
set "VcPkgDeps=boost-filesystem boost-property-tree boost-log eigen3 opencv[ffmpeg] pangolin"
call :BootstrapVcPkgExe
call "%VcPkgDir%\vcpkg.exe" upgrade %VcPkgDeps% --no-dry-run --triplet %VcPkgTriplet%
call "%VcPkgDir%\vcpkg.exe" install %VcPkgDeps% --triplet %VcPkgTriplet%
endlocal & ^
set "VcPkgDir=%VcPkgDir%" & ^
set "VcPkgTriplet=%VcPkgTriplet%" & ^
set "Platform=%Platform%" & ^
set "Toolset=%Toolset%" & ^
set "Platform=%Platform%" & ^
set "BuildType=%BuildType%"

goto :eof

:BootstrapVcPkgExe
    call :RemoveBootstrappingLocks
    if not exist "%VcPkgDir%\%VcPkgCommitLock%.lock" (
        call "%VcPkgDir%\bootstrap-vcpkg.bat" & echo %VcPkgCommit% > "%VcPkgDir%\%VcPkgCommitLock%.lock"
    )

goto :eof

rem ---------------------------------- 
rem Remove all bootstrap lock files NOT matching current commit tag:
rem ----------------------------------
:RemoveBootstrappingLocks
    rem powershell.exe -NoProfile -ExecutionPolicy Bypass -command "Remove-Item ^"%VcPkgDir%^" -Include *.tag.txt -Exclude *%VcPkgCommit%.tag.txt"
    for /f "tokens=*" %%F in ('dir /b /s ^"%VcPkgDir%\*.lock^"') do (
        if /i not "%%~nF%%~xF"=="%VcPkgCommitLock%.lock" (
            del /q "%%F"
        )
    )

goto :eof
