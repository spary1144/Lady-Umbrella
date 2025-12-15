@echo off
REM ========= CONFIGURA ESTAS RUTAS =========
set UNREAL_ENGINE_PATH="G:\EPIC\UE\UE_5.5.3\Windows\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set PROJECT_PATH="C:\Users\jorge\Desktop\LadyUmbrella\lady-umbrella_g2_2425\LadyUmbrella.uproject"

REM ========= RESAVE PACKAGES =========
%UNREAL_ENGINE_PATH% %PROJECT_PATH% -run=ResavePackages -unattended -replace -autocheckout -projectonly -log=ResaveLevels.log
pause
