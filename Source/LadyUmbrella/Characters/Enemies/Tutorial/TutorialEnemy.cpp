// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialEnemy.h"

#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"



ATutorialEnemy::ATutorialEnemy()
	: DamageMultiplier(2.f)
{
}

bool ATutorialEnemy::TryToFire()
{
	if (!IsValid(EquippedWeapon) || !IsValid(FGlobalPointers::PlayerCharacter))
	{
		return false;
	}
	
	FHitResult OutHit;
	GetFireHitResult(OutHit, 0.f);

	if (!IsReloading())
	{
		EquippedWeapon->Fire();
		return true;
	}
	return false;
}

float ATutorialEnemy::CalculateWeaponDamage() const
{
	float WeaponDamage = EquippedWeapon->GetWeaponDamage();

	// Check if player is facing towards the enemy
	const float PlayerDotProduct = FVector::DotProduct(GetActorForwardVector(), FGlobalPointers::PlayerCharacter->GetActorForwardVector());
	if (PlayerDotProduct < 0 || FGlobalPointers::PlayerCharacter->GetVelocity().Size() <= KINDA_SMALL_NUMBER)
	{
		WeaponDamage *= DamageMultiplier;
	}
	//FLogger::DebugLog("Weapon Damage = %f", WeaponDamage);
	return WeaponDamage;
}

void ATutorialEnemy::BulletInteraction(const FHitResult& OutHit) const
{
	if (!IsValid(OutHit.GetActor()))
	{
		return;
	}
	
	if(!OutHit.GetActor()->Implements<UHealthInterface>())
	{
		return;
	}
	// New
	IHealthInterface* HealthInterface = Cast<IHealthInterface>(OutHit.GetActor());
	if (!HealthInterface)
	{
		return;
	}

	if (OutHit.GetActor()->IsA(APlayerCharacter::StaticClass()))
	{
		HealthInterface->SubtractHealth(CalculateWeaponDamage(), GetWeaponType(), this->GetActorLocation());
		return;
	}

	if (OutHit.GetActor()->IsA(AUmbrella::StaticClass()))
	{
		HealthInterface->SubtractHealth(CalculateWeaponDamage(), GetWeaponType());
		return;
	}
// Old
	/*
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(OutHit.GetActor()))
	{
		if (OutHit.GetActor()->IsA(APlayerCharacter::StaticClass()))
		{
			HealthInterface->SubtractHealth(CalculateWeaponDamage(),GetWeaponType(), this->GetActorLocation());
		}

		if (AUmbrella* UmbrellaObj = Cast<AUmbrella>(OutHit.GetActor()))
		{
			UmbrellaObj->SubtractHealth(CalculateWeaponDamage(), GetWeaponType());
		}
	}
	*/
}
