@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\BuildWindows.ps1" -Mode Diagnose %*
set "result=%errorlevel%"
pause
exit /b %result%
