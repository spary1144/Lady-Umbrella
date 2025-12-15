// 
// EQC_Player.h
// 
// Enviroment query context -> get player as context in EQS
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQC_Player.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQC_Player : public UEnvQueryContext
{
	GENERATED_BODY()
	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
