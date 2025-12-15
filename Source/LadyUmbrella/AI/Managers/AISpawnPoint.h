// 
// AISpawnPoint.h
// 
// Actor that represents an SpawnPoint managed by ArenaManager
// Contains Enemy Arquetype and Spline Path Data
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawnPoint.generated.h"

//fordward declarations
class ASplinePath;
class AEnemyCharacter;
class AArenaManager;
class UArrowComponent;
class AGenericWeapon;
class AReactiveElement;
class UBillboardComponent;
class ATriggerArenaDoor;
class ATriggerArenaWindow;

//targets
UENUM(BlueprintType)
enum class ETarget : uint8
{
	Enemy,
	Interactable
};

UCLASS()
class LADYUMBRELLA_API AAISpawnPoint : public AActor
{
	GENERATED_BODY()
private:
#pragma region COMPONENTS
	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	UBillboardComponent* Billboard;

	// Billboard targets
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* MafiaIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* AgencyIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* AgencyShieldIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UTexture2D* InteractableIcon;
#if WITH_EDITORONLY_DATA

	UPROPERTY(VisibleAnywhere, Category="EditorOnly")
	UArrowComponent* Arrow;
#endif
#pragma endregion COMPONENTS
	
public:
#pragma region CONFIGURATION_DATA
	//Spawn Data
	UPROPERTY(EditAnywhere, Category="Config",meta = (BlueprintBaseOnly, DisplayPriority = 0))
	ETarget Target;
	
	UPROPERTY(EditInstanceOnly, Category="Config", meta = (BlueprintBaseOnly, DisplayPriority = 0,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	TSubclassOf<AEnemyCharacter> EnemyType;

	UPROPERTY(EditInstanceOnly, Category="Config", meta = (BlueprintBaseOnly, DisplayPriority = 0,EditCondition = "Target == ETarget::Interactable",EditConditionHides))
	TSubclassOf<AReactiveElement> ReactiveElementType;

	UPROPERTY(VisibleDefaultsOnly, Transient, Category="Config")
	bool bIsFlankerPropertyVisible = false;
    	
	UPROPERTY(EditInstanceOnly, Category="Config",meta = (DisplayPriority = 0,EditCondition="bIsFlankerPropertyVisible", EditConditionHides))
	bool bIsFlanker;

	UPROPERTY(EditInstanceOnly, Category="Config",meta = (DisplayPriority = 0,EditCondition="bIsFlankerPropertyVisible", EditConditionHides))
	bool bIsFirstCombatAgent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", meta = (DisplayPriority = 0,AllowPrivateAccess = true,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	TSubclassOf<AGenericWeapon> WeaponBP;
	
	UPROPERTY(EditDefaultsOnly,Category="Config",meta=(DisplayPriority = 1,EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	TSubclassOf<ASplinePath> SplineClass;

	UPROPERTY(EditInstanceOnly,Category="Config",meta=(DisplayPriority = 1))
	bool bHasPath;
	
	UPROPERTY(VisibleAnywhere, Category="Config", meta=(DisplayPriority = 1,EditCondition = "bHasPath",EditConditionHides))
	ASplinePath* Path;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta=(DisplayPriority = 1))
	TArray<int32> WavesOnBeActivated;

	UPROPERTY(EditInstanceOnly,Category="Config",meta=(DisplayPriority = 1))
	bool bActivatedbyEvent;

	UPROPERTY(EditInstanceOnly,Category="Config",meta=(DisplayPriority = 1,EditCondition = "bActivatedbyEvent",EditConditionHides))
	int EnterArenaGroup;
	
	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	bool bIsReinforcement;

	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	bool bEnterArenaThroughDoor;

	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "bEnterArenaThroughDoor",EditConditionHides))
	ATriggerArenaDoor* DoorReference;
	
	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	bool bEnterArenaThroughWindow;
	
	UPROPERTY(EditInstanceOnly,Category="Config",meta=(EditCondition = "bEnterArenaThroughWindow",EditConditionHides))
	ATriggerArenaWindow* WindowReference;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int32 SpawnOrder;

	UPROPERTY(VisibleAnywhere,Category="Config",meta=(EditCondition = "Target == ETarget::Enemy",EditConditionHides))
	int NumZone = 0;
	
	UPROPERTY(VisibleAnywhere,Category="Config")
	AArenaManager* OwningArenaManager;
	
	UPROPERTY(EditInstanceOnly,Category="Config")
	bool bIsActive = true;
#pragma endregion CONFIGURATION_DATA
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void ChangeBillboardSprite();
#endif
	
public:
	// Sets default values for this actor's properties
	AAISpawnPoint();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void SetZone(int newzone);

	UFUNCTION()
	int GetZone() const;

	void SetArenaReference(AArenaManager* AttachedArena);
	
	UStaticMeshComponent* GetStaticMesh() const { return StaticMesh;} 

	AArenaManager* GetArenaManager() const { return OwningArenaManager;}
private:
	virtual void Destroyed() override;

	UFUNCTION()
	void RegisterInArenaManager();
};
