// 
// StatusEffectComponent.cpp
// 
// Implementation of the 'StatusEffectComponent' class.  
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "StatusEffectComponent.h"

#include "LadyUmbrella/Assets/VFXAssets.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectDTO.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

// +-------------------------------------------------+
// |                                                 |
// |                    FUNCTIONS                    |
// |                                                 |
// +-------------------------------------------------+
UStatusEffectComponent::UStatusEffectComponent()
{
	AttachedComponent = nullptr;

	StatusEffect = EStatusEffect::NORMAL;
	StatusEffectDTO.Emplace(EStatusEffect::SMOKED, { FTimerHandle(), nullptr, FVFXAssets::ElectricNiagaraSystem });

	FmodStatusComponent = CreateDefaultSubobject<UFModComponentInterface>("Status Effect Fmod Component Interface");

	UFMODEvent* ElectrifiedEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/GENERIC_CHARACTER_ELECTRIFIED.GENERIC_CHARACTER_ELECTRIFIED'");
	FmodUtils::TryAddEvent(ElectrifiedEvent, FmodStatusComponent);
	
}

void UStatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	const bool bIsEnemy = Owner->IsA(AEnemyCharacter::StaticClass());
	UNiagaraSystem* ChosenSystem = bIsEnemy
		? FVFXAssets::ElectricEnemiesNiagaraSystem
		: FVFXAssets::ElectricNiagaraSystem;

	AttachedComponent = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!AttachedComponent)
	{
		AttachedComponent = Owner->GetRootComponent();
	}

	StatusEffectDTO.Emplace(EStatusEffect::ELECTRIFIED, { FTimerHandle(), nullptr, ChosenSystem });
}


void UStatusEffectComponent::ApplyElectricityEffect(const float Duration)
{
	if (IsElectrified())
	{
		return;
	}
	
	if (FmodStatusComponent->SetCurrentEvent("GENERIC_CHARACTER_ELECTRIFIED"))
	{
		FmodStatusComponent->PlayEvent();
	}
	ChangeStatus(EStatusEffect::ELECTRIFIED, Duration, true);
}

void UStatusEffectComponent::ApplySmokeEffect(const float Duration)
{
	if (IsSmoked())
	{
		return;
	}

	ChangeStatus(EStatusEffect::SMOKED, Duration, true);
}

void UStatusEffectComponent::ChangeStatus(const EStatusEffect Key, float Duration, bool bStun)
{
	FStatusEffectDTO* DTO = StatusEffectDTO.Find(Key);
	if (!DTO || !DTO->NiagaraSystem)
	{
		return;
	}

	if (StatusEffect == Key)
	{
		return;
	}
	
	StatusEffect = Key;
	OnChangeStatusEffectDelegate.Broadcast(StatusEffect, bStun);

	DTO->NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		DTO->NiagaraSystem,
		AttachedComponent,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		false,
		false
	);

	if (USkeletalMeshComponent* Skel = Cast<USkeletalMeshComponent>(AttachedComponent))
	{
		DTO->NiagaraComponent->SetVariableObject(FName(TEXT("User.SkeletalMesh")), Skel);
	}

	DTO->NiagaraComponent->Activate(true);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("ResetStatus"), Key);
	GetWorld()->GetTimerManager().SetTimer(DTO->TimerHandle, TimerDelegate, Duration, false);
}


void UStatusEffectComponent::ResetStatus(const EStatusEffect Key)
{
	FStatusEffectDTO* DTO = StatusEffectDTO.Find(Key);
	if (!DTO)
	{
		FLogger::ErrorLog("Missing DTO in ResetStatus");
		return;
	}

	if (DTO->NiagaraComponent)
	{
		if (DTO->NiagaraComponent->IsActive())
		{
			DTO->NiagaraComponent->Deactivate();
		}
		DTO->NiagaraComponent->DestroyComponent();
		DTO->NiagaraComponent = nullptr;  // IMPORTANT
	}

	GetWorld()->GetTimerManager().ClearTimer(DTO->TimerHandle);

	StatusEffect = EStatusEffect::NORMAL;
	OnChangeStatusEffectDelegate.Broadcast(EStatusEffect::NORMAL, false);
}

// +-------------------------------------------------+
// |                                                 |
// |                GETTERS & SETTERS                |
// |                                                 |
// +-------------------------------------------------+
bool UStatusEffectComponent::IsElectrified() const
{
	return StatusEffect == EStatusEffect::ELECTRIFIED;
}

bool UStatusEffectComponent::IsSmoked() const
{
	return StatusEffect == EStatusEffect::SMOKED;
}

bool UStatusEffectComponent::IsStunned() const
{
	return StatusEffect != EStatusEffect::NORMAL;
}

void UStatusEffectComponent::SetInComms(const bool bNewInComms)
{
	StatusEffect = bNewInComms ? EStatusEffect::COMMS : EStatusEffect::NORMAL;
}

bool UStatusEffectComponent::IsInComms() const
{
	return StatusEffect == EStatusEffect::COMMS;
}