#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Basics/State/EScreenPosition.h"
#include "HintBase.generated.h"

class UOverlay;
class UImage;
class UTextBlock;
class UWidgetSwitcher;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowChangeTo, bool, bShowing);

UCLASS()
class LADYUMBRELLA_API UHintBase : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization", meta = (AllowPrivateAccess = "true"))
	FName TitleCode; // If it does not have any leave as "None"
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization", meta = (AllowPrivateAccess = "true"))
	FName DescriptionCode; // If it does not have any leave as "None"
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* HintContainer;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidget* InputType;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* HintTitle;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* Separator;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* HintDescription;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeIn;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* ScaleIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bShowInputOnHint = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bShowTitle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bScaleInOut = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bTimed = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(EditCondition="bTimed", EditConditionHides=true, AllowPrivateAccess=true))
	float DisplayDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	float AnimSpeed;

public:
	
	FOnShowChangeTo OnShowChangeTo;
	
	FText GetDescriptionText();
	FText GetTitleText();
	void SetDescriptionText(FText Description);
	void SetTitleText(FText Title);
	
	void SetDescriptionCode(FText Description) { TitleCode = FName(Description.ToString()); }
	void SetTitleCode(FText Title) { TitleCode = FName(Title.ToString()); }

	void SetHintAlignment(EScreenPosition Alignment, FVector2D Offset);
	
	FORCEINLINE bool GetHasTitle() const { return bShowTitle; }
	FORCEINLINE bool GetHasInputImage() const { return bShowInputOnHint; }
	FORCEINLINE void SetHasInputImage(const bool bItDoes) { bShowInputOnHint = bItDoes; }

	FORCEINLINE void SetHasTitle(bool bItDoes) { bShowTitle = bItDoes; }
	FORCEINLINE void SetIsTimed(bool bItIs) { bTimed = bItIs; }

	FORCEINLINE bool IsShowing() const { return bIsShowing; }
	FORCEINLINE bool IsTimed() const { return bTimed; }
	
	UFUNCTION() void ToggleHint();

protected:
	bool bIsShowing;
	FTimerHandle TimerHandle;
	
	UFUNCTION() void OnLanguageChanged();
	
	virtual void NativeOnInitialized() override;
	virtual void SynchronizeProperties() override;
};
