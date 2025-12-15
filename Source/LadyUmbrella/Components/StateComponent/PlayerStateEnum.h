// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStateEnum.generated.h"

/**
 * Used in Player Character to determine the state of the player.
 */
UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	FreeMovement UMETA(DisplayName = "FreeMovement"),
	Falling UMETA(DisplayName = "Falling"),
	LimitedMovement UMETA(DisplayName = "LimitedMovement"),
	AimingUmbrella UMETA(DisplayName = "AimingUmbrella"),
	OpenShield UMETA(DisplayName = "OpenShield"),
	InCover UMETA(DisplayName = "InCover"),
	InCoverAiming UMETA(DisplayName = "InCoverAiming"),
	InTraversalNoControls UMETA(DisplayName = "InTraversalNoControls"),
    InMeleeAnimation UMETA(DisplayName = "InMeleeAnimation"),
    ControlsDisabled UMETA(DisplayName = "ControlsDisabled"),
	InComms UMETA(DisplayName = "InComms") //Can move with joystick and that's it
};

static FString ToString(const EPlayerState PlayerState)
{
	switch (PlayerState)
	{
	case EPlayerState::FreeMovement:
		return FString("FreeMovement");
	case EPlayerState::Falling:
		return FString("Falling");
	case EPlayerState::LimitedMovement:
		return FString("LimitedMovement");
	case EPlayerState::AimingUmbrella:
		return FString("AimingUmbrella");
	case EPlayerState::OpenShield:
		return FString("OpenShield");
	case EPlayerState::InCover:
		return FString("InCover");
	case EPlayerState::InCoverAiming:
		return FString("InCoverAiming");
	case EPlayerState::InTraversalNoControls:
		return FString("InTraversalNoControls");
	case EPlayerState::InMeleeAnimation:
		return FString("InMeleeAnimation");
	case EPlayerState::ControlsDisabled:
		return FString("ControlsDisabled");
	case EPlayerState::InComms:
        return FString("InComms");
	}

	return FString("UNKNOWN STATE");
}
