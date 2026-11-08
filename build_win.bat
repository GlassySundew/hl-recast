@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "BUILD_DIR=%SCRIPT_DIR%\build-win"

cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -A x64 -G "Visual Studio 17 2022"
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --config Release --target recast.hdll
if errorlevel 1 exit /b %errorlevel%

echo Built recast.hdll in %SCRIPT_DIR%\..\..
