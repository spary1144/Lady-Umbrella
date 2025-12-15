#include "AimAssistComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FUmbrellaDataDriven.h"

float UAimAssistComponent::GetUmbrellaRangeFromData() const
{
      
    FDataTableRowHandle DataTableRowHandler;
    DataTableRowHandler.DataTable = FDatatableAssets::UmbrellaSystemData;

    if (IsValid(DataTableRowHandler.DataTable))
    {
        DataTableRowHandler.RowName = "BP_Umbrella";
    }
    if (const FUmbrellaDataDriven* UmbrellaData = FDataTableHelpers::ValidateTableAndGetData<FUmbrellaDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName))
    {
        return UmbrellaData->ShotgunMaxRange;
    }
    return BASE_RANGE;
}

UAimAssistComponent::UAimAssistComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    
    
    // Snapping
    CapsuleRadius  = 150.0f;
    CapsuleLength  = GetUmbrellaRangeFromData();
    AimAssistSpeed = 15.0f;
    AimAssistSpeedMultiplier = 1.0f;
    AimAngleThreshold = 0.5f;
    TargetOffset = FVector(0.f, 0.f, 60.f);
    ObjectTargetSpeedMultiplier = 0.25;
    bDoObjectSnapping = false;
    
    // Soft Lock
    SoftLockAngle = 5.0f;
    SoftLockSlowMultiplier = 0.5f;
    bShouldSlowAiming = false;
    bIsAiming = false;
    SoftLockRadius = 100.0f;
    SoftLockLength = 2000.0f;
    SoftLockMultiplierActors = 0.5f;
    bDoObjectSoftLock = true;
    
    // Debug
    bShowDebugSnap = false;
    bShowDebugSoftLock = false;
    bShowDebugVisibility = false;
    bDebugMessages = false;

    // Internal
    CurrentTarget = nullptr;
    CameraSnapStartOffset = 500.0f;
    StopFollowingMaxTime = 1.0f;
    CameraInputBreakThreshold = 0.05f;

    
    
    bTargetIsActor = false;
}



void UAimAssistComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAimAssistComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const APawn* OwnerPawn = Cast<APawn>(GetOwner());
    APlayerController* PC  = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
    if (!IsValid(PC)) return;

    if (!bIsHoldingAimInput)
    {
        SetCameraMultiplier(PC, 1.0f);
        return;
    }

    const bool bHasActorTarget     = (CurrentTarget && !IsTargetDead());
    const bool bHasComponentTarget = IsValid(CurrentHookPoint);

    if (bIsAiming && (bHasActorTarget || bHasComponentTarget))
    {
        if (!DidPlayerMoveCamera(PC))
        {
            if (bTargetIsActor)
            {
                RotateViewToTarget(PC, CurrentTarget->GetActorLocation(), DeltaTime);
            }
            else if (bHasComponentTarget && bDoObjectSnapping)
            {
                RotateViewToTarget(PC, CurrentHookPoint->GetComponentLocation(), DeltaTime);
            }
        }
        else
        {
            CurrentTarget     = nullptr;
            CurrentHookPoint  = nullptr;
            bIsAiming         = false;
            ClearLockAndTimers();
        }
    }
    else
    {
        CheckForSoftLock();
    }
}

bool UAimAssistComponent::IsTargetDead()
{
    AGenericCharacter* CharacterTarget = Cast<AGenericCharacter>(CurrentTarget);
    if (IsValid(CharacterTarget))
    {
        return CharacterTarget->GetHealthComponent()->IsDead();
    }
    return false;
}

void UAimAssistComponent::OnAimInputPressed()
{
	bIsHoldingAimInput = true;
	OnStartAiming();
}

void UAimAssistComponent::OnAimInputReleased()
{
	bIsHoldingAimInput = false;
	OnEndAiming();
	StopRotation();
}

void UAimAssistComponent::StartFollowTimer()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            AimFollowTimer,
            this,
            &UAimAssistComponent::StopRotation,
            StopFollowingMaxTime,
            true
        );
    }
}

void UAimAssistComponent::OnStartAiming()
{
    APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
    if (!PC) return;

    FVector CamLoc; FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);
    const FVector CamDir = CamRot.Vector();

    const TArray<FHitResult> Hits   = PerformSphereTrace(CapsuleRadius, CapsuleLength, bShowDebugSnap);
    const TArray<AGenericCharacter*> Enemies = FilterUniqueEnemies(Hits);

    if (Enemies.Num() > 0)
    {
        bTargetIsActor   = true;
        CurrentHookPoint = nullptr;

        AGenericCharacter* Potential = FindBestTarget(Enemies, CamLoc, CamDir);

        // LoS against visibility
        FHitResult Hit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(AimAssistVisibilityTrace), true, GetOwner());
        Params.AddIgnoredActor(GetOwner());

        const FVector TraceStart = CamLoc;
        const FVector TraceEnd   = Potential->GetActorLocation() + TargetOffset;

        const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
        const bool bVisible = !bBlocked || Hit.GetActor() == Potential;

        if (bVisible)
        {
            CurrentTarget = Potential;
            StartFollowTimer();
            if (bShowDebugVisibility) DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.5f, 0, 2.f);
        }
        else
        {
            CurrentTarget = nullptr;
            if (bShowDebugVisibility)
            {
                DrawDebugLine(GetWorld(), TraceStart, Hit.Location, FColor::Red, false, 1.5f, 0, 2.f);
                DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Red, false, 1.5f);
            }
        }
    }
    else
    {
        bTargetIsActor  = false;
        CurrentTarget   = nullptr;

        for (const FHitResult& Hit : Hits)
        {
            const AActor* A = Hit.GetActor();
            UPrimitiveComponent* Comp = Hit.GetComponent();

            if (!IsValid(A) || !IsValid(Comp))
               
                continue;
            
            if(bDebugMessages)
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("[AimAssist] Hook cand -> Comp=%s (Class=%s), Owner=%s, ObjType=%d, Bone=%s, Item=%d, Loc=%s"),
                    *GetNameSafe(Comp),
                    *Comp->GetClass()->GetName(),
                    *GetNameSafe(A),
                    (int32)Comp->GetCollisionObjectType(),
                    *Hit.BoneName.ToString(),
                    Hit.Item,
                    *Comp->GetComponentLocation().ToCompactString()
                );
    
                if (GEngine && false)
                {
                    GEngine->AddOnScreenDebugMessage(
                        -1, 2.f, FColor::Cyan,
                        FString::Printf(TEXT("Hook: %s on %s"),
                            *GetNameSafe(Comp), *GetNameSafe(A))
                    );
                }
    
                DrawDebugString(
                    GetWorld(),
                    Comp->GetComponentLocation(),
                    Comp->GetName(),             
                    nullptr,
                    FColor::Green,
                    1.5f,
                    true
                );
            }

            CurrentHookPoint = Comp;
            StartFollowTimer();
            break;
        }
    }

    bIsAiming = true;
}

void UAimAssistComponent::OnEndAiming()
{
    bIsAiming = false;
}

TArray<FHitResult> UAimAssistComponent::PerformSphereTrace(float TraceRadius, float TraceLength, bool bShowDebug)
{
    TArray<FHitResult> OutHits;
    if (TraceRadius <= 0.f || TraceLength <= 0.f) return OutHits;

    const APawn* OwnerPawn = Cast<APawn>(GetOwner());
    const APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
    if (!PC) return OutHits;

    FVector CamLoc; FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    const FVector Start = CamLoc + (CamRot.Vector() * CameraSnapStartOffset);
    const FVector End   = Start  + (CamRot.Vector() * TraceLength);

    TArray<AActor*> Ignore; Ignore.Add(const_cast<APawn*>(OwnerPawn));
    const EDrawDebugTrace::Type DebugType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    UKismetSystemLibrary::SphereTraceMulti(
        this,
        Start,
        End,
        TraceRadius,
        UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel6),
        false,                    
        Ignore,
        DebugType,
        OutHits,
        true                        
    );

    return OutHits;
}

TArray<AGenericCharacter*> UAimAssistComponent::FilterUniqueEnemies(const TArray<FHitResult>& HitResults)
{
    TArray<AGenericCharacter*> UniqueEnemies;

    for (const FHitResult& Hit : HitResults)
    {
        AGenericCharacter* Enemy = Cast<AGenericCharacter>(Hit.GetActor());
        if (Enemy && !UniqueEnemies.Contains(Enemy))
        {
            UniqueEnemies.Add(Enemy);
        }
    }

    return UniqueEnemies;
}

AGenericCharacter* UAimAssistComponent::FindBestTarget(const TArray<AGenericCharacter*>& Enemies, const FVector& ViewLocation, const FVector& ViewDirection)
{
    AGenericCharacter* BestTarget = nullptr;
    float SmallestAngle = MAX_flt;

    for (AGenericCharacter* Enemy : Enemies)
    {
        FVector ToEnemy = (Enemy->GetActorLocation() - ViewLocation).GetSafeNormal();
        float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ViewDirection, ToEnemy)));

        if (Angle < SmallestAngle)
        {
            SmallestAngle = Angle;
            BestTarget = Enemy;
        }
    }

    return BestTarget;
}

void UAimAssistComponent::RotateViewToTarget(APlayerController* PC, const FVector& RawTargetLocation, float DeltaTime)
{
    FVector CamLoc; FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    // Only offset actor targets; components already pass exact point
    const FVector AimTarget = bTargetIsActor ? (RawTargetLocation + TargetOffset) : RawTargetLocation;

    const FVector To = (AimTarget - CamLoc).GetSafeNormal();
    const FRotator Want = To.Rotation();

    const float Cos = FVector::DotProduct(CamRot.Vector(), To);
    const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Cos, -1.f, 1.f)));

    const float Speed = bTargetIsActor ? AimAssistSpeed + (AngleDeg * AimAssistSpeedMultiplier) : ObjectTargetSpeedMultiplier * (AimAssistSpeed + (AngleDeg * AimAssistSpeedMultiplier));
    const FRotator NewRot = FMath::RInterpTo(CamRot, Want, DeltaTime, Speed);
    PC->SetControlRotation(NewRot);
}

void UAimAssistComponent::StopRotation()
{
    CurrentTarget = nullptr;
    CurrentHookPoint = nullptr;
    GetWorld()->GetTimerManager().ClearTimer(AimFollowTimer);
    if (APlayerController* PC = FGlobalPointers::MainController)
    {
        SetCameraMultiplier(PC, 1.0f);
    }
}

void UAimAssistComponent::CheckForSoftLock()
{
    if (APlayerController* PC = FGlobalPointers::MainController)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
        FVector ViewDirection = CameraRotation.Vector();

        TArray<FHitResult> Hits = PerformSphereTrace(SoftLockRadius, SoftLockLength, bShowDebugSoftLock);
        TArray<AGenericCharacter*> Enemies = FilterUniqueEnemies(Hits);

        bShouldSlowAiming = false;
        float NewMultiplier = 1.0f;

        if (Enemies.Num() > 0)
        {
            for (AGenericCharacter* Enemy : Enemies)
            {
                if (!IsValid(Enemy) || !IsValid(Enemy->GetHealthComponent()))
                {
                    continue;
                }
                
                if (IsValid(Enemy) && IsValid(Enemy->GetHealthComponent()) && !Enemy->GetHealthComponent()->IsDead())
                {
                   
                    float Angle =  GetSoftLockAngle(Enemy, CameraLocation, ViewDirection);

                    if (Angle <= SoftLockAngle)
                    {
                        bShouldSlowAiming = true;
                        NewMultiplier = SoftLockSlowMultiplier;
                        break;
                    }
                }
            }
        }
        else
        {
            for (FHitResult Hit: Hits)
            {
                AActor* Actor = Hit.GetActor();
                if (IsValid(Actor) && bDoObjectSoftLock)
                {
                    float Angle = GetSoftLockAngle(Actor, CameraLocation, ViewDirection);

                    if (Angle <= SoftLockAngle)
                    {
                        bShouldSlowAiming = true;
                        NewMultiplier = SoftLockMultiplierActors;
                        break;
                    }
                }
            }
        }
        SetCameraMultiplier(PC, NewMultiplier);
    }
}

float UAimAssistComponent::GetSoftLockAngle(const AActor* TargetActor, const FVector& CameraLocation, const FVector& ViewDirection) const
{
    FVector ToActor = (TargetActor->GetActorLocation() + TargetOffset - CameraLocation).GetSafeNormal();
    float Dot = FVector::DotProduct(ViewDirection, ToActor);
    return FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));
}

void UAimAssistComponent::SetCameraMultiplier(APlayerController* PC, float NewMult)
{
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn()))
    {
        PlayerCharacter->GetCameraSystemComponent()->SetCameraLookMultiplierAimAssist(NewMult);
    }
}

bool UAimAssistComponent::DidPlayerMoveCamera(APlayerController* PC) const
{
    if (!PC) return false;

    float MouseDX=0.f, MouseDY=0.f;
    PC->GetInputMouseDelta(MouseDX, MouseDY);
    const float MouseMag = FMath::Abs(MouseDX) + FMath::Abs(MouseDY);

    const float RX = PC->GetInputAnalogKeyState(EKeys::Gamepad_RightX);
    const float RY = PC->GetInputAnalogKeyState(EKeys::Gamepad_RightY);
    const float StickMag = FMath::Abs(RX) + FMath::Abs(RY);

    const float AxisTurn   = PC->GetInputAxisValue(TEXT("Turn"));
    const float AxisLookUp = PC->GetInputAxisValue(TEXT("LookUp"));
    const float AxisMag = FMath::Abs(AxisTurn) + FMath::Abs(AxisLookUp);

    return FMath::Max3(MouseMag, StickMag, AxisMag) > CameraInputBreakThreshold;
}

void UAimAssistComponent::ClearLockAndTimers()
{
    CurrentTarget    = nullptr;
    CurrentHookPoint = nullptr;
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(AimFollowTimer);
    }
    if (APlayerController* PC = FGlobalPointers::MainController)
    {
        SetCameraMultiplier(PC, 1.0f);
    }
}