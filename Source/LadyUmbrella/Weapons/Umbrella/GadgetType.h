// 
// EGadgetType.h
// 
// Lists what types of gadgets are available for the umbrella to use. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

UENUM(BlueprintType)
enum class EGadgetType : uint8
{
	Grapple		UMETA(DisplayName = "Grapple"),
	Electric	UMETA(DisplayName = "Electric"),
	None		UMETA(DisplayName = "None")
};
