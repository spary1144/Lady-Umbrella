// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSaveStructPlayer.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="FSaveStructPlayer"))
struct FSaveStructPlayer
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	int32 UmbrellaInventoryAmmo = 20;

	UPROPERTY(SaveGame)
	int32 UmbrellaCurrentMagAmmo = 4;

	UPROPERTY(SaveGame)
	int32 UmbrellaCurrentMaxAmmo = 4;

	UPROPERTY(SaveGame)
	int32 CurrentUpgradePieces = 0; 

	UPROPERTY(SaveGame)
	FTransform SaveFileLastPlayerTransform = FTransform();

	UPROPERTY(SaveGame)
	FVector LastForwardVector = FVector(0, 0, 0);

	UPROPERTY(SaveGame)
	FVector LastRightVector = FVector(0, 0, 0);
	
};