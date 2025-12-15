// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlgebraLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UAlgebraLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static float GetXYDegreeAngleFromVectorToVector(const FVector& FromVector, const FVector& ToVector);
};
