// 
// CoreCharacter.cpp
// 
// Implementation of the CoreCharacter class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "GenericCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineActor.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineDTO.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineState.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

AGenericCharacter::AGenericCharacter()
{
	bAiming			= false;
	bIsReloading	= false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	InteractiveMovementComponent = CreateDefaultSubobject<UInteractiveMovementComponent>("InteractiveMovementComponent");
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	VoiceLineComponent = CreateDefaultSubobject<UVoiceLineComponent>("VoiceLineComponent");
	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>("StatusEffectComponent");
	StatusEffectComponent->SetAttachedComponent(GetMesh());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	// turn in place calculation defaults
	PrevYaw = 0.f;
	bHavePrevYaw = false;
	YawSpeedRaw = 0.f;
	YawSpeedSmoothed = 0.f;
	YawDeadZoneDegPerSec = 1.0f;
	YawSmoothingSpeed = 10.0f;
}

void AGenericCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(HealthComponent))
	{
		FLogger::ErrorLog("Invalid HealthComponent pointer in GenericCharacter.BeginPlay()");
		return;
	}

	HealthComponent->SetMaxHealth(100.0f);
	HealthComponent->SetHealth(100.0f);
	if (!HealthComponent->OnDeathDelegate.IsAlreadyBound(this, &AGenericCharacter::OnDeath))
		HealthComponent->OnDeathDelegate.AddDynamic(this, &AGenericCharacter::OnDeath);
	
	if (!IsValid(InteractiveMovementComponent))
	{
		FLogger::ErrorLog("Invalid InteractiveMovementComponent pointer in GenericCharacter.BeginPlay()");
		return;
	}

	if (!InteractiveMovementComponent->OnVaultDelegate.IsAlreadyBound(this, &AGenericCharacter::OnVaultListener))
	{
		InteractiveMovementComponent->OnVaultDelegate.AddDynamic(this, &AGenericCharacter::OnVaultListener);
	}
	StatusEffectComponent->OnChangeStatusEffectDelegate.AddDynamic(this, &AGenericCharacter::OnStatusEffectChangeListener);
}

void AGenericCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateRotationSpeed(DeltaTime);
}

void AGenericCharacter::CalculateRotationSpeed(float DeltaTime)
{
	if (DeltaTime <= KINDA_SMALL_NUMBER)
	{
		return;
	}
	
    const float CurrYaw = GetActorRotation().Yaw;

    if (bHavePrevYaw)
    {
    	const float DeltaYaw = FMath::FindDeltaAngleDegrees(PrevYaw, CurrYaw);
    	float YawPerSec = DeltaYaw / DeltaTime;

    	// Kill noise
    	if (FMath::Abs(YawPerSec) < YawDeadZoneDegPerSec)
    	{
    		YawPerSec = 0.f;
    	}
    	YawSpeedRaw = YawPerSec;

    	// Exponential smoothing
    	SmoothRotation(DeltaTime);
    }
    else
    {
    	bHavePrevYaw = true;
    	YawSpeedSmoothed = 0.f;
    }

    PrevYaw = CurrYaw;
}

void AGenericCharacter::SmoothRotation(float DeltaTime)
{
	if (YawSmoothingSpeed > 0.f)
	{
		YawSpeedSmoothed = FMath::FInterpTo(
			YawSpeedSmoothed,      
			YawSpeedRaw,           
			DeltaTime,
			YawSmoothingSpeed
		);
	}
	else
	{
		YawSpeedSmoothed = YawSpeedRaw;
	}
}

void AGenericCharacter::KillEntity() const
{
	GetHealthComponent()->SubtractHealth(999);
}

void AGenericCharacter::OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned)
{
	// TODO: Extend PlayerStateComponent to include GenericCharacter.
}

void AGenericCharacter::OnVaultListener()
{
}

bool AGenericCharacter::PlayMontage(UAnimMontage* Montage, const float PlayRate) const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(Montage))
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_SetPlayRate(Montage, PlayRate);
		return true;
	}
	return false;
}


// TODO: Move to StateComponent.

bool AGenericCharacter::UpdateAimState(const bool bIsAiming)
{
	if(IsAiming() == bIsAiming)
	{
		return IsAiming();
	}
		
	SetAiming(bIsAiming);

	// If we are covered, we change to covered states.
	if(IsValid(CoverMovementComponent) && CoverMovementComponent->GetCoveredState() == ECoveredState::InCover)
	{
		EInsideCoverState NewState = IsAiming() ? EInsideCoverState::Aiming : EInsideCoverState::BaseCovered;
		CoverMovementComponent->SetInsideCoverState(NewState);
	}

	return IsAiming();
}



// TODO: Move to HealthComponent.
void AGenericCharacter::SubtractHealth(const float HealthToSubtract, EWeaponType DamageType)
{
	if (!IsValid(HealthComponent))
	{
		FLogger::ErrorLog("Invalid HealthComponent pointer in GenericCharacter.TriggerVoiceLine()");
		return;	
	}

	// Save last damage type
	LastDamageType = DamageType;
	
	HealthComponent->SubtractHealth(HealthToSubtract);
}

void AGenericCharacter::SetHealth(const float NewHealth)
{
	if (!IsValid(HealthComponent))
	{
		FLogger::ErrorLog("Invalid HealthComponent pointer in GenericCharacter.SetHealth()");
		return;	
	}
	
	HealthComponent->SetHealth(NewHealth);
}

const float AGenericCharacter::GetHealth() const
{
	return IsValid(HealthComponent) ? HealthComponent->GetHealth() : 0;
}
void AGenericCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Generic Character On Death"));
	SetDead();
}

bool AGenericCharacter::IsDead() const
{
	
	if (!IsValid(this)) return false;
	
	UHealthComponent* HealthComp = GetHealthComponent();
	if (!IsValid(HealthComp))
	{
		return false;
	}

	if (HealthComp->IsDead())
	{
		return true;
	}

	return false;
}


void AGenericCharacter::SetDead()
{
	if (IsValid(HealthComponent) && HealthComponent->IsDead())
	{
		// Lanza el delegate
		OnDeathChanged.Broadcast(this);
	}
}
