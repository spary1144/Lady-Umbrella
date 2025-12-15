// 
// Crosshair.h
// 
// Crosshair interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"

class UImage;
class UCanvasPanel;

UCLASS()
class LADYUMBRELLA_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()
	
	const float ANGLE_TO_FULL_SPAN = 2.0f;
	const float CROSSHAIR_VERTICAL_CENTER = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* Crosshair;

	UPROPERTY(EditAnywhere, Category="On Enemy Color", meta = (AllowPrivateAccess = true))
	FLinearColor OnEnemyCrosshairColor;

	UPROPERTY(EditAnywhere, Category="Crosshair Border", meta = (AllowPrivateAccess = true))
	bool bIsLeftBorder;

	UPROPERTY(EditAnywhere, Category="Crosshair Border", meta = (AllowPrivateAccess = true))
	bool bIsRightBorder;

	UPROPERTY()
	UCanvasPanel* CrosshairPanel;

	UPROPERTY()
	FLinearColor BaseCrosshairColor;

	UPROPERTY()
	UUserWidget* OwnerWidget;

	UPROPERTY()
	UWidgetAnimation* ShootingCadenceAnim;

	void UpdateCrosshairState(const bool IsAiming, const bool IsShieldOpen, const bool IsOnEnemy,const bool IsOnHookable);

	void UpdateCrosshairSize(const float BulletRange, const float SpreadAngleX, const float SpreadAngleZ) const;

	void UpdateCrosshairAnimation(const bool bIsShooting) const;

	void UpdateCrosshairCadence(const FTimerHandle ShootingTimer) const;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();
 
	FORCEINLINE bool GetIsLeftBorder() const { return bIsLeftBorder; }
	FORCEINLINE bool GetIsRightBorder() const { return bIsRightBorder; }
	
	void SetAnimationOwner(UUserWidget* InOwner, UWidgetAnimation* InAnim);

protected:

	virtual void NativeConstruct() override;
	
};
