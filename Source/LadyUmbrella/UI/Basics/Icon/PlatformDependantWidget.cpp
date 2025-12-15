// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformDependantWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/GlobalUtil.h"

void UPlatformDependantWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (const UWorld* World = GetWorld())
	{
		if (ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(UGameplayStatics::GetGameInstance(World)))
		{
			OnPlatformUpdatedDelegateHandle = GameInstance->OnPlatformUpdated.AddUObject(this, &UPlatformDependantWidget::SetWidgetIndex);
			FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UPlatformDependantWidget::UpdateForController);
			SetWidgetIndex(GameInstance->GetPlatformName());
		}
	}
}

void UPlatformDependantWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (!IsValid(FGlobalPointers::GameInstance))
	{
		return;
	}
	
	if (OnPlatformUpdatedDelegateHandle.IsValid() && FGlobalPointers::GameInstance->OnPlatformUpdated.IsBoundToObject(this))
	{
		FGlobalPointers::GameInstance->OnPlatformUpdated.Remove(OnPlatformUpdatedDelegateHandle);
	}
}

EPlatform UPlatformDependantWidget::GetPlatformName()
{
	if (!IsValid(DependantSwitch) && DependantSwitch->GetNumWidgets() <= 0)
	{
		return EPlatform::PC;
	}
	return static_cast<EPlatform>(DependantSwitch->GetActiveWidgetIndex());
}

void UPlatformDependantWidget::SetWidgetIndex(EPlatform Platform)
{
	int32 PlatformIndex = static_cast<int32>(Platform);
	DependantSwitch->SetActiveWidgetIndex(PlatformIndex);
	//FLogger::DebugLog("Dependant Switch %s -> %d", *GetName(), PlatformIndex);
}

void UPlatformDependantWidget::UpdateForController(bool bConnected, FPlatformUserId UserId, int32 ControllerId)
{
	SetWidgetIndex(bConnected ? EPlatform::PS : EPlatform::PC);
}

