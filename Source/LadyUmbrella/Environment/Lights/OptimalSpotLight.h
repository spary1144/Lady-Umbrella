// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOptimalLight.h"
#include "OptimalSpotLight.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AOptimalSpotLight : public ABaseOptimalLight
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light Controls")
	class USpotLightComponent* SpotLightComponent;

public:
	virtual ULightComponent* GetLightComponent() override;
	
	AOptimalSpotLight();
};
