// Fill out your copyright notice in the Description page of Project Settings.


#include "FreeRoamCoverComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FreeRoamCoverAnimationState.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"

#define COVER_LOG(Verbosity, Format, ...) \
if (DoDebug) UE_LOG(LogTemp, Verbosity, Format, ##__VA_ARGS__)

// Sets default values for this component's properties
UFreeRoamCoverComponent::UFreeRoamCoverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// State
	IsInFreeCover = false;
	IsAtCorner = false;
	DoDebug = false;

	// Tuning values
	StartDetectionRadius = 45.0f;
	PullForce = -15.0f;
	HeightCoverCheckRadius = 30.0f;
	HeightCheckZOffset = 50.0f;
	CornerCheckRange = 40.0f;
	NormalCoverSpeed = 300.f;
	TransitionTraceDistance = 500.0f;
	RotationInterpSpeed = 10.0f;
	
	// Capsule defaults
	CapsuleOffsetZ = -40.f;
	CapsuleRadius = 34.f;
	CapsuleHalfHeight = 40.f;

	// Cover positioning
	CoverOffsetFromWall = 10.0f;
	CoverHeightForwardTraceLength = 60.0f;

	// Corner checking
	CornerTraceForwardScale = 0.7f;
	CornerTraceSideScale = -1.5f;

	// Cover check tuning
	CoverValidationRadiusMultiplier = 1.1f;
	FacingAwayThreshold = 0.3f;


	// Exit
	ExitAwayDotThreshold = 0.75f;
	ExitProbeDistance    = 60.f;
	ExitSlackFromWall    = 12.f;
	ExitGraceTime = 0.15f;
	ExitMinInputMagnitude = 0.4f;
	ExitMinSpeed = 50.f;

	// Animation
	ConstructorHelpers::FObjectFinder<UAnimMontage> CoverTransitionMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Montages/AllCharacters/Movement/AM_Dodge.AM_Dodge'"));
	if (CoverTransitionMontageAsset.Succeeded())
	{
		CoverTransitionMontage = CoverTransitionMontageAsset.Object;
	}
}

void UFreeRoamCoverComponent::InitializeCapsule()
{
	CoverCapsule = NewObject<UCapsuleComponent>(GetOwner(), TEXT("CoverCapsule"));
	CoverCapsule->RegisterComponent();
	CoverCapsule->AttachToComponent(GetOwner()->GetRootComponent(), 
		FAttachmentTransformRules::KeepRelativeTransform);
	CoverCapsule->SetRelativeLocation(FVector(0, 0, CapsuleOffsetZ));
	CoverCapsule->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	CoverCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DoDebug)
	{
		CoverCapsule->SetVisibility(true);
		CoverCapsule->SetHiddenInGame(false);
	}

}
void UFreeRoamCoverComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AGenericCharacter>(GetOwner());
	if (IsValid(OwnerCharacter))
	{
		InitializeCapsule();
	}
	else
	{
		COVER_LOG(Error, TEXT("Failed to cast owner to AGenericCharacter! Owner class: %s"), 
		   *GetOwner()->GetClass()->GetName());
	}
}


// Called every frame
void UFreeRoamCoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsInFreeCover)
	{
		OwnerCharacter->SetInFreeRoamCover(true);
		PullToCover();
		CheckCoverHeight();
		ToggleCapsuleCollision();
		if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->GetMovementComponent()))
		{
			if (CheckCorner())
			{
				MovementComponent->MaxWalkSpeed = 0.f;
			}
			else
			{
				MovementComponent->MaxWalkSpeed = NormalCoverSpeed;
			}
		}
		if (!CheckIsInCover())
		{
			ResetCoverVariables();
			
		}
		if (ShouldExitCoverByInput())
		{
			ResetCoverVariables();
		}
	}
}

void UFreeRoamCoverComponent::PullToCover()
{
	if (!IsInFreeCover)
	{
		return;
	}
	
	if (!IsValid(OwnerCharacter))
	{
		return;
	}
	
	// Pull slightly into the wall to stay stuck
	FVector PullVector = LastCoverNormal * PullForce;
	FVector NewLocation = OwnerCharacter->GetActorLocation() + PullVector;
	OwnerCharacter->SetActorLocation(NewLocation, true);

	// Sphere trace to average wall normals
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.bTraceComplex = true;
	
	TArray<FHitResult> Hits = SphereTraceMulti(NewLocation, NewLocation, StartDetectionRadius);
	if (Hits.Num() > 0)
	{
		TArray<FVector> TempNormals;
		TempNormals.Reset();

		for (const FHitResult& Hit : Hits)
		{
			if (Hit.bBlockingHit)
			{
				TempNormals.Add(Hit.ImpactNormal);
			}
		}

		if (TempNormals.Num() > 0)
		{
			FVector AverageNormal = FVector::ZeroVector;
			for (const FVector& Normal : TempNormals)
			{
				AverageNormal += Normal;
			}
			AverageNormal.Normalize();

			LastCoverNormal = AverageNormal;
		}

		TempNormals.Empty(); // Clear after use
	}
}

void UFreeRoamCoverComponent::StartCover()
{
	if (IsInFreeCover)
	{
		ResetCoverVariables();
		//TryCoverTransition();
		return;
	}

	if (!IsValid(OwnerCharacter))
	{
		COVER_LOG(Error, TEXT("[CoverComponent] No owner found!"));
		return;
	}

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.bTraceComplex = true;

	FHitResult HitResult = SphereTraceSingle(Start, End, StartDetectionRadius);
	bool bHit = HitResult.bBlockingHit;
	
	if (DoDebug) DrawDebugSphere(GetWorld(), Start, StartDetectionRadius, 12, FColor::Green, false, 1.0f);

	if (bHit)
	{
		LastCoverNormal = HitResult.ImpactNormal;

		// Calculate the position slightly offset from the wall
		CoverLocation = HitResult.ImpactPoint - LastCoverNormal * CoverOffsetFromWall;

		if (DoDebug)
		{
			COVER_LOG(Warning, TEXT("[CoverComponent] Hit Actor: %s"), *HitResult.GetActor()->GetName());
			COVER_LOG(Warning, TEXT("[CoverComponent] Cover Normal: %s"), *LastCoverNormal.ToString());
			COVER_LOG(Warning, TEXT("[CoverComponent] Cover Location: %s"), *CoverLocation.ToString());
		}
		// Move the player to cover location
		if (OwnerCharacter)
		{
			OwnerCharacter->SetActorLocation(CoverLocation, true);
		}

		//If no directional input we can rotate the character in a default rotation
		FVector InputDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal();
		if (InputDirection.IsNearlyZero())
		{
			// Rotate the actor to face away from the surface normal
			FRotator NewRotation = LastCoverNormal.ToOrientationRotator();
			OwnerCharacter->SetActorRotation(NewRotation);
		}


		IsInFreeCover = true;
		CoverEnterTime = GetWorld()->GetTimeSeconds();
	}
}


void UFreeRoamCoverComponent::CheckCoverHeight()
{
	if (!IsValid(OwnerCharacter) || !IsInFreeCover)
	{
		CoverHeightState = ECoverHeightState::NoCover;
		return;
	}

	FVector Start = OwnerCharacter->GetActorLocation() + FVector(0, 0, HeightCheckZOffset);
	FVector End = Start + (LastCoverNormal * -CoverHeightForwardTraceLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.bTraceComplex = true;

	FHitResult HitResult = SphereTraceSingle(Start, End, HeightCoverCheckRadius);
	bool bHit = HitResult.bBlockingHit;

	// Debug visualization
	if (DoDebug)
	{
		DrawDebugSphere(GetWorld(), Start, HeightCoverCheckRadius, 12, FColor::Blue, false, -1.0f, 0, 0.5f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, -1.0f, 0, 2.0f);
	}
	if (!bHit)
	{
		CoverHeightState = ECoverHeightState::Crouching;
	}
	else
	{
		CoverHeightState = ECoverHeightState::Standing;
	}

	OwnerCharacter->GetCoverMovementComponent()->SetCoverHeightState(CoverHeightState);
}

bool UFreeRoamCoverComponent::CheckCorner()
{
	if (!IsValid(OwnerCharacter) || !IsInFreeCover)
	{
		return false;
	}
	// Determine which direction to check based on dot product with cover normal
	FVector RightVector = OwnerCharacter->GetActorRightVector();
	float DotProduct = FVector::DotProduct(RightVector, LastCoverNormal);
    
	// Choose direction - if dot > 0, we go right, otherwise left
	FVector TraceDirection = (DotProduct > 0) ? RightVector : (RightVector * -1.0f);
	// Calculate start position
	FVector Start = OwnerCharacter->GetActorLocation() + (OwnerCharacter->GetActorForwardVector() * CornerCheckRange * CornerTraceForwardScale);
	FVector End = Start + TraceDirection * CornerCheckRange * CornerTraceSideScale;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.bTraceComplex = true;

	FHitResult HitResult = SphereTraceSingle(Start, End, HeightCoverCheckRadius);
	bool bHit = HitResult.bBlockingHit;

	// Debug visualization
	if (DoDebug)
	{
		DrawDebugSphere(GetWorld(), Start, HeightCoverCheckRadius, 12, FColor::Purple, false, 0.1f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.1f, 0, 2.0f);
	}
	// Return true if there's NO blocking hit
	IsAtCorner = !bHit;
	return !bHit;
}


bool UFreeRoamCoverComponent::CheckIsInCover() const
{
	if (!IsValid(OwnerCharacter))
	{
		return false;
	}

	FVector TraceStart = OwnerCharacter->GetActorLocation();
	FVector TraceEnd = TraceStart; // in case we want to make it end somewhere else, still define this, what's the worst that could happen

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.bTraceComplex = true;

	FHitResult HitResult = SphereTraceSingle(TraceStart, TraceEnd, StartDetectionRadius * CoverValidationRadiusMultiplier);
	bool bHit = HitResult.bBlockingHit;
	
	return bHit;
}

void UFreeRoamCoverComponent::ResetCoverVariables()
{
	IsInFreeCover = false;
	CoverHeightState = ECoverHeightState::NoCover;
	OwnerCharacter->GetCoverMovementComponent()->SetCoverHeightState(CoverHeightState);
	OwnerCharacter->SetInFreeRoamCover(false);
	OnCoverExited.Broadcast();
}

void UFreeRoamCoverComponent::ToggleCapsuleCollision() const
{
	switch (CoverHeightState)
	{
	case ECoverHeightState::Crouching:
		OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		CoverCapsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		break;
	default:
		OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		CoverCapsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		break;
	}
	if (OwnerCharacter->IsAiming())
	{
		OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		CoverCapsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
}


void UFreeRoamCoverComponent::TryCoverTransition()
{
    if (!IsInFreeCover || !IsValid(OwnerCharacter))
    {
        return;
    }

    // Check if we're facing away from cover
    if (IsFacingAwayFromCover())
    {
        // Get movement input direction
        FVector InputDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal();
        
        // If no input, use character's forward vector
        if (InputDirection.IsNearlyZero())
        {
            InputDirection = OwnerCharacter->GetActorForwardVector();
        }

        // Perform trace in input direction
        FVector TraceStart = OwnerCharacter->GetActorLocation();
        FVector TraceEnd = TraceStart + (InputDirection * TransitionTraceDistance);

        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(OwnerCharacter);
        QueryParams.bTraceComplex = true;
    	FHitResult HitResult = SphereTraceSingle(TraceStart, TraceEnd, StartDetectionRadius);
    	bool bHit = HitResult.bBlockingHit;


        // Debug visualization
		if (DoDebug)
		{
	        DrawDebugSphere(GetWorld(), TraceStart, StartDetectionRadius, 12, FColor::Yellow, false, 2.0f);
	        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow, false, 2.0f, 0, 2.0f);
		}
    	
        if (bHit)
        {
            // Play transition animation
            if (CoverTransitionMontage && OwnerCharacter->GetMesh())
            {
            	ResetCoverVariables();
	            if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
            	{
            		AnimInstance->Montage_Play(CoverTransitionMontage);
                
            		// Bind lambda to call StartCover when montage ends
            		FOnMontageEnded MontageEndedDelegate;
            		MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
					{
						if (!bInterrupted)
						{
							StartCover();
						}
					});
            		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, CoverTransitionMontage);
            	}
            }
        }
        else
        {
            // No new cover found - exit normally
            ResetCoverVariables();
        }
    }
    else
    {
        // Not facing away - normal cover exit
        ResetCoverVariables();
    }
}

bool UFreeRoamCoverComponent::IsFacingAwayFromCover() const
{
    if (!IsValid(OwnerCharacter))
    {
	    return false;
    }
	
    // Get movement input direction
    FVector InputDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal();
    
    // If no input, use character's forward vector
    if (InputDirection.IsNearlyZero())
    {
        InputDirection = OwnerCharacter->GetActorForwardVector();
    }

    // Calculate dot product between input direction and cover normal
    float DotProduct = FVector::DotProduct(InputDirection, LastCoverNormal);

    // Facing away if dot product is positive (angle < 90 degrees)
    return DotProduct > FacingAwayThreshold; // Small threshold to prevent edge cases
}	


FHitResult UFreeRoamCoverComponent::SphereTraceSingle(const FVector& Start, const FVector& End, float Radius) const
{
	FHitResult Hit;
	if (!IsValid(OwnerCharacter))
	{
		return Hit;
	}
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.bTraceComplex = true;

	GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	return Hit;
}

TArray<FHitResult> UFreeRoamCoverComponent::SphereTraceMulti(const FVector& Start, const FVector& End, float Radius) const
{
	TArray<FHitResult> Hits;
	if (!IsValid(OwnerCharacter))
	{
		return Hits;
	}
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.bTraceComplex = true;

	GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	return Hits;
}

bool UFreeRoamCoverComponent::ShouldExitCoverByInput() const
{
	if (!IsValid(OwnerCharacter) || !IsInFreeCover) return false;

	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - CoverEnterTime < ExitGraceTime)
		return false;

	FVector RawInput = OwnerCharacter->GetLastMovementInputVector();
	FVector Dir2D = FVector::ZeroVector;

	if (RawInput.SizeSquared2D() >= FMath::Square(ExitMinInputMagnitude))
	{
		Dir2D = RawInput.GetSafeNormal2D();
	}
	else
	{
		const FVector Vel = OwnerCharacter->GetVelocity();
		if (Vel.Size2D() >= ExitMinSpeed)
			Dir2D = Vel.GetSafeNormal2D();
		else
			return false;
	}

	const FVector WallN = LastCoverNormal.GetSafeNormal2D();

	const float Dot = FVector::DotProduct(Dir2D, WallN);
	if (Dot < ExitAwayDotThreshold)
		return false;

	const FVector Start = OwnerCharacter->GetActorLocation() + FVector(0,0,HeightCheckZOffset);
	const FVector End   = Start + (WallN * ExitProbeDistance);
	const FHitResult Hit = SphereTraceSingle(Start, End, HeightCoverCheckRadius);
	if (Hit.bBlockingHit)
		return false;

	const float DistFromWall = FVector::DotProduct(OwnerCharacter->GetActorLocation() - CoverLocation, WallN);
	return DistFromWall > ExitSlackFromWall;
}