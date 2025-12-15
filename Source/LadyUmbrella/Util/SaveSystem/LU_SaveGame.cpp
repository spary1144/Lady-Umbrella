// Fill out your copyright notice in the Description page of Project Settings.


#include "LU_SaveGame.h"

#include "FChapterNames.h"
#include "LadyUmbrella/Environment/TriggerLevelChange/EChapterNames.h"


ULU_SaveGame::ULU_SaveGame()
{
	/*
	for (FString Chapter : ChapterNames::ChapterList)
	{
		ChaptersProgress.Add(FName(Chapter), {false, false, false});
	}
	*/
}

void ULU_SaveGame::InitializeDefaults()
{
	for (const FString& Chapter : ChapterNames::ChapterList)
	{
		ChaptersProgress.Add(FName(Chapter), {false, false, false, false });
	}
	
	ChaptersProgress.Find(ToName(EChapterNames::Chapter0))->bLastChapterPlayed = true;
	ChaptersProgress.Find(ToName(EChapterNames::Chapter0))->bChapterUnlocked	 = true;
	ChaptersProgress.Find(ToName(EChapterNames::Chapter0))->bLastChapterUnlocked = true;
}
