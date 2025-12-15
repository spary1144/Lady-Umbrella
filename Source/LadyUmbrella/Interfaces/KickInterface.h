// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KickInterface.generated.h"

UINTERFACE(MinimalAPI)
class UKickInterface : public UInterface
{
	GENERATED_BODY()
};


class LADYUMBRELLA_API IKickInterface
{
	GENERATED_BODY()

public:
	virtual void KickActor(const FVector& ForceDirection) = 0;
	virtual void PrepareKnockBackActor() = 0;
};