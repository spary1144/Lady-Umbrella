// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoverEnums.generated.h"

/**
 * Used in CoverMovementComponent to determine when and how the cover system is affecting the character.
 */
UENUM(BlueprintType)
enum class ECoveredState : uint8
{
	NoCover UMETA(DisplayName = "NoCover"),
	InCover UMETA(DisplayName = "InCover"),
	EnteringCover UMETA(DisplayName = "EnteringCover"),
	ExitingCover UMETA(DisplayName = "ExitingCover"),
};


/**
 * Used in CoverMovementComponent to determine the state of a character while inside a Cover.
 */
UENUM(BlueprintType)
enum class EInsideCoverState : uint8
{
	BaseCovered UMETA(DisplayName = "BaseCovered"),
	Aiming UMETA(DisplayName = "Aiming"),
	MovingBetweenCovers UMETA(DisplayName = "MovingBetweenCovers"),
};


/**
 * Used in CoverMovementComponent to determine how a character enters a cover.
 */
UENUM(BlueprintType)
enum class EEnteringCoverState : uint8
{
	NotEnteringCover UMETA(DisplayName = "NotEnteringCover"),
	WalkingTowardsCover UMETA(DisplayName = "WalkingTowardsCover"),
	InterpolatingToCover UMETA(DisplayName = "InterpolatingToCover"),
	InterpolatingWithSlideToCover UMETA(DisplayName = "InterpolatingWithSlideToCover"),
};


/**
 * Used in CoverMovementComponent to determine how a character is moving between covers.
 */
UENUM(BlueprintType)
enum class EMovementBetweenCoversState : uint8
{
	NoMovement UMETA(DisplayName = "NoMovement"),
	ContinuousLeft UMETA(DisplayName = "ContinuousLeft"),
	ContinuousRight UMETA(DisplayName = "ContinuousRight"),
	TurningExteriorCornerLeft UMETA(DisplayName = "TurningExteriorCornerLeft"),
	TurningExteriorCornerRight UMETA(DisplayName = "TurningExteriorCornerRight"),
};
