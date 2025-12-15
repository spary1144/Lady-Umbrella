#pragma once

#include "Blueprint/UserWidget.h"
#include "LoadingScreen.generated.h"

class UImage;

UCLASS()
class LADYUMBRELLA_API ULoadingScreen : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Fade;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* LoadingRot;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* LoadingRotAlt;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* RotTest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FadeSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bUseAltAnimation = true;

public:
	
	void Show();
	UFUNCTION() void Hide();
	UFUNCTION() void OpenLevel();
	UFUNCTION() void Remove();
	UFUNCTION() void Loading();

	virtual void NativeConstruct() override;

	FORCEINLINE FString GetMapPath() { return MapToLoad; }
	FORCEINLINE void SetMapPath(FString Path) { MapToLoad = Path; }
	FORCEINLINE FString GetSublevelPath() { return SublevelToLoad; }
	FORCEINLINE void SetSublevelPath(FString Path) { SublevelToLoad = Path; }
	
private:
	FString MapToLoad;
	FString SublevelToLoad;
	void OnMapLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);
	void OnSublevelLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);
};
