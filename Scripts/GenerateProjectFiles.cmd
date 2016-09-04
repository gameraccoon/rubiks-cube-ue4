@echo off

for /f "tokens=2" %%i in ('findstr EngineAssociation "..\Rubik.uproject"') do set ueVersion=%%i
set ueVersion=%ueVersion:~1,4%

set uePath=C:\Program Files\Epic Games\%ueVersion%

cd ..
set projectDir=%CD%

C:
cd "%uePath%\Engine\Binaries\DotNET"
UnrealBuildTool.exe -projectfiles -project "%projectDir%\Rubik.uproject" -game -rocket