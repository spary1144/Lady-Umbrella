#include "SwingRope.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "Components/TimelineComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Math/ParabolicMovement.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

FOnSwingRopeStateChanged ASwingRope::OnSwingRopeStateChanged;

// Sets default values
ASwingRope::ASwingRope()
	: bShowDebugLines(false)
	, SwingAnimationPlayRate(1)
	, SwingTimeLinePlayRate(1)
	, TravelTimeToSwingLocation(1.f)
	, TravelTimeToTargetLocation(1)
	, MinFacingDotThreshold(0.6f)
	, TimelineLaunchMoment(0.4)
	, bEnterPrimarySide(true)
	, Cooldown(0.5)
	, bIsInCooldown(false)
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	VisibleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisibleRoot"));
	VisibleRoot->SetupAttachment(Root);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Props/SM_PillarFrame.SM_PillarFrame'"));
	UStaticMesh* Asset = MeshAsset.Object;
	StaticMeshComponent->SetStaticMesh(Asset);
	StaticMeshComponent->AddLocalRotation(FRotator(180, 0, 0));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	StaticMeshComponent->SetupAttachment(VisibleRoot);

	StaticMeshFinalRotationA = StaticMeshComponent->GetRelativeRotation() + FRotator(0, 0, 40);
	StaticMeshFinalRotationB = StaticMeshComponent->GetRelativeRotation() - FRotator(0, 0, 40);
	
	BoxCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider1"));
	BoxCollider1->SetupAttachment(VisibleRoot);
	BoxCollider1->SetRelativeTransform(FTransform(FRotator(180, 0, 0), FVector(0, 100, 200), FVector(1.0, 1.0, 1.0)));
	BoxCollider1->SetBoxExtent(FVector(200, 50, 200), false);
	
	BoxCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider2"));
	BoxCollider2->SetupAttachment(VisibleRoot);
	BoxCollider2->SetRelativeTransform(FTransform(FRotator(180, 0, 0), FVector(0, -100, 200), FVector(1.0, 1.0, 1.0)));
	BoxCollider2->SetBoxExtent(FVector(200, 50, 200), false);

	SwingLocation1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwingLocation1"));
	SwingLocation1->SetupAttachment(StaticMeshComponent);
	SwingLocation1->SetRelativeTransform(FTransform(FRotator(180, 90, 0), FVector(10, 100, 200), FVector(1.0, 1.0, 1.0)));
	
	SwingLocation2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwingLocation2"));
	SwingLocation2->SetupAttachment(StaticMeshComponent);
	SwingLocation2->SetRelativeTransform(FTransform(FRotator(180, -90, 0), FVector(0, -100, 200), FVector(1.0, 1.0, 1.0)));

	SwingLocation1Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwingLocation1Label"));
	SwingLocation1Label->SetupAttachment(SwingLocation1);
	SwingLocation1Label->SetRelativeLocation(FVector(0, 0, 50));
	SwingLocation1Label->SetText(FText::FromString("Swing Location A"));
	SwingLocation1Label->SetHorizontalAlignment(EHTA_Center);
	SwingLocation1Label->SetTextRenderColor(FColor::Blue);
	SwingLocation1Label->SetWorldSize(30.f);
	SwingLocation1Label->AddLocalRotation(FRotator(0, 180, 0));
	SwingLocation1Label->SetHiddenInGame(true);

	SwingLocation2Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwingLocation2Label"));
	SwingLocation2Label->SetupAttachment(SwingLocation2);
	SwingLocation2Label->SetRelativeLocation(FVector(0, 0, 50));
	SwingLocation2Label->SetText(FText::FromString("Swing Location B"));
	SwingLocation2Label->SetHorizontalAlignment(EHTA_Center);
	SwingLocation2Label->SetTextRenderColor(FColor::Red);
	SwingLocation2Label->SetWorldSize(30.f);
	SwingLocation2Label->AddLocalRotation(FRotator(0, 180, 0));
	SwingLocation2Label->SetHiddenInGame(true);

	
	TargetPoint1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("TargetPoint1"));
	TargetPoint1->SetupAttachment(VisibleRoot);
	TargetPoint1->SetRelativeLocation(FVector(0, 300, 300));
	TargetPoint1->AddLocalRotation(FRotator(180, -90, 0));

	TargetPoint2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("TargetPoint2"));
	TargetPoint2->SetupAttachment(VisibleRoot);
	TargetPoint2->SetRelativeLocation(FVector(0, -300, 300));
	TargetPoint2->AddLocalRotation(FRotator(180, 90, 0));

	TargetPoint1Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TargetPoint1Label"));
	TargetPoint1Label->SetupAttachment(TargetPoint1);
	TargetPoint1Label->SetRelativeLocation(FVector(0, 0, 50));
	TargetPoint1Label->SetText(FText::FromString("Target B"));
	TargetPoint1Label->SetHorizontalAlignment(EHTA_Center);
	TargetPoint1Label->SetTextRenderColor(FColor::Red);
	TargetPoint1Label->SetWorldSize(30.f);
	TargetPoint1Label->SetHiddenInGame(true);

	TargetPoint2Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TargetPoint2Label"));
	TargetPoint2Label->SetupAttachment(TargetPoint2);
	TargetPoint2Label->SetRelativeLocation(FVector(0, 0, 50));
	TargetPoint2Label->SetText(FText::FromString("Target A"));
	TargetPoint2Label->SetHorizontalAlignment(EHTA_Center);
	TargetPoint2Label->SetTextRenderColor(FColor::Blue);
	TargetPoint2Label->SetWorldSize(30.f);
	TargetPoint2Label->SetHiddenInGame(true);

	SwingTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("SwingTimeLine"));
	
	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("AnimMontage'/Game/Animations/Montages/AllCharacters/Movement/AM_Swing.AM_Swing'"));
	if (MontageAsset.Succeeded())
	{
		SwingMontage = MontageAsset.Object;
	}
	FmodSwingSoundComponent = CreateDefaultSubobject<UFModComponentInterface>("Swing Rope Fmod Sound Component");
	
	UFMODEvent* ReloadEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/SWING_BAR.SWING_BAR'");
	FmodUtils::TryAddEvent(ReloadEvent, FmodSwingSoundComponent);

}

// Called when the game starts or when spawned
void ASwingRope::BeginPlay()
{
	Super::BeginPlay();

	FmodSwingSoundComponent->InitializeMap();
	FmodUtils::AttachFmodComponentToComponent(FmodSwingSoundComponent, GetRootComponent());
	
	
	StaticMeshOriginalRotation = StaticMeshComponent->GetRelativeRotation();

	if (IsValid(SwingTimeLine))
	{
		SwingTimeLine->SetPlayRate(SwingTimeLinePlayRate);
		
		FOnTimelineFloat SwingProgressUpdate;
		SwingProgressUpdate.BindUFunction(this, FName("SwingTimelineProgress"));

		FOnTimelineEvent SwingFinishedEvent;
		SwingFinishedEvent.BindUFunction(this, FName("SwingTimelineFinish"));
		
		if (IsValid(SwingRopeMovementCurve))
		{
			SwingTimeLine->AddInterpFloat(SwingRopeMovementCurve, SwingProgressUpdate);
			SwingTimeLine->SetTimelineFinishedFunc(SwingFinishedEvent);
		}
	}
}

// Called every frame
void ASwingRope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASwingRope::Interact(AActor* OtherActor)
{
	if (SwingTimeLine->IsReversing())
	{
		return false;
	}
	
	if (bIsInCooldown)
	{
		return false;
	}
	
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return false;
	}
	
	MovementComponent = FGlobalPointers::PlayerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return false;
	}
	
	// CHECK IF PRIMARY SIDE
	const FVector PlayerLocation = FGlobalPointers::PlayerCharacter->GetActorLocation();
	const float MinDistance = FVector::Dist(PlayerLocation, SwingLocation1->GetComponentLocation());
	UBillboardComponent* CurrentSwingLocation;
	
	if (MinDistance < FVector::Dist(PlayerLocation, SwingLocation2->GetComponentLocation()))
	{
		bEnterPrimarySide = true;
		TargetLocation = TargetPoint2->GetComponentLocation();
		CurrentSwingLocation = SwingLocation1;
	}
	else
	{
		bEnterPrimarySide = false;
		TargetLocation = TargetPoint1->GetComponentLocation();
		CurrentSwingLocation = SwingLocation2;
	}

	if (!CheckIfPlayerFacingBar(CurrentSwingLocation->GetForwardVector()))
	{
		return false;
	}

	if (IsValid(FGlobalPointers::PlayerCharacter->Controller))
	{
		FGlobalPointers::PlayerCharacter->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		FGlobalPointers::PlayerCharacter->Controller->SetIgnoreMoveInput(true);
	}

	// Setting the Player State
	if (IsValid(FGlobalPointers::PlayerCharacter) && FGlobalPointers::PlayerCharacter->CanChangeToPlayerState(EPlayerState::InTraversalNoControls))
	{
		FGlobalPointers::PlayerCharacter->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
	}
	
	MovementComponent->SetMovementMode(MOVE_Flying);
	FGlobalPointers::PlayerCharacter->SetActorRotation(CurrentSwingLocation->GetComponentRotation());
	LaunchParabolicTo(PlayerLocation, CurrentSwingLocation->GetComponentLocation(), TravelTimeToSwingLocation);
	
	FGlobalPointers::PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetWorldTimerManager().SetTimer(MoveToSwingLocationTimerHandler, this, &ASwingRope::MoveComponentToSwingTargetFinished, TravelTimeToSwingLocation, false);

	FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->SetIsInRope(true);
	OnSwingRopeStateChanged.Broadcast(EUmbrellaLocation::UmbrellaCenter, true, false);
	
	return true;
}

void ASwingRope::MoveComponentToSwingTargetFinished() const
{
	MovementComponent->StopMovementImmediately();
	MovementComponent->SetMovementMode(MOVE_Flying);

	if (bEnterPrimarySide)
	{
		FGlobalPointers::PlayerCharacter->AttachToComponent(SwingLocation1, FAttachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		FGlobalPointers::PlayerCharacter->AttachToComponent(SwingLocation2, FAttachmentTransformRules::KeepWorldTransform);
	}
	
	if (IsValid(SwingTimeLine))
	{
		SwingTimeLine->PlayFromStart();
		//Swing();
		FmodSwingSoundComponent->PlayEvent();
		if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
		{
			FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::SWING);
		}
	}
}

void ASwingRope::SwingTimelineProgress(float Value)
{
	FRotator CurrentRot;
	if (bEnterPrimarySide)
	{
		CurrentRot = FMath::Lerp(StaticMeshOriginalRotation, StaticMeshFinalRotationB, Value);
	}
	else
	{
		CurrentRot = FMath::Lerp(StaticMeshOriginalRotation, StaticMeshFinalRotationA, Value);
	}
	if (SwingTimeLine->GetPlaybackPosition() >= TimelineLaunchMoment)
	{
		if (FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->IsInRope() && !bIsInCooldown)
		{
			bIsInCooldown = true;
			FGlobalPointers::PlayerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			
			LaunchParabolicTo(FGlobalPointers::PlayerCharacter->GetActorLocation(), TargetLocation, TravelTimeToTargetLocation);
			
			// Setting the Player State
			if (IsValid(FGlobalPointers::PlayerCharacter) && FGlobalPointers::PlayerCharacter->CanChangeToPlayerState(EPlayerState::FreeMovement))
			{
				FGlobalPointers::PlayerCharacter->SetPlayerGeneralState(EPlayerState::FreeMovement);
			}

			if (IsValid(FGlobalPointers::PlayerCharacter->Controller))
			{
				FGlobalPointers::PlayerCharacter->Controller->SetIgnoreMoveInput(false);
			}
			
			FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->SetIsInRope(false);
			OnSwingRopeStateChanged.Broadcast(EUmbrellaLocation::Hand, true, false);
			OnSwingRopeStateChanged.Broadcast(EUmbrellaLocation::Back, false, true);
			FGlobalPointers::PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			GetWorldTimerManager().SetTimer(ParabolicLaunchTimerHandler, this, &ASwingRope::TryToClearParabolicLaunchTimer, TravelTimeToTargetLocation, false);
			GetWorldTimerManager().SetTimer(CooldownTimerHandler, this, &ASwingRope::CooldownReset, Cooldown * 2, false);
		}
	}
	StaticMeshComponent->SetRelativeRotation(CurrentRot);
}

void ASwingRope::SwingTimelineFinish() const
{
	if (SwingTimeLine->GetPlaybackPosition() != 0)
	{
		SwingTimeLine->Reverse();
	}
}

bool ASwingRope::CheckIfPlayerFacingBar(const FVector& RefLocation)
{
	const float DotProduct = FVector::DotProduct(RefLocation, FGlobalPointers::PlayerCharacter->GetActorForwardVector());

	if (DotProduct < MinFacingDotThreshold)
	{
		return false;
	}
	return true;
}


void ASwingRope::CooldownReset()
{
	bIsInCooldown = false;
}

void ASwingRope::Swing() const
{
	UAnimInstance* AnimInstance = FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance) && IsValid(SwingMontage))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ASwingRope::OnSwingCompleted);
		AnimInstance->Montage_Play(SwingMontage, SwingAnimationPlayRate, EMontagePlayReturnType::MontageLength, 0);
	}
}

void ASwingRope::OnSwingCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == SwingMontage)
	{
		FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &ASwingRope::OnSwingCompleted);
		OnSwingRopeStateChanged.Broadcast(EUmbrellaLocation::Hand, true, false);
	}
}

void ASwingRope::LaunchParabolicTo(const FVector& Start, const FVector& End, const float Time) const
{
	MovementComponent->SetMovementMode(MOVE_Falling);
	float GravityZ = MovementComponent->GetGravityZ();

	FVector LaunchVelocity = ParabolicMovement::CalculateParabolicLaunchVelocity(Start, End, Time, GravityZ);

	if (bShowDebugLines)
	{
		ParabolicMovement::DrawDebugParabola(GetWorld(), Start, LaunchVelocity, GravityZ, Time);
		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 100.0f, FLinearColor::Green, 2.0f, 3.0f);
	}

	MovementComponent->Launch(LaunchVelocity);
}

void ASwingRope::TryToClearParabolicLaunchTimer()
{
	if (FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->IsInRope())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(ParabolicLaunchTimerHandler);
}


