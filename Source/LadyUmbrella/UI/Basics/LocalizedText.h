#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "LocalizedText.generated.h"

UCLASS()
class ULocalizedText : public UTextBlock
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	FName TextCode;
	
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	FORCEINLINE void SetTextCode(const FName Code) { TextCode = Code; }
	FORCEINLINE const FName GetTextCode(const FName Code) { return TextCode; }
	
	UFUNCTION() void OnLanguageChanged();
	
};

