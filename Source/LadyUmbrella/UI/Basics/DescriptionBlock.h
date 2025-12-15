#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DescriptionBlock.generated.h"

class UBorder;
class UImage;
class UTextBlock;

UCLASS()
class UDescriptionBlock : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UBorder* ImageContainer;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* DescriptiveImage;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* TitleText;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* DescriptionText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bHasImage = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bShowPlaceHolder = false;

public:
	FORCEINLINE TObjectPtr<UTextBlock> GetTitle() { return TitleText; }
	FORCEINLINE TObjectPtr<UTextBlock> GetDescription() { return DescriptionText; }
	FORCEINLINE TObjectPtr<UImage> GetImage() { return DescriptiveImage; }
	FORCEINLINE void SetImage(UImage* Image) { DescriptiveImage = Image; }
	
	FText GetDescriptionText() const;
	FText GetTitleText() const;
	void SetTitleText(const FText& NewText) const;
	void SetDescriptionText(const FText& NewText) const;

private:
	void SynchronizeProperties();
};
