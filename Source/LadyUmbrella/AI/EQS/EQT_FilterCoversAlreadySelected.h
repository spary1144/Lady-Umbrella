// 
// EQT_FilterCoversAlreadySelected.h
// 
// Enviroment query test -> filter and score covers already selected by Mafia AI
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQT_FilterCoversAlreadySelected.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQT_FilterCoversAlreadySelected : public UEnvQueryTest
{
	GENERATED_BODY()
	
	UEQT_FilterCoversAlreadySelected();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
