import unreal
import time

# Carpeta donde están tus niveles
root_path = "/Game/Maps/Levels"

DELAY_BETWEEN_LEVELS = 2.0

asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
all_assets = asset_registry.get_assets_by_path(root_path, recursive=True)

# Filtrar solo niveles tipo "World"
levels = [asset.package_name for asset in all_assets if asset.asset_class_path.asset_name == "World"]

if not levels:
    unreal.log("No se encontraron niveles para procesar.")
else:
    unreal.log(f"Se encontraron {len(levels)} niveles para procesar...")

failed = []
processed_count = 0

for level_path in levels:
    try:
        unreal.log(f"Procesando nivel: {level_path}")

        # Abrir, guardar y cerrar nivel
        unreal.EditorLevelLibrary.load_level(level_path)
        unreal.EditorLevelLibrary.save_current_level()

        # Liberar memoria
        unreal.SystemLibrary.collect_garbage()

        processed_count += 1
        unreal.log(f"Nivel procesado correctamente: {level_path}")

        time.sleep(DELAY_BETWEEN_LEVELS)

    except Exception as e:
        unreal.log_error(f"ERROR procesando {level_path}: {e}")
        failed.append(level_path)

unreal.log("====================================")
unreal.log(f"Niveles procesados: {processed_count}")
unreal.log(f"Niveles fallidos: {len(failed)}")
if failed:
    unreal.log("Lista de fallidos:")
    for f in failed:
        unreal.log(f" - {f}")
