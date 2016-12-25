@echo off

for /f "delims=" %%i in ('Helpers\GetProjectPath.cmd') do set projectPath=%%i

dir /b %projectPath%\*.uproject | findstr .uproject$