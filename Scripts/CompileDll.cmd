@echo off

for /f "tokens=2" %%i in ('findstr EngineAssociation "..\Rubik.uproject"') do set ueVersion=%%i
set ueVersion=%ueVersion:~1,4%

set uePath=C:\Program Files\Epic Games\%ueVersion%

cd ..
set projectDir=%CD%

"%uePath%\Engine\Binaries\DotNET\UnrealBuildTool.exe" Rubik Development Win64 -project="%projectDir%\Rubik.uproject" -rocket -editorrecompile -progress -noubtmakefiles -2015
