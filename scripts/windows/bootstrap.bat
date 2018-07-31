@echo off
setlocal
    set "Platform=x64"
    set "Toolset=v141"
    set "BuildType=Release"
    set "Debug=1"
    if NOT "%~1"=="" set "Platform=%~1"
    if NOT "%~2"=="" set "Toolset=%~2"
    if NOT "%~3"=="" set "BuildType=%~3"

    rem ==============================
    rem Locate or download vcpkg
    rem Version 0.0.108 (in toolsrc/VERSION.txt) commit 15e4f46b45c432a41ee6a962609039bc2497ec19
    rem ==============================
    set "VcPkgCommitHash=15e4f46b45c432a41ee6a962609039bc2497ec19"
    set "VcPkgDir=%USERPROFILE%\.vcpkg\vcpkg"
    set "VcPkgTriplet=%Platform%-windows"
    set "VendorDir=%~dp0..\..\.vendor"
    ( call :InstallFFMPEGLibs ^
    && call :BootstrapVcPkg ^
    && call :InstallVcPkgDeps "boost-program-options boost-accumulators boost-filesystem boost-property-tree boost-log boost-math boost-test eigen3 opencv[ffmpeg] pangolin gtest" ^
    && call :InitVendorFolder ^
    && call :InstallCotire ) || ( if defined Debug ( echo Bootstrapping error occured! ) & exit /b 1 )

    call :Success %~dp0..\..
endlocal & set "VcPkgDir=%VcPkgDir%" ^
    & set "VcPkgTriplet=%VcPkgTriplet%" ^
    & set "Platform=%Platform%" ^
    & set "Toolset=%Toolset%" ^
    & set "Platform=%Platform%" ^
    & set "BuildType=%BuildType%" ^
    & exit /b %errorlevel%

goto :eof

:BootstrapVcPkg
setlocal
    if defined Debug ( echo Enter :BootstrapVcPkg )
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
        git clone --recursive https://github.com/Microsoft/vcpkg.git "%VcPkgDir%" || exit /b 1
    )
    echo vcpkg at %VcPkgDir%... 
    ( pushd "%VcPkgDir%" ^
    && git fetch origin %VcPkgCommitHash% ^
    && git checkout FETCH_HEAD ^
    && popd ^
    && call :BootstrapVcPkgExe) || ( exit /b 1 )
endlocal & set "VcPkgDir=%VcPkgDir%" ^
         & set "VcPkgTriplet=%VcPkgTriplet%" ^
         & exit /b %errorlevel%

goto :eof

:InstallVcPkgDeps
setlocal
    if defined Debug ( echo Enter :InstallVcPkgDeps )
    set "VcPkgDeps=%~1"
    call "%VcPkgDir%\vcpkg.exe" install %VcPkgDeps% --triplet %VcPkgTriplet%
endlocal & exit /b %errorlevel%

goto :eof

:BootstrapVcPkgExe
setlocal
    if defined Debug ( echo Enter :BootstrapVcPkgExe )
    call :RemoveBootstrappingLocks && if not exist "%VcPkgDir%\%VcPkgCommitHash%.lock" (
        ( call "%VcPkgDir%\bootstrap-vcpkg.bat" ^
         && echo %VcPkgCommitHash% > "%VcPkgDir%\%VcPkgCommitHash%.lock" )
    )
endlocal & exit /b %errorlevel%

goto :eof

:RemoveBootstrappingLocks
setlocal
    rem powershell.exe -NoProfile -ExecutionPolicy Bypass -command "Remove-Item ^"%VcPkgDir%^" -Include *.tag.txt -Exclude *%VcPkgCommit%.tag.txt"
    if defined Debug ( echo Enter :RemoveBootstrappingLocks: comparing against "%VcPkgCommitHash%.lock" )
    for /f "tokens=*" %%F in ('dir /b /s ^"%VcPkgDir%\*.lock^"') do (
        if defined Debug ( echo RemoveBootstrappingLocks: checking "%%~nF%%~xF"... )
        if /i not "%%~nF%%~xF"=="%VcPkgCommitHash%.lock" (
            if defined Debug ( echo RemoveBootstrappingLocks: deleting "%%~nF%%~xF"... )
            del /q "%%F"
        )
    )
endlocal & exit /b %errorlevel%

goto :eof

rem ==============================
rem Prepare vendor folder. Mimics the vcpkg folder structure
rem ==============================
:InitVendorFolder
setlocal
    if defined Debug ( echo Enter :InitVendorFolder )
    if not exist "%VendorDir%" md "%VendorDir%" 
    if not exist "%VendorDir%\tools" md "%VendorDir%\tools"
    if not exist "%VendorDir%\downloads" md "%VendorDir%\downloads"
    if not exist "%VendorDir%\buildtrees" md "%VendorDir%\buildtrees"
    if not exist "%VendorDir%\installed\%Platform%-windows" md "%VendorDir%\installed\%Platform%-windows"
endlocal & set "VendorDir=%VendorDir%" & exit /b %errorlevel%

goto :eof

:InstallCotire
setlocal
    if defined Debug ( echo Enter :InstallCotire )
    if not exist "%VendorDir%\downloads\cotire-master.zip" (
        ( pushd "%VendorDir%\downloads" ^
        && powershell.exe -NoProfile -ExecutionPolicy Bypass -command "& {[Net.ServicePointManager]::SecurityProtocol = 'tls12, tls11, tls'; Invoke-WebRequest 'https://github.com/sakra/cotire/archive/master.zip' -OutFile cotire-master.zip; }" ^
        && popd ) || exit /b 1
    )
    if not exist "%VendorDir%\tools\cotire" (
        ( pushd "%VendorDir%\tools" ^
        && powershell.exe -nologo -noprofile -command "& {Expand-Archive -Path '%VendorDir%\downloads\cotire-master.zip' -DestinationPath '%VendorDir%\downloads\tmp\cotire'; Move-Item -Path '%VendorDir%\downloads\tmp\cotire\cotire-master' -Destination cotire; Remove-Item '%VendorDir%\downloads\tmp\cotire'; }" ^
        && popd ) || exit /b 1
    )
endlocal

goto :eof

rem ==============================
rem temporary hack to copy opencv_ffmpeg dlls until this is solved in vcpkg
rem ==============================
:InstallFFMPEGLibs
setlocal
    if defined Debug ( echo Enter :InstallFFMPEGLibs )
    ( if not exist "%~dp0..\..\products" mkdir "%~dp0..\..\products" ^
    && echo xcopy "%VcPkgDir%\installed\%VcPkgTriplet%\bin\opencv_ffmpeg*.dll" "%%~1/" /y /f > "%~dp0..\..\products\copy_ffmpeg.bat" ^
    && echo xcopy "%VcPkgDir%\installed\%VcPkgTriplet%\debug\bin\opencv_ffmpeg*.dll" "%%~1/" /y /f > "%~dp0..\..\products\copy_ffmpegd.bat" ) || exit /b 1
endlocal

goto :eof

:Success
setlocal
    ( pushd %~dp1 && echo Project %CD% successfully bootstrapped! && popd )
endlocal
