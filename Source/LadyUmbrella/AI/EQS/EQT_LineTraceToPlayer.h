// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQT_LineTraceToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQT_LineTraceToPlayer : public UEnvQueryTest
{
	GENERATED_BODY()
	
	UEQT_LineTraceToPlayer();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
