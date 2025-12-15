// LevelLightingBuilder.cpp
#if WITH_EDITOR
#include "LevelLightingBuilder.h"
#include "Editor.h"
#include "LightingBuildOptions.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/StaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Ticker.h"
#include "Editor/UnrealEd/Public/FileHelpers.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "HAL/PlatformTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UnrealType.h" // FindFProperty

TSet<ULevel*> ULevelLightingBuilder::PendingLevels;
FTSTicker::FDelegateHandle ULevelLightingBuilder::TickerHandle;

TArray<FString> ULevelLightingBuilder::GetAllLevelsInPath(const FString& RootPath)
{
    TArray<FString> Result;

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> LevelAssets;

    FARFilter Filter;
    Filter.ClassPaths.Add(FTopLevelAssetPath(TEXT("/Script/Engine"), TEXT("World")));
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Add(*RootPath);

    AssetRegistryModule.Get().GetAssets(Filter, LevelAssets);

    for (const FAssetData& AssetData : LevelAssets)
    {
        Result.Add(AssetData.GetObjectPathString());
    }

    return Result;
}

void ULevelLightingBuilder::BuildLightingForAllLevels(const FString& RootPath)
{
    TArray<FString> LevelsToProcess = GetAllLevelsInPath(RootPath);
    if (LevelsToProcess.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No levels found in %s"), *RootPath);
        return;
    }

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid editor world found"));
        return;
    }

    for (const FString& LevelPath : LevelsToProcess)
    {
        UE_LOG(LogTemp, Display, TEXT("Loading level: %s"), *LevelPath);

        if (!FEditorFileUtils::LoadMap(LevelPath))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load level: %s"), *LevelPath);
            continue;
        }

        // Inicializamos niveles pendientes y arrancamos ticker para esperar carga
        StartProcessingLevels(World);
    }
}

void ULevelLightingBuilder::LogActorsWithEmptyStaticMeshes()
{
    if (!GEditor) return;

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return;

    const FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("EmptyMeshActors.log");

    // Limpiar log previo
    IFileManager::Get().Delete(*LogFilePath);

    FString TotalLog;
    TotalLog += FString::Printf(TEXT("=== Empty Mesh Actor Scan - %s ===\n"), *FDateTime::Now().ToString());

    // Recorremos todos los niveles cargados
    TArray<ULevel*> LevelsToCheck;
    LevelsToCheck.Add(World->PersistentLevel);

    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel)
        {
            ULevel* Loaded = StreamingLevel->GetLoadedLevel();
            if (Loaded)
            {
                LevelsToCheck.Add(Loaded);
            }
        }
    }

    for (ULevel* Level : LevelsToCheck)
    {
        if (!IsValid(Level)) continue;

        TotalLog += FString::Printf(TEXT("\n--- Level: %s ---\n"), *Level->GetOutermost()->GetName());

        for (AActor* Actor : Level->Actors)
        {
            if (!IsValid(Actor)) continue;

            bool bTransientOrEditorOnly = Actor->IsPendingKillPending() || Actor->HasAnyFlags(RF_Transient) || Actor->IsEditorOnly();

            TArray<UStaticMeshComponent*> MeshComponents;
            Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

            bool bHasEmptyMesh = false;
            for (UStaticMeshComponent* SMC : MeshComponents)
            {
                if (SMC && !SMC->GetStaticMesh())
                {
                    bHasEmptyMesh = true;
                    break;
                }
            }

            if (bHasEmptyMesh)
            {
                FString ParentName = TEXT("None");
                FString ChildActorCompName = TEXT("None");

                if (Actor->GetAttachParentActor())
                {
                    ParentName = Actor->GetAttachParentActor()->GetName();

                    // También intentar detectar si es un ChildActorComponent
                    TArray<UChildActorComponent*> ChildComps;
                    Actor->GetAttachParentActor()->GetComponents<UChildActorComponent>(ChildComps);
                    for (UChildActorComponent* Comp : ChildComps)
                    {
                        if (Comp && Comp->GetChildActor() == Actor)
                        {
                            ChildActorCompName = Comp->GetName();
                            break;
                        }
                    }
                }

                FString ActorLog = FString::Printf(
                    TEXT("Actor: %s | Level: %s | Parent Actor: %s | ChildActorComp: %s | Transient/EditorOnly: %s"),
                    *Actor->GetName(),
                    *Level->GetOutermost()->GetName(),
                    *ParentName,
                    *ChildActorCompName,
                    bTransientOrEditorOnly ? TEXT("Yes") : TEXT("No")
                );

                UE_LOG(LogTemp, Warning, TEXT("%s"), *ActorLog);

                FFileHelper::SaveStringToFile(ActorLog + LINE_TERMINATOR, *LogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
            }
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Empty mesh actor scan completed. Log saved to: %s"), *LogFilePath);
}


void ULevelLightingBuilder::StartProcessingLevels(UWorld* World)
{
    PendingLevels.Empty();
    
    World = GEditor->GetEditorWorldContext().World();
    
    if (!IsValid(World)) return;
    
    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel && StreamingLevel->IsLevelLoaded())
        {
            ULevel* Loaded = StreamingLevel->GetLoadedLevel();
            if (IsValid(Loaded))
            {
                PendingLevels.Add(Loaded);
            }
        }
    }

    // Cancelar ticker previo si existe
    if (TickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
        TickerHandle.Reset();
    }

    // Añadir un ticker que se llama cada 0.1s
    TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([](float DeltaTime)
        {
            UWorld* CurrentWorld = nullptr;
            if (GEditor)
            {
                CurrentWorld = GEditor->GetEditorWorldContext().World();
            }

            if (!IsValid(CurrentWorld))
            {
                return true; // seguimos, pero no hacemos nada
            }

            ULevelLightingBuilder::CheckLoadedLevels(CurrentWorld);

            return true; // devolver false para auto-detener (no lo hacemos aquí)
        }),
        0.1f // intervalo
    );
}

void ULevelLightingBuilder::CheckLoadedLevels(UWorld* World)
{
    if (!World) return;

    TSet<ULevel*> CurrentlyLoaded;
    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel && StreamingLevel->IsLevelLoaded())
        {
            ULevel* Loaded = StreamingLevel->GetLoadedLevel();
            if (IsValid(Loaded))
            {
                CurrentlyLoaded.Add(Loaded);
            }
        }
    }

    PendingLevels = CurrentlyLoaded;

    if (PendingLevels.Num() == World->GetStreamingLevels().Num())
    {
        if (TickerHandle.IsValid())
        {
            FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
            TickerHandle.Reset();
        }
        CheckIfAllLevelsLoaded(World);
    }
}

void ULevelLightingBuilder::CheckIfAllLevelsLoaded(UWorld* World)
{
    if (!World) return;

    UE_LOG(LogTemp, Display, TEXT("All sublevels loaded. Cleaning empty StaticMeshComponents..."));

    TArray<ULevel*> LevelsToCheck;
    LevelsToCheck.Add(World->PersistentLevel);

    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel)
        {
            ULevel* Loaded = StreamingLevel->GetLoadedLevel();
            if (IsValid(Loaded))
            {
                LevelsToCheck.Add(Loaded);
            }
        }
    }

    for (ULevel* Level : LevelsToCheck)
    {
        if (!IsValid(Level)) continue;

        for (AActor* Actor : Level->Actors)
        {
            if (!Actor) continue;

            TArray<UStaticMeshComponent*> MeshComponents;
            Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

            bool bHasEmptyMesh = false;
            for (UStaticMeshComponent* SMC : MeshComponents)
            {
                if (SMC && !SMC->GetStaticMesh())
                {
                    bHasEmptyMesh = true;
                    break;
                }
            }

            if (bHasEmptyMesh)
            {
                UE_LOG(LogTemp, Warning, TEXT("Destroying actor %s because it has empty StaticMeshComponent"), *Actor->GetName());
                Actor->Destroy();
            }
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Finished cleaning. Building lighting..."));

    // Guardar todos los niveles antes de build
    for (ULevel* Level : LevelsToCheck)
    {
        SaveLevelSilently(Level);
    }

    if (GEditor)
    {
        FLightingBuildOptions Options;
        GEditor->BuildLighting(Options);
    }
}

void ULevelLightingBuilder::SaveLevelSilently(ULevel* Level)
{
    if (!IsValid(Level)) return;

    // Esto guarda el nivel sin pedir diálogo
    if (!FEditorFileUtils::SaveLevel(Level))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save level: %s"), *Level->GetOutermost()->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Level saved: %s"), *Level->GetOutermost()->GetName());
    }
}

void ULevelLightingBuilder::LogAllBrokenReferencesInWorld()
{
    if (!GEditor) return;

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return;

    const FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("BrokenReferences.log");
    FString TotalLog;
    TotalLog += FString::Printf(TEXT("=== Broken References Scan - %s ===\n"), *FDateTime::Now().ToString());

    TArray<ULevel*> LevelsToCheck;
    LevelsToCheck.Add(World->PersistentLevel);

    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel)
        {
            ULevel* Loaded = StreamingLevel->GetLoadedLevel();
            if (IsValid(Loaded))
            {
                LevelsToCheck.Add(Loaded);
            }
        }
    }

    for (ULevel* Level : LevelsToCheck)
    {
        if (!IsValid(Level)) continue;

        for (AActor* Actor : Level->Actors)
        {
            if (!IsValid(Actor)) continue;

            TArray<UActorComponent*> Components;
            Actor->GetComponents(Components);

            bool bActorHasBrokenRefs = false;

            for (UActorComponent* Comp : Components)
            {
                if (!Comp)
                {
                    bActorHasBrokenRefs = true;
                    TotalLog += FString::Printf(TEXT("Actor %s has nullptr component!\n"), *Actor->GetName());
                    continue;
                }

                // Comprobamos Outer
                UObject* Outer = Comp->GetOuter();
                if (!IsValid(Outer))
                {
                    bActorHasBrokenRefs = true;
                    TotalLog += FString::Printf(TEXT("Component %s of actor %s has invalid Outer!\n"), *Comp->GetName(), *Actor->GetName());
                }

                // Comprobamos propiedades UObject
                for (TFieldIterator<FObjectProperty> PropIt(Comp->GetClass()); PropIt; ++PropIt)
                {
                    FObjectProperty* ObjProp = *PropIt;
                    UObject* PropValue = ObjProp->GetObjectPropertyValue_InContainer(Comp);

                    if (!PropValue || !IsValid(PropValue))
                    {
                        bActorHasBrokenRefs = true;
                        FString PropName = ObjProp->GetName();
                        FString TargetName = PropValue ? PropValue->GetName() : TEXT("nullptr");
                        TotalLog += FString::Printf(TEXT("Property %s in component %s of actor %s is invalid (%s)\n"),
                            *PropName, *Comp->GetName(), *Actor->GetName(), *TargetName);
                    }
                }
            }

            if (bActorHasBrokenRefs)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("Actor with broken references: %s"), *Actor->GetName()));
            }
        }
    }

    // Guardar log completo
    FFileHelper::SaveStringToFile(TotalLog, *LogFilePath);
    UE_LOG(LogTemp, Display, TEXT("Broken references scan completed. Log saved to: %s"), *LogFilePath);
}

void ULevelLightingBuilder::CleanActorsWithEmptyMeshes()
{
    if (!GEditor) return;
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return;

    TArray<ULevel*> LevelsToCheck;
    LevelsToCheck.Add(World->PersistentLevel);
    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (StreamingLevel && StreamingLevel->GetLoadedLevel())
        {
            LevelsToCheck.Add(StreamingLevel->GetLoadedLevel());
        }
    }

    for (ULevel* Level : LevelsToCheck)
    {
        if (!IsValid(Level)) continue;

        for (AActor* Actor : Level->Actors)
        {
            if (!IsValid(Actor)) continue;

            TArray<UStaticMeshComponent*> MeshComponents;
            Actor->GetComponents<UStaticMeshComponent>(MeshComponents);

            if (MeshComponents.Num() == 0) continue;

            int32 ValidMeshes = 0;
            for (UStaticMeshComponent* SMC : MeshComponents)
            {
                if (SMC && SMC->GetStaticMesh())
                {
                    ValidMeshes++;
                }
            }

            if (ValidMeshes == 0)
            {
                // El actor tiene solo StaticMeshComponents vacíos → eliminar actor
                UE_LOG(LogTemp, Warning, TEXT("Destroying actor %s because all StaticMeshComponents are empty"), *Actor->GetName());
                Actor->Destroy();
            }
            else
            {
                // Hay algunos meshes válidos, eliminar solo los vacíos
                for (UStaticMeshComponent* SMC : MeshComponents)
                {
                    if (SMC && !SMC->GetStaticMesh())
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Destroying component %s in actor %s because it has empty mesh"), *SMC->GetName(), *Actor->GetName());
                        SMC->DestroyComponent();
                    }
                }
            }
        }
    }
}

void ULevelLightingBuilder::ListActorsWithBrokenLightmaps()
{
    if (!GEditor) return;

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World) return;

    FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("BrokenLightmapActors.log");
    FString TotalLog;

    TotalLog += FString::Printf(TEXT("Broken Lightmap Actors Scan - %s\n"), *FDateTime::Now().ToString());

    TArray<ULevel*> LevelsToCheck;
    LevelsToCheck.Add(World->PersistentLevel);

    // Añadimos subniveles cargados
    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (!StreamingLevel) continue;
        ULevel* Loaded = StreamingLevel->GetLoadedLevel();
        if (IsValid(Loaded))
        {
            LevelsToCheck.Add(Loaded);
        }
    }

    for (ULevel* Level : LevelsToCheck)
    {
        if (!IsValid(Level)) continue;

        for (AActor* Actor : Level->Actors)
        {
            if (!IsValid(Actor)) continue;

            TArray<UStaticMeshComponent*> SMComponents;
            Actor->GetComponents<UStaticMeshComponent>(SMComponents);

            for (UStaticMeshComponent* SMComp : SMComponents)
            {
                if (!SMComp || !SMComp->GetStaticMesh()) continue;

                UStaticMesh* StaticMesh = SMComp->GetStaticMesh();

                if (!StaticMesh->GetRenderData() || StaticMesh->GetRenderData()->LODResources.Num() == 0) continue;

                const FStaticMeshLODResources& LOD = StaticMesh->GetRenderData()->LODResources[0];
                int32 NumUVs = LOD.GetNumTexCoords();
                int32 LightmapIndex = StaticMesh->GetLightMapCoordinateIndex();

                if (LightmapIndex >= NumUVs)
                {
                    TotalLog += FString::Printf(TEXT("Actor=%s | Mesh=%s | MeshPath=%s | Level=%s | Problems: BadLightmapUVIndex: %d (NumUVs=%d)\n"),
                        *Actor->GetName(),
                        *StaticMesh->GetName(),
                        *StaticMesh->GetPathName(),  // <-- ruta completa
                        *Level->GetName(),
                        LightmapIndex,
                        NumUVs
                    );
                }
            }
        }
    }

    FFileHelper::SaveStringToFile(TotalLog, *LogFilePath);
    UE_LOG(LogTemp, Display, TEXT("Broken lightmap scan completed. Log saved to: %s"), *LogFilePath);
}

#endif
