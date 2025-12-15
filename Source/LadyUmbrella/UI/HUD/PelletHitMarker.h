// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PelletHitMarker.generated.h"

class UImage;
class UCanvasPanel;
class AGenericCharacter;

UCLASS()
class LADYUMBRELLA_API UPelletHitMarker : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* HitMarker;

	UPROPERTY(EditAnywhere, Category="On Enemy Death", meta = (AllowPrivateAccess = true))
	FLinearColor HitMarkerKillColor;

	UPROPERTY()
	UCanvasPanel* HitMarkerPanel;

	UPROPERTY()
	TArray<UImage*> HitMarkerArray;

	UPROPERTY()
	FLinearColor BaseHitMarker;

	void UpdateHitMarkersArray(const int32 MaxNrBulletPerShot);

	void UpdateHitMarkers(const bool bHasHitGenericCharacter, TSet<AGenericCharacter*> EnemiesHitByPellets) const;

public:

	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();

protected:

	virtual void NativeConstruct() override;
	
};
