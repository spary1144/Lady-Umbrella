// 
// MafiaCharacter.h
// 
// Enemy character belonging to the Mafia.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "MafiaCharacter.generated.h"

class AGrenade;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrenadeTokenReleased);

UCLASS()
class LADYUMBRELLA_API AMafiaCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY()
	AGrenade* Grenade;
	
	UPROPERTY(VisibleAnywhere, Category = "Grenade")
	bool bHasGrenadeToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = true))
	TSubclassOf<AGrenade> GrenadeBP;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float MinThrowGrenadeDistance;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float MaxThrowGrenadeDistance;

	//Speeds
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when chracter is fleeing",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnFleeing;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is close to player",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnCloseCombat;
	
	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is enter arena called by event",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnEnterArena;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is patroling",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnPatroling;

	UPROPERTY(EditAnywhere, Category = "Speeds",meta=(ToolTip = "Speed when character is moving towards one Cover",ClampMin="0", UIMin="0", ForceUnits="cm/s")) 
	float SpeedOnMoveToCover;

	UPROPERTY()
	FTimerHandle ResetCollisionTimerHandler;

	const FString FMOD_EVENT_IDENTIFIER = FString(TEXT("MAFIA_")); //MAFIA_
	const FString OWNER_NAME = FString(TEXT("Mafia"));

	UFUNCTION()
	void OnMontageNotifyBeginMafia(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void ResetCollision();

	UFUNCTION()
	void OnGrenadeBouncedByPlayer();
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Temp")
	void OnGrenadeThrowEvent();
	
	UPROPERTY(EditAnywhere, Category = "Grenade")
	FVector GrenadeEndPosition;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeEndLocationZOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeEndLocationForwardOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeEndLocationLateralOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeMinLateralOffset;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	float GrenadeArc;
	
	virtual void LoadEnemyFromDataTable() override;
	virtual void SetDead() override;
public:
	AMafiaCharacter();

	virtual void PostInitProperties() override;
	
	virtual void OnDetectPlayer() override;
	
	static FOnGrenadeTokenReleased OnGrenadeTokenReleased;

	UFUNCTION()
	void OnThrowGrenadeEnd(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void ThrowGrenade();

	FORCEINLINE void SetHasGrenadeToken(bool HasGrenadeToken) { bHasGrenadeToken = HasGrenadeToken; }
	FORCEINLINE bool GetHasGrenadeToken() const { return bHasGrenadeToken; }

	//speeds getters
	FORCEINLINE float GetSpeedOnFleeing() const { return SpeedOnFleeing; }
	FORCEINLINE float GetSpeedOnCloseCombat() const { return SpeedOnCloseCombat; }
	FORCEINLINE float GetSpeedOnEnterArena() const { return SpeedOnEnterArena; }
	FORCEINLINE float GetSpeedOnPatrolling() const { return SpeedOnPatroling; }
	FORCEINLINE float GetSpeedOnMoveToCover() const { return SpeedOnMoveToCover; }
	FORCEINLINE float GetMinThrowGrenadeDistance() const { return MinThrowGrenadeDistance; }
	FORCEINLINE float GetMaxThrowGrenadeDistance() const { return MaxThrowGrenadeDistance; }
};
