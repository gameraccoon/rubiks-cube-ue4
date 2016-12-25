@echo off

for /f "delims=" %%i in ('Helpers\GetUeFolderPath.cmd') do set uePath=%%i
for /f "delims=" %%i in ('Helpers\GetProjectName.cmd') do set projectName=%%i
for /f "delims=" %%i in ('Helpers\GetProjectFileName.cmd') do set projectFileName=%%i
for /f "delims=" %%i in ('Helpers\GetProjectPath.cmd') do set projectPath=%%i

set scriptDir=%CD%
set projectDrive=%cd:~0,2%

set projectDir=%CD%\%projectPath%

%1

%projectDrive%
cd %scriptDir%