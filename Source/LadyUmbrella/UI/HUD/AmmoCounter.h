// 
// AmmoCounter.h
// 
// Ammo counter interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoCounter.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class UOverlay;
class UHorizontalBox;
class UHorizontalBoxSlot;
class UScaleBox;
class UPlatformDependantWidget;
enum class EGadgetType : uint8;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UAmmoCounter : public UUserWidget
{
	GENERATED_BODY()

	const int32 MIN_NUMBER_BEFORE_DISPLAY_TWO_DIGITS = 10;
	const float GADGET_READY = 1.0f;
	const float FULL_HEALTH = 1.0f;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* Bullet;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* TotalAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* Hook;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* ElectricDart;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* HookBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* DartBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* HookOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* DartOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* UmbrellaHealth;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UScaleBox* SwapGadgetKeysContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UPlatformDependantWidget* SwapGadgetKeys;
	
	UPROPERTY(EditAnywhere, Category="Gadget Scale When Selected", meta = (AllowPrivateAccess = true))
	float SelectedGadgetX;

	UPROPERTY(EditAnywhere, Category="Gadget Scale When Selected", meta = (AllowPrivateAccess = true))
	float SelectedGadgetY;

	UPROPERTY(EditAnywhere, Category="Color When Hook Is Selected", meta = (AllowPrivateAccess = true))
	FLinearColor SelectedHookColor;

	UPROPERTY(EditAnywhere, Category="Color When Dart Is Selected", meta = (AllowPrivateAccess = true))
	FLinearColor SelectedDartColor;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* CurrentBulletsContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* LowOnBullets;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* ImageDriver;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* Umbrella;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* ShieldOverlay;
	
	UPROPERTY()
	UMaterialInstanceDynamic* CustomShieldFilling;

	UPROPERTY()
	TArray<UImage*> BulletsArray;

	UPROPERTY()
	UHorizontalBoxSlot* OriginalBulletSlot;

	FSlateColor TotalAmmoOriginalColor;
	FVector2D UnSelectedHookScale;
	FVector2D UnSelectedDartScale;
	FProgressBarStyle OriginalHookStyle;
	FProgressBarStyle OriginalDartStyle;

public:
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();

	void UpdateAmmoOnLoad(const int32 AmmoCurrent, const int32 AmmoCurrentMax, const int32 AmmoReserve);
	void UpdateCurrentAmmo(const bool bIsReloading, const int32 AmmoCurrent,const int32 AmmoCurrentMax , const int32 AmmoReserve);
	void UpdateTotalAmmo(int32 NewTotalAmmo) const;
	void UpdateGrappleCooldown(const bool bCanShootGrapple, const FTimerHandle Timer, const float MaxDuration) const;
	void UpdateElectricDartCooldown(const bool bCanShootElectric, const FTimerHandle Timer, const float MaxDuration) const;
	void UpdateSelectedGadget(const EGadgetType GadgetType) const;
	void UpdateGadget(UProgressBar* GadgetBorder, UProgressBar* Gadget, bool bCanShootGadget, const float Value, const float MaxValue) const;
	void UpdateUmbrellaHealth(const int32 Health, const int32 MaxHealth) const;
	void UpdateGadgetUnlocking(const EGadgetType GadgetType, const bool IsUnlocked) const;
	void UpdateBulletsArray(const int32 AmmoCurrentMax);
	static void CopySlotValues(UHorizontalBoxSlot* Target, const UHorizontalBoxSlot* Source);
	void ChangeKeyToMatchGadget(const ESlateVisibility EHook, const ESlateVisibility EDart) const;
	
	FORCEINLINE UImage* GetBullet() const { return Bullet; }
	FORCEINLINE UTextBlock* GetTotalAmmo() const{ return TotalAmmo; }
	FORCEINLINE UProgressBar* GetHook() const { return Hook; }
	FORCEINLINE UProgressBar* GetElectricDart() const { return ElectricDart; }
	FORCEINLINE UProgressBar* GetHookBorder() const { return HookBorder; }
	FORCEINLINE UProgressBar* GetDartBorder() const { return DartBorder; }
	FORCEINLINE UOverlay* GetHookOverlay() const { return HookOverlay; }
	FORCEINLINE UOverlay* GetDartOverlay() const{ return DartOverlay; }
	FORCEINLINE UImage* GetUmbrellaHealth() const { return UmbrellaHealth; }
	FORCEINLINE UScaleBox* GetSwapGadgetKeysContainer() const { return SwapGadgetKeysContainer; }
	FORCEINLINE UPlatformDependantWidget* GetSwapGadgetKeys() const { return SwapGadgetKeys; }
	FORCEINLINE UOverlay* GetShieldOverlay() const { return ShieldOverlay; }
	FORCEINLINE void SetBullet(UImage* NewShotgunBullet) { Bullet = NewShotgunBullet; }
	FORCEINLINE void SetTotalAmmo(UTextBlock* NewTotalAmmo) { TotalAmmo = NewTotalAmmo; }

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
