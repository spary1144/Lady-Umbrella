
#include "LoadingScreen.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void ULoadingScreen::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(0);
}

void ULoadingScreen::Hide()
{
	StopAllAnimations();
	UnbindAllFromAnimationFinished(Fade);
	FScriptDelegate Delegate;
	PlayAnimation(Fade, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Reverse, FadeSpeed);
	Delegate.BindUFunction(this, FName("Remove"));
	BindToAnimationFinished(Fade,FWidgetAnimationDynamicEvent(Delegate));	
}

void ULoadingScreen::Show()
{
	FScriptDelegate Delegate;
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(Fade, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Forward, FadeSpeed);
	Loading();
	Delegate.BindUFunction(this, FName("OpenLevel"));
	BindToAnimationFinished(Fade,FWidgetAnimationDynamicEvent(Delegate));	
}

void ULoadingScreen::OpenLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), *FPaths::GetBaseFilename(GetMapPath()));
}

void ULoadingScreen::Remove()
{
	UnbindAllFromAnimationFinished(LoadingRotAlt);
	UnbindAllFromAnimationFinished(Fade);
	SetVisibility(ESlateVisibility::Hidden);
	SetRenderOpacity(0);
}

void ULoadingScreen::Loading()
{
	if (bUseAltAnimation)
	{
		PlayAnimation(LoadingRotAlt, FWidgetAssets::START_AT_BEGINNING, 20, EUMGSequencePlayMode::Forward, RotationSpeed);
		return;
	}
	PlayAnimation(RotTest, FWidgetAssets::START_AT_BEGINNING, 0, EUMGSequencePlayMode::PingPong, RotationSpeed);

}
