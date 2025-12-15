// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Basics/ProgressBar/NumericProgressBar.h"
#include "LoadingShaderScreen.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ULoadingShaderScreen : public UNumericProgressBar
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Percentage", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* ShaderNumber;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* StartGameAnim;
	
public:
	
	//virtual void UpdateProgressBar(const float Value) override;

	void PlayStartAnim(const UObject* WorldContextObject, FName LevelName);
	UTextBlock* GetShaderNumber() const;
	void SetShaderNumber(UTextBlock* NewText);
	
	UFUNCTION() void LaunchGame(const UObject* WorldContextObject, FName LevelName);
	
private:
	UPROPERTY() FTimerHandle ShootingTimer;
};
