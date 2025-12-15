// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "Components/ScrollBox.h"
#include "MenuSection.generated.h"

class UPanelWidget;
class APlayerCharacter;

UCLASS()
class LADYUMBRELLA_API UMenuSection : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Section", meta = (AllowPrivateAccess = "true"))
	FName SectionIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Section", meta = (AllowPrivateAccess = "true"))
	TArray<UWidget*> Controls;

public:
	void Show();
	FORCEINLINE FName GetIdentifier() const { return SectionIdentifier; }

protected:
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

};