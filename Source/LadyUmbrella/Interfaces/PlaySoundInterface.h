// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "PlaySoundInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlaySoundInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LADYUMBRELLA_API IPlaySoundInterface
{
	GENERATED_BODY()
	
public:
	virtual void PlayStepsSounds(const uint8 StepSurface) = 0;
	virtual void PlayUmbrellaGadgetSound(const FString& EventToPlay) = 0;
	virtual void PlayNotifiedActionSound(const FString& EventToPlay) = 0;
	virtual void PlayNotifiedActionSoundWithParameter(const FString& EventToPlay, const float NewParameter) = 0;
	virtual void PlayNotifiedVoiceSound(const FString& EventToPlay) = 0;
};
