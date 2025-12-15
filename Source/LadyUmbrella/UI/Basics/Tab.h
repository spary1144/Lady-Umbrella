#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Tab.generated.h"

//class URichTextBlock;

UCLASS()
class UTab : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* Switcher;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (AllowPrivateAccess = "true"))
	FName TextCode;
	
public:
	FORCEINLINE TObjectPtr<UWidgetSwitcher> GetSwitcher() const { return Switcher; }
	FORCEINLINE TObjectPtr<UTextBlock> GetTextWidget() const { return Text; }
	FORCEINLINE bool IsActive() const { return bActive; }
	FORCEINLINE void ToggleTab() { bActive = !bActive; }
	FORCEINLINE void SetTextCode(const FName Code) { TextCode = Code; }
	FORCEINLINE const FName GetTextCode(const FName Code) { return TextCode; }
		
	FText GetText() const;
	void SetText(FText NewText);
	void UpdateTab() const;

	virtual void NativeOnInitialized() override;
	void OnLanguageChanged();

protected:
	bool bActive = false;
};
