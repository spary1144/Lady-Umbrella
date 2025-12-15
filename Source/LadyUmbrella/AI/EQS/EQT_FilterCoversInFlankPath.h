// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQT_FilterCoversInFlankPath.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQT_FilterCoversInFlankPath : public UEnvQueryTest
{
	GENERATED_BODY()

	UEQT_FilterCoversInFlankPath();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
