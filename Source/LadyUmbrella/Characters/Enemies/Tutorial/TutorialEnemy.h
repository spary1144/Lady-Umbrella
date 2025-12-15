// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../EnemyCharacter.h"
#include "TutorialEnemy.generated.h"


UCLASS()
class LADYUMBRELLA_API ATutorialEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess = true))
	float DamageMultiplier;

	UFUNCTION()
	float CalculateWeaponDamage() const;

public:
	ATutorialEnemy();
	
	virtual bool TryToFire() override;
	virtual void BulletInteraction(const FHitResult& OutHit) const override;
};
