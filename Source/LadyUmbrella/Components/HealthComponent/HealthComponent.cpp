
#include "HealthComponent.h"

#include "LadyUmbrella/Util/AssetUtil.h"

UHealthComponent::UHealthComponent()
{
	// Set default values
	PrimaryComponentTick.bCanEverTick = true;
	CurrentHealth		 = 100;
	MaxHealth			 = 100;
	bCanRegenerate		 = false;
	RegenerationSpeed	 = .2f;
	RegenerationCooldown = 3.0f;
	CumulativeRegenTimer = 0.0f;

	RegenerationCurve = AssetUtils::FindObject<UCurveFloat>("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_HealthRegenerationFactor.C_HealthRegenerationFactor'");
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// CurrentHealth = MaxHealth;
	StartRegenHealth();	
}

#pragma region PRIVATE FUNCTIONS

void UHealthComponent::RegenerateHealth()
{
	if (GetHealth() < GetMaxHealth())
	{
		CumulativeRegenTimer += GetWorld()->GetTimerManager().GetTimerElapsed(RegenTimerHandle);

		if (IsValid(RegenerationCurve))
		{
			AddHealth(RegenerationCurve->GetFloatValue(CumulativeRegenTimer));			
		}
	}
	else
	{
		StopRegenHealth();
	}
}

void UHealthComponent::StartRegenHealth()
{
	if (bCanRegenerate)
	{
		CumulativeRegenTimer = 0.0f;
		
		GetWorld()->GetTimerManager().SetTimer
		(
			RegenTimerHandle,
			this,
			&UHealthComponent::RegenerateHealth,
			RegenerationSpeed,
			true
		);
	}
}

void UHealthComponent::StopRegenHealth()
{
	if (bCanRegenerate)
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
	}
}

void UHealthComponent::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDeath"));
}

#pragma endregion

#pragma region PUBLIC FUNCTIONS

void UHealthComponent::SetHealth(int32 NewHealth)
{
	// Clamping the NewHealth.
	NewHealth = FMath::Clamp(NewHealth, 0, MaxHealth);

	// If after the clamp we are not actually changing our life amount, we return.
	if (NewHealth == CurrentHealth)
	{
		return;
	}

	const int32 OldHealth = CurrentHealth;
	CurrentHealth = NewHealth;

	// Delegate broadcast when the health amount changes.
	
	OnHealthAmountChanged.Broadcast(GetHealth(), OldHealth > CurrentHealth);
	
	// Delegate broadcast when the owner dies.
	if (IsDead())
	{
		OnDeathDelegate.Broadcast();
	}
}

void UHealthComponent::AddHealth(int32 HealthToAdd)
{
	if (HealthToAdd < 0)
	{
		HealthToAdd = -HealthToAdd;
	}
	
	SetHealth(CurrentHealth + HealthToAdd);
}

void UHealthComponent::SubtractHealth(int32 HealthToSubtract)
{
	if (HealthToSubtract < 0)
	{
		HealthToSubtract = -HealthToSubtract;
	}
	
	SetHealth(CurrentHealth - HealthToSubtract);

	// Stopping the regeneration of health.
	StopRegenHealth();

	// And setting the timer to start regenerating again soon.
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (bCanRegenerate)
	{
		if (TimerManager.IsTimerActive(RegenDelayHandle))
		{
			TimerManager.ClearTimer(RegenDelayHandle);
		}
		TimerManager.SetTimer
		(
			RegenDelayHandle,
			this,
			&UHealthComponent::StartRegenHealth,
			RegenerationCooldown,
			false
		);
	}
}

#pragma endregion

#pragma region GETTERS AND SETTERS

void UHealthComponent::SetMaxHealth(const int32 NewMaxHealth)
{
	MaxHealth = NewMaxHealth;

	// If our MaxHealth is smaller than our current health, we need to clamp our current health.
	if (MaxHealth < CurrentHealth)
	{
		SetHealth(MaxHealth);
	}
}

#pragma endregion
