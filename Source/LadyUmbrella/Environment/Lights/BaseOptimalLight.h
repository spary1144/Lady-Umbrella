// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseOptimalLight.generated.h"

enum class EFlickeringState : uint8
{
	WaitingForFlickering,
	EnteringFlicker,
	ExitingFlicker
};

UCLASS()
class LADYUMBRELLA_API ABaseOptimalLight : public AActor
{
	GENERATED_BODY()
	
	
	/**
	 * Makes the Light start flickering on Play.
	 */
	UPROPERTY(EditAnywhere, Category = "Light Controls|Flickering")
	bool bStartWithFlickeringEnabled;
	
	/**
	 * Determines how fast the Light changes intensity. Making it too low will make the flickering feel slow, like in slow motion. Making it too high might make it harder to notice.
	 */
	UPROPERTY(EditAnywhere, Category = "Light Controls|Flickering", meta=(UIMin = 0.f, UIMax = 15.f, ClampMin = 0.f, ClampMax = 15.f, EditConditionHides = "bFlickeringEnabled"))
	float LightIntensityChangeSpeed;
	
	/**
	 * Determines the Minimum value in the range of possible values that the Light Intensity can drop to when it flickers.
	 */
	UPROPERTY(EditAnywhere, Category = "Light Controls|Flickering", meta=(UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f, EditConditionHides = "bFlickeringEnabled"))
	float MinFlickeringIntensityPercentage;
	
	/**
	 * Determines the Maximum value in the range of possible values that the Light Intensity can drop to when it flickers.
	 */
	UPROPERTY(EditAnywhere, Category = "Light Controls|Flickering", meta=(UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f, EditConditionHides = "bFlickeringEnabled"))
	float MaxFlickeringIntensityPercentage;
	
	/**
	 * Determines the Minimum value in the range of time that the light can wait until another flicker occurs.
	 */
	UPROPERTY(EditAnywhere, Category = "Light Controls|Flickering", meta=(UIMin = 0.f, UIMax = 10.f, ClampMin = 0.f, ClampMax = 10.f, EditConditionHides = "bFlickeringEnabled"))
	float MinWaitBetweenFlickersTime;
	
	/**
	 * Determines the Maximum value in the range of time that the light can wait until another flicker occurs.
	 */
	UPROPERTY(EditAnywhere, Category = "Light Controls|Flickering", meta=(UIMin = 0.f, UIMax = 10.f, ClampMin = 0.f, ClampMax = 10.f, EditConditionHides = "bFlickeringEnabled"))
	float MaxWaitBetweenFlickersTime;

	bool bFlickeringEnabled;
	float OriginalLightIntensity;
	float CurrentWaitBetweenFlickersTime;
	float CurrentTimeWaitedBetweenFlickersTime;
	float CurrentFlickeringIntensityPercentage;
	float CurrentTargetFlickeringIntensityPercentage;
	EFlickeringState FlickeringState;

	
public:	
	// Sets default values for this actor's properties
	ABaseOptimalLight();

	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
	virtual ULightComponent* GetLightComponent() { return nullptr; };
	
	void SetLightIntensity(const float NewIntensity);

	void WaitingForFlickeringTickFunctionality(const float DeltaTime);
	void EnteringFlickerTickFunctionality(const float DeltaTime);
	void ExitingFlickerTickFunctionality(const float DeltaTime);
	
	/**
	 * Enables / Disables the Light's flicker.
	 * @param bIsEnabled
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Controls")
	void SetFlickeringEnabled(const bool bIsEnabled);

	/**
	 * Returns true if the light is flickering.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "Light Controls")
	bool IsFlickeringEnabled() const;
};
