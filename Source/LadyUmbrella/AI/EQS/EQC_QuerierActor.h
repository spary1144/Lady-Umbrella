// 
// EQC_QuerierActor.h
// 
// Enviroment query context -> get AI pawn as context in EQS ( when eqspawntest is active)
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQC_QuerierActor.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQC_QuerierActor : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName BlackboardKeyName = "SelectedCover";
};
