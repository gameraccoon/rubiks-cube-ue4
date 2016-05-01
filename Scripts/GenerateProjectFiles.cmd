@echo off

set ueVersion=4.10
set uePath=C:\Program Files\Epic Games\%ueVersion%

cd ..
set projectDir=%CD%

C:
cd "%uePath%\Engine\Build\BatchFiles"
RocketGenerateProjectFiles -project "%projectDir%\Rubik.uproject" -game
