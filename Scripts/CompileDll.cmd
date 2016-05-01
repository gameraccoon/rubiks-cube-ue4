@echo off

set ueVersion=4.10
set uePath=C:\Program Files\Epic Games\%ueVersion%

cd ..
set projectDir=%CD%

"%uePath%\Engine\Binaries\DotNET\UnrealBuildTool.exe" Rubik Development Win64 -project="%projectDir%\Rubik.uproject" -rocket -editorrecompile -progress -noubtmakefiles -2015
