// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPointTrigger.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS()
class LADYUMBRELLA_API ACheckPointTrigger : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBoxComponent;

	UFUNCTION()	
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Searches for the next available ID for the current level in the Data Table
	 * @param CurrentLevelName: Name of the current level
	 * @return: Next available ID for the current level to insert into the Data Table
	 */ 
	int32 GetNextIDForLevel(const FString& CurrentLevelName) const;
	
	/**
	 * Searches for the next available ID in the data table to get a Unique ID
	 * @return: Next available UNIQUE ID to insert into the Data Table
	 */
	int32 GetNextAvailableCheckpointId() const;

	/**
	 * Marks the checkpoint as an encounter for later iteration from Encounter Menu
	 */
	UPROPERTY()
	bool bIsEncounter;
	
protected:
	
#if WITH_EDITOR

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Destroyed() override;
	
#endif
	
public:	

	ACheckPointTrigger();

	FVector GetArrowForwardVector() const noexcept;

	FVector GetArrowPosition () const noexcept;

	[[nodiscard]] bool CheckHitPlayer(const AActor* OtherActor) const;

	[[nodiscard]] bool GetTableIsValid() const;
	
	const FTransform& GetArrowTransform () const noexcept;

	void SetIsActive(const bool bIsActive);

	FORCEINLINE bool GetIsEncounter() const noexcept { return bIsEncounter; }

	FORCEINLINE void SetIsEncounter(const bool bNewIsEncounter) { bIsEncounter = bNewIsEncounter; }

	FORCEINLINE int32 GetUniqueCheckpointId() const noexcept { return UniqueCheckpointId; }

	FORCEINLINE void SetUniqueCheckpointId (const int32 NewUniqueCheckpointId) noexcept{ UniqueCheckpointId = NewUniqueCheckpointId; }

	FORCEINLINE TObjectPtr<UArrowComponent> GetPlayerCheckpointArrow() const { return PlayerCheckpointArrow; } 

	// Not sure if this functions have to be public
	
#if WITH_EDITOR
	UFUNCTION(CallInEditor, DisplayName = "Register As Encounter", Category = "Data Driven", meta = (AllowPrivateAccess = "true"))
	void RegisterCheckpointAsEncounter();

	UFUNCTION(CallInEditor, DisplayName = "Unregister As Encounter", Category = "Data Driven", meta = (AllowPrivateAccess = "true"))
	void UnRegisterCheckpointAsEncounter() const;

#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Driven", meta = (AllowPrivateAccess = "true"))
	FName EncounterName;

	UPROPERTY()
	int32 LevelId;
	
	const FName& GetEncounterName() const noexcept;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsActive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* PlayerCheckpointArrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform CheckPointTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	int32 UniqueCheckpointId;
	
	/*
	 *UPROPERTY()
	 *FGameplayTag CheckPointTags;
	*/
	
#pragma region DATADRIVEN
	
	UPROPERTY()
	UDataTable* DataTablePtr;

	/**
	 * Element to control Rows from DataTablePtr Type
	 */
	UPROPERTY()
	FDataTableRowHandle DataTableRowHandler;

#pragma endregion

};



