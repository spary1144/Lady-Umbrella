// Fill out your copyright notice in the Description page of Project Settings.

#include "ControllerSettingsSection.h"

#include "LadyUmbrella/UI/Basics/Input/InputType/SelectorInput.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

void UControllerSettingsSection::NativeConstruct()
{
	Super::NativeConstruct();
}

void UControllerSettingsSection::GetInputMappingList(TArray<FString>& OutList)
{
	OutList.Add(FString("Input 1"));
	OutList.Add("Input 2");
	OutList.Add("Input 3");
}


void UControllerSettingsSection::UpdateInputMapping(uint8 Index)
{
	FLogger::InfoLog("[TODO] Update Input Mapping");
}
