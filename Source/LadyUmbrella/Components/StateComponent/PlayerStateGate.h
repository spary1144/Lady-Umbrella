// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateGateStruct.h"
#include "PlayerStateGate.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LADYUMBRELLA_API UPlayerStateGate : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStates", meta = (AllowPrivateAccess = "true"))
	EPlayerState OwnerPlayerState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStates", meta = (AllowPrivateAccess = "true"))
	FPlayerStateGateStruct PlayerStateGate;
};
