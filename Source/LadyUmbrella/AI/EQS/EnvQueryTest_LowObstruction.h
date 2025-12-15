// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_LowObstruction.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEnvQueryTest_LowObstruction : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_LowObstruction();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;	
};
