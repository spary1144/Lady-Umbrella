// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathScreenComponent.h"

#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

// Sets default values for this component's properties
UDeathScreenComponent::UDeathScreenComponent()
:	RespawnTimerDuration				{ 2.f },
	DeathScreenTimerDuration			{ 1.5f },
	FadeInAfterDeathScreenDuration		{ 0.5f },
	DeathScreenState					{ EDeathScreenState::Deactivated }
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDeathScreenComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DeathScreenWidgetClass))
	{
		DeathScreenWidget = CreateWidget<UDeathScreen>(GetWorld(), DeathScreenWidgetClass);

		if (IsValid(DeathScreenWidget))
		{
			DeathScreenWidget->AddToViewport();
			DeathScreenWidget->SetDeathScreenImageTransparency(0.0f);
		}
	}
}

void UDeathScreenComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(DeathScreenWidget))
	{
		DeathScreenWidget->RemoveFromParent();
		DeathScreenWidget = nullptr;
	}
}

// Called every frame
void UDeathScreenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	switch (DeathScreenState)
	{
	case EDeathScreenState::Deactivated:
		break;
	case EDeathScreenState::FadeOutBeforeDeathScreen:
		// Setting the Transparency of the Death Screen Widget.
		if (World->GetTimerManager().IsTimerActive(RespawnTimer) && IsValid(DeathScreenWidget))
		{
			const float PercentageFadeOutProgress = World->GetTimerManager().GetTimerElapsed(RespawnTimer) / RespawnTimerDuration;
			DeathScreenWidget->SetDeathScreenImageTransparency(FMath::Clamp(PercentageFadeOutProgress, 0.0f, 1.0f));
		}
		break;
	case EDeathScreenState::WaitingForDeathScreen:
		break;
	case EDeathScreenState::FadeInAfterDeathScreen:
		// Setting the Transparency of the Death Screen Widget.
		if (World->GetTimerManager().IsTimerActive(FadeInAfterDeathScreenTimer) && IsValid(DeathScreenWidget))
		{
			const float PercentageFadeInProgress = 1.0f - World->GetTimerManager().GetTimerElapsed(FadeInAfterDeathScreenTimer) / FadeInAfterDeathScreenDuration;
			DeathScreenWidget->SetDeathScreenImageTransparency(FMath::Clamp(PercentageFadeInProgress, 0.0f, 1.0f));
		}
		break;
	}
}

void UDeathScreenComponent::TriggerDeathScreen()
{
	// Invalidating any leftover timers.
	RespawnTimer.Invalidate();
	DeathScreenTimer.Invalidate();
	FadeInAfterDeathScreenTimer.Invalidate();

	// Setting the Respawn Timer.
	FTimerManager& WorldTimerManager = GetWorld()->GetTimerManager();
	WorldTimerManager.SetTimer
	(
		RespawnTimer,
			this,
			&UDeathScreenComponent::OnRespawnTimerFinished,
			RespawnTimerDuration,
			false
		);

	DeathScreenState = EDeathScreenState::FadeOutBeforeDeathScreen;
}

void UDeathScreenComponent::OnRespawnTimerFinished()
{
	// Invalidating the previous timer.
	RespawnTimer.Invalidate();

	// Setting the Death Screen Timer.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer
		(
			DeathScreenTimer,
			this,
			&UDeathScreenComponent::OnDeathScreenTimerFinished,
			DeathScreenTimerDuration,
			false
		);
	}

	DeathScreenState = EDeathScreenState::WaitingForDeathScreen;

	// Setting the Transparency of the Death Screen Widget to 1.
	if (IsValid(DeathScreenWidget))
	{
		DeathScreenWidget->SetDeathScreenImageTransparency(1.0f);
	}

	// Trying to respawn our parent Player Character.
	FGlobalPointers::PlayerCharacter->Respawn();
}

void UDeathScreenComponent::OnDeathScreenTimerFinished()
{
	// Invalidating the previous timer.
	DeathScreenTimer.Invalidate();

	// Setting the Death Screen Timer.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer
		(
			FadeInAfterDeathScreenTimer,
			this,
			&UDeathScreenComponent::OnFadeInAfterDeathScreenTimerFinished,
			FadeInAfterDeathScreenDuration,
			false
		);
	}

	DeathScreenState = EDeathScreenState::FadeInAfterDeathScreen;
}

void UDeathScreenComponent::OnFadeInAfterDeathScreenTimerFinished()
{
	// Invalidating the previous timer.
	FadeInAfterDeathScreenTimer.Invalidate();

	DeathScreenState = EDeathScreenState::Deactivated;

	// Setting the Transparency of the Death Screen Widget to 0.
	if (IsValid(DeathScreenWidget))
	{
		DeathScreenWidget->SetDeathScreenImageTransparency(0.0f);
	}

	// Trying to reset the input of our parent Player Character.
	FGlobalPointers::PlayerCharacter->ResetInputAfterRespawnFadeFinishes();
}