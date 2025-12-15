// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "ChapterSelectorMenu.generated.h"

class UAnimatedMenuButton;
class UDescriptionBlock;

UCLASS()
class LADYUMBRELLA_API UChapterSelectorMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ChapterOne;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ChapterTwo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ChapterThree;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ChapterFour;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ChapterFive;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UDescriptionBlock* Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Iconic Image", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> ChaptersImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chapter Description", meta = (AllowPrivateAccess = "true"))
	TArray<FString> ChaptersDescription;

	UPROPERTY()
	TArray<UAnimatedBaseButton*> Chapters;

	UFUNCTION()
	void HandleLoadChapters(const int32 Chapter) const;

	UFUNCTION()
	void HandleChapterDescription(const int32 Chapter);

	UFUNCTION()
	void LoadChapterOne();
	
	UFUNCTION()
	void LoadChapterTwo();

	UFUNCTION()
	void LoadChapterThree();

	UFUNCTION()
	void LoadChapterFour();

	UFUNCTION()
	void LoadChapterFive();

	UFUNCTION()
	void UpdateChapterOneDescription();

	UFUNCTION()
	void UpdateChapterTwoDescription();

	UFUNCTION()
	void UpdateChapterThreeDescription();

	UFUNCTION()
	void UpdateChapterFourDescription();

	UFUNCTION()
	void UpdateChapterFiveDescription();

public:

	UFUNCTION()
	void Show();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
