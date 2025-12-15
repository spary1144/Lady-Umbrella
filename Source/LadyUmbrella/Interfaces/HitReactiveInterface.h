// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "UObject/Interface.h"
#include "HitReactiveInterface.generated.h"

UENUM(BlueprintType)
enum class HitSource : uint8 {
	PLAYER_B    UMETA(DisplayName = "Player Bullet"),
	PLAYER_M    UMETA(DisplayName = "Player Melee"),
	PLAYER_G    UMETA(DisplayName = "Player Grapple"),
	ENEMY       UMETA(DisplayName = "Enemy Bullet"),
	ENVIROMENT  UMETA(DisplayName = "Enviroment Destruction"),
	ELECTRICITY	UMETA(DisplayName = "Electric Dart"),
	EXPLOSION	UMETA(DisplayName = "Explosion"),
	NONE        UMETA(DisplayName = "No Source")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UHitReactiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class LADYUMBRELLA_API IHitReactiveInterface
{
	GENERATED_BODY()

public:
	virtual void Reaction(HitSource Source) = 0;
};
