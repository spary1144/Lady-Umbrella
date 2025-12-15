#include "ToggleInput.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void UToggleInput::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpdateOptions();
}

void UToggleInput::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ToggleButton->OnClicked.AddDynamic(this, &UToggleInput::UpdateOptions);
}

void UToggleInput::UpdateOptions()
{
	EUMGSequencePlayMode::Type PlaybackDir = !GetStatus() ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse;
	PlayAnimation(ToggleOptionAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, PlaybackDir, AnimationSpeedMultiplier);
	SetStatus(!GetStatus());
	OnToggleUpdated.Broadcast(GetStatus());
	
	if (IsValid(SelectorSound))
	{
		UGameplayStatics::SpawnSound2D(this, SelectorSound);
	}
}
