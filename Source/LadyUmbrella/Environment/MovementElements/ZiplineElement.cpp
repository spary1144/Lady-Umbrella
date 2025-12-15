#include "ZiplineElement.h"

#include "CineCameraComponent.h"
#include "EnhancedInputComponent.h"
#include "FMODEvent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

// Static
TWeakObjectPtr<AZiplineElement> AZiplineElement::ActiveZipline;

AZiplineElement::AZiplineElement()
{
	PrimaryActorTick.bCanEverTick = true;
	Cube1Position = FVector::ZeroVector;
	Cube2Position = FVector(1000.f, 0.f, 0.f);
	Cube1Scale = FVector(0.4f, 0.4f, 3.0f);
	Cube2Scale = FVector(0.4f, 0.4f, 3.0f);
	Cube1Rotation = FRotator(0.0f, 0.0f, 0.0f);
	Cube2Rotation = FRotator(0.0f, 0.0f, 0.0f);
	Box1Extent = FVector(300.0f, 500.0f, 50.0f);
	CollisionOffset = FVector(0.0f, 0.0f, 0.0f);
	ZipLineSpeed = 300.0f;
	V_MeshScale = FVector2D(0.1f, 0.1f);
	UIHeightOffset = 10.0f;
	SnappingInterpSpeed = 5.0f;
	SplineTimeOffset = 0.2f;
	IsInRange = false;
	IsSnapping = false;
	SnapMinDistanceThreshold = 5.0f;
	CameraLookClampMin = 0.0f;
	CameraLookClampMax = 1.0f;
	MaxStartTime = 0.9;
	CurrentSplineTime = 0.0f;
	StartOffset = FVector::ZeroVector;
	EndOffset = FVector::ZeroVector;
	CableHeightOffset = 0.f;
	SplineMaxTime = 1.0f;
	SnapMinDistanceThreshold = 5.0f;
	
	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent  = DefaultRoot;

	ZiplineEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ZiplineEffect"));
	ZiplineEffect->SetupAttachment(RootComponent);
	ZiplineEffect->bAutoActivate = false;

	
	Cube1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube1"));
	Cube1->SetupAttachment(RootComponent);

	Cube2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube2"));
	Cube2->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Game/Meshes/StaticMeshes/Placeholder/Primitives/Shape_Cube.Shape_Cube"));
	if (CubeMesh.Succeeded())
	{
		Cube1->SetStaticMesh(CubeMesh.Object);
		Cube2->SetStaticMesh(CubeMesh.Object);
	}

	StartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	StartPoint->SetupAttachment(RootComponent);

	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(RootComponent);

	ZipLineSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ZipLineSpline"));
	ZipLineSpline->SetupAttachment(RootComponent);

	ZiplineMesh = CreateDefaultSubobject<USplineMeshComponent>(TEXT("ZipLineMesh"));
	ZiplineMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SplineMesh(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder"));
	if (SplineMesh.Succeeded())
	{
		ZiplineMesh->SetStaticMesh(SplineMesh.Object);
		ZiplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ZiplineMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ZiplineMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		ZiplineMesh->SetGenerateOverlapEvents(true);
	}

	Cube1CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Cube1CollisionBox"));
	Cube1CollisionBox->SetupAttachment(RootComponent);
	Cube1CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Cube1CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	Cube1CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	Cube1CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Cube1CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCube1Overlap);
	Cube1CollisionBox->OnComponentEndOverlap  .AddDynamic(this, &ThisClass::OffCube1Overlap);

#if WITH_EDITORONLY_DATA
	StartPoint->bVisualizeComponent = true;
	EndPoint->bVisualizeComponent   = true;
#endif

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Particles/CascadeParticleSystems/CPS_Sparks.CPS_Sparks"));
	if (ParticleAsset.Succeeded())
	{
		ZiplineEffect->SetTemplate(ParticleAsset.Object);
	}

	// Widget
	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_JumpIcons.WBP_JumpIcons_C'"));
	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}

	// WORKAROUND to make zipline compatible with generic interactable.
	GetSphereComponent()->SetupAttachment(RootComponent);
	GetWidgetRootLocation()->SetupAttachment(SphereComponent);
	
	FModSoundComponent = CreateDefaultSubobject<UFModComponentInterface>("Fmod Sound Component");

	UFMODEvent* ZiplineEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/ZIPLINE.Zipline'");
	FmodUtils::TryAddEvent(ZiplineEvent, FModSoundComponent);

	FModHookingComponent = CreateDefaultSubobject<UFModComponentInterface>("Ufmod Hooking Component Interface");
	//UFMODEvent* HookingToTheZiplineEvent = AssetUtils::FindObject<UFMODEvent>("");
	
}

void AZiplineElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Cube1->SetRelativeLocation(Cube1Position);
	Cube1->SetRelativeScale3D(Cube1Scale);
	Cube1->SetRelativeRotation(Cube1Rotation);

	Cube2->SetRelativeLocation(Cube2Position);
	Cube2->SetRelativeScale3D(Cube2Scale);
	Cube2->SetRelativeRotation(Cube2Rotation);

	const FVector StartWorld = Cube1->GetComponentTransform().TransformPosition(StartOffset);
	const FVector EndWorld   = Cube2->GetComponentTransform().TransformPosition(EndOffset);

	StartPoint->SetWorldLocation(StartWorld);
	EndPoint  ->SetWorldLocation(EndWorld);

	ZipLineSpline->ClearSplinePoints(false);
	ZipLineSpline->AddSplinePoint(StartWorld, ESplineCoordinateSpace::World, false);
	ZipLineSpline->AddSplinePoint(EndWorld,   ESplineCoordinateSpace::World, false);
	ZipLineSpline->UpdateSpline();

	const FVector StartMeshPos = StartWorld + FVector(0, 0, CableHeightOffset);
	const FVector EndMeshPos   = EndWorld   + FVector(0, 0, CableHeightOffset);

	ZiplineMesh->SetStartPosition(StartMeshPos);
	ZiplineMesh->SetEndPosition(EndMeshPos);

	ZiplineMesh->SetStartTangent(ZipLineSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World));
	ZiplineMesh->SetEndTangent  (ZipLineSpline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World));

	ZiplineMesh->SetStartScale(V_MeshScale);
	ZiplineMesh->SetEndScale  (V_MeshScale);

	const FVector CollisionBoxWorld = Cube1->GetComponentTransform().TransformPosition(CollisionOffset);
	Cube1CollisionBox->SetWorldLocation(CollisionBoxWorld);
	Cube1CollisionBox->SetBoxExtent(Box1Extent);


	
}

void AZiplineElement::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(ZiplineMesh))
	{
		if (!ZiplineMesh->OnComponentBeginOverlap.IsAlreadyBound(this, &AZiplineElement::OnSplineOverlapBegin))
			ZiplineMesh->OnComponentBeginOverlap.AddDynamic(this, &AZiplineElement::OnSplineOverlapBegin);

		if (!ZiplineMesh->OnComponentEndOverlap.IsAlreadyBound(this, &AZiplineElement::OnSplineOverlapEnd))
			ZiplineMesh->OnComponentEndOverlap.AddDynamic(this, &AZiplineElement::OnSplineOverlapEnd);
	}

	// NEVER
	PlayerCharacterZiplineRef = nullptr;


	if (IsValid(FModSoundComponent->GetFModAudioComponent()))
	{
		FModSoundComponent->GetFModAudioComponent()->SetupAttachment(GetRootComponent());
		FModSoundComponent->InitializeMap();
	}
}

void AZiplineElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerCharacterZiplineRef.IsValid()) return;

	if (IsActiveThis() && PlayerCharacterZiplineRef->GetInteractiveMovementComponent()->IsInZipline() && CurrentSplineTime <= 1.0f)
	{
		MoveCharacterAlongSpline();
	}
	if (IsInRange && !IsSnapping && !PlayerCharacterZiplineRef->GetInteractiveMovementComponent()->IsInZipline())
	{
		AdjustSnappingPoint();
	}

	if (IsSnapping && !PlayerCharacterZiplineRef->GetInteractiveMovementComponent()->IsInZipline())
	{
		const FVector CurrentLocation      = PlayerCharacterZiplineRef->GetActorLocation();
		const FVector InterpolatedLocation = FMath::VInterpTo(CurrentLocation, SnapLocation, DeltaTime, SnappingInterpSpeed);
		PlayerCharacterZiplineRef->SetActorLocation(InterpolatedLocation);

		if (FVector::Dist(InterpolatedLocation, SnapLocation) <= SnapMinDistanceThreshold)
		{
			PlayerCharacterZiplineRef->SetActorLocation(SnapLocation);
			IsSnapping = false;
		}
	}
}

void AZiplineElement::AdjustSnappingPoint()
{
	if (!PlayerCharacterZiplineRef.IsValid() || !ZipLineSpline) return;

	if (auto* CamSys = PlayerCharacterZiplineRef->GetCameraSystemComponent())
	{
		const FVector SplineFwd = ZipLineSpline->GetForwardVector();
		const FVector CamFwd    = IsValid(CamSys->GetFollowCamera()) ? CamSys->GetFollowCamera()->GetForwardVector() : PlayerCharacterZiplineRef->GetActorForwardVector();

		const float CameraLookOffset = FMath::Clamp(FVector::DotProduct(SplineFwd, CamFwd), CameraLookClampMin, CameraLookClampMax);
		const FVector ClosestPoint   = ZipLineSpline->FindLocationClosestToWorldLocation(PlayerCharacterZiplineRef->GetActorLocation(), ESplineCoordinateSpace::World);
		const float SplineDist       = ZipLineSpline->GetDistanceAlongSplineAtLocation(ClosestPoint, ESplineCoordinateSpace::World);

		CurrentSplineTime = ZipLineSpline->GetTimeAtDistanceAlongSpline(SplineDist) + SplineTimeOffset * CameraLookOffset;
		SnapLocation      = ZipLineSpline->GetLocationAtTime(CurrentSplineTime, ESplineCoordinateSpace::World) + FVector(0, 0, CableHeightOffset);

		if (UWidgetComponent* Widget = GetWidgetComponent())
		{
			Widget->SetWorldLocation(SnapLocation + FVector(0, 0, UIHeightOffset));
		}
	}
}

void AZiplineElement::OnCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacterZiplineRef = PC;

		if (PC->Controller && !PC->GetInteractiveMovementComponent()->IsInZipline())
		{
			AdjustSnappingPoint();

			if (APlayerController* PCtrl = GetWorld()->GetFirstPlayerController())
			{
				EnableInput(PCtrl);
				if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
				{
					if (IsValid(JumpAction))
					{
						EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
					}
				}
				if (auto* Icon = GetInteractiveIcon())
				{
					Icon->SetShouldShow(true);
				}
				IsInRange = true;
			}
		}
	}
}

void AZiplineElement::OffCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		IsInRange = false;

		if (!IsSnapping && (!PlayerCharacterZiplineRef.IsValid() || !PlayerCharacterZiplineRef->GetInteractiveMovementComponent()->IsInZipline()))
		{
			if (APlayerController* PCtrl = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
			{
				DisableInput(PCtrl);
			}
			ClearLocalPlayerRef();
		}
	}
}

void AZiplineElement::OnSplineOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacterZiplineRef = PC;

		if (!PC->GetInteractiveMovementComponent()->IsInZipline())
		{
			IsInRange = true;

			const FVector ClosestPoint = ZipLineSpline->FindLocationClosestToWorldLocation(OtherActor->GetActorLocation(), ESplineCoordinateSpace::Local);
			const float   SplineDist    = ZipLineSpline->GetDistanceAlongSplineAtLocation(ClosestPoint, ESplineCoordinateSpace::Local);
			CurrentSplineTime           = ZipLineSpline->GetTimeAtDistanceAlongSpline(SplineDist);

			if (CurrentSplineTime <= MaxStartTime)
			{
				ProcessOverlap(OtherActor);
			}
		}
	}
}

void AZiplineElement::OnSplineOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		if (!IsActiveThis())
		{
			ClearLocalPlayerRef();
			IsInRange = false;
			IsSnapping = false;
		}
	}
}

void AZiplineElement::BecomeActive()
{
	if (ActiveZipline.IsValid() && ActiveZipline.Get() != this)
	{
		ActiveZipline->ResignIfActive();
	}
	ActiveZipline = this;
}

void AZiplineElement::ResignIfActive()
{
	if (IsActiveThis())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(MovementTimerHandle);
		}
	}
}

void AZiplineElement::MoveCharacterAlongSpline()
{
	if (!PlayerCharacterZiplineRef.IsValid() || !ZipLineSpline) return;

	// Camera Shake (hope I didn't break it)
	if (auto* CamSys = PlayerCharacterZiplineRef->GetCameraSystemComponent())
	{
		CamSys->MakeCameraShake(EShakeSource::VIBRATION);
	}

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector NewPosition   = ZipLineSpline->GetLocationAtTime(CurrentSplineTime, ESplineCoordinateSpace::World);
	const FVector SplineTangent = ZipLineSpline->GetTangentAtTime(CurrentSplineTime, ESplineCoordinateSpace::World);
	const FRotator NewRotation  = SplineTangent.Rotation();

	if (auto* Icon = GetInteractiveIcon())
	{
		Icon->SetShouldShow(false);
	}

	PlayerCharacterZiplineRef->SetActorLocation(NewPosition);
	PlayerCharacterZiplineRef->SetActorRotation(FRotator(PlayerCharacterZiplineRef->GetActorRotation().Pitch, NewRotation.Yaw, PlayerCharacterZiplineRef->GetActorRotation().Roll));

	if (ZipLineSpline->GetSplineLength() > 0.f)
	{
		CurrentSplineTime += (ZipLineSpeed * World->DeltaTimeSeconds) / ZipLineSpline->GetSplineLength();
	}

	if (CurrentSplineTime >= SplineMaxTime)
	{
		World->GetTimerManager().ClearTimer(MovementTimerHandle);

		PlayerCharacterZiplineRef->GetInteractiveMovementComponent()->SetInZipline(false);
		CurrentSplineTime = 0.0f;
		IsSnapping = false;
		IsInRange = false;

		if (auto* Icon = GetInteractiveIcon())
		{
			Icon->SetShouldShow(true);
		}

		if (APlayerController* PCtrl = World->GetFirstPlayerController())
		{
			DisableInput(PCtrl);
		}

		if (PlayerCharacterZiplineRef->CanChangeToPlayerState(EPlayerState::Falling))
		{
			PlayerCharacterZiplineRef->SetPlayerGeneralState(EPlayerState::Falling);
		}

		if (IsValid(ZiplineEffect))
		{
			ZiplineEffect->DeactivateSystem();
			ZiplineEffect->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}

		// finished, clear active zipline
		if (IsActiveThis())
		{
			ActiveZipline = nullptr;
		}

		// Clear our player ref
		ClearLocalPlayerRef();

		//Stop FMod Event
		if (IsValid(FModSoundComponent))
		{
			FModSoundComponent->StopEvent();
		}
	}
}

void AZiplineElement::ProcessOverlap(AActor* OtherActor)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		PlayerCharacterZiplineRef = PC;

		BecomeActive();

		PC->GetInteractiveMovementComponent()->SetInZipline(true);

		GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AZiplineElement::MoveCharacterAlongSpline, 0.01f, true);

		if (ZiplineEffect && PC->GetRootComponent())
		{
			ZiplineEffect->AttachToComponent(PC->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ZiplineEffect->SetRelativeLocation(FVector(0.0f, 0.0f, CableHeightOffset));
			ZiplineEffect->ActivateSystem(true);
		}
	}
}

void AZiplineElement::Jump()
{
	if (!PlayerCharacterZiplineRef.IsValid() || !IsInRange) return;

	if (UCharacterMovementComponent* MoveComp = PlayerCharacterZiplineRef->GetCharacterMovement())
	{
		if (!MoveComp->IsMovingOnGround()) return;
	}

	IsSnapping = true;

	if (PlayerCharacterZiplineRef->CanChangeToPlayerState(EPlayerState::InTraversalNoControls))
	{
		PlayerCharacterZiplineRef->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
	}

	PlayerCharacterZiplineRef->Jump();

	//Play FMOD event HERE
	if (IsValid(FModSoundComponent))
	{
		FModSoundComponent->SetParameter( ParameterName, bIsInAir );
		FModSoundComponent->PlayEvent();
	}
}

void AZiplineElement::ClearLocalPlayerRef()
{
	PlayerCharacterZiplineRef = nullptr;
}


void AZiplineElement::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MovementTimerHandle);
	}

	if (IsActiveThis())
	{
		ActiveZipline = nullptr;
	}

	if (PlayerCharacterZiplineRef.IsValid())
	{
		if (TObjectPtr<UInteractiveMovementComponent> IMC = PlayerCharacterZiplineRef->GetInteractiveMovementComponent())
		{
			if (IMC->IsInZipline()) IMC->SetInZipline(false);
		}
	}

	if (ZiplineEffect) ZiplineEffect->DeactivateSystem();
	if (FModSoundComponent) FModSoundComponent->StopEvent();

	Super::EndPlay(EndPlayReason);
}