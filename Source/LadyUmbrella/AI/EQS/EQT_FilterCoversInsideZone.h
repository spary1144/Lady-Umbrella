// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQT_FilterCoversInsideZone.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEQT_FilterCoversInsideZone : public UEnvQueryTest
{
	GENERATED_BODY()

	UEQT_FilterCoversInsideZone();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

public:

	//InsideZone = true -> test filter all covers inside this zone
	//InsideZone = false -> test filter all covers outside this zone
	UPROPERTY(EditAnywhere)
	bool insideZone;
	//With insideZone = false, then only filter smaller zones than current
	UPROPERTY(EditAnywhere)
	bool ZonesSmallerthanCurrentZone; 
};
