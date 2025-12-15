// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOptimalLight.h"
#include "OptimalPointLight.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AOptimalPointLight : public ABaseOptimalLight
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light Controls")
	class UPointLightComponent* PointLightComponent;

public:
	virtual ULightComponent* GetLightComponent() override;
	
	AOptimalPointLight();
};
