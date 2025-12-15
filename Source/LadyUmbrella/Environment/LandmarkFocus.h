#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/GenericInteractable.h"
#include "LandmarkFocus.generated.h"

class UInputAction;
UCLASS(/*HideCategories = (Rendering, Replication, Input, Actor, LOD, Cooking, HLOD, Collision, Physics, AssetUserData, Navigation)*/)
class ALandmarkFocus : public AGenericInteractable
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category=LandmarkFocus, meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* FocusPointIcon;
	
	UPROPERTY(EditAnywhere, Category=LandmarkFocus, meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* OriginPointIcon;
#endif

	UPROPERTY(EditAnywhere, Category=LandmarkFocus, meta = (AllowPrivateAccess = "true"))
	float LookTime;
	
	UPROPERTY(VisibleAnywhere, Category=LandmarkFocus, meta = (MakeEditWidget = true, AllowPrivateAccess = "true"))
	FTransform FocusPointLocation;

public:
	ALandmarkFocus();
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	FORCEINLINE FTransform GetFocusPointTransform() const { return FocusPointLocation; }
	FORCEINLINE FVector GetFocusPointLocation() const { return FocusPointLocation.GetLocation(); }
	FORCEINLINE virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return PLAYER_CHARACTER; };
	
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	UFUNCTION() void StopFocusing();

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

private:
	FTimerHandle ActionHandler;
};