#include "ModularBuildingGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

AModularBuildingGenerator::AModularBuildingGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    BandOffset = FVector(-100.f,0.f,200.f);
    RoofCornerLocalOffset = FVector(201.0f,-200.0f,231.0f);
    DoorOffset = FVector(25.f, -20.f, 0.f);

    BalconyRightOffset = FVector(110.f,0.f,280.f);
    BalconyLeftOffset = BalconyRightOffset;

    Depth = 4;
    Width = 2;
    Height = 2;
    BigWallDefaultLength = 400.0f;
    SmallWallDefaultLength = 200.0f;
    WidthInUnits = Width*SmallWallDefaultLength;
    DepthInUnits = Depth*SmallWallDefaultLength;

    WindowChance = 0.2f;
    WallHeight = 400.0f;
    WallThickness = 20.0f;
    DoorChance = 0.2f;
    RoofPieceOffset = FVector(-198.633538f, -199.345655f, 228.704557f);
    RoofCornerLocalRotation = FRotator(0.f, 90.f, 0.0f);
    SeedValue = 12345;
    bEnableBalconies = true;
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> LargeWallMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Wall_Plain_400x400.SM_Wall_Plain_400x400"));
    if (LargeWallMeshObj.Succeeded())
    {
        LargeWallMesh = LargeWallMeshObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SmallWallMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Wall_Plain_400x200.SM_Wall_Plain_400x200"));
    if (SmallWallMeshObj.Succeeded())
    {
        SmallWallMesh = SmallWallMeshObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CornerMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Wall_CornerRocks.SM_Wall_CornerRocks"));
    if (CornerMeshObj.Succeeded())
    {
        CornerMesh = CornerMeshObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> RoofMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Roof_Rooftiles.SM_Roof_Rooftiles"));
    if (RoofMeshObj.Succeeded())
    {
        RoofMesh = RoofMeshObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> RoofCornerMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Roof_RooftilesCorner02.SM_Roof_RooftilesCorner02"));
    if (RoofCornerMeshObj.Succeeded())
    {
        RoofCornerMesh = RoofCornerMeshObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> BandMesh200Obj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Wall_HorizontalOrnaments.SM_Wall_HorizontalOrnaments"));
    if (BandMesh200Obj.Succeeded())
    {
        BandMesh200 = BandMesh200Obj.Object;
    }


    static ConstructorHelpers::FObjectFinder<UStaticMesh> WindowMeshobj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Wall_Window_LongCurved.SM_Wall_Window_LongCurved"));
    if (WindowMeshobj.Succeeded())
    {
        WindowMesh = WindowMeshobj.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorWallMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_Wall_DoorHoleSingle01.SM_Wall_DoorHoleSingle01"));
    if (DoorWallMeshObj.Succeeded())
    {
        DoorWallMesh = DoorWallMeshObj.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMeshObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_DoorSingle01.SM_DoorSingle01"));
    if (DoorMeshObj.Succeeded())
    {
        DoorMesh = DoorMeshObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> BalconyCornerObj(TEXT("/Game/Meshes/StaticMeshes/Buildings/Modules/Streets/SM_SpiralFence_Corner.SM_SpiralFence_Corner"));
    if (BalconyCornerObj.Succeeded())
    {
        BalconyCornerMesh = BalconyCornerObj.Object;
    }
}

void AModularBuildingGenerator::ClearInstances()
{
    for (auto& Elem : ISMMap)
    {
        if (Elem.Value)
        {
            Elem.Value->ClearInstances();
            Elem.Value->DestroyComponent();
        }
    }
    ISMMap.Empty();
}

UInstancedStaticMeshComponent* AModularBuildingGenerator::GetOrCreateISM(UStaticMesh* Mesh, TMap<UStaticMesh*, UInstancedStaticMeshComponent*>& Map)
{
    if (!IsValid(Mesh) || !Mesh->IsValidLowLevelFast())
    {
        return nullptr;
    }

    if (!Map.Contains(Mesh))
    {
        FString Name = FString::Printf(TEXT("ISM_%s"), *Mesh->GetName());
        UInstancedStaticMeshComponent* NewComp = NewObject<UInstancedStaticMeshComponent>(this, *Name);
        NewComp->SetupAttachment(RootComponent);
        NewComp->SetStaticMesh(Mesh);
        NewComp->RegisterComponent();
        Map.Add(Mesh, NewComp);
    }

    return Map[Mesh];
}

void AModularBuildingGenerator::GenerateWallLine(const FVector& Start, const FVector& Direction, const float Length, const FRotator& Rotation)
{
    float Position = 0.0f; 

    while (Position < Length - 1.0f) 
    {
        float Remaining = Length - Position;
        float WallWidth = (Remaining >= BigWallDefaultLength && FMath::RandBool()) ? BigWallDefaultLength : SmallWallDefaultLength;

        FVector Offset = Direction * (Position + WallWidth);

        for (int32 Z = 0; Z < Height; ++Z)
        {
            UStaticMesh* MeshToUse = nullptr;

            if (WallWidth == BigWallDefaultLength)
            {
                MeshToUse = LargeWallMesh;
            }
            else
            {
                bool bUseDoor = (Z == 0 && DoorWallMesh && FMath::FRand() < DoorChance);
                bool bUseWindow = (WindowMesh && !bUseDoor && FMath::FRand() < WindowChance);
                
                if (bEnableBalconies && Z > 0 && bUseWindow && IsValid(BalconyCornerMesh))
                {
                    FVector BalconyLocation = Start + Offset;
                    BalconyLocation.Z += Z * WallHeight;
                    FVector LeftOffset = Rotation.RotateVector(BalconyLeftOffset);
                    FVector RightOffset = Rotation.RotateVector(BalconyRightOffset);
                    FRotator RotationOffsetJustForThePiece = FRotator(0.f, -90.f, 0.f);
                    
                    FTransform LeftCornerTransform(Rotation, BalconyLocation + LeftOffset);
                    FTransform RightCornerTransform(
                        Rotation + RotationOffsetJustForThePiece, 
                        BalconyLocation + RightOffset
                    );

                    if (UInstancedStaticMeshComponent* BalconyISM = GetOrCreateISM(BalconyCornerMesh, ISMMap))
                    {
                        BalconyISM->AddInstance(LeftCornerTransform);
                        BalconyISM->AddInstance(RightCornerTransform);
                    }
                }
                if (bUseDoor)
                {
                    MeshToUse = DoorWallMesh;
                }
                else if (bUseWindow)
                {
                    MeshToUse = WindowMesh;
                }
                else
                {
                    MeshToUse = SmallWallMesh;
                }

                // Also spawn the actual door mesh when we use a door frame
                if (bUseDoor && IsValid(DoorMesh))
                {
                    FVector DoorLocation = Start + Offset;
                    DoorLocation.Z += WallHeight / 2.0f;
                    DoorLocation += Rotation.RotateVector(DoorOffset);

                    if (UInstancedStaticMeshComponent* DoorISM = GetOrCreateISM(DoorMesh, ISMMap))
                    {
                        FTransform DoorTransform(Rotation, DoorLocation);
                        DoorISM->AddInstance(DoorTransform);
                    }
                }
            }

            if (!IsValid(MeshToUse)) continue;

            FVector Location = Start + Offset;
            Location.Z += Z * WallHeight + WallHeight / 2.0f;

            UInstancedStaticMeshComponent* ISM = GetOrCreateISM(MeshToUse, ISMMap);
            if (!IsValid(ISM)) continue;

            ISM->AddInstance(FTransform(Rotation, Location));
        }

        Position += WallWidth;
    }
}

void AModularBuildingGenerator::SpawnCornerAt(const FVector& Location, const FRotator& Rotation)
{
    UInstancedStaticMeshComponent* ISM = GetOrCreateISM(CornerMesh, ISMMap);
    if (!IsValid(ISM)) return;

    for (int32 Z = 0; Z < Height; ++Z)
    {
        FVector Pos = Location + FVector(0, 0, Z * WallHeight + WallHeight / 2.f);
        ISM->AddInstance(FTransform(Rotation, Pos));
    }
}

void AModularBuildingGenerator::SpawnRoofCorner(const FVector& BaseCorner, const FRotator& Rotation)
{
    UInstancedStaticMeshComponent* ISM = GetOrCreateISM(RoofCornerMesh, ISMMap);
    if (!IsValid(ISM)) return;

    FVector Offset = Rotation.RotateVector(RoofCornerLocalOffset);
    FVector Final = BaseCorner + Offset;
    Final.Z += Height * WallHeight + RoofCornerMesh->GetBounds().BoxExtent.Z;
    ISM->AddInstance(FTransform(Rotation, Final));
}

void AModularBuildingGenerator::SpawnRoofLine(const FVector& StartCorner, const FVector& EndCorner, const FRotator& Rotation)
{
    UInstancedStaticMeshComponent* ISM = GetOrCreateISM(RoofMesh, ISMMap);
    if (!IsValid(ISM)) return;

    FVector Direction = EndCorner - StartCorner;
    float TotalLength = Direction.Size();
    Direction.Normalize();

    int TileCount = FMath::FloorToInt(TotalLength / SmallWallDefaultLength);
    FVector Step = Direction * SmallWallDefaultLength;
    FVector LocalOffset = Rotation.RotateVector(RoofPieceOffset);
    float ZOffset = 0.0f;
    if (IsValid(RoofMesh) && RoofMesh->IsValidLowLevelFast())
    {
        ZOffset = Height * WallHeight + RoofMesh->GetBounds().BoxExtent.Z;
    }

    for (int i = 1; i < TileCount - 1; ++i)
    {
        FVector Loc = StartCorner + Step * i + LocalOffset;
        Loc.Z += ZOffset;
        ISM->AddInstance(FTransform(Rotation, Loc));
    }
}

void AModularBuildingGenerator::SpawnFloorBandLine(const FVector& Start, const FVector& Direction, const float Length, const float ZHeight, const FRotator& Rotation)
{
    float Position = 0.0f;

    while (Position < Length - 1.0f)
    {
        float Remaining = Length - Position;
        float TrimWidth = (Remaining >= BigWallDefaultLength && FMath::RandBool()) ? SmallWallDefaultLength : SmallWallDefaultLength; //only using one type of trim but might have a bigger one later so the code is already there thank you

        UStaticMesh* Mesh = (TrimWidth == BigWallDefaultLength) ? BandMesh400 : BandMesh200;
        UInstancedStaticMeshComponent* ISM = GetOrCreateISM(Mesh, ISMMap);
        if (!IsValid(ISM)) return;

        FVector Offset = Direction * (Position + TrimWidth);
        FVector OriginFix = Direction * (-TrimWidth / 2.f);
        FVector Location = Start + Offset + OriginFix;
        Location += Rotation.RotateVector(BandOffset);
        Location.Z += ZHeight;

        ISM->AddInstance(FTransform(Rotation, Location));

        Position += TrimWidth;
    }
}

void AModularBuildingGenerator::ConvertAllISMsToStaticMeshActors()
{
    UWorld* World = GetWorld();
    if (!IsValid(World)) return;

    for (auto& Elem : ISMMap)
    {
        UInstancedStaticMeshComponent* ISM = Elem.Value;
        UStaticMesh* Mesh = Elem.Key;

        if (!IsValid(ISM) || !IsValid(Mesh)) continue;

        int32 Count = ISM->GetInstanceCount();
        for (int32 i = 0; i < Count; ++i)
        {
            FTransform InstanceTransform;
            ISM->GetInstanceTransform(i, InstanceTransform, true);

            if (AStaticMeshActor* NewActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), InstanceTransform))
            {
                NewActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
                #if WITH_EDITOR
                NewActor->SetActorLabel(FString::Printf(TEXT("%s_Converted_%d"), *Mesh->GetName(), i));
                #endif
                NewActor->Tags.Add(FName("ConvertedByModularGen"));
            }
        }
        ISM->DestroyComponent();
        ISM = nullptr;  // Explicitly null it to avoid any accidental use later.
    }

    ISMMap.Empty(); // Cleanup map
}

void AModularBuildingGenerator::BeginPlay()
{
    GenerateBuilding();
}

void AModularBuildingGenerator::RandomizeAndGenerate()
{
    SeedValue = FMath::RandRange(10000, 10000000);
    GenerateBuilding();
}

void AModularBuildingGenerator::GenerateBuilding()
{
    FMath::RandInit(SeedValue);
    ClearInstances();
    WidthInUnits = Width*SmallWallDefaultLength;
    DepthInUnits = Depth*SmallWallDefaultLength;
    float HalfWidth = WidthInUnits / 2.0f;
    float HalfDepth = DepthInUnits / 2.0f;

    FVector CornerA(-HalfWidth, -HalfDepth, 0);
    FVector CornerB(HalfWidth, -HalfDepth, 0);
    FVector CornerC(HalfWidth, HalfDepth, 0);
    FVector CornerD(-HalfWidth, HalfDepth, 0);

    GenerateWallLine(CornerA, FVector(1, 0, 0), WidthInUnits, FRotator(0, 180, 0));
    GenerateWallLine(CornerB, FVector(0, 1, 0), DepthInUnits, FRotator(0, -90, 0));
    GenerateWallLine(CornerC, FVector(-1, 0, 0), WidthInUnits);
    GenerateWallLine(CornerD, FVector(0, -1, 0), DepthInUnits, FRotator(0, 90, 0));

    SpawnCornerAt(CornerA, FRotator(0, 0, 0));
    SpawnCornerAt(CornerB, FRotator(0, 0, 0));
    SpawnCornerAt(CornerC, FRotator(0, 0, 0));
    SpawnCornerAt(CornerD, FRotator(0, 0, 0));

    SpawnRoofCorner(CornerA, RoofCornerLocalRotation);
    SpawnRoofCorner(CornerB, FRotator(0, 90, 0) + RoofCornerLocalRotation);
    SpawnRoofCorner(CornerC, FRotator(0, 180, 0) + RoofCornerLocalRotation);
    SpawnRoofCorner(CornerD, FRotator(0, -90, 0) + RoofCornerLocalRotation);

    SpawnRoofLine(CornerA, CornerB, FRotator(0, 180, 0));
    SpawnRoofLine(CornerB, CornerC, FRotator(0, -90, 0));
    SpawnRoofLine(CornerC, CornerD, FRotator(0, 0, 0));
    SpawnRoofLine(CornerD, CornerA, FRotator(0, 90, 0));

    for (int32 Z = 1; Z < Height; ++Z)
    {
        float ZHeight = Z * WallHeight;
        SpawnFloorBandLine(CornerA, FVector(1, 0, 0), WidthInUnits, ZHeight, FRotator(0, 180, 0));
        SpawnFloorBandLine(CornerB, FVector(0, 1, 0), DepthInUnits, ZHeight, FRotator(0, -90, 0));
        SpawnFloorBandLine(CornerC, FVector(-1, 0, 0), WidthInUnits, ZHeight, FRotator(0, 0, 0));
        SpawnFloorBandLine(CornerD, FVector(0, -1, 0), DepthInUnits, ZHeight, FRotator(0, 90, 0));
    }
    
}
