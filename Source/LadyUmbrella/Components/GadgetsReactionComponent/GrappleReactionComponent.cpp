// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleReactionComponent.h"

#include "CableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Assets/AnimationAssets.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

// Sets default values for this component's properties
UGrappleReactionComponent::UGrappleReactionComponent():
	bStartsFallingAfterBeingHooked(false), bIsBehindCover(false), AlreadyResetGrapplePosition(false),
	UmbrellaShotRange(0),
	LaunchSpeedFactorOnFall(300.f),
	LaunchSpeedFactor(500.f),
	ExtraUpwardForce(100.f),
	GrapplingMaxTime(0), StunTime(2),
	Umbrella(nullptr), PlayerRef(nullptr), AnimInstance(nullptr),
	MovementComponentRef(nullptr),
	CoverMovement(nullptr),
	StunnedStepsMontage(nullptr), FallingOnHookedMontage(nullptr),
	StunnedStepsMontagePlayRate(0), FallingOnHookedMontagePlayRate(0),
	GenericCharacter(nullptr)
{
}

// Called when the game starts
void UGrappleReactionComponent::BeginPlay()
{
	Super::BeginPlay();

		GenericCharacter = Cast<AEnemyCharacter>(GetOwner());

	USkeletalMeshComponent* Mesh = GenericCharacter->GetMesh();
	if (!IsValid(Mesh))
	{
		return;
	}
	AnimInstance = Mesh->GetAnimInstance();

	if (!IsValid(FGlobalPointers::Umbrella))
	{
		return;
	}
	
	UmbrellaShotRange = FGlobalPointers::Umbrella->GetBulletRange() - OFFSET_SHOOT_RANGE;
}

void UGrappleReactionComponent::Initialize()
{
	PlayerRef = FGlobalPointers::PlayerCharacter;// Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));//FGlobalPointers::PlayerCharacter;
	if (!IsValid(PlayerRef))
	{
		FLogger::ErrorLog("[UGrappleReactionComponent][BeginPlay]: PlayerRef not valid");
	}

	Umbrella = FGlobalPointers::Umbrella;

	if (!IsValid(Umbrella))
	{
		FLogger::ErrorLog("[UGrappleReactionComponent][BeginPlay]: Umbrella not valid");
	}
}

// +-------------------------------------------------+
// |                                                 |
// |					Grapple                      |
// |                                                 |
// +-------------------------------------------------+

void UGrappleReactionComponent::CheckEnemyBehindCover(const FVector& SocketHookLocation)
{
	if (!IsValid(PlayerRef) || !IsValid(GenericCharacter) || !IsValid(MovementComponentRef))
	{
		return;
	}
	
	//check if enemy movement is controlled by cover
	bool bMovementControlledByCover = false;
	bIsBehindCover = false;	
	CoverMovement = GenericCharacter->GetCoverMovementComponent();
	if (IsValid(CoverMovement) && CoverMovement->IsMovementControlledByCover())
	{
		//exit cover
		CoverMovement->NotifyExitCover();
		bMovementControlledByCover = true;
	}
	
	//Enemy Pos
	FVector EnemyFeetLocation = MovementComponentRef->GetFeetLocation();	
		
	FHitResult OutHitResult;	
	OutHitResult.Init();
		
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GenericCharacter);

	if (!IsValid(PlayerRef))
	{
		FLogger::ErrorLog("Invalid PlayerRef in EnemyCharacter.IsEnemyBehindCover()");
		PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		EnemyFeetLocation,
		PlayerRef->GetActorLocation(),
		ECC_Visibility,
		Params
		);
	
	//if line trace collides with cover means that enemy is just behin cover
	if (IsValid(OutHitResult.GetActor()) && OutHitResult.GetActor()->IsA(ACoverParent::StaticClass()))
	{
		float DistanceBetweenCoverAndEnemy = UKismetMathLibrary::Vector_Distance(OutHitResult.ImpactPoint,EnemyFeetLocation);
		float DistanceBetweenCoverAndPlayer = UKismetMathLibrary::Vector_Distance(OutHitResult.ImpactPoint,PlayerRef->GetActorLocation());
		if (DistanceBetweenCoverAndEnemy <= MIN_DISTANCE_TO_CONSIDER_BEHIND_COVER && DistanceBetweenCoverAndPlayer >= MIN_DISTANCE_TO_CONSIDER_BEHIND_COVER)
		{
			bIsBehindCover = true;
		}
	}
	
	//if enemy movement is controlled by cover but not behind cover then face rotate to player
	if (bMovementControlledByCover && !bIsBehindCover)
	{
		FVector VectorEnemyHook = SocketHookLocation - GenericCharacter->GetActorLocation();
		GenericCharacter->SetActorRotation(VectorEnemyHook.Rotation());	
	}
}

bool UGrappleReactionComponent::CheckIfWillFallOnHooked(FVector& LaunchDirection)
{
	if (!IsValid(GenericCharacter))
	{
		return false;
	}
	
	LaunchDirection.Z = 0.f; // not launch in Z hear
	if (GenericCharacter->WillFallOffLedge(LaunchDirection))
	{
		FVector LaunchVelocity = GenericCharacter->GetActorForwardVector() * LaunchSpeedFactorOnFall + FVector(0.f, 0.f, ExtraUpwardForce);
		GenericCharacter->LaunchCharacter(LaunchVelocity, true, true);
		
		PlayFallingAnimation(FallingOnHookedMontage,FallingOnHookedMontagePlayRate);
		
		bStartsFallingAfterBeingHooked = true;

		EndGrapplingPosition();
		
		return true;
	}
	
	return false;
}

void UGrappleReactionComponent::SetGrapplingPosition(FVector& SocketHookLocation)
{
	if (!IsValid(GenericCharacter) || !IsValid(CoverMovement) || !IsValid(Umbrella))
	{
		return;
	}
	FVector ActorLocation = GenericCharacter->GetActorLocation();
	FVector CalculusVector = ActorLocation - SocketHookLocation;
	
	//check on launch if enemy is going to fall from hight location
	FVector LaunchDirection = UKismetMathLibrary::GetDirectionUnitVector(ActorLocation, SocketHookLocation);
	// if (CheckIfWillFallOnHooked(LaunchDirection))
	// {
	// 	return;
	// }
	
	if (bIsBehindCover)
	{
		EndGrapplingPosition();
		return;
	}

	
	//launch character to player
	LaunchDirection.Z = 0.f;
	GenericCharacter->LaunchCharacter(LaunchDirection * LaunchSpeedFactor, true, true);

	float DistanceToPlayer = FVector::Dist(SocketHookLocation, ActorLocation);
	if (DistanceToPlayer <= (UmbrellaShotRange + GRAPPLING_POSITION_ERROR_TOLERANCE))
	{
		EndGrapplingPosition();
		return;
	}
	//check if grapple has return to umbrella
	Umbrella->SetCableEndLocation(ActorLocation);
	Umbrella->GetCableComponent()->SetVisibility(true);
	Umbrella->GetCableComponent()->SetComponentTickEnabled(true);
	
	bool NearlyEqual = UKismetMathLibrary::NearlyEqual_FloatFloat(CalculusVector.Length(),0, GRAPPLING_POSITION_ERROR_TOLERANCE);
	if (NearlyEqual || !CoverMovement->CanCharacterBePulledByHook())
	{
		EndGrapplingPosition();
	}
}

void UGrappleReactionComponent::EndGrapplingPosition()
{
	if (!IsValid(MovementComponentRef) || !IsValid(Umbrella) || AlreadyResetGrapplePosition)
	{
		return;
	}

	AlreadyResetGrapplePosition = true;
	
	MovementComponentRef->StopMovementImmediately();
	MovementComponentRef->SetMovementMode(EMovementMode::MOVE_Walking);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_GrapplingDelay);

	if (IsValid(Umbrella->GetCableComponent()))
	{
		Umbrella->GetCableComponent()->EndLocation = FVector::ZeroVector;
		Umbrella->GetCableComponent()->SetVisibility(false);
		Umbrella->GetCableComponent()->SetComponentTickEnabled(false);
	}
}

void UGrappleReactionComponent::EndGrappleStun()
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	GenericCharacter->ResumeAILogic(CauseToResumeLogic);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_GrapplingStun);
}


void UGrappleReactionComponent::ManageGrapplePosition(FVector& SocketHookLocation)
{
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "SetGrapplingPosition", SocketHookLocation);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingDelay, Delegate, GetWorld()->GetDeltaSeconds(), true);
	
	FTimerDelegate DelegateMaxTime;
	if(IsValid(this)) 
	{
		DelegateMaxTime.BindUFunction(this, "EndGrapplingPosition");
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingMaxTime, DelegateMaxTime, GrapplingMaxTime, false);
	
}

void UGrappleReactionComponent::PlayStunStepsAnimation()
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(FAnimationAssets::EnemyHookedMontage))
	{
		GenericCharacter->ResumeAILogic(CauseToResumeLogic);
		return;
	}
	
	if (!IsValid(AnimInstance) || AnimInstance->Montage_IsPlaying(FAnimationAssets::EnemyHookedMontage))
	{
		return;
	}
	
	AnimInstance->Montage_Play(FAnimationAssets::EnemyHookedMontage, StunnedStepsMontagePlayRate);
	if (!AnimInstance->OnMontageEnded.IsBound())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UGrappleReactionComponent::OnStunStepsMontageCompleted);
	}
}

void UGrappleReactionComponent::PlayFallingAnimation(UAnimMontage* FallingAnimation, float PlayRate) const
{
	if (!IsValid(FallingAnimation) || !IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(AnimInstance) || AnimInstance->Montage_IsPlaying(FallingAnimation))
	{
		return;
	}
	
	AnimInstance->Montage_Play(FallingAnimation);
	AnimInstance->Montage_SetPlayRate(FallingAnimation, PlayRate);
}

float UGrappleReactionComponent::CalculateDynamicStunTime(const FVector& SocketHookLocation)
{
	if (!IsValid(GenericCharacter))
	{
		return StunTime; // fixed time
	}

	FVector ActorLocation = GenericCharacter->GetActorLocation();
	float Distance = FVector::Dist(ActorLocation, SocketHookLocation);

	// velocidad horizontal aproximada
	float LaunchSpeed = LaunchSpeedFactor;
	if (LaunchSpeed <= 0.f)
	{
		LaunchSpeed = 600.f; // valor por defecto
	}

	float TravelTime = Distance / LaunchSpeed;

	return FMath::Clamp(TravelTime, 0.3f, GrapplingMaxTime);
}

void UGrappleReactionComponent::OnStunStepsMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	// if (bStartsFallingAfterBeingHooked)
	// {
	// 	return;
	// }
	// if (IsValid(GenericCharacter))
	// {
	// 	GenericCharacter->ResumeAILogic(CauseToResumeLogic);
	// }
	
	// if (!IsValid(FAnimationAssets::EnemyHookedMontage) || Montage != FAnimationAssets::EnemyHookedMontage || !IsValid(AnimInstance))
	// {
	// 	return;
	// }
	
	//AnimInstance->OnMontageEnded.RemoveDynamic(this, &UGrappleReactionComponent::OnStunStepsMontageCompleted);
}

void UGrappleReactionComponent::ContactWithCable(FVector& SocketHookLocation)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}

	AlreadyResetGrapplePosition = false;
	
	MovementComponentRef = GenericCharacter->GetCharacterMovement();
	if (!IsValid(MovementComponentRef))
	{
		return;
	}

	//change movement mode
	MovementComponentRef->SetMovementMode(EMovementMode::MOVE_Walking);
	
	//check if is behind cover
	CheckEnemyBehindCover(SocketHookLocation);
	
	//Start stun animation
	PlayStunStepsAnimation();
	//pause AI logic
	GenericCharacter->PauseAILogic(CauseToPauseLogic);

	float AppliedStunTime = bIsBehindCover ? StunTime : CalculateDynamicStunTime(SocketHookLocation);

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UGrappleReactionComponent::EndGrappleStun);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingStun, Delegate, AppliedStunTime, false);
	
	//grappling logic
	if (!bIsBehindCover)
	{
		ManageGrapplePosition(SocketHookLocation);
	}
	else
	{
		EndGrapplingPosition(); //return back grapple
	}
	
	// Frame Freeze
	OnGrappleContact.Broadcast();
}