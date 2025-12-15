@echo off
echo Holii jajaja, Hiii, Ciao, Bonjour, Hej, kaixo, Maitei
echo.
echo Try to play all levels without cheats. Explore every possible path so all the PSOs are collected. Do not forget to check all menus (and their tabs, if they have any). P.S. you can leave that for the end. And above all, enjoy Lady Umbrella!
echo.
echo It must be a development build.
echo.

:AskPath
set /p folderPath="Introduce the path where the .exe file is (e.g. C:\Matematicas\build\Windows\): "

if not exist "%folderPath%" (
    echo [ERROR] Could not find the path.
    echo.
    goto AskPath
)

for %%f in ("%folderPath%\*.exe") do (
    set exePath=%%f
    goto :foundExe
)

echo.
echo [ERROR] Could not find any .exe file inside "%folderPath%".
echo Please write the correct path.
echo.
goto AskPath

:foundExe
echo Running: %exePath% -logPSO
start "" "%exePath%" -logPSO