#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "LadyUmbrella/UI/Basics/State/UIState.h"
#include "InputElement.generated.h"

class UOverlay;
class UWidgetSwitcher;
class UImage;

UENUM()
enum class EInputElementType : uint8
{
	SELECTOR,
	TOGGLABLE,
	SLIDER,
	OTHER
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFocusChanged, bool);

UCLASS()
class LADYUMBRELLA_API UInputElement : public UUserWidget
{
	GENERATED_BODY()
	
	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* Section;
	
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* Title;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* SelectionOverlay;
	
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* InputWidget;

	// Animations
	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FocusAnim;

	//Text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom", meta = (AllowPrivateAccess = true))
	FName TextCode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom", meta = (AllowPrivateAccess = true))
	FName DescriptionCode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom", meta = (AllowPrivateAccess = true))
	EInputElementType InputType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom", meta = (EditCondition = "InputType == EInputElementType::SELECTOR", EditConditionHides, AllowPrivateAccess = true))
	TArray<FString> SelectionList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	FName InputIdentifier;
	
public:
	FOnFocusChanged OnFocusChanged;
	
	TObjectPtr<UOverlay> GetSection() const { return Section; }
	FORCEINLINE FName GetIdentifier() const { return InputIdentifier; }
	FString GetSectionText() const { return Title->GetText().ToString(); }
	void SetSectionText(const FText& NewText) { Title->SetText(NewText); }
	FORCEINLINE FName GetTitleText() const { return TextCode; }
	FORCEINLINE void SetTitleText(const FName& NewText) { TextCode = NewText; }
	FORCEINLINE EUIState GetState() const { return State; }
	FORCEINLINE FName GetDescription() const { return DescriptionCode; }
	FORCEINLINE void SetDescription(const FName& NewDescription) { DescriptionCode = NewDescription; }
	FORCEINLINE void SetState(EUIState NewState) { State = NewState; }
	FORCEINLINE EInputElementType GetInputType() const { return InputType; }
	void SetInputType(EInputElementType NewType)
	{
		InputType = NewType;
		SynchronizeProperties();
	}
	TObjectPtr<UWidget> GetInputWidget() const { return InputWidget->GetActiveWidget(); }
	FORCEINLINE TArray<FString> GetSelectionList() const { return SelectionList; }

	bool HandleInput(bool bInteractLeft);

protected:
	UPROPERTY() EUIState State;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	void OnLanguageChanged();
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual void SynchronizeProperties() override;

	virtual void BeginDestroy() override;
};

