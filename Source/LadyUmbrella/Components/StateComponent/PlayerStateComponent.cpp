// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateComponent.h"

// Sets default values for this component's properties
UPlayerStateComponent::UPlayerStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	PlayerState = EPlayerState::FreeMovement;
}

void UPlayerStateComponent::OnRegister()
{
	Super::OnRegister();

	for (TSubclassOf<UPlayerStateGate> PlayerStateClass : PlayerStateGateClasses)
	{
		if (IsValid(PlayerStateClass))
		{
			// TSharedPtr<UPlayerStateGate> StateGate = TSharedPtr<UPlayerStateGate>(NewObject<UPlayerStateGate>(PlayerStateClass));

			if (PlayerStateGates.Contains(PlayerStateClass.GetDefaultObject()->OwnerPlayerState))
			{
				PlayerStateGates[PlayerStateClass.GetDefaultObject()->OwnerPlayerState] = PlayerStateClass.GetDefaultObject()->PlayerStateGate;
			}
			else
			{
				PlayerStateGates.Add(PlayerStateClass.GetDefaultObject()->OwnerPlayerState, PlayerStateClass.GetDefaultObject()->PlayerStateGate);
			}
		}
	}
}

#pragma region STATE CONTROL FUNCTIONS

bool UPlayerStateComponent::CanChangeToPlayerState(const EPlayerState NewPlayerState) const
{
	if (PlayerState == NewPlayerState)
	{
		return false;
	}
	
	if (PlayerStateGates.Contains(PlayerState) && PlayerStateGates[PlayerState].AllowedTransitionsMap.Contains(NewPlayerState))
	{
		return PlayerStateGates[PlayerState].AllowedTransitionsMap[NewPlayerState];
	}

	return false;
}

bool UPlayerStateComponent::SetPlayerState(const EPlayerState NewPlayerState)
{
	if (CanChangeToPlayerState(NewPlayerState))
	{
		PlayerState = NewPlayerState;
		return true;
	}

	return false;
}

bool UPlayerStateComponent::CanPlayerMove() const
{
	return GetPlayerState() == EPlayerState::FreeMovement
		|| GetPlayerState() == EPlayerState::LimitedMovement
		|| GetPlayerState() == EPlayerState::Falling
		|| GetPlayerState() == EPlayerState::AimingUmbrella
		|| GetPlayerState() == EPlayerState::InCover
		|| GetPlayerState() == EPlayerState::OpenShield
		|| GetPlayerState() == EPlayerState::InComms;
}

bool UPlayerStateComponent::CanPlayerReload() const
{
	return GetPlayerState() == EPlayerState::FreeMovement
		|| GetPlayerState() == EPlayerState::LimitedMovement
		|| GetPlayerState() == EPlayerState::AimingUmbrella
		|| GetPlayerState() == EPlayerState::InCover
		|| GetPlayerState() == EPlayerState::InCoverAiming
		|| GetPlayerState() == EPlayerState::OpenShield;
}

bool UPlayerStateComponent::CanInteractWithOverlappingElements() const
{
	return GetPlayerState() == EPlayerState::FreeMovement || GetPlayerState() == EPlayerState::InCover;
}

bool UPlayerStateComponent::CanShootUmbrella() const
{
	return GetPlayerState() == EPlayerState::FreeMovement
		|| GetPlayerState() == EPlayerState::AimingUmbrella
		|| GetPlayerState() == EPlayerState::InCover
		|| GetPlayerState() == EPlayerState::InCoverAiming;
}

#pragma endregion 

#pragma region STATE CHECKS

	

#pragma endregion
