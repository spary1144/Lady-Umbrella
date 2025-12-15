// 
// AnimatedButton.cpp
// 
// Implementation of the AnimatedButton class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"

#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/InputSettings.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "Widgets/SViewport.h"


void UAnimatedBaseButton::NativeOnInitialized() // this is called only once for the UUserWidget. If you have one-time things to establish up-front (like binding callbacks to events on BindWidget properties), do so here
{
	Super::NativeOnInitialized();
	
	Button->OnClicked.AddDynamic(this, &UAnimatedBaseButton::ActivateButton);
	FWidgetAssets::OnLanguageChanged.AddUObject(this, &UAnimatedBaseButton::OnLanguageChanged);
	OnLanguageChanged();
}

void UAnimatedBaseButton::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = true;
		
		FGeometry Geometry = Button->GetCachedGeometry();
		FVector2D LocalSize = Geometry.GetLocalSize();
		FVector2D ScreenPosition = Geometry.LocalToAbsolute(LocalSize * 0.5f);
		
		UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
		if (!ViewportClient) return;

		TSharedPtr<SViewport> SlateViewport = ViewportClient->GetGameViewportWidget();
		if (!SlateViewport.IsValid()) return;

		const FGeometry& ViewportGeo = SlateViewport->GetCachedGeometry();
		FVector2D ViewportLocal = ViewportGeo.AbsoluteToLocal(ScreenPosition);
		
		PC->SetMouseLocation(StaticCast<int32>(ViewportLocal.X), StaticCast<int32>(ViewportLocal.Y));
		
		Button->OnHovered.Broadcast();
		PC->bShowMouseCursor = false;
	}
	
	OnFocusChanged.Broadcast(true);
	return Super::NativeOnAddedToFocusPath(InFocusEvent);
}

void UAnimatedBaseButton::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	Button->OnUnhovered.Broadcast();
	OnFocusChanged.Broadcast(false);
}

void UAnimatedBaseButton::OnLanguageChanged()
{
	int32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;
	
	if (ButtonIdentifier != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(ButtonIdentifier);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		ButtonName->SetText(NewText);
	}
}

void UAnimatedBaseButton::ActivateButton()
{
	OnButtonActivated.Broadcast();
}

/*FReply UAnimatedBaseButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	if (State != EUIState::Hovered)
	{
		TSharedPtr<SWidget> SafeWidget = Button->GetCachedWidget();
		if (SafeWidget.IsValid())
		{
			SafeWidget->OnMouseEnter(InGeometry, FPointerEvent());
		}
	}

	return FReply::Handled();
}*/

/*void UAnimatedBaseButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	
	if (!IsFocusable())
	{
		return;
	}
	
	if (State != EUIState::Idle)
	{
		TSharedPtr<SWidget> SafeWidget = Button->GetCachedWidget();
		if (SafeWidget.IsValid())
		{
			SafeWidget->OnMouseLeave(FPointerEvent());
		}
	}
}*/