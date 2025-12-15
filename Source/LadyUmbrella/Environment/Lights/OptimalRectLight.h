// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOptimalLight.h"
#include "OptimalRectLight.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AOptimalRectLight : public ABaseOptimalLight
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light Controls")
	class URectLightComponent* RectLightComponent;

public:
	virtual ULightComponent* GetLightComponent() override;
	
	AOptimalRectLight();
};
