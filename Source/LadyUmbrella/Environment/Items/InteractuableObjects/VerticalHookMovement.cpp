// Fill out your copyright notice in the Description page of Project Settings.


#include "VerticalHookMovement.h"

#include "FMODEvent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

FOnVerticalHookStateChanged AVerticalHookMovement::OnVerticalHookStateChanged;

AVerticalHookMovement::AVerticalHookMovement()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	StaticMeshComponent		      = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh Component");
	ArrowComponent				  = CreateDefaultSubobject<UArrowComponent>("Arrow Component");
	PlayerMovementSplineComponent = CreateDefaultSubobject<USplineComponent>("Player Movement Spline Component");
	CameraMovingComponent		  = CreateDefaultSubobject<UCameraComponent>("CameraMoving Component");

	SplineMoveSpeed					= 1000.0f;
	BlendTime						= 1.5f;
	bMovingToTarget					= false;	
	
	PlayerMovementSplineComponent->SetupAttachment(GetRootComponent());
	ArrowComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	CameraMovingComponent->SetupAttachment(GetRootComponent());

}

void AVerticalHookMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bMovingToTarget)
	{
		float Distance = FVector::Dist2D(FGlobalPointers::PlayerCharacter->GetActorLocation(), PlayerMovementSplineComponent->GetWorldLocationAtTime(0));
		if (IsValid(ArrowComponent) && Distance <= ACCEPTANCE_RADIUS)
		{
			FGlobalPointers::PlayerCharacter->SetActorRotation((ArrowComponent->GetComponentLocation() - (ArrowComponent->GetComponentLocation() + ArrowComponent->GetForwardVector() * KINDA_SMALL_NUMBER)).Rotation());
			PlayVerticalHookAnimation(FGlobalPointers::PlayerCharacter);
			bMovingToTarget = false;
			FGlobalPointers::PlayerCharacter->GetController()->StopMovement();
		}
	}
}

void AVerticalHookMovement::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SplineTimer);
}

void AVerticalHookMovement::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(GetInteractiveIcon()) && IsValid(ArrowComponent))
	{
		GetInteractiveIcon()->SetShouldOffset(true);
		GetInteractiveIcon()->SetTraceOffset(ArrowComponent->GetComponentLocation());		
	}
}

bool AVerticalHookMovement::CheckPlayerComponentsAndVisibility(APlayerCharacter*& PlayerInteractor, UCharacterMovementComponent*& CharMovComp,
	APlayerController*& PlayerController)
{
	bool CheckVisibility = PlayerInteractor->GetCameraSystemComponent()->IsComponentInFrustum(ArrowComponent);	
    
	if (!CheckVisibility)
	{
		ClearTimer();
		return 0;
	}
	
	if ( !IsValid(PlayerInteractor) )
	{
		ClearTimer();
		return false;
	}
	
	CharMovComp = PlayerInteractor->GetCharacterMovement();
	PlayerController = Cast<APlayerController>(PlayerInteractor->GetController());

	if (!IsValid(CharMovComp) || !IsValid(PlayerController))
	{
		ClearTimer();
		return false;
	}
	return true;
}

int32 AVerticalHookMovement::Interacting(APlayerCharacter* Interactor)
{
	if (bMovingToTarget)
	{
		return 0;
	}
	
	UCharacterMovementComponent* CharMovComp = nullptr;
	APlayerController* PlayerController		 = nullptr;

	if (!CheckPlayerComponentsAndVisibility(Interactor, CharMovComp, PlayerController))
	{
		return 0;
	}
	
	if (!IsValid(PlayerController))
	{
		return 0;
	}

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (IsValid(NavigationSystem))
	{
		bMovingToTarget = true;
		Interactor->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
		const FVector TargetLocation(PlayerMovementSplineComponent->GetWorldLocationAtTime(0));
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, TargetLocation);
	}
	
	return 0;
	
}

void AVerticalHookMovement::MovePlayerToStartOfSpline(APlayerCharacter* Interactor)
{
	const FVector SplineWorldPos	 = PlayerMovementSplineComponent->GetWorldLocationAtTime(0);
	const FVector InteractorPosition = Interactor->GetActorLocation(); 
	FVector Speed = InteractorPosition - SplineWorldPos;
		
	Speed.Normalize();
	
	Interactor->AddMovementInput(Speed, -1,false);
	FVector2d Speed2d(Speed.X, Speed.Y);
	
	if ( Speed2d.IsNearlyZero(0.01) )
	{
		PlayVerticalHookAnimation(Interactor);
	}
}

void AVerticalHookMovement::PlayVerticalHookAnimation(APlayerCharacter* Interactor)
{
	if (!IsValid(Interactor) || !IsValid(FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()))
	{
		return;
	}
	AnimInstance = Interactor->GetMesh()->GetAnimInstance();
	UAnimMontage* VerticalHookMontage = FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->GetVerticalHookMovementMontage();
	if (IsValid(AnimInstance) && IsValid(VerticalHookMontage))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AVerticalHookMovement::OnVerticalMovementCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AVerticalHookMovement::OnMontageNotifyBegin);
		AnimInstance->Montage_Play(VerticalHookMontage, FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->GetVerticalHookAnimPlayRate());
		OnVerticalHookStateChanged.Broadcast(EUmbrellaLocation::Hand, true, false);

		UCharacterMovementComponent* CharacterMovement = FGlobalPointers::PlayerCharacter->GetCharacterMovement();
		if (IsValid(CharacterMovement))
		{
			CharacterMovement->SetMovementMode(MOVE_Flying);
		}
	}
}

void AVerticalHookMovement::OnMontageNotifyBegin(FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName.IsEqual("ReturnInput"))
	{
		AnimInstance->Montage_Stop(0.25f, AnimInstance->GetCurrentActiveMontage());
	}
}

void AVerticalHookMovement::OnVerticalMovementCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && IsValid(FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()) && Montage == FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->GetVerticalHookMovementMontage() && IsValid(AnimInstance))
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AVerticalHookMovement::OnVerticalMovementCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AVerticalHookMovement::OnMontageNotifyBegin);
		
		OnVerticalHookStateChanged.Broadcast(EUmbrellaLocation::Back, false, false);

		UCharacterMovementComponent* CharacterMovement = FGlobalPointers::PlayerCharacter->GetCharacterMovement();
		if (IsValid(CharacterMovement))
		{
			CharacterMovement->SetMovementMode(MOVE_Walking);
		}
	}

	if (FGlobalPointers::PlayerCharacter->GetPlayerGeneralState() == EPlayerState::InTraversalNoControls)
	{
		FGlobalPointers::PlayerCharacter->ResetPlayerStateToFreeMovementAndCheckAim();
	}
}


void AVerticalHookMovement::StartSplineMovement(APlayerCharacter* Interactor)
{

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	
	if (!IsValid(Interactor))
	{
		World->GetTimerManager().ClearTimer(SplineTimer);
		return;
	}
	
	const float DeltaTime = World->GetDeltaSeconds();
	SplineProgress += SplineMoveSpeed * DeltaTime;

	const FVector NewLocation = PlayerMovementSplineComponent->GetLocationAtDistanceAlongSpline(SplineProgress, ESplineCoordinateSpace::World);

	Interactor->GetCapsuleComponent()->SetWorldLocation(NewLocation);// SetActorLocation(NewLocation);
	
	if (SplineProgress >= PlayerMovementSplineComponent->GetSplineLength())
	{
		SplineProgress = 0;
		//Cast<APlayerController>(PlayerCharacter->GetController())->SetViewTargetWithBlend(PlayerCharacter, BlendTime, VTBlend_Linear, 0, false);

		World->GetTimerManager().ClearTimer(SplineTimer);
		Interactor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		Interactor->ResetPlayerStateToFreeMovementAndCheckAim();
	}
}
