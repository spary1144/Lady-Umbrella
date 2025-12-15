// 
// EQG_GenerateCovers.h
// 
// Enviroment query Generator -> generate items of class CoverPoint and could filter by tag
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ActorsOfClass.h"
#include "EQG_GenerateCovers.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQG_GenerateCovers : public UEnvQueryGenerator_ActorsOfClass
{
	GENERATED_BODY()
	
	void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FName TagFilter;
};
