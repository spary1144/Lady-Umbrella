// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerArenaWindow.h"

#include "Components/BoxComponent.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"

// Sets default values
ATriggerArenaWindow::ATriggerArenaWindow()
	: PlayRate(1.5),
      bNotCheckIfHasKeyTheFirstTime(false)
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(Root);
	
	WindowFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorHole"));
	WindowFrame->SetupAttachment(Root);

	ShutterLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShutterLeft"));
	ShutterLeft->SetupAttachment(WindowFrame);

	ShutterRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShutterRight"));
	ShutterRight->SetupAttachment(WindowFrame);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(WindowFrame);

	//create collision channel for player
	BlockingBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBoxCollider"));
	BlockingBoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockingBoxCollider->SetupAttachment(WindowFrame);

	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Environment/Windows/C_WindowArenaOpen.C_WindowArenaOpen'"));
	if (CurveAsset.Succeeded())
	{
		RotationCurve= CurveAsset.Object;
	}

}

void ATriggerArenaWindow::SetOwnerArena(AArenaManager* NewOwnerArena)
{
	OwnerArena = NewOwnerArena;
	if (IsValid(OwnerArena))
	{
		OwnerArena->OnResetTriggersDelegate.AddUObject(this,&ThisClass::Reset);
	}
}

// Called when the game starts or when spawned
void ATriggerArenaWindow::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(BoxCollider))
	{
		return;
	}
	
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

bool ATriggerArenaWindow::HasKey(AActor* OtherActor)
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

void ATriggerArenaWindow::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(AEnemyCharacter::StaticClass()) || !IsValid(RotationCurve) || !IsValid(BlockingBoxCollider))
	{
		return;
	}
	
	if (!HasKey(OtherActor) || bHasBegunClose)
	{
		return;
	}

	//enable collision in bloccking element and activate channel for player
	BlockingBoxCollider->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	BlockingBoxCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel15, ECR_Block);
	//Play Open Rotation Door
	//get keys
	const TArray<FRichCurveKey>& Keys = RotationCurve->FloatCurve.Keys;
	//get last key time
	if (Keys.Num() > 0)
	{
		const FRichCurveKey& LastKey = Keys.Last();
		PlayRate = LastKey.Time;
		FinalRotationValue = LastKey.Value;
		InitialRotationShutterRight = ShutterRight->GetRelativeRotation();
		InitialRotationShutterLeft = ShutterLeft->GetRelativeRotation();
		//InitialYaw = 0.f;
		FinalRotation = FRotator(0.f,FinalRotationValue,0.f);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		RotationTimer,
		this,
		&ATriggerArenaWindow::RotateActorDynamic,
		InRate,
		true
	);
}

void ATriggerArenaWindow::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(AEnemyCharacter::StaticClass()))
	{
		return;
	}

	if (!HasKey(OtherActor) || bHasBegunClose)
	{
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	RotationElapsed = 0.f;
	bHasBegunClose = true;
	GetWorld()->GetTimerManager().SetTimer(
        		RotationTimer,
        		this,
        		&ATriggerArenaWindow::RotateActorDynamicInverse,
        		InRate,
        		true
        		);
}

void ATriggerArenaWindow::Reset()
{
	bHasBegunClose = false;
}

void ATriggerArenaWindow::RotateActorDynamic()
{
	if (!RotationTimer.IsValid())
	{
		return;
	}
	
	RotationElapsed += GetWorld()->GetTimerManager().GetTimerElapsed(RotationTimer);

	const float Alpha = RotationElapsed;

	if (!IsValid(RotationCurve) || !IsValid(ShutterLeft) || !IsValid(ShutterRight))
	{
		return;
	}
	
	const float CurveValue = RotationCurve->GetFloatValue(Alpha);
	
	FRotator NewRotationLeft = FRotator(0.f, InitialRotationShutterLeft.Yaw - CurveValue,0.f);
	FRotator NewRotationRight = FRotator(0.f,InitialRotationShutterRight.Yaw + CurveValue,0.f);
	

	ShutterLeft->SetRelativeRotation(NewRotationLeft);
	ShutterRight->SetRelativeRotation(NewRotationRight);
	
	if (FMath::IsNearlyEqual(NewRotationLeft.Yaw, FinalRotation.Yaw) || bHasBegunClose)
	{
		bIsOpen = true;
		GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	}
}

void ATriggerArenaWindow::RotateActorDynamicInverse()
{
	if (!RotationTimer.IsValid() || !GetWorld())
	{
		return;
	}
	
	RotationElapsed += GetWorld()->GetTimerManager().GetTimerElapsed(RotationTimer);

	const float Alpha = FMath::Max(PlayRate - RotationElapsed,0.f);

	if (!IsValid(RotationCurve) || !IsValid(ShutterLeft) || !IsValid(ShutterRight))
	{
		return;
	}
	
	const float CurveValue = RotationCurve->GetFloatValue(Alpha);
	
	FRotator NewRotationLeft = FRotator(0.f, InitialRotationShutterLeft.Yaw - CurveValue,0.f);
	FRotator NewRotationRight = FRotator(0.f,InitialRotationShutterRight.Yaw + CurveValue,0.f);
	

	ShutterLeft->SetRelativeRotation(NewRotationLeft);
	ShutterRight->SetRelativeRotation(NewRotationRight);
	
	if (FMath::IsNearlyEqual(NewRotationLeft.Yaw, FinalRotation.Yaw))
	{
		bIsOpen = false;
		GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	}
}



