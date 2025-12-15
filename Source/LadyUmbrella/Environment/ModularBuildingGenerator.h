#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularBuildingGenerator.generated.h"

UCLASS()
class LADYUMBRELLA_API AModularBuildingGenerator : public AActor
{
    GENERATED_BODY()

public:
    AModularBuildingGenerator();

    UFUNCTION(CallInEditor, Category = "Conversion")
    void ConvertAllISMsToStaticMeshActors();

protected:
    virtual void BeginPlay() override;
    void ClearInstances();

    void GenerateWallLine(const FVector& Start, const FVector& Direction, const float Length, const FRotator& Rotation = FRotator::ZeroRotator);
    void SpawnCornerAt(const FVector& Location, const FRotator& Rotation);
    void SpawnRoofCorner(const FVector& BaseCorner, const FRotator& Rotation);
    void SpawnRoofLine(const FVector& StartCorner, const FVector& EndCorner, const FRotator& Rotation);
    void SpawnFloorBandLine(const FVector& Start, const FVector& Direction, float Length, float ZHeight, const FRotator& Rotation);

    UFUNCTION(CallInEditor, Category="Building")
    void RandomizeAndGenerate();
    
    UFUNCTION(CallInEditor, Category="Building")
    void GenerateBuilding();
    
    UInstancedStaticMeshComponent* GetOrCreateISM(UStaticMesh* Mesh, TMap<UStaticMesh*, UInstancedStaticMeshComponent*>& Map);

private:

    // Static Meshes for each part of the building
    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> LargeWallMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> SmallWallMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> WindowMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> BalconyCornerMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> DoorWallMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> DoorMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> CornerMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> RoofMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> RoofCornerMesh;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TObjectPtr<UStaticMesh> BandMesh200;
    
    TObjectPtr<UStaticMesh> BandMesh400;

    UPROPERTY(EditAnywhere, Category = "Building")
    bool bEnableBalconies;

    UPROPERTY(EditAnywhere, Category = "Building", meta = (ClampMin = "1", ClampMax = "10"))
    int32 Width;

    UPROPERTY(EditAnywhere, Category = "Building", meta = (ClampMin = "1", ClampMax = "10"))
    int32 Depth;

    UPROPERTY(EditAnywhere, Category = "Building")
    int32 Height;

    UPROPERTY(EditAnywhere, Category = "Building")
    float WindowChance;

    UPROPERTY(EditAnywhere, Category = "Building")
    float WallHeight;

    UPROPERTY(EditAnywhere, Category = "Building")
    float DoorChance;


    
    FVector RoofPieceOffset;
    FVector DoorOffset;
    FVector BalconyLeftOffset;
    FVector BalconyRightOffset;
    FVector RoofCornerLocalOffset;
    FVector BandOffset;
    FRotator RoofCornerLocalRotation;

    int32 SeedValue;
    int32 WidthInUnits;
    int32 DepthInUnits;
    
    float WallThickness;
    float BigWallDefaultLength;
    float SmallWallDefaultLength;

    TMap<UStaticMesh*, UInstancedStaticMeshComponent*> ISMMap;
};
