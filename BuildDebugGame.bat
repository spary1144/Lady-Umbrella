REM @echo off
REM setlocal
REM /*
REM REM ==== CONFIG ====
REM set PROJECT_NAME=LadyUmbrella
REM set PROJECT_PATH=%~dp0%PROJECT_NAME%.uproject
REM set ENGINE_PATH=C:\Program Files\Epic Games\UE_5.5
REM set UNREAL_BUILD_TOOL_PATH=%ENGINE_PATH%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe
REM set UAT_BAT_PATH=%ENGINE_PATH%\Engine\Build\BatchFiles\RunUAT.bat
REM set PLATFORM=Win64
REM set CONFIG=DebugGame
REM 
REM echo.
REM echo ================================
REM echo Building %PROJECT_NAME% (%CONFIG% %PLATFORM%)
REM echo ================================
REM echo.
REM 
REM REM --- Build the game binary in DebugGame ---
REM "%UNREAL_BUILD_TOOL_PATH%" LadyUmbrella Win64 DebugGame -Project="%PROJECT_PATH%" -WaitMutex
REM 
REM if errorlevel 1 (
REM     echo Build failed!
REM     exit /b 1
REM )
REM 
REM echo.
REM echo ================================
REM echo Cooking and staging content
REM echo ================================
REM echo.
REM 
REM REM --- Cook & Stage assets for DebugGame ---
REM "%UAT_BAT_PATH%" BuildCookRun ^
REM  -project="%PROJECT_PATH%" ^
REM  -noP4 -platform=%PLATFORM% -clientconfig=%CONFIG% ^
REM  -cook -stage -map=YourMainMap -unattended -utf8output
REM 
REM if errorlevel 1 (
REM     echo Cook/Stage failed!
REM     exit /b 1
REM )
REM 
REM echo.
REM echo ================================
REM echo Done!
REM REM echo Your DebugGame .exe is in:
REM REM echo %~dp0Saved\StagedBuilds\WindowsNoEditor\%PROJECT_NAME%\Binaries\%PLATFORM%\
REM echo ================================
REM echo.
echo WIP! .bat not finished
pause
endlocal