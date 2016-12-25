@echo off

for /f "delims=" %%i in ('Helpers\GetProjectPath.cmd') do set projectPath=%%i
for /f "delims=" %%i in ('Helpers\GetProjectFileName.cmd') do set projectFileName=%%i

for /f "tokens=2" %%i in ('findstr EngineAssociation "%projectPath%\%projectFileName%"') do set ueVersion=%%i
set ueVersion=%ueVersion:~1,4%

if not exist "C:\Program Files\Epic Games" goto DIR86
set uePath=C:\Program Files\Epic Games\%ueVersion%
goto ENDCHECKDIR
:DIR86
set uePath=C:\Program Files (x86)\Epic Games\%ueVersion%
:ENDCHECKDIR

echo %uePath%