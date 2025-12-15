// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/MenuSection.h"
#include "ControllerSettingsSection.generated.h"

class UAnimatedBaseButton;
class UTextBlock;
class APlayerCharacter;
class UInputElement;

UCLASS()
class LADYUMBRELLA_API UControllerSettingsSection : public UMenuSection
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void GetInputMappingList(TArray<FString>& OutList);
	
	UFUNCTION() void UpdateInputMapping(uint8 Index);
};