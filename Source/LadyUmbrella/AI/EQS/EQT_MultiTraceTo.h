// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQT_MultiTraceTo.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQT_MultiTraceTo : public UEnvQueryTest
{
	GENERATED_BODY()

	UEQT_MultiTraceTo();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
