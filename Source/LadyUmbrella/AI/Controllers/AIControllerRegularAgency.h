// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerGeneric.h"
#include "AIControllerRegularAgency.generated.h"

// UENUM(BlueprintType)
// enum EStates
// {
// 	Biter 		UMETA(DisplayName = "Biter"),
// 	Flanker 	UMETA(DisplayName = "Flanker")
// };

class AAICoverEntryPoint;

//enum
enum MyEnumFlankCoverStates : uint8
{
	SearchFlankCover,
	SearchIntermediatePoints,
	MoveAroundCovers
};


/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AAIControllerRegularAgency : public AAIControllerGeneric
{
	GENERATED_BODY()

	//Blackboard keys names
	FName const BlackboardKeyNameToKnowCurrentZone = TEXT("CurrentZone");
	FName const BlackboardKeyNameToKnowIfIsFlanker = TEXT("IsFlanker");
	FName const BlackboardKeyNameToKnowIfIsReinforcement = TEXT("IsReinforcement");
	FName const BlackboardKeyNameRushPlayer = TEXT("RushPlayer");
	
	//Covers already selected
	UPROPERTY();
	TArray<AAICoverEntryPoint*> CoversSelected;

	//radio que define cuando se entra en close combat
	UPROPERTY(EditAnywhere, Category = "BiterData")
	float CloseRadius;
	//radio que define si avanza la IA de cover en función de la ddistancia al player
	UPROPERTY(EditAnywhere, Category = "BiterData")
	float AdvanceRadius;
	//radio que define si un flanker debe disparar dardo eléctrico al player
	UPROPERTY(EditAnywhere, Category = "FlankerData")
	float ShootRadius;
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void InitializeController(AArenaManager* ArenaManager, const AAISpawnPoint& SpawnPoint) override;

	virtual void SetZone(int Newzone) override;
public:
	//Deprecated - change to actual system
	bool IsFlankCoverEmpty() const { return CoversSelected.IsEmpty(); }
	void AddFlankCover(AAICoverEntryPoint* newCover) {
		//flankCovers.Add(newCover);
		CoversSelected.Add(newCover);
	}
	TArray<AAICoverEntryPoint*>& GetCoversSelected() { return CoversSelected; }
	void AddCoverAsSelected(AAICoverEntryPoint* cover);
};
