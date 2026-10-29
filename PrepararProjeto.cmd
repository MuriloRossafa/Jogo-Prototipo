@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\BuildWindows.ps1" -Mode Prepare %*
set "result=%errorlevel%"
pause
exit /b %result%
