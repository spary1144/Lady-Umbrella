@echo off
setlocal

REM --- .bat for deleting generated files, generate vs project files and opening the project in Unreal Editor or Opening the .sln file
REM --- Copyright Zulo Interactive. All Rights Reserved.
REM -- Author: 
REM -- Jorge Marin Ferrandiz @spary1144

:: ===============================
:: Configuración base
:: ===============================

set PROJECT_NAME=LadyUmbrella
set PROJECT_PATH=%cd%\%PROJECT_NAME%.uproject
set PLATFORM=Win64
::set UE_PATH="C:\Program Files\Epic Games\UE_5.5"
:: Carpeta raíz de builds
::set UE_PATH="D:\UnrealEngine\UE_5.5.3\Windows\"
set UE_PATH="G:\EPIC\UE\UE_5.5.3\Windows\"
set OUTPUT_DIR=%cd%\Packaged

:: ===============================
:: Menú de opciones
:: ===============================
:MENU
cls
echo ================================
echo Selecciona modo de empaquetado:
echo ================================
echo [1] DebugGame
echo [2] Development
echo [3] Shipping
echo [Q] Salir
echo.
choice /c 123Q /n /m "Elige una opcion: "

if errorlevel 4 goto END
if errorlevel 3 set CONFIGURATION=Shipping & goto BUILD
if errorlevel 2 set CONFIGURATION=Development & goto BUILD
if errorlevel 1 set CONFIGURATION=DebugGame & goto BUILD

:BUILD
set OUTPUT_PATH=%OUTPUT_DIR%\%PLATFORM%_%CONFIGURATION%

echo ================================
echo Empaquetando %PROJECT_NAME% en %CONFIGURATION%
echo ================================

call %UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat BuildCookRun ^
 -project="%PROJECT_PATH%" ^
 -noP4 ^
 -platform=%PLATFORM% ^
 -clientconfig=%CONFIGURATION% ^
 -targetplatform=%PLATFORM% ^
 -cook ^
 -clean ^
 -build ^
 -stage ^
 -pak ^
 -archive ^
 -archivedirectory="%OUTPUT_PATH%" ^
 -utf8output ^
 -DisableTouch

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ❌ Error al empaquetar en %CONFIGURATION%
    pause
    goto MENU
)

echo.
echo ✅ Juego empaquetado en: %OUTPUT_PATH%

explorer.exe %OUTPUT_PATH%

pause
goto MENU



:END
echo Saliendo...
endlocal

