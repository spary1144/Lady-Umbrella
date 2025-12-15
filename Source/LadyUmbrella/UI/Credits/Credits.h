// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCreditSlide.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Credits.generated.h"

UCLASS()
class LADYUMBRELLA_API UCredits : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Up", meta = (AllowPrivateAccess = "true"))
	UDataTable* CreditsTable;

	UPROPERTY(BlueprintReadWrite, Category = "Set Up", meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* Titles;
	
	UPROPERTY(BlueprintReadWrite, Category = "Set Up", meta = (BindWidget, AllowPrivateAccess = "true"))
	UHorizontalBox* Names;
	
	UPROPERTY(BlueprintReadWrite, Category = "Set Up", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* FadeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Up", meta = (AllowPrivateAccess = "true"))
	float TimeBetweenTransition = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Up", meta = (AllowPrivateAccess = "true"))
	float FadeSpeed = 2.f;
public:
	void Show();
	void Hide();
	void TransitionToNext();
	
	FORCEINLINE uint8 GetTitleColumnNumber() const { return TitleColumNumber; }
	FORCEINLINE void SetTitleColumnNumber(uint8 Cols){ TitleColumNumber = Cols; }
	FORCEINLINE uint8 GetNameColumnNumber() const { return NameColumNumber; }
	FORCEINLINE void SetNameColumnNumber(uint8 Cols){ NameColumNumber = Cols; }
	
	UFUNCTION() void ShowSlide();

protected:

	const uint8 MAX_SLIDE_NUM = 28;
	
	uint8 TitleColumNumber;
	uint8 NameColumNumber;
	uint8 CurrentSlide;
	uint8 RowIndex;

	FTimerHandle SlideTimer;

	TArray<FCreditSlide*> AllRows;

	void UpdateColumns() const;

	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
};
