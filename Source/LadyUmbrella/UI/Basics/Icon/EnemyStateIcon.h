// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyStateIcon.generated.h"

/**
 * 
 */
class UImage;
UCLASS()
class LADYUMBRELLA_API UEnemyStateIcon : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* StateImage;

public:
	UFUNCTION(BlueprintCallable, Category = "Widget Estado")
	void SetImageState(UTexture2D* NewTexture);
	
};
