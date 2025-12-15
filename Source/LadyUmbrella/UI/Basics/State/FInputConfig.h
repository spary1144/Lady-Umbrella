#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Basics/Input/InputElement.h"
#include "FInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FInputConfig // Created this so that it can be hashable in editor with TMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInputElementType InputType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true, EditConditionHides, EditCondition = "InputType != EInputElementType::OTHER"))
	FText Description = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditConditionHides, EditCondition = "InputType != EInputElementType::OTHER"))
	float InitialState = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditConditionHides, EditCondition = "InputType == EInputElementType::SELECTOR"))
	FName GetterFunctionForNameList = FName(); // Must return a String Array (option list)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditConditionHides, EditCondition = "InputType == EInputElementType::SELECTOR"))
	FName SetterForNewIndex = FName(); // Must receive an Integer (and index)
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditConditionHides, EditCondition = "InputType == EInputElementType::SLIDER"))
	FName SetterForNewValue = FName(); // Must receive a Float between 0.1 and 1.0
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditConditionHides, EditCondition = "InputType == EInputElementType::TOGGLABLE"))
	FName SetterForActivation = FName(); // Must receive a Boolean
};

USTRUCT(BlueprintType)
struct FSectionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputConfig> InputConfigs;
};
