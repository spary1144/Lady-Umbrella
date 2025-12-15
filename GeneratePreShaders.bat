@echo off
setlocal enabledelayedexpansion
echo PLEASE READ THE DOCUMENTATION at:
echo "READ FROM HERE IF YOU USED PLAYBUILD.BAT"
echo https://www.notion.so/Pre-Compile-Shaders-Guide-1f818b47cae3801d843ee441d5a0d6a1?pvs=97#27a18b47cae3805598cde2f101e2f555
echo It explains what this .bat asks from you before executing. ;)
echo.

:InputLoop
set /p inputFolder="Introduce the PSOCache path where the files are stored: "

set /p outputBase=" Name for the output (e.g. GoldX): "

set UEPath="C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
if not exist %UEPath% (
    echo ERROR: UnrealEditor-Cmd.exe not found at %UEPath%
    echo Please make sure UE5 is installed correctly.
    echo.
    goto InputLoop
)

for %%f in ("%inputFolder%\*.rec.upipelinecache") do set recFile=%%f
for %%f in ("%inputFolder%\ShaderStableInfo-*.shk") do set shkFile=%%f

if not defined recFile (
    echo ERROR: No .rec.upipelinecache file found in folder.
    goto InputLoop
)
if not defined shkFile (
    echo ERROR: No ShaderStableInfo-*.shk file found in folder.
    goto InputLoop
)

for %%f in ("%recFile%") do (
    set recName=%%~nf
    for /f "tokens=2,3 delims=_" %%a in ("!recName:*_LadyUmbrella_=!") do (
        set shaderFormatRec=%%a
        set smLevelRec=%%b
    )
)

for %%f in ("%shkFile%") do (
    set shkName=%%~nf
    for /f "tokens=2,3 delims=-_" %%a in ("!shkName:*ShaderStableInfo-=!") do (
        set shaderFormatShk=%%a
        set smLevelShk=%%b
    )
)

if /i "!shaderFormatRec!" neq "!shaderFormatShk!" (
    echo ERROR: Both files have different shader format.
    echo.
    goto InputLoop
)
if /i "!smLevelRec!" neq "!smLevelShk!" (
    echo ERROR: Both files have different SM level.
    echo.
    goto InputLoop
)

set outputFile=%inputFolder%\%outputBase%_LadyUmbrella_!shaderFormatRec!_!smLevelRec!.spc

echo.
echo Do not close this tab.
echo.

%UEPath% -run=ShaderPipelineCacheTools expand "%recFile%" "%shkFile%" "%outputFile%"

echo.
echo Process completed. Check if the file %outputFile% is in folder %inputFolder%
pause