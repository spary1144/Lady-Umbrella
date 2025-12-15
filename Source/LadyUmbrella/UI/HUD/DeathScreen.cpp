// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathScreen.h"

#include "Components/Image.h"

void UDeathScreen::SetDeathScreenImageTransparency(const float NewTransparency)
{
	if (!IsValid(DeathScreenImage))
	{
		return;
	}

	DeathScreenImage->SetOpacity(FMath::Clamp(NewTransparency, 0.0f, 1.0f));
}
