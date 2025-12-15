// 
// TriggerBoxArena.h
// 
// TriggerBox that notify to the Arena linked if need to be Activated, StartCombat or SelectZones for Covers
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TriggerBoxArena.generated.h"

//forward declarations
class AArenaManager;

UENUM()
enum class EPurpose : uint8
{
 StartCombatArena,
 ActivateArena,
 SelectZones,
 EnterArenaEvent,
 ExitArenaEvent		
};
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ATriggerBoxArena : public ATriggerBox
{
 GENERATED_BODY()
#pragma region Configuration
	//purpose of current trigger arena
	UPROPERTY(EditInstanceOnly,Category= "Configuration")
	EPurpose Efunc;
	//arena reference
	UPROPERTY(VisibleAnywhere, Category= "Configuration", meta = (EditCondition = "Efunc == EPurpose::ActivateArena || Efunc == EPurpose::StartCombatArena || Efunc == EPurpose::SelectZones || Efunc == EPurpose::EnterArenaEvent", EditConditionHides))
	AArenaManager* ArenaManagerToActivate;
	//zone to set
	UPROPERTY(EditAnywhere,Category= "Configuration", meta = (EditCondition = "Efunc == EPurpose::SelectZones", EditConditionHides))
	uint8 NumZone;
	//cover class to find in beginplay
	UPROPERTY(EditAnywhere,Category="Configuration")
	UClass* CoverClass;
	//EnterGroup to active
	UPROPERTY(EditAnywhere,Category= "Configuration", meta = (EditCondition = "efunc == EPurpose::EnterArenaEvent", EditConditionHides))
	uint8 NumGroup;
#pragma endregion
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UTexture2D>  SpriteCombat;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteActivate;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteZones;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteEnter;
	UPROPERTY()
	TObjectPtr<UTexture2D> SpriteExit;
#endif
public:
	ATriggerBoxArena();

	void SetArenaReference(AArenaManager* AttachedArena);

#if WITH_EDITORONLY_DATA
	void SetEFunc(const EPurpose& NewPurpose);
	void ResetBillboardSprite();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void NotifyArenaPurpose(AActor* OverlappedActor, AActor* OtherActor);

	//Active arena
	void ActiveArena(AActor* OverlappedActor, AActor* OtherActor) const;
	//Start combat
	void StartCombatArena(AActor* OverlappedActor, AActor* OtherActor) const;
	//Enter Arena Event 
	void EnterArenaEvent(AActor* OverlappedActor, AActor* OtherActor) const;
	//Exit Arena
	void ExitArenaEvent(AActor* OverlappedActor, AActor* OtherActor) const;
	
	
	//Editor function to set zones to spawns and covers
	UFUNCTION(CallInEditor, Category="Configuration",meta = (EditCondition = "efunc == EPurpose::SelectZones",EditConditionHides))
	void SetZones() const;

	UFUNCTION(CallInEditor, Category="Configuration",meta = (EditCondition = "efunc == EPurpose::SelectZones",EditConditionHides))
	void ClearZones() const;

	
};

