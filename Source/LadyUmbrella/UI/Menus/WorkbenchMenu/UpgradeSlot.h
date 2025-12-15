// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeComponent.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "UpgradeSlot.generated.h"

DECLARE_DELEGATE_OneParam(ProgressBarEndedDelegate, int32);

class UHorizontalBox;
class UTextBlock;
class UProgressBar;
class UButton;
class UBorder;
class UWidgetSwitcher;

UCLASS()
class LADYUMBRELLA_API UUpgradeSlot : public UAnimatedBaseButton
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* TextUpgradePrice;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* IconUpgradePrice;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* UpgradeIcon;

	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* HoldAnim;
	
	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* UnavailableAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	float HoldTime;

	UPROPERTY(EditAnywhere, Category = "Behaviour")
	USoundBase* UpgradeSound;
	
public:
	ProgressBarEndedDelegate ProgressEndedDelegate;
	
	FORCEINLINE int32 GetUpgradeId() const{ return UpgradeId; }
	FORCEINLINE float GetHoldTime() const{ return HoldTime; }
	FORCEINLINE FName& GetDescription() { return UpgradeDescription; }
	FORCEINLINE TObjectPtr<UTextBlock> GetPriceBlock() { return TextUpgradePrice; }
	FText GetPriceText() { return TextUpgradePrice->GetText(); }
	FORCEINLINE TObjectPtr<UImage> GetUpgradeIcon() { return UpgradeIcon; }
	FORCEINLINE TObjectPtr<UTexture2D> GetIconImage() { return Cast<UTexture2D>(UpgradeIcon->GetBrush().GetResourceObject()); }

	FORCEINLINE void SetUpgradeId(const int32 UpgradeIdentifier) { UpgradeId = UpgradeIdentifier; }
	FORCEINLINE void SetHoldTime(const float NewTime) { HoldTime = NewTime; }
	FORCEINLINE void SetDescription(const FName& Description) { UpgradeDescription = Description; }
	FORCEINLINE void SetUpgradeImage(const TObjectPtr<UTexture2D> NewTexture) { UpgradeIcon->SetBrushFromTexture(NewTexture); }
	void SetPriceText(const FText& Price);
	void UpdateLevel(FUpgrade& UpgradeToShow);
	
	/**
     * Prepares the widget to show the info from the Upgrade given in the TPair
     * @param UpgradeToShow Contained in the UpgradeComponent->Upgrades. Gives the info from an upgrade
     */
    void SetupUpgradeSlots(const FUpgrade& UpgradeToShow);

	UFUNCTION() void StartHold();
	UFUNCTION() void StopHold();
	
protected:
	FTimerHandle TimerHandleHeldButton;
	
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	UPROPERTY() UAudioComponent* AudioComp;
	
private:
	int32 UpgradeId;
	FName UpgradeDescription;
	bool bIsUnavailable;
	
	UFUNCTION() void FinishHold();
};
