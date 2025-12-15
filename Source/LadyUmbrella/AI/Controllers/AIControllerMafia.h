// 
// <AIController_generic.h>
// 
// Generic class for AIController
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "AIControllerGeneric.h"
#include "AIControllerMafia.generated.h"

//fordward declarations
struct FAIStimulus;
class ACoverPoint;
class AArenaManager;
//States
// enum EStates : uint8_t
// {
// 	EnterArena,
// 	Idle,
// 	Patrol,
// 	SearchCover,
// 	Attack
// };
//UENUM(BlueprintType)
UENUM(BlueprintType)
enum EStates
{
	EnterArena 	UMETA(DisplayName = "EnterArena"),
	Idle 		UMETA(DisplayName = "Idle"),
	Patrol 		UMETA(DisplayName = "Patrol"),
	SearchCover UMETA(DisplayName = "SearchCover"),
	Attack 		UMETA(DisplayName = "Attack")
};
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AAIControllerMafia : public AAIControllerGeneric
{
	GENERATED_BODY()
	
#pragma region IA
public:
	UPROPERTY(EditAnywhere, Category = "IA")
	float FleeRadius;
	
	UPROPERTY(EditAnywhere, Category = "IA")
	float CloseRadius;

	UPROPERTY(EditAnywhere, Category = "IA")
	bool bActiveSight;

	UPROPERTY(EditAnywhere, Category = "IA")
	bool bActiveHear;
	
	// UPROPERTY()
	// APawn* OwnerPawn;
protected:
	// UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAcees = "true"))
	// UBehaviorTree* behaviourTree;
#pragma endregion
	//Covers already selected
	// UPROPERTY();
	// TArray<AActor*> CoversSelected;
private:

	//virtual void BeginPlay() override;
	//AArenaManager* CurrentArenaManager;

	class UAISenseConfig_Sight* SightConfig;

	class UAISenseConfig_Hearing* HearingConfig;
public:
	explicit AAIControllerMafia(FObjectInitializer const& ObjectInitializer);

	//void InitializeCoordination();
	//void Add Cover
	//void AddCoverAsSelected(AActor* cover);
	
	virtual void DetectPlayer(AActor* PlayerCharacter,bool bDetectionAccordingToDistance) override;

	virtual void InitializeController(AArenaManager* ArenaManager, const AAISpawnPoint& SpawnPoint) override;
protected:
	virtual void OnPossess(APawn* InPawn) override;

	//begin play -> set zone in bb
	virtual void BeginPlay() override;
private:
	// void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
