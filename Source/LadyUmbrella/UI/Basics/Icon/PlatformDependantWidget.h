// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Basics/State/EPlatform.h"
#include "PlatformDependantWidget.generated.h"

class UWidgetSwitcher;
class UWidget;

UCLASS()
class LADYUMBRELLA_API UPlatformDependantWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Dependant Behavior", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* DependantSwitch;

	UPROPERTY(BlueprintReadWrite, Category = "Dependant Behavior", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidget* WidgetPC;

	UPROPERTY(BlueprintReadWrite, Category = "Dependant Behavior", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidget* WidgetPS;

	UPROPERTY(BlueprintReadWrite, Category = "Dependant Behavior", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidget* WidgetXB;

public:
	EPlatform GetPlatformName();
	UFUNCTION() void SetWidgetIndex(EPlatform Platform);
	void UpdateForController(bool bConnected, FPlatformUserId UserId, int32 ControllerId);

	FORCEINLINE UWidgetSwitcher* GetDependantSwitch() const { return DependantSwitch; }

protected:
	FDelegateHandle OnPlatformUpdatedDelegateHandle;
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
};
