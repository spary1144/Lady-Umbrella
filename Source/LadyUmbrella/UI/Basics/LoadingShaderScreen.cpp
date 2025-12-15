// Fill out your copyright notice in the Description page of Project Settings.

#include "LadyUmbrella/UI/Basics/LoadingShaderScreen.h"

#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void ULoadingShaderScreen::PlayStartAnim(const UObject* WorldContextObject, FName LevelName)
{
	PlayAnimation(StartGameAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Forward);
	GetWorld()->GetTimerManager().SetTimer(
		ShootingTimer,
		[this, WorldContextObject, LevelName]()
		{
			bool bSuccess = true;
			ULevelStreamingDynamic* LevelStreaming = ULevelStreamingDynamic::LoadLevelInstance(
				GetWorld(),
				"L_MainMenu",
				FVector::ZeroVector, 
				FRotator::ZeroRotator, 
				bSuccess
			);

			if (!bSuccess || IsValid(LevelStreaming))
			{
				UGameplayStatics::OpenLevel(WorldContextObject, LevelName);
			}
		},
		StartGameAnim->GetEndTime() - GetAnimationCurrentTime(StartGameAnim),
		false
	);
}

UTextBlock* ULoadingShaderScreen::GetShaderNumber() const
{
	return ShaderNumber;
}

void ULoadingShaderScreen::SetShaderNumber(UTextBlock* NewText)
{
	ShaderNumber = NewText;
}

void ULoadingShaderScreen::LaunchGame(const UObject* WorldContextObject, FName LevelName)
{
	UGameplayStatics::OpenLevel(WorldContextObject, LevelName);
}