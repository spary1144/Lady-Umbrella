// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStateIcon.h"

#include "Components/Image.h"

void UEnemyStateIcon::SetImageState(UTexture2D* NewTexture)
{
	if (IsValid(StateImage) && IsValid(NewTexture))
	{
		StateImage->SetBrushFromTexture(NewTexture);
	}
}

