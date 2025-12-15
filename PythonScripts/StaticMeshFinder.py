import unreal

# Obtener todos los actores del nivel actual
actors = unreal.EditorLevelLibrary.get_all_level_actors()

for actor in actors:
    # Iterar sobre todos los componentes del actor
    for comp in actor.get_components_by_class(unreal.StaticMeshComponent):
        sm = comp.static_mesh
        if sm is None:
            print(f"WARNING: Actor '{actor.get_name()}' tiene un StaticMeshComponent sin asset")
