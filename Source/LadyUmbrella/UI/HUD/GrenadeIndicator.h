// 
// GrenadeIndicator.h
// 
// Grenade widget that shows the location of a thrown grenade.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GrenadeIndicator.generated.h"

class UCanvasPanel;
class UBorder;
class UOverlay;
class AGrenade;
class APlayerController;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UGrenadeIndicator : public UUserWidget
{
	GENERATED_BODY()

	const float ALIGN_CENTER = 0.5f;
	const float ALIGN_ABOVE_GRENADE = 1.3f;
	const float RESET_ANGLE = 0.0f;
	const float DEGREE_TO_MATCH_IMAGE = 90.0f;
	const float CLAMP_MIN_OFF_SCREEN = -0.1f;
	const float CLAMP_MAX_OFF_SCREEN = 1.1f;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBorder* Indicator;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* GrenadeImage;

	UPROPERTY()
	UCanvasPanel* GrenadePanel;
	
	UPROPERTY()
	TWeakObjectPtr<AGrenade> BoundGrenade;

	UFUNCTION()
	void UpdateGrenadeIndicator(const FVector& GrenadeLocation);

	UFUNCTION()
	void ShowOnScreen(const FVector2D Grenade2D, const FVector2D ViewportSize, const FVector2D CanvasPanelSize) const;

	UFUNCTION()
	void ShowOffScreen(const FVector& Grenade3D, const APlayerController* PlayerController, const FVector2D CanvasPanelSize) const;

public:

	UFUNCTION()
	void Show(AGrenade* Grenade);

	UFUNCTION()
	void Hide();

protected:
	
	virtual void NativeConstruct() override;
};
