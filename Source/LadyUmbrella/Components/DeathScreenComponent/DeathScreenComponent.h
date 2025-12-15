// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LadyUmbrella/UI/HUD/DeathScreen.h"
#include "DeathScreenComponent.generated.h"

UENUM(BlueprintType)
enum class EDeathScreenState : uint8
{
	Deactivated UMETA(DisplayName = "Deactivated"),
	FadeOutBeforeDeathScreen UMETA(DisplayName = "FadeOutBeforeDeathScreen"),
	WaitingForDeathScreen UMETA(DisplayName = "WaitingForDeathScreen"),
	FadeInAfterDeathScreen UMETA(DisplayName = "FadeInAfterDeathScreen"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UDeathScreenComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Death Screen")
	float RespawnTimerDuration;

	UPROPERTY(EditAnywhere, Category = "Death Screen")
	float DeathScreenTimerDuration;

	UPROPERTY(EditAnywhere, Category = "Death Screen")
	float FadeInAfterDeathScreenDuration;

	UPROPERTY(EditAnywhere, Category = "Death Screen")
	TSubclassOf<UUserWidget> DeathScreenWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Death Screen")
	EDeathScreenState DeathScreenState;

	UPROPERTY()
	FTimerHandle RespawnTimer;
	UPROPERTY()
	FTimerHandle DeathScreenTimer;
	UPROPERTY()
	FTimerHandle FadeInAfterDeathScreenTimer;

	UPROPERTY()
	TObjectPtr<UDeathScreen> DeathScreenWidget;

public:	
	UDeathScreenComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE float GetRespawnTimerDuration() const { return RespawnTimerDuration; };
	FORCEINLINE float GetFadeInDurationAfterRespawn() const { return FadeInAfterDeathScreenDuration; };
	
	UFUNCTION()
	void TriggerDeathScreen();

private:
	UFUNCTION()
	void OnRespawnTimerFinished();

	UFUNCTION()
	void OnDeathScreenTimerFinished();

	UFUNCTION()
	void OnFadeInAfterDeathScreenTimerFinished();
};
