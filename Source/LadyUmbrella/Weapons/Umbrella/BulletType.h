// 
// BulletType.h
// 
// Lists what types of bullets are available for the umbrella to use. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

UENUM(BlueprintType)
enum class EBulletType : uint8
{
	Normal    UMETA(DisplayName = "Normal"),
	Electric  UMETA(DisplayName = "Electric")
};


// ,
// 	Exploding UMETA(DisplayName = "Exploding"),
// 	Smoke     UMETA(DisplayName = "Smoke")