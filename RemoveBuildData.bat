@echo off
REM =====================================================
REM Batch para eliminar todos los *BuiltData.uasset en el proyecto
REM =====================================================

set PROJECT_DIR=C:\Users\jorge\Desktop\LadyUmbrella\lady-umbrella_g2_2425

echo Buscando y eliminando archivos *BuiltData.uasset...

for /R "%PROJECT_DIR%" %%f in (*BuiltData.uasset) do (
    echo Eliminando: %%f
    del /Q "%%f"
)

echo Limpieza completada.
pause
