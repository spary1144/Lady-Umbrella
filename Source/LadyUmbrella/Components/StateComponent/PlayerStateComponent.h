// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateGate.h"
#include "PlayerStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UPlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region VARIABLES
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerStates", meta = (AllowPrivateAccess = "true"))
	EPlayerState PlayerState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="PlayerStates", meta = (AllowPrivateAccess = "true"), DisplayName="Player State Gates")
	TArray<TSubclassOf<UPlayerStateGate>> PlayerStateGateClasses;
	
	TMap<EPlayerState, FPlayerStateGateStruct> PlayerStateGates;

#pragma endregion 

public:	
	// Sets default values for this component's properties
	UPlayerStateComponent();

protected:
	virtual void OnRegister() override;

#pragma region STATE CONTROL FUNCTIONS
public:
	UFUNCTION()
	bool CanChangeToPlayerState(const EPlayerState NewPlayerState) const;

	UFUNCTION()
	FORCEINLINE EPlayerState GetPlayerState() const { return PlayerState; }
	
	UFUNCTION()
	bool SetPlayerState(const EPlayerState NewPlayerState);

#pragma endregion

#pragma region STATE CHECKS
public:
	UFUNCTION()
	bool CanPlayerMove() const;

	UFUNCTION()
	bool CanPlayerReload() const;

	UFUNCTION()
	bool CanInteractWithOverlappingElements() const;

	UFUNCTION()
	bool CanShootUmbrella() const;

#pragma endregion 
};
