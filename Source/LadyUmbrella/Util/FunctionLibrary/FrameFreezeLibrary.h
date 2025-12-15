// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrameFreezeLibrary.generated.h"

UCLASS()
class LADYUMBRELLA_API UFrameFreezeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="GameFeel")
	static void FrameFreeze(UObject* WorldContextObject, const float FreezeDilation, const float Time, const bool bHasPriority = false);
	
	UFUNCTION(BlueprintCallable, Category="GameFeel")
	static void CustomFrameFreeze(UObject* WorldContextObject, AActor* FreezedActor, const float FreezeDilation, const float Time);

	static FTimerHandle FrameFreezeTimerHandle;
	static FTimerHandle CustomFrameFreezeTimerHandle;
};
