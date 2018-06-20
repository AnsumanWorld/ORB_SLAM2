@echo off

setlocal
call "%~dp0abspath.bat" "%~dp0.."
docker build --tag=shanmukhananda/orb-slam2:latest --file="%AbsPath%/docker/Dockerfile" .

endlocal