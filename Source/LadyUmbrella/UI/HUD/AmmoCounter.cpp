// 
// AmmoCounter.cpp
// 
// Implementation of the AmmoCounter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "AmmoCounter.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Overlay.h"
#include "LadyUmbrella/UI/Basics/Icon/PlatformDependantWidget.h"
#include "Components/ScaleBox.h"
#include "Components/WidgetSwitcher.h"

void UAmmoCounter::NativeConstruct()
{
	Super::NativeConstruct();

	TotalAmmoOriginalColor = TotalAmmo->GetColorAndOpacity();
	UnSelectedHookScale = GetHookOverlay()->GetRenderTransform().Scale;
	UnSelectedDartScale = GetDartOverlay()->GetRenderTransform().Scale;
	OriginalHookStyle = GetHook()->GetWidgetStyle();
	OriginalDartStyle = GetElectricDart()->GetWidgetStyle();
	OriginalBulletSlot = Cast<UHorizontalBoxSlot>(GetBullet()->Slot);

	ImageDriver->SetVisibility(ESlateVisibility::Hidden);
	GetHookOverlay()->SetVisibility(ESlateVisibility::Hidden);
	GetSwapGadgetKeysContainer()->SetVisibility(ESlateVisibility::Hidden);
	GetDartOverlay()->SetVisibility(ESlateVisibility::Hidden);

	if (UMaterialInterface* Material = Cast<UMaterialInterface>(UmbrellaHealth->GetBrush().GetResourceObject()))
	{
		CustomShieldFilling = UMaterialInstanceDynamic::Create(Material, this);
		UmbrellaHealth->SetBrushFromMaterial(CustomShieldFilling);
	}
}

void UAmmoCounter::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsAnimationPlaying(LowOnBullets))
	{
		const FSlateColor CurrentColor = ImageDriver->GetColorAndOpacity();
		for (UImage* BulletInArray : BulletsArray)
		{
			BulletInArray->SetColorAndOpacity(FLinearColor(
				BulletInArray->GetColorAndOpacity().R,
				CurrentColor.GetSpecifiedColor().G,
				CurrentColor.GetSpecifiedColor().B,
				BulletInArray->GetColorAndOpacity().A)
				);
		}
		TotalAmmo->SetColorAndOpacity(FLinearColor(
				TotalAmmo->GetColorAndOpacity().GetSpecifiedColor().R,
				CurrentColor.GetSpecifiedColor().G,
				CurrentColor.GetSpecifiedColor().B,
				TotalAmmo->GetColorAndOpacity().GetSpecifiedColor().A)
				);
	}
}

void UAmmoCounter::Show()
{
	FGlobalPointers::Umbrella->OnCurrentBulletsChanged.BindUObject(this, &UAmmoCounter::UpdateCurrentAmmo);
	FGlobalPointers::Umbrella->OnTotalBulletsChanged.BindUObject(this, &UAmmoCounter::UpdateTotalAmmo);
	FGlobalPointers::Umbrella->OnGrappleCooldown.BindUObject(this, &UAmmoCounter::UpdateGrappleCooldown);
	FGlobalPointers::Umbrella->OnElectricDartCooldown.BindUObject(this, &UAmmoCounter::UpdateElectricDartCooldown);
	FGlobalPointers::Umbrella->OnGadgetSwap.BindUObject(this, &UAmmoCounter::UpdateSelectedGadget);
	FGlobalPointers::Umbrella->OnUmbrellaHealthChanged.BindUObject(this, &UAmmoCounter::UpdateUmbrellaHealth);
	FGlobalPointers::Umbrella->OnUnlockingGadget.BindUObject(this, &UAmmoCounter::UpdateGadgetUnlocking);
	FGlobalPointers::Umbrella->OnUmbrellaAmmoCurrentMax.BindUObject(this, &UAmmoCounter::UpdateBulletsArray);
	FGlobalPointers::Umbrella->OnUmbrellaLoadAmmo.BindUObject(this, &UAmmoCounter::UpdateAmmoOnLoad);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UAmmoCounter::Hide()
{
	FGlobalPointers::Umbrella->OnGrappleCooldown.Unbind();
	FGlobalPointers::Umbrella->OnElectricDartCooldown.Unbind();
	FGlobalPointers::Umbrella->OnGadgetSwap.Unbind();
	FGlobalPointers::Umbrella->OnUmbrellaHealthChanged.Unbind();
	FGlobalPointers::Umbrella->OnUnlockingGadget.Unbind();
	FGlobalPointers::Umbrella->OnUmbrellaAmmoCurrentMax.Unbind();
	SetVisibility(ESlateVisibility::Hidden);
}

void UAmmoCounter::UpdateAmmoOnLoad(const int32 AmmoCurrent, const int32 AmmoCurrentMax, const int32 AmmoReserve)
{
	for (int32 Index = 0; Index < BulletsArray.Num(); ++Index)
	{
		if (UImage* BulletIndex = BulletsArray[Index])
		{
			FLinearColor Color = BulletIndex->GetColorAndOpacity();
			
			if (Index < AmmoCurrent)
			{
				Color.A = 1.0f;
				Color.R = 1.0f;
				Color.G = 1.0f;
				Color.B = 1.0f;
			}
			else
			{
				Color.A = 0.2f; 
				Color.R = 1.0f;
				Color.G = 1.0f;
				Color.B = 1.0f;
			}

			BulletIndex->SetColorAndOpacity(Color);
		}
	}
	
	if (AmmoCurrent <= 0 && AmmoReserve <= 0)
	{
		Umbrella->SetColorAndOpacity(FLinearColor::Red);

		for (UImage* BulletIndex : BulletsArray)
		{
			if (BulletIndex)
			{
				BulletIndex->SetColorAndOpacity(FLinearColor::Red);
			}
		}

		TotalAmmo->SetColorAndOpacity(FLinearColor::Red);
		
		if (IsAnimationPlaying(LowOnBullets))
		{
			StopAnimation(LowOnBullets);
		}

		return;
	}
	
	if (AmmoCurrent <= FMath::CeilToInt(AmmoCurrentMax * 0.25f))
	{
		if (!IsAnimationPlaying(LowOnBullets))
		{
			PlayAnimation(LowOnBullets, 0.0f, 0, EUMGSequencePlayMode::PingPong);
		}
	}
	else if (IsAnimationPlaying(LowOnBullets))
	{
		StopAnimation(LowOnBullets);
	}
	
	Umbrella->SetColorAndOpacity(FLinearColor::White);
	TotalAmmo->SetColorAndOpacity(FLinearColor::White);
}

void UAmmoCounter::UpdateCurrentAmmo(const bool bIsReloading, const int32 AmmoCurrent, const int32 AmmoCurrentMax , const int32 AmmoReserve)
{
	int32 BulletIndex = AmmoCurrent;
	if (bIsReloading)
	{
		--BulletIndex;
	}
	if (BulletsArray.IsValidIndex(BulletIndex))
	{
		UImage* CurrentBullet = BulletsArray[BulletIndex];
		FLinearColor Color = CurrentBullet->GetColorAndOpacity();
		Color.A = bIsReloading ? 1.0f : 0.4f;
		CurrentBullet->SetColorAndOpacity(Color);
	}
	if (AmmoCurrent <= FMath::CeilToInt(AmmoCurrentMax * 0.25f))
	{
		if (!IsAnimationPlaying(LowOnBullets))
		{
			PlayAnimation(LowOnBullets, 0.0f, 0, EUMGSequencePlayMode::PingPong);
		}
	}
	else if (IsAnimationPlaying(LowOnBullets))
	{
		StopAnimation(LowOnBullets);
		for (UImage* BulletCopy : BulletsArray)
		{
			FLinearColor Color = BulletCopy->GetColorAndOpacity();
			Color.G = (AmmoReserve > 0) ? 1.0f : 0.0f;
			Color.B = (AmmoReserve > 0) ? 1.0f : 0.0f;
			BulletCopy->SetColorAndOpacity(Color);
			TotalAmmo->SetColorAndOpacity(FLinearColor::White);
		}
	}
	if (AmmoCurrent <= 0 && AmmoReserve <= 0)
	{
		Umbrella->SetColorAndOpacity(FLinearColor::Red);
		if (IsAnimationPlaying(LowOnBullets))
		{
			StopAnimation(LowOnBullets);
		}
		for (UImage* BulletCopy : BulletsArray)
		{
			FLinearColor Color = BulletCopy->GetColorAndOpacity();
			Color.R = 1.0f;
			Color.G = 0.0f;
			Color.B = 0.0f;
			BulletCopy->SetColorAndOpacity(Color);
		}
		
		TotalAmmo->SetColorAndOpacity(FLinearColor::Red);
	}
	else
	{
		Umbrella->SetColorAndOpacity(FLinearColor::White);
	}
}

void UAmmoCounter::UpdateTotalAmmo(const int32 NewTotalAmmo) const
{
	FNumberFormattingOptions Options;

	if (NewTotalAmmo < MIN_NUMBER_BEFORE_DISPLAY_TWO_DIGITS)
	{
		Options.MinimumIntegralDigits = 1; 
	}
	else
	{
		Options.MinimumIntegralDigits = 2; 
	}
	
	TotalAmmo->SetText(FText::AsNumber(NewTotalAmmo, &Options));
}

void UAmmoCounter::UpdateGrappleCooldown(const bool bCanShootGrapple, const FTimerHandle Timer, const float MaxDuration) const
{
	const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(Timer);
	UpdateGadget(GetHookBorder(),GetHook(), bCanShootGrapple, ElapsedTime, MaxDuration);
}

void UAmmoCounter::UpdateElectricDartCooldown(const bool bCanShootElectric, const FTimerHandle Timer, const float MaxDuration) const
{
	const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(Timer);
	UpdateGadget(GetDartBorder(),GetElectricDart(), bCanShootElectric, ElapsedTime, MaxDuration);
}

void UAmmoCounter::UpdateSelectedGadget(const EGadgetType GadgetType) const
{
	GetHookOverlay()->SetRenderScale(UnSelectedHookScale);
	GetDartOverlay()->SetRenderScale(UnSelectedDartScale);
	GetHook()->SetWidgetStyle(OriginalHookStyle);
	GetElectricDart()->SetWidgetStyle(OriginalDartStyle);
	FProgressBarStyle Style;
	
	if (GadgetType == EGadgetType::Grapple)
	{
		GetHookOverlay()->SetRenderScale(FVector2D(SelectedGadgetX, SelectedGadgetY));
		if (GetSwapGadgetKeys()->GetPlatformName() != EPlatform::PC)
		{
			ChangeKeyToMatchGadget(ESlateVisibility::Visible, ESlateVisibility::Hidden);
		}
		if (GetHookBorder()->GetPercent() == GADGET_READY)
		{
			Style = GetHook()->GetWidgetStyle();
			Style.FillImage.TintColor = FSlateColor(SelectedHookColor);
			GetHook()->SetWidgetStyle(Style);
		}
	}
	
	if (GadgetType == EGadgetType::Electric)
	{
		GetDartOverlay()->SetRenderScale(FVector2D(SelectedGadgetX, SelectedGadgetY));
		if (GetSwapGadgetKeys()->GetPlatformName() != EPlatform::PC)
		{
			ChangeKeyToMatchGadget(ESlateVisibility::Hidden, ESlateVisibility::Visible);
		}
		if (GetDartBorder()->GetPercent() == GADGET_READY)
		{
			Style = GetElectricDart()->GetWidgetStyle();
			Style.FillImage.TintColor = FSlateColor(SelectedDartColor);
			GetElectricDart()->SetWidgetStyle(Style);
		}
	}
}

void UAmmoCounter::UpdateGadget(UProgressBar* GadgetBorder, UProgressBar* Gadget, const bool bCanShootGadget, const float Value, const float MaxValue) const
{
	float Percent = GADGET_READY;
	if (!bCanShootGadget)
	{
		Percent = Value / MaxValue;
	}
	GadgetBorder->SetPercent(Percent);
	Gadget->SetPercent(Percent);
}

void UAmmoCounter::UpdateUmbrellaHealth(const int32 Health, const int32 MaxHealth) const
{
	GetShieldOverlay()->SetVisibility(ESlateVisibility::Hidden);
	const float Value = static_cast<float>(Health) / static_cast<float>(MaxHealth);
	if (Value < FULL_HEALTH)
	{
		GetShieldOverlay()->SetVisibility(ESlateVisibility::Visible);
	}
	CustomShieldFilling->SetScalarParameterValue(FName("Percent"), Value);
}

void UAmmoCounter::UpdateGadgetUnlocking(const EGadgetType GadgetType, const bool IsUnlocked) const
{
	if (GadgetType == EGadgetType::Grapple && IsUnlocked)
	{
		GetHookOverlay()->SetVisibility(ESlateVisibility::Visible);
		if (GetDartOverlay()->IsVisible())
		{
			GetSwapGadgetKeysContainer()->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (GadgetType == EGadgetType::Electric && IsUnlocked)
	{
		GetDartOverlay()->SetVisibility(ESlateVisibility::Visible);
		if (GetHookOverlay()->IsVisible())
		{
			GetSwapGadgetKeysContainer()->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UAmmoCounter::UpdateBulletsArray(const int32 AmmoCurrentMax)
{
	if (BulletsArray.Num() < AmmoCurrentMax)
	{
		if (!BulletsArray.IsEmpty())
		{
			CurrentBulletsContainer->RemoveChildAt(BulletsArray.Num() - 1);
			BulletsArray.Pop();
		}
		for (int32 i = BulletsArray.Num(); i < AmmoCurrentMax - 1; ++i)
		{
			UImage* BulletCopy = DuplicateObject<UImage>(GetBullet(), this);
			UHorizontalBoxSlot* BulletCopySlot = CurrentBulletsContainer->AddChildToHorizontalBox(BulletCopy);
			CopySlotValues(BulletCopySlot, OriginalBulletSlot);
			BulletsArray.Add(BulletCopy);
		}
		CurrentBulletsContainer->InsertChildAt(AmmoCurrentMax - 1, GetBullet());
		UHorizontalBoxSlot* BulletSlot = Cast<UHorizontalBoxSlot>(GetBullet()->Slot);
		CopySlotValues(BulletSlot, OriginalBulletSlot);
		BulletsArray.Add(Bullet);
	}
}

void UAmmoCounter::CopySlotValues(UHorizontalBoxSlot* Target, const UHorizontalBoxSlot* Source)
{
	Target->SetHorizontalAlignment(Source->GetHorizontalAlignment());
	Target->SetVerticalAlignment(Source->GetVerticalAlignment());
	Target->SetPadding(Source->GetPadding());
	Target->SetSize(Source->GetSize());
}

void UAmmoCounter::ChangeKeyToMatchGadget(const ESlateVisibility EHook, const ESlateVisibility EDart) const
{
	if (IsValid(GetSwapGadgetKeys()->GetDependantSwitch()))
	{
		UWidget* ActiveKey = GetSwapGadgetKeys()->GetDependantSwitch()->GetActiveWidget();
		if (const UOverlay* KeyOverlay = Cast<UOverlay>(ActiveKey))
		{
			if (KeyOverlay->GetChildrenCount() >= 2)
			{
				KeyOverlay->GetChildAt(0)->SetVisibility(EHook);
				KeyOverlay->GetChildAt(1)->SetVisibility(EDart);
			}
		}
	}
}