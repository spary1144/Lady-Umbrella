#include "SwingBar.h"

#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Math/ParabolicMovement.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

FOnSwingStateChanged ASwingBar::OnSwingStateChanged;

// Sets default values
ASwingBar::ASwingBar()
	: bShowDebugLines(false)
	, bIsActive(true)
	, TravelTimeToSwingLocation(0.7f)
	, CloseTravelTimeToSwingLocation(0.3f)
	, SwingStartTimeOffset(0.3f)
	, TravelTimeToTargetLocation(1)
	, Cooldown(TravelTimeToTargetLocation)
	, bIsInCooldown(false)
	, bIsFastSwing(false)
	, MinFacingDotThreshold(0.6f)
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	VisibleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisibleRFoot"));
	VisibleRoot->SetupAttachment(Root);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Meshes/StaticMeshes/EditorOnly/LevelDesignTools/SM_SwingPole3m.SM_SwingPole3m'"));
	UStaticMesh* Asset = MeshAsset.Object;
	StaticMeshComponent->SetupAttachment(VisibleRoot);
	StaticMeshComponent->SetStaticMesh(Asset);
	StaticMeshComponent->AddLocalRotation(FRotator(-90, 0, 0));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
	BoxCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider1"));
	BoxCollider1->SetupAttachment(VisibleRoot);
	BoxCollider1->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(200, 100, 200), FVector(1.0, 1.0, 1.0)));
	BoxCollider1->SetBoxExtent(FVector(200, 50, 200), false);
	
	BoxCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider2"));
	BoxCollider2->SetupAttachment(VisibleRoot);
	BoxCollider2->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(200, -100, 200), FVector(1.0, 1.0, 1.0)));
	BoxCollider2->SetBoxExtent(FVector(200, 50, 200), false);

	CloseBoxCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseBoxCollider1"));
	CloseBoxCollider1->SetupAttachment(VisibleRoot);
	CloseBoxCollider1->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(200, 100, 200), FVector(1.0, 1.0, 1.0)));
	CloseBoxCollider1->SetBoxExtent(FVector(200, 50, 200), false);
	
	CloseBoxCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseBoxCollider2"));
	CloseBoxCollider2->SetupAttachment(VisibleRoot);
	CloseBoxCollider2->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(200, -100, 200), FVector(1.0, 1.0, 1.0)));
	CloseBoxCollider2->SetBoxExtent(FVector(200, 50, 200), false);

	SwingLocation1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwingLocation1"));
	SwingLocation1->SetupAttachment(StaticMeshComponent);
	SwingLocation1->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(0, 100, 200), FVector(1.0, 1.0, 1.0)));
	
	SwingLocation2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwingLocation2"));
	SwingLocation2->SetupAttachment(StaticMeshComponent);
	SwingLocation2->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(0, -100, 200), FVector(1.0, 1.0, 1.0)));

	SwingLocation1Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwingLocation1Label"));
	SwingLocation1Label->SetupAttachment(SwingLocation1);
	SwingLocation1Label->SetRelativeLocation(FVector(0, 0, 50));
	SwingLocation1Label->SetText(FText::FromString("Swing Location B"));
	SwingLocation1Label->SetHorizontalAlignment(EHTA_Center);
	SwingLocation1Label->SetTextRenderColor(FColor::Blue);
	SwingLocation1Label->SetWorldSize(30.f);
	SwingLocation1Label->SetHiddenInGame(true);

	SwingLocation2Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwingLocation2Label"));
	SwingLocation2Label->SetupAttachment(SwingLocation2);
	SwingLocation2Label->SetRelativeLocation(FVector(0, 0, 50));
	SwingLocation2Label->SetText(FText::FromString("Swing Location A"));
	SwingLocation2Label->SetHorizontalAlignment(EHTA_Center);
	SwingLocation2Label->SetTextRenderColor(FColor::Red);
	SwingLocation2Label->SetWorldSize(30.f);
	SwingLocation2Label->SetHiddenInGame(true);

	
	TargetPoint1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("TargetPoint1"));
	TargetPoint1->SetupAttachment(VisibleRoot);
	TargetPoint1->SetRelativeLocation(FVector(500, 100, 0));

	TargetPoint2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("TargetPoint2"));
	TargetPoint2->SetupAttachment(VisibleRoot);
	TargetPoint2->SetRelativeLocation(FVector(500, -100, 0));

	TargetPoint1Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TargetPoint1Label"));
	TargetPoint1Label->SetupAttachment(TargetPoint1);
	TargetPoint1Label->SetRelativeLocation(FVector(0, 0, 50));
	TargetPoint1Label->SetText(FText::FromString("Target A"));
	TargetPoint1Label->SetHorizontalAlignment(EHTA_Center);
	TargetPoint1Label->SetTextRenderColor(FColor::Red);
	TargetPoint1Label->SetWorldSize(30.f);
	TargetPoint1Label->SetHiddenInGame(true);

	TargetPoint2Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TargetPoint2Label"));
	TargetPoint2Label->SetupAttachment(TargetPoint2);
	TargetPoint2Label->SetRelativeLocation(FVector(0, 0, 50));
	TargetPoint2Label->SetText(FText::FromString("Target B"));
	TargetPoint2Label->SetHorizontalAlignment(EHTA_Center);
	TargetPoint2Label->SetTextRenderColor(FColor::Blue);
	TargetPoint2Label->SetWorldSize(30.f);
	TargetPoint2Label->SetHiddenInGame(true);

	SwingSoundFmodComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Fmod Swing Component Interface");
	UFMODEvent* SwingEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/SWING_BAR.SWING_BAR'");
	FmodUtils::TryAddEvent(SwingEvent, SwingSoundFmodComponentInterface);
	
}

// Called when the game starts or when spawned
void ASwingBar::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(CloseBoxCollider1) && !CloseBoxCollider1->OnComponentBeginOverlap.IsAlreadyBound(this, &ASwingBar::OnCapsuleBeginOverlap))
	{
		CloseBoxCollider1->OnComponentBeginOverlap.AddDynamic(this, &ASwingBar::OnCapsuleBeginOverlap);
	}
	if (IsValid (CloseBoxCollider2) && !CloseBoxCollider2->OnComponentBeginOverlap.IsAlreadyBound(this, &ASwingBar::OnCapsuleBeginOverlap))
	{
		CloseBoxCollider2->OnComponentBeginOverlap.AddDynamic(this, &ASwingBar::OnCapsuleBeginOverlap);
	}
	
	if (IsValid(SwingSoundFmodComponentInterface) && IsValid(SwingSoundFmodComponentInterface->GetFModAudioComponent()) && IsValid(StaticMeshComponent))
	{
		SwingSoundFmodComponentInterface->GetFModAudioComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		SwingSoundFmodComponentInterface->GetFModAudioComponent()->SetRelativeLocation(FVector(175, 0, 0));
		SwingSoundFmodComponentInterface->InitializeMap();
	}
}

bool ASwingBar::Interact(AActor* OtherActor)
{
	if (!bIsActive)
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
	if (!IsValid(MovementComponent))
	{
		return false;
	}
	
	return TryMoveToClosestLocation();
	//Swing();
}

void ASwingBar::SetIsActive(bool IsActive)
{
	bIsActive = IsActive;
}

bool ASwingBar::GetIsActive() const
{
	return bIsActive;
}

bool ASwingBar::CheckIfPlayerFacingBar(const FVector& RefLocation) const
{
	const float DotProduct = FVector::DotProduct(RefLocation, FGlobalPointers::PlayerCharacter->GetActorForwardVector());

	if (DotProduct < MinFacingDotThreshold)
	{
		return false;
	}
	return true;
}

void ASwingBar::SetupSwing()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("[ASwingBar::SetupSwing]: PlayerCharacter null");
		return;
	}
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		MovementComponent->Velocity = FVector::ZeroVector;
		PlayerController->SetIgnoreMoveInput(true);
		FGlobalPointers::PlayerCharacter->GetPlayerStateComponent()->SetPlayerState(EPlayerState::InTraversalNoControls);
	}
		
	MovementComponent->SetMovementMode(MOVE_Flying);
	FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->SetIsSwinging(true);

	if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::SWING);
	}
}

void ASwingBar::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(FGlobalPointers::PlayerCharacter) && !FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->IsSwinging())
	{
		bIsFastSwing = true;
		Interact(OtherActor);
	}
}


bool ASwingBar::TryMoveToClosestLocation()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("[ASwingBar::MoveToClosestLocation]: PlayerCharacter null");
		return false;
	}
	
	FVector PlayerLocation = FGlobalPointers::PlayerCharacter->GetActorLocation();
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	
	USceneComponent* ChosenTargetPoint;

	if (FVector::Dist(PlayerLocation, SwingLocation1->GetComponentLocation()) <
	    FVector::Dist(PlayerLocation, SwingLocation2->GetComponentLocation()))
	{
		if (!CheckIfPlayerFacingBar(SwingLocation1->GetForwardVector()))
		{
			return false;
		}
		ChosenSwingLocation = SwingLocation1;
		ChosenTargetPoint = TargetPoint2;
	}
	else
	{
		if (!CheckIfPlayerFacingBar(SwingLocation2->GetForwardVector()))
		{
			return false;
		}
		ChosenSwingLocation = SwingLocation2;
		ChosenTargetPoint = TargetPoint1;
	}

	FGlobalPointers::PlayerCharacter->SetActorRotation(ChosenSwingLocation->GetComponentRotation());
	SetupSwing();
	OnSwingStateChanged.Broadcast(EUmbrellaLocation::UmbrellaCenter, true, false);
	
	constexpr float DesiredApproachSpeed = 100.0f;

	const float Distance = FVector::Dist(PlayerLocation, ChosenSwingLocation->GetComponentLocation());
	float TravelTimeToSwing = Distance / DesiredApproachSpeed;

	TravelTimeToSwing = FMath::Clamp(TravelTimeToSwing, CloseTravelTimeToSwingLocation, TravelTimeToSwingLocation);

	TravelTimeToSwing *= 0.7f;
	
	LaunchParabolicTo(TravelTimeToSwing, PlayerLocation, ChosenSwingLocation->GetComponentLocation());
	TargetLocation = ChosenTargetPoint->GetComponentLocation();

	bIsFastSwing = false;

	float TravelTimeRate = FMath::Max(TravelTimeToSwing - SwingStartTimeOffset, MIN_TRAVEL_TIME_RATE);
	GetWorldTimerManager().SetTimer(ParabolicLaunchToSwingLocationTimerHandler, this, &ASwingBar::Swing, TravelTimeRate, false);

	if (!bIsFastSwing)
	{
		GetWorldTimerManager().SetTimer(ParabolicLaunchTimerHandler,
			FTimerDelegate::CreateLambda([this]()
			{
				constexpr int32 ForwardVelocity = 10;
				MovementComponent->Velocity = FGlobalPointers::PlayerCharacter->GetActorForwardVector() * ForwardVelocity;
				MovementComponent->SetMovementMode(MOVE_Flying);
			}), TravelTimeToSwing, false);
	}
	SwingSoundFmodComponentInterface->PlayEvent();
	return true;
}


void ASwingBar::Swing()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()))
	{
		FLogger::ErrorLog("[ASwingBar::Swing]: PlayerCharacter null");
		return;
	}

	if (IsValid(FGlobalPointers::PlayerCharacter->GetMotionWarpingComponent()))
	{
		FMotionWarpingTarget WarpingTarget;
		WarpingTarget.Location = ChosenSwingLocation->GetComponentLocation();
		WarpingTarget.Rotation = FGlobalPointers::PlayerCharacter->GetActorRotation();
		WarpingTarget.Name = "SwingStart";
		FGlobalPointers::PlayerCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTarget(WarpingTarget);
	}
	
	UAnimInstance* AnimInstance = FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* SwingMontage = FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->GetSwingMontage();
	if (IsValid(AnimInstance) && IsValid(SwingMontage))
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASwingBar::OnMontageNotifyBegin);
		AnimInstance->OnMontageEnded.AddDynamic(this, &ASwingBar::OnSwingCompleted);
		AnimInstance->Montage_Play(SwingMontage, FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->GetSwingAnimPlayRate(),  EMontagePlayReturnType::MontageLength, 0);
	}
}

void ASwingBar::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName.IsEqual("SwingRelease"))
	{
		UAnimInstance* AnimInstance = FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			AnimInstance->Montage_Stop(0.25f);
		}
		
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (IsValid(PlayerController))
		{
			PlayerController->SetIgnoreMoveInput(false);
			
			FGlobalPointers::PlayerCharacter->GetPlayerStateComponent()->SetPlayerState(EPlayerState::Falling);
		}
		
		MovementComponent->SetMovementMode(MOVE_Falling);
		LaunchParabolicTo(TravelTimeToTargetLocation, FGlobalPointers::PlayerCharacter->GetActorLocation(),TargetLocation);
		
		bIsInCooldown = true;
		return;
	}

	if (NotifyName.IsEqual("ReturnInput"))
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (IsValid(PlayerController))
		{
			PlayerController->SetIgnoreMoveInput(false);
			
			FGlobalPointers::PlayerCharacter->GetPlayerStateComponent()->SetPlayerState(EPlayerState::Falling);
		}
	}
}

void ASwingBar::OnSwingCompleted(UAnimMontage* Montage, bool bInterrupted)
{	
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(MovementComponent))
	{
		FLogger::ErrorLog("[ASwingBar::OnSwingCompleted]: PlayerCharacter null");
		return;
	}
	if (IsValid(Montage) && Montage == FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->GetSwingMontage())
	{
		FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &ASwingBar::OnSwingCompleted);

		MovementComponent->SetMovementMode(MOVE_Falling);
		
		FGlobalPointers::PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ASwingBar::OnMontageNotifyBegin);

		FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->SetIsSwinging(false);
		OnSwingStateChanged.Broadcast(EUmbrellaLocation::Hand, true, false);
		OnSwingStateChanged.Broadcast(EUmbrellaLocation::Back, false, true);
		GetWorldTimerManager().SetTimer(CooldownTimerHandler, this, &ASwingBar::CooldownReset, Cooldown, false);

		if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()))
		{
			FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->CheckSwing();
		}
	}
}

void ASwingBar::LaunchParabolicTo(const float Time, const FVector& Start, const FVector& End)
{
	if (!IsValid(MovementComponent))
	{
		FLogger::ErrorLog("[ASwingBar::LaunchParabolicTo]: MovementComponent null");
		return;
	}
	MovementComponent->SetMovementMode(MOVE_Falling);
	float GravityZ = MovementComponent->GetGravityZ();

	FVector LaunchVelocity = ParabolicMovement::CalculateParabolicLaunchVelocity(Start, End, Time, GravityZ);

	if (bShowDebugLines)
	{
		ParabolicMovement::DrawDebugParabola(GetWorld(), Start, LaunchVelocity, GravityZ, Time);

		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 100.0f, FLinearColor::Green, 2.0f, 3.0f);
	}

	MovementComponent->Launch(LaunchVelocity);

	GetWorldTimerManager().SetTimer(ParabolicLaunchTimerHandler, this, &ASwingBar::TryToClearParabolicLaunchTimer, Time, false);
}

void ASwingBar::TryToClearParabolicLaunchTimer()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("[ASwingBar::TryToClearParabolicLaunchTimer]: PlayerCharacter null");
		return;
	}
	if (FGlobalPointers::PlayerCharacter->GetInteractiveMovementComponent()->IsSwinging())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(ParabolicLaunchTimerHandler);
}

void ASwingBar::CooldownReset()
{
	bIsInCooldown = false;
	GetWorldTimerManager().ClearTimer(CooldownTimerHandler);
}
