#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Weapons/Umbrella/GadgetType.h"

#include "TriggerBoxGadgets.generated.h"

class USkipButtom;
class ATriggerLevelChange;
class ULevelSequence;
class UBoxComponent;
class ATriggerLevelChange;

UCLASS()
class LADYUMBRELLA_API ATriggerBoxGadgets : public AActor
{

	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	bool IsActive;
	
	bool CinematicTriggered;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionTriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Box Gadgets", meta = (AllowPrivateAccess = "true"))
	EGadgetType UnlockType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Box Gadgets", DisplayName = "Sequence to Reproduce", meta = (AllowPrivateAccess = "true"))
	ULevelSequence* LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI",meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> WidgetSkipClass;

	UPROPERTY()
	USkipButtom* ActiveWidget;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	ATriggerLevelChange* LevelChangeRef;
	
	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleSequenceFinished();

	UFUNCTION()
	void HandleSequenceStop();
	
	ATriggerBoxGadgets();
	
};
