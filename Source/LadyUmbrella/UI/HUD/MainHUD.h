//  
// MainHUD.h
// 
// HUD for the main game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LadyUmbrella/UI/Menus/CheatMenu/CheatMenu.h"
#include "MainHUD.generated.h"

class APlayerCharacter;
class UPauseMenu;
class UAmmoCounter;
class UPelletHitMarker;
class UCrosshair;
class USettingsMenu;
class USubtitles;
class UGrenadeIndicator;
class AGrenade;

UCLASS()
class LADYUMBRELLA_API AMainHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY()
	UPauseMenu* PauseMenuInstance;

	UPROPERTY()
	UCheatMenu* CheatMenuInstance;

	UPROPERTY()
	USettingsMenu* SettingsMenuInstance;

	UPROPERTY()
	UAmmoCounter* AmmoCounterInstance;

	UPROPERTY()
	UGrenadeIndicator* GrenadeIndicatorInstance;

	UPROPERTY()
	UPelletHitMarker* PelletHitMarkerInstance;

	UPROPERTY()
	UCrosshair* CrosshairLeftBorderInstance;
	
	UPROPERTY()
	UCrosshair* CrosshairDot;
	
	UPROPERTY()
	UCrosshair* CrosshairRightBorderInstance;

	UPROPERTY()
	USubtitles* SubtitlesInstance;

	UPROPERTY()
	UUserWidget* HUDInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPauseMenu> PauseMenuWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCheatMenu> CheatMenuWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menus", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USettingsMenu> SettingsMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USubtitles> SubtitlesWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	float HidePlayerHUDTime;
	
	FTimerHandle HidePlayerHUDTimerHandle;
	
	bool bCheatMenuCanToggle;

	UPROPERTY()
	bool bIsOn;

public:
	
	UFUNCTION()
	void OnUmbrellaSpawned();

	UFUNCTION(BlueprintCallable) void HideEverythingExceptSubtitle() const;
	UFUNCTION(BlueprintCallable) void ShowEverythingExceptMenus() const;

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu() const;
	void ToggleCheatMenu();
	void EnableToggleCheatMenu();

	UFUNCTION(BlueprintCallable)
	void ToggleSettingsMenu() const;
	
	UFUNCTION(BlueprintCallable)
	void TogglePlayerHUD();

	UFUNCTION(BlueprintCallable)
	void UnTogglePlayerHUD();

	UFUNCTION(BlueprintCallable)
	void ToggleGrenadeIndicator(AGrenade* Grenade) const;

	UFUNCTION(BlueprintCallable)
	void UnToggleGrenadeIndicator() const;

	void AddSubtitle(const FString& CharacterName, const FString& Subtitle,const float& Duration) const;
	void RemoveSubtitle(const FString& CharacterName) const;
	void HidePlayerHUDOnTimer();
	void ResetHidePlayerHUDTimer();

	UPauseMenu* GetPauseMenuInstance() const;
	TSubclassOf<UPauseMenu> GetPauseMenuWidget() const;
	UAmmoCounter* GetAmmoCounterInstance() const;
	UCheatMenu* GetCheatMenuInstance() const;
	TSubclassOf<UCheatMenu> GetCheatMenuWidget() const;
	USettingsMenu* GetSettingsMenuInstance() const;
	TSubclassOf<USettingsMenu> GetSettingsMenuWidget() const;
	USubtitles* GetSubtitlesInstance() const;
	TSubclassOf<USubtitles> GetSubtitlesWidget() const;
	UGrenadeIndicator* GetGrenadeIndicatorInstance() const;
	UUserWidget* GetHUDInstance() const;
	TSubclassOf<UUserWidget> GetHUDWidget() const;
	bool GetPlayerHUDState() const;
	
	void SetPauseMenuInstance(const TSubclassOf<UPauseMenu>& NewPauseMenuWidget);
	void SetCheatMenuInstance(const TSubclassOf<UCheatMenu>& NewCheatMenuWidget);
	void SetSettingsMenuInstance(const TSubclassOf<USettingsMenu>& NewSettingsMenuWidget);
	void SetSubtitleInstance(const TSubclassOf<USubtitles>& NewSubtitleWidget);
	void SetHUDInstance(const TSubclassOf<UUserWidget>& NewHUDWidget);
	void SetPlayerHUDState(const bool State);
	
protected:

	virtual void BeginPlay() override;

	template <typename T>
	static T* FindWidgetOfClass(UUserWidget* RootWidget);

	template <typename T>
	static void FindAllWidgetsOfClass(UUserWidget* RootWidget, TArray<T*>& OutWidgets);

	static UWidgetAnimation* FindAnimationByName(const UUserWidget* Widget, const FName& AnimName);
};
