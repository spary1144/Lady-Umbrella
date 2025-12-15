// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgencyCharacter.h"
#include "ShieldAgencyCharacter.generated.h"

class AAIControllerShieldAgency;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AShieldAgencyCharacter : public AAgencyCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "weapon", meta = (AllowPrivateAccess = "true"))
	AAgencyShield* Shield;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAgencyShield> ShieldSubclass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "weapon", meta = (AllowPrivateAccess = "true"))
	float StunDurationWhenShieldBreaks;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(AllowPrivateAccess = "true",ToolTip = "Speed when chracter is moving to pladyer",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedAproachPlayer;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(AllowPrivateAccess = "true",ToolTip = "Speed when chracter is being shooted by Player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnBeingAttacked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage* StunAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = true))
	float StunMontagePlayRate;

	UPROPERTY()
	AAIControllerShieldAgency* ShieldController;

	//Contants
	FString const ResumeLogicReason = FString("EndStun");
	FString const PauseLogicReason = FString("StartStun");
public:
	// Delegate que se dispara cuando el escudo se destruye
	// UPROPERTY(BlueprintAssignable, Category = "Shield")
	// FOnShieldDestroyedSignature OnShieldDestroyed;
	
	UFUNCTION()
	void HandleShieldDestroyed();

	UFUNCTION()
	void OnStunMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	virtual AActor* GetShield() const override;

	virtual void BeginPlay() override;

	//speed getters
	float GetSpeedOnBeingAttacked() const { return SpeedOnBeingAttacked; };
	float GetSpeedOnFindPlayer() const { return SpeedOnFindPlayerCovered; };
	float GetSpeedOnAproachPlayer() const { return SpeedAproachPlayer; };
protected:
	virtual void LoadEnemyFromDataTable() override;
private:
	AShieldAgencyCharacter();
};
