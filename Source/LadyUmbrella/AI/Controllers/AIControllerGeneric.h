// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerGeneric.generated.h"

class AArenaManager;
class AAISpawnPoint;
/**
 * 
 */


UCLASS()
class LADYUMBRELLA_API AAIControllerGeneric : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAcees = "true"))
	UBehaviorTree* behaviourTree;

	UPROPERTY()
	AArenaManager* CurrentArenaManager;
	
	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	bool bShowDebugIcons;

	UPROPERTY(VisibleAnywhere,Category="IA")
	int CurrentZone;

	//detection
	UPROPERTY(EditAnywhere,Category="IA")
	float BaseTime;

	UPROPERTY(EditAnywhere,Category="IA")
	float DetectionSpeedFactor;
	
	virtual void OnPossess(APawn* InPawn) override;
	//begin play -> set zone in bb
	virtual void BeginPlay() override;
	
public:
	AAIControllerGeneric(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void InitializeController(AArenaManager* ArenaManager, const AAISpawnPoint& SpawnPoint);
	
	void SetPlayerInBlackboard(AActor* PlayerCharacter) const; 

	void SetRushPlayerBlackboard() const;
	
	virtual void DetectPlayer(AActor* PlayerCharacter,bool DetectionAccordingToDistance);

	virtual void SetZone(int Newzone);

	void TryReduceCloseCombatCounter() const;

	void StopBehaviourTree();

	void CleanBlackboard() const;
	
	virtual AArenaManager* GetArena() const;

	UFUNCTION()
	int GetZone() const { return CurrentZone; };
	
	UFUNCTION()
	virtual bool GetShowDebugIcon();

	bool IsInCloseCombat() const;
};
