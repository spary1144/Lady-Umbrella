// 
// StatusEffect.h
// 
// Defines the 'status effects' that can impact an actor.  
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
	ELECTRIFIED UMETA(DisplayName = "Electrified"),
	NORMAL  UMETA(DisplayName = "Normal"),
	SMOKED  UMETA(DisplayName = "Smoked"),
	COMMS  UMETA(DisplayName = "Comms"),
	OTHER UMETA(DisplayName = "Other")
};
