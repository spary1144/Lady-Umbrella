// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkipButtom.generated.h"

class ULevelSequencePlayer;
class ULevelSequence;
class UButton;
class ATriggerLevelChange;
class UPlatformDependantWidget;
class UHorizontalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkipButtonClicked);


UCLASS()
class LADYUMBRELLA_API USkipButtom : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer;

	UPROPERTY()
	ATriggerLevelChange* LevelChangeRef;

	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UPlatformDependantWidget* SkipImages;

	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* SkipHorizontalBox;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* SkipButton;

public:
	UFUNCTION()
	void OnClickedSkipButton();

	UPROPERTY(BlueprintAssignable)
	FOnSkipButtonClicked SkipButtonClicked;

	FORCEINLINE void SetSequencePlayer(ULevelSequencePlayer* SequencePlayerRef) { SequencePlayer = SequencePlayerRef; }
	FORCEINLINE void SetTriggerChangeLevel(ATriggerLevelChange* TriggerLevelRef) { LevelChangeRef = TriggerLevelRef; }

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

};
