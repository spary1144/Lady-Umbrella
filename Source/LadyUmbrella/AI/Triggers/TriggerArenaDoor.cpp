// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerArenaDoor.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"

// Sets default values
ATriggerArenaDoor::ATriggerArenaDoor()
: PlayRate(1.5f),
  bKeepDoorOpen(false),
  bNotCheckIfHasKeyTheFirstTime(false),
  bIsOpen(false),
  bStartClose(false),
  FinalRotation(FRotator(0.f,0.f,0.f)),
  FinalRotationValue(0.f),
  RotationElapsed(0.f)
{
	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DoorRoot);
	
	DoorHole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorHole"));
	DoorHole->SetupAttachment(DoorRoot);

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(DoorHole);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(DoorHole);

	//create collision channel for player
	BlockingBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBoxCollider"));
	BlockingBoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockingBoxCollider->SetupAttachment(DoorHole);
	BlockingBoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Environment/Doors/C_DoorArenaOpen.C_DoorArenaOpen'"));
	if (CurveAsset.Succeeded())
	{
		RotationCurve= CurveAsset.Object;
	}
	IsActive = true;
}

void ATriggerArenaDoor::SetOwnerArena(AArenaManager* NewOwnerArena)
{
	OwnerArena = NewOwnerArena;
	if (IsValid(OwnerArena))
	{
		OwnerArena->OnResetTriggersDelegate.AddUObject(this,&ThisClass::Reset);
	}
}

void ATriggerArenaDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
}

// Called when the game starts or when spawned
void ATriggerArenaDoor::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(BoxCollider))
	{
		return;
	}
	
	if (!IsActive)
	{
		return;
	}
	
	if (!BoxCollider->OnComponentBeginOverlap.IsAlreadyBound(this, &ThisClass::OnBeginOverlap))
	{
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	}
	if (!BoxCollider->OnComponentEndOverlap.IsAlreadyBound(this, &ThisClass::OnEndOverlap))
	{
		BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
	}
}

bool ATriggerArenaDoor::HasKey(AActor* OtherActor)
{
	if (bNotCheckIfHasKeyTheFirstTime)
	{
		return true;
	}

	bNotCheckIfHasKeyTheFirstTime = false;
	
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
	if (!IsValid(Enemy))
	{
		return false;
	}
	
	return Enemy->GetUnlockID() == GetUniqueID();
}

void ATriggerArenaDoor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(RotationCurve) || !OtherActor->IsA(AEnemyCharacter::StaticClass()) || !IsValid(BlockingBoxCollider))
	{
		return;
	}

	//Check if the enemy overlaping has the correct key
	if (!HasKey(OtherActor) || bStartClose)
	{
		return;
	}
	
	//Play Open Rotation Door
	//get keys
	const TArray<FRichCurveKey>& Keys = RotationCurve->FloatCurve.Keys;
	//get last key time
	if (Keys.Num() > 0)
	{
		const FRichCurveKey& LastKey = Keys.Last();
		PlayRate = LastKey.Time;
		FinalRotationValue = LastKey.Value;
		InitialRotation = Door->GetRelativeRotation();
		FinalRotation = InitialRotation + FRotator(0.f,FinalRotationValue,0.f);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		RotationTimer,
		this,
		&ATriggerArenaDoor::RotateActorDynamic,
		InRate,
		true
	);
	
}

void ATriggerArenaDoor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(AEnemyCharacter::StaticClass()) || bKeepDoorOpen || !IsValid(GetWorld()))
	{
		return;
	}

	if (!HasKey(OtherActor) || bStartClose)
	{
		return;
	}
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(RotationTimer);
	RotationElapsed = 0.f;
	bStartClose = true;
	TimerManager.SetTimer(
		RotationTimer,
		this,
		&ATriggerArenaDoor::RotateActorDynamicInverse,
		InRate,
		true
	);
}

void ATriggerArenaDoor::RotateActorDynamic()
{
	if (!RotationTimer.IsValid() || !IsValid(GetWorld()))
	{
		return;
	}
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	RotationElapsed += TimerManager.GetTimerElapsed(RotationTimer);

	//const float Alpha = RotationElapsed;
	FRotator CurrentRotation = FRotator::ZeroRotator, NewRotation = FRotator::ZeroRotator; 
	SetNewRotation(RotationElapsed,CurrentRotation,NewRotation);
	
	if (FMath::IsNearlyEqual(NewRotation.Yaw, FinalRotation.Yaw) || bStartClose)
	{
		bIsOpen = true;
		TimerManager.ClearTimer(RotationTimer);
	}
}

void ATriggerArenaDoor::RotateActorDynamicInverse()
{
	if (!RotationTimer.IsValid() || !IsValid(GetWorld()) || IsPendingKillPending())
	{
		return;
	}
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	RotationElapsed += TimerManager.GetTimerElapsed(RotationTimer);

	const float Alpha = FMath::Max(PlayRate - RotationElapsed,0.f);

	FRotator CurrentRotation;
	FRotator NewRotation; 
	SetNewRotation(Alpha,CurrentRotation,NewRotation);

	if(FMath::IsNearlyEqual(NewRotation.Yaw, InitialRotation.Yaw))
	{
		//Door->SetRelativeRotation(NewRotation);
		bIsOpen = false;
		TimerManager.ClearTimer(RotationTimer);
	}
}

void ATriggerArenaDoor::SetNewRotation(const float Alpha,FRotator& CurrentRotation,FRotator& NewRotation) const
{
	if (!IsValid(RotationCurve) || !IsValid(Door))
	{
		return;
	}
	
	const float CurveValue = RotationCurve->GetFloatValue(Alpha);
	
	NewRotation = FRotator(0.f,CurveValue,0.f);
	
	Door->SetRelativeRotation(NewRotation);
}

void ATriggerArenaDoor::Reset()
{
	bStartClose = false;
}



