#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PiecesCount.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAcquiredPieces, uint32, ToAdd, uint32, CurrTotal);
UCLASS()
class LADYUMBRELLA_API UPiecesCount : public UUserWidget
{
	GENERATED_BODY()
	
	const float ANGLE_TO_FULL_SPAN = 2.0f;
	const float CROSSHAIR_VERTICAL_CENTER = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* Total;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* PrevTotal;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* Added;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* AddPiecesAnim;
	
public:

	static FAcquiredPieces AcquiredPieces;

	UFUNCTION()
	void ShowAdding(uint32 ToAdd, uint32 CurrTotal);

protected:
	virtual void NativeConstruct() override;
};
