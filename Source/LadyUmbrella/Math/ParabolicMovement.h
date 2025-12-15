// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class LADYUMBRELLA_API ParabolicMovement
{
public:	
	static FVector CalculateParabolicLaunchVelocity(const FVector& Start, const FVector& End, const float Time, const float GravityZ);
	static void DrawDebugParabola(UWorld* World, const FVector& Start, const FVector& InitialVelocity, const float GravityZ, const float TotalTime);
};
