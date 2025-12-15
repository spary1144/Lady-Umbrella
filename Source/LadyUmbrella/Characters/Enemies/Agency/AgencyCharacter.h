// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "AgencyCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDestroyedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnElectricBulletTokenRequest, bool, bIsRequesting, AAgencyCharacter*, AgencyCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlankTokenRequest, bool, bIsRequesting, AAgencyCharacter*, AgencyCharacter);

class AAgencyShield;

UCLASS()
class LADYUMBRELLA_API AAgencyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

	const FString FMOD_EVENT_IDENTIFIER = FString(TEXT("AGENCY_"));
	const FString OWNER_NAME = FString(TEXT("Agency"));
	const FVector PLAYER_CHEST_OFFSET_FROM_ORIGIN = FVector(0.f, 0.f, 30.f);
	const float CHECK_ENEMY_IN_FRON_TRACE_DISTANCE = 1000.f;
	const float SPHERE_TRACE_RADIUS = 32.f;
	const float MIN_DOT_PRODUCT_TO_CONSIDER_OUT_OF_CAMERA = -0.3;

	UPROPERTY()
	FTimerHandle ActivatingElectricDardDelegate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int32 NumRafaguesRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int32 MinNumRafagues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int32 MaxNumRafagues;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	bool bHasElectricBullet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	bool bHasFlankToken;

	//Speeds
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is moving crounched between Covers",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnMoveBetweenCovers;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is close to player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnCloseCombat;
	
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is moving around their own cover",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnMoveAroundCover;


protected:
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character find player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnFindPlayerCovered;
	//virtual void BeginPlay() override;
	AAgencyCharacter();
public:
	
	static FOnElectricBulletTokenRequest OnElectricBulletTokenRequest;
	static FOnFlankTokenRequest OnFlankTokenRequest;
	
	FORCEINLINE int32 GetRemainingRafagues() { return NumRafaguesRemaining;}
	FORCEINLINE void SetRemainingRafagues(int32 NewRafaguesRemaining) { NumRafaguesRemaining = NewRafaguesRemaining; }
	void SetRandomNumRafagues();

	//VoiceLines
	void PlayFlankVoiceLine();
	
	virtual void BeginPlay() override;

	virtual void LoadEnemyFromDataTable() override;

	virtual void SetDead();

	virtual void OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned) override;

	void FireElectricProjectile();
	
	UFUNCTION()
	void SpawnElectricProjectile();

	void ResetElectricShootLogic() const;
	
	void AbortElectricShoot();
	
	bool IsEnemyInFrontOf();
	bool IsPlayerTurnHisBack() const;

	//Speeds getters
	float GetSpeedOnMoveBetweenCovers() const { return SpeedOnMoveBetweenCovers;}
	float GetSpeedOnCloseCombat() const { return SpeedOnCloseCombat;}
	float GetSpeedOnMoveAroundCover() const { return SpeedOnMoveAroundCover;}
	float GetSpeedOnFindPlayer() const { return SpeedOnFindPlayerCovered;}
	
	FORCEINLINE void SetHasElectricBullet(bool bNewHasElectricBullet) { bHasElectricBullet = bNewHasElectricBullet; }
	FORCEINLINE bool GetHasElectricBullet() const { return bHasElectricBullet; }

	FORCEINLINE void SetHasFlankToken(bool bNewHasFlankToken) { bHasFlankToken = bNewHasFlankToken; }
	FORCEINLINE bool GetHasFlankToken() const { return bHasFlankToken; }
};
