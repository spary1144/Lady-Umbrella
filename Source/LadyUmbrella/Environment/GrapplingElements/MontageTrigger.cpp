// Fill out your copyright notice in the Description page of Project Settings.


#include "MontageTrigger.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/GlobalUtil.h"

bool AMontageTrigger::TryToPlayMontage()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return false;
	}
	AnimInstance = FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(MontageToPlay))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMontageTrigger::OnMontageCompleted);
		AnimInstance->Montage_Play(MontageToPlay, 1.f);
		return true;
	}
	return false;
}

void AMontageTrigger::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == MontageToPlay && IsValid(AnimInstance))
	{
		//FGlobalPointers::PlayerCharacter->SetPlayerGeneralState(EPlayerState::FreeMovement);	
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AMontageTrigger::OnMontageCompleted);
	}
}

AMontageTrigger::AMontageTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerBox");
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMontageTrigger::OnOverlapBegin);
	
	TargetLocation = CreateDefaultSubobject<UArrowComponent>("TargetLocation");
	TargetLocation->SetupAttachment(Root);

	bShouldMoveToLocation	= true;
	MontageStartDelay		= 2.f;
	MovePlayerDelay			= 0.f;
	bIsActive				= true;
}

void AMontageTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMontageTrigger::PlayDelayedMontage()
{
	FTimerHandle WaitToPlayTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitToPlayTimerHandle, [this]() -> void
	{
		if (!TryToPlayMontage())
		{
			FGlobalPointers::PlayerCharacter->SetPlayerGeneralState(EPlayerState::FreeMovement);
		}
				
	}, MontageStartDelay, false);
}

void AMontageTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bMovingToTarget && IsValid(TargetLocation))
	{
		float Distance = FVector::Dist2D(FGlobalPointers::PlayerCharacter->GetActorLocation(), TargetLocation->GetComponentLocation());
		if (Distance <= ACCEPTANCE_RADIUS)
		{
			FGlobalPointers::PlayerCharacter->SetActorLocation(TargetLocation->GetComponentLocation());
			FGlobalPointers::PlayerCharacter->SetActorRotation(((TargetLocation->GetComponentLocation() + TargetLocation->GetForwardVector() * KINDA_SMALL_NUMBER) - TargetLocation->GetComponentLocation()).Rotation());
			OnMovementCompleted.Broadcast();
			PlayDelayedMontage();
			
			bIsActive = false;
			bMovingToTarget = false;
			FGlobalPointers::PlayerCharacter->GetController()->StopMovement();
		}
	}
}

void AMontageTrigger::TryToMovePlayerToLocation()
{
	if (bShouldMoveToLocation && IsValid(FGlobalPointers::MainController))
	{
		bMovingToTarget = true;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(FGlobalPointers::MainController, TargetLocation->GetComponentLocation());
	}
	else
	{
		PlayDelayedMontage();
		FGlobalPointers::PlayerCharacter->GetController()->StopMovement();
		FGlobalPointers::PlayerCharacter->SetActorLocation(TargetLocation->GetComponentLocation());
		FGlobalPointers::PlayerCharacter->SetActorRotation(((TargetLocation->GetComponentLocation() + TargetLocation->GetForwardVector() * KINDA_SMALL_NUMBER) - TargetLocation->GetComponentLocation()).Rotation());
		bIsActive = false;
	}
}

void AMontageTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()) || !IsValid(FGlobalPointers::PlayerCharacter) || !bIsActive)
	{
		return;
	}
	FGlobalPointers::PlayerCharacter->ResetAiming();
	FGlobalPointers::PlayerCharacter->SetPlayerGeneralState(EPlayerState::ControlsDisabled);

	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->UnTogglePlayerHUD();
	}

	if (IsValid(FGlobalPointers::PlayerCharacter->GetMesh()) && IsValid(FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()))
	{
		FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->StopAllMontages(0);
	}
	
	FTimerHandle MovePlayerTimerHandle;
	FTimerDelegate MovePlayerDelegate;
	MovePlayerDelegate.BindUFunction(this, "TryToMovePlayerToLocation");

	if (MovePlayerDelay == 0.f)
	{
		TryToMovePlayerToLocation();
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(MovePlayerTimerHandle, MovePlayerDelegate, MovePlayerDelay, false);
}

