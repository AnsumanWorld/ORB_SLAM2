@echo off

setlocal
call "%~dp0abspath.bat" "%~dp0.."
docker run --rm -it --name orb-slam2-build --volume="%AbsPath%:/root/orb-slam2" ^
    shanmukhananda/orb-slam2:latest ^
    /bin/bash -c ^
    "/root/orb-slam2/scripts/linux/build.sh"
endlocal