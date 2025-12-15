#pragma once

#include "LadyUmbrella/UI/Menus/Menu.h"
#include "CheatMenu.generated.h"

class UEditableTextBox;
class UButton;
class UVerticalBox;

UCLASS()
class LADYUMBRELLA_API UCheatMenu : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Vertical Box", meta = (BindWidget, AllowPrivateAccess = "true"))
	UVerticalBox* CheatsVB;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* GodmodeButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* NoclipButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* InfiniteAmmoButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* UnlockGadgetsButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* UnlockUpgradesButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UEditableTextBox* AmmoCurrentTextBox;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UEditableTextBox* AmmoReserveTextBox;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UEditableTextBox* ComponentsTextBox;

public:
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Hide();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Godmode();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Noclip();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void InfiniteAmmo();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void UnlockGadgets();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void UnlockUpgrades();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void SetAmmoCurrent(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void SetAmmoReserve(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void SetComponents(const FText& Text, ETextCommit::Type CommitMethod);
	
	FORCEINLINE UButton* GetGodmodeButton() const { return GodmodeButton; }
	FORCEINLINE UButton* GetNoclipButton() const { return NoclipButton; }
	FORCEINLINE UButton* GetInfiniteAmmoButton() const { return InfiniteAmmoButton; }
	FORCEINLINE UButton* GetUnlockGadgetsButton() const { return UnlockGadgetsButton; }
	FORCEINLINE UButton* GetUnlockUpgradesButton() const { return UnlockUpgradesButton; }
	FORCEINLINE UEditableTextBox* GetAmmoCurrentTextBox() const { return AmmoCurrentTextBox; }
	FORCEINLINE UEditableTextBox* GetAmmoReserveTextBox() const { return AmmoReserveTextBox; }
	FORCEINLINE UEditableTextBox* GetComponentsTextBox() const { return ComponentsTextBox; }

protected:
	virtual void NativeConstruct() override;
	
};
