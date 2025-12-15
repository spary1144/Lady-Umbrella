// 
// StatusEffectComponent.h
// 
// Controls the 'Status effect' of an actor, which is anything that causes a 'status' change.
// For example, electrification, smoke bomb, etc...
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "StatusEffect.h"

#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectDTO.h"
#include "StatusEffectComponent.generated.h"

class UFModComponentInterface;
class UNiagaraComponent;
class UNiagaraSystem;

enum class EStatusEffect : uint8;
enum class ECoveredState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeStatusEffect, EStatusEffect, StatusEffect, bool, bStun);

UCLASS()
class LADYUMBRELLA_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

	// +-------------------------------------------------+
	// |                                                 |
	// |                    VARIABLES                    |
	// |                                                 |
	// +-------------------------------------------------+
	EStatusEffect StatusEffect;
	TMap<EStatusEffect, FStatusEffectDTO> StatusEffectDTO;

	UPROPERTY()TObjectPtr<USceneComponent> AttachedComponent;

	UPROPERTY()
	TObjectPtr<UFModComponentInterface> FmodStatusComponent;
	
public:

	// NOTE: Delegates CANNOT be private because otherwise they cant broadcast outwards.
	FOnChangeStatusEffect OnChangeStatusEffectDelegate;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	UStatusEffectComponent();
	virtual void BeginPlay() override;

	void ApplyElectricityEffect(const float Duration);
	void ApplySmokeEffect(const float Duration);

private:

	void ChangeStatus(const EStatusEffect Key, const float Duration, const bool bStun);
	UFUNCTION() void ResetStatus(const EStatusEffect Key);

public:
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                GETTERS & SETTERS                |
	// |                                                 |
	// +-------------------------------------------------+
	FORCEINLINE EStatusEffect GetStatusEffect() const { return StatusEffect; }

	FORCEINLINE void SetAttachedComponent(const TObjectPtr<USceneComponent>& Value) { AttachedComponent = Value; }
	
	// NOTE: These seem redundant but are required for specific blueprint functionality.
	UFUNCTION(BlueprintCallable) bool IsElectrified() const;
	UFUNCTION(BlueprintCallable) bool IsSmoked()  const;
	UFUNCTION(BlueprintCallable) bool IsStunned() const;
	UFUNCTION(BlueprintCallable) bool IsInComms() const;
	UFUNCTION(BlueprintCallable) void SetInComms(bool bNewInComms);
};
