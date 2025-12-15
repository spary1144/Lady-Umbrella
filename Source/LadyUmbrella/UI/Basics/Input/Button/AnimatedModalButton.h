#pragma once

#include "CoreMinimal.h"
#include "AnimatedBaseButton.h"
#include "LadyUmbrella/UI/Basics/ModalWindow.h"
#include "AnimatedModalButton.generated.h"

UCLASS()
class UAnimatedModalButton : public UAnimatedBaseButton
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UModalWindow* ModalWindow;
	
	// Anim Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* ClickedAnim;
	
protected:
	virtual void NativeOnInitialized() override;
	//virtual void GoToPressed() override;
	virtual void ActivateButton() override;

	UFUNCTION()
	void ManageChoice(bool bConfirmed);
};
