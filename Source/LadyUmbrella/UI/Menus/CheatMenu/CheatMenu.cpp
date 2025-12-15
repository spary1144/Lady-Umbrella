
#include "CheatMenu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Controllers/MainController.h"

void UCheatMenu::Show()
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.Show()");
		return;
	}
	FGlobalPointers::MainController->SetInputMode(FInputModeGameAndUI());
	FGlobalPointers::MainController->bShowMouseCursor = true;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
}

void UCheatMenu::Hide()
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.Hide()");
		return;
	}
	FGlobalPointers::MainController->SetInputMode(FInputModeGameOnly());
	FGlobalPointers::MainController->bShowMouseCursor = false;
	SetVisibility(ESlateVisibility::Hidden);
}

void UCheatMenu::Godmode()
{
	if (!IsValid(FGlobalPointers::MainController) || !IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.Godmode()");
		return;
	}

	FGlobalPointers::MainController->Godmode();

	FString DebugString = "Godmode is now ";
	DebugString.Append(FGlobalPointers::PlayerCharacter->IsGodmode() ? "on!" : "off!");
	FLogger::DebugLog(DebugString);
}

void UCheatMenu::Noclip()
{
	if (!IsValid(FGlobalPointers::MainController) || !IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.Noclip()");
		return;
	}
	
	FGlobalPointers::MainController->NoClip();

	FString DebugString = "Noclip is now ";
	DebugString.Append(FGlobalPointers::PlayerCharacter->IsNoClip() ? "on!" : "off!");
	FLogger::DebugLog(DebugString);
}

void UCheatMenu::InfiniteAmmo()
{
	if (!IsValid(FGlobalPointers::MainController) || !IsValid(FGlobalPointers::Umbrella))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.InfiniteAmmo()");
		return;
	}
	
	FGlobalPointers::MainController->InfiniteAmmo();

	FString DebugString = "Infinite Ammo is now ";
	DebugString.Append(FGlobalPointers::Umbrella->IsInfiniteAmmo() ? "on!" : "off!");
	FLogger::DebugLog(DebugString);
}

void UCheatMenu::UnlockGadgets()
{
	if (!IsValid(FGlobalPointers::MainController) || !IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.UnlockGadgets()");
		return;
	}
	
	FGlobalPointers::MainController->UnlockGadgets();
	FLogger::DebugLog("Gadgets are now unlocked!");
}

void UCheatMenu::UnlockUpgrades()
{
	FGlobalPointers::MainController->UnlockUpgrades();
	FLogger::DebugLog("Unlocked all upgrades!");
}

void UCheatMenu::SetAmmoCurrent(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.SetAmmoCurrent()");
		return;
	}
	
	if (!UE::String::IsNumeric(Text.ToString()))
	{
		FLogger::ErrorLog("Invalid AmmoCurrent Value!");
		return;
	}

	FGlobalPointers::MainController->SetAmmoCurrent(FCString::Atoi(*Text.ToString()));

	FString DebugString = "Current Ammo set to ";
	DebugString.Append(Text.ToString());
	DebugString.Append("!");
	FLogger::DebugLog(DebugString);
}

void UCheatMenu::SetAmmoReserve(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.SetAmmoReserve()");
		return;
	}
	
	if (!UE::String::IsNumeric(Text.ToString()))
	{
		FLogger::ErrorLog("Invalid AmmoReserve Value!");
		return;
	}

	FGlobalPointers::MainController->SetAmmoReserve(FCString::Atoi(*Text.ToString()));

	FString DebugString = "Reserve Ammo set to ";
	DebugString.Append(Text.ToString());
	DebugString.Append("!");
	FLogger::DebugLog(DebugString);
}

void UCheatMenu::SetComponents(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in CheatMenu.SetComponents()");
		return;
	}
	
	if (!UE::String::IsNumeric(Text.ToString()))
	{
		FLogger::ErrorLog("Invalid AmmoCurrent Value!");
		return;
	}
	
	FGlobalPointers::MainController->SetComponents(FCString::Atoi(*Text.ToString()));

	FString DebugString = "Components set to ";
	DebugString.Append(Text.ToString());
	DebugString.Append("!");
	FLogger::DebugLog(DebugString);
}

void UCheatMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetGodmodeButton()->OnClicked.Clear();
	GetGodmodeButton()->OnClicked.AddDynamic(this, &UCheatMenu::Godmode);
	
	GetNoclipButton()->OnClicked.Clear();
	GetNoclipButton()->OnClicked.AddDynamic(this, &UCheatMenu::Noclip);
	
	GetInfiniteAmmoButton()->OnClicked.Clear();
	GetInfiniteAmmoButton()->OnClicked.AddDynamic(this, &UCheatMenu::InfiniteAmmo);
	
	GetUnlockGadgetsButton()->OnClicked.Clear();
	GetUnlockGadgetsButton()->OnClicked.AddDynamic(this, &UCheatMenu::UnlockGadgets);
	
	GetUnlockUpgradesButton()->OnClicked.Clear();
	GetUnlockUpgradesButton()->OnClicked.AddDynamic(this, &UCheatMenu::UnlockUpgrades);

	GetAmmoCurrentTextBox()->OnTextCommitted.AddDynamic(this, &UCheatMenu::SetAmmoCurrent);
	GetAmmoReserveTextBox()->OnTextCommitted.AddDynamic(this, &UCheatMenu::SetAmmoReserve);
	GetComponentsTextBox()->OnTextCommitted.AddDynamic(this, &UCheatMenu::SetComponents);
	
}
