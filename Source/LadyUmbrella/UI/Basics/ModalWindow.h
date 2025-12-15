#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModalWindow.generated.h"

class UButton;
class UImage;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceMade, bool, bConfirmed);

UCLASS()
class UModalWindow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* ConfirmButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* DenyButton;

	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UOverlay* SelectionOverlay;
	
	// Anim Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* ModalAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Button|Value", meta = (AllowPrivateAccess = "true"))
	FName ModalIdentifier;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Custom Button|Event")
	FOnChoiceMade OnChoice;

	UFUNCTION() FORCEINLINE FName GetIdentifier() const { return ModalIdentifier; }

private:
	virtual void NativeOnInitialized() override;

	UFUNCTION() FORCEINLINE void ConfirmClicked() { OnChoice.Broadcast(true); }
	UFUNCTION() FORCEINLINE void DenyClicked() { OnChoice.Broadcast(false); }

};
