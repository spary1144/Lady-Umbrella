#pragma once

#include "DescriptionBlock.h"

#include "Components/Border.h"
#include "Components/Image.h"


FText UDescriptionBlock::GetDescriptionText() const { return DescriptionText->GetText(); }
FText UDescriptionBlock::GetTitleText() const { return TitleText->GetText(); }
void UDescriptionBlock::SetTitleText(const FText& NewText) const { TitleText->SetText(NewText); }
void UDescriptionBlock::SetDescriptionText(const FText& NewText) const { DescriptionText->SetText(NewText); }

void UDescriptionBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (IsValid(ImageContainer))
	{
		ImageContainer->SetVisibility(bHasImage || bShowPlaceHolder ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (IsValid(DescriptiveImage))
	{
		DescriptiveImage->SetVisibility(bHasImage ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}
