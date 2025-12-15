// 
// TubeSpawnType.h
// 
// Lists what types of spawns are available for the TubeElement Class to use. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

UENUM(BlueprintType)
enum class ESpawnType : uint8 {
	SMOKE   UMETA(DisplayName = "Smoke"),
	PUDDLE  UMETA(DisplayName = "Puddle"),
	NONE    UMETA(DisplayName = "No Spawn")
};
