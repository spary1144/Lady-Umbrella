// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreen.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UDeathScreen : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UImage> DeathScreenImage;

public:
	UFUNCTION()
	void SetDeathScreenImageTransparency(const float NewTransparency);
};
