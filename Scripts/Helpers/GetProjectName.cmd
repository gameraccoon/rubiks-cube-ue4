@echo off

for /f "tokens=1" %%i in ('Helpers\GetProjectFileName.cmd') do set ueFile=%%i

echo %ueFile:~0,-9%