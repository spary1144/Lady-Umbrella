
#include "Subtitles.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void USubtitles::NativeConstruct()
{
	Super::NativeConstruct();

	StartFade = 3.0f;
	FadeSpeed = 0.01f;
	TargetOpacity = 0.0f;
	DefaultOpacity = 1.0f;
	InterpolationRate = 0.5f;

	BottomColon->SetOpacity(0.0f);
	MiddleColon->SetOpacity(0.0f);
	TopColon->SetOpacity(0.0f);

	SubtitlesOwners.Add("");
	SubtitlesOwners.Add("");
	SubtitlesOwners.Add("");
}

void USubtitles::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void USubtitles::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void USubtitles::AddSubtitle(const FString& CharacterName, const FString& Subtitle,const float Duration)
{
	const int EarliestIndex = GetEarliestZeroOpacityIndex();

	if (EarliestIndex >= 0)
	{
		//Add owner to index
		SubtitlesOwners[EarliestIndex] = CharacterName;
		
		const TObjectPtr<UTextBlock> SubtitleBlock = MapIndexToSubtitle(EarliestIndex);

		if (IsValid(SubtitleBlock))
		{
			UpdateText(EarliestIndex, SubtitleBlock, CharacterName, Subtitle, Duration);	
		}
	}
}

void USubtitles::RemoveSubtitle(const FString& CharacterName)
{
	int Index = SubtitlesOwners.IndexOfByKey(CharacterName);
	if (Index < 0 || Index > 2)
	{
		return;
	}

	SubtitlesOwners[Index] = "";
	Opacities[Index] = 0.f;   
	
	const TObjectPtr<UTextBlock> SubtitleBlock = MapIndexToSubtitle(Index);
	UTextBlock* CorrespondingName = MapIndexToName(Index);
	UTextBlock* Colon = BottomColon;
	if (!IsValid(SubtitleBlock) || !IsValid(Colon) || !IsValid(CorrespondingName))
	{
		return;
	}
	
	switch (Index)
	{
	case 1:
		Colon = MiddleColon;
		break;
	case 2:
		Colon = TopColon;
		break;
	default:
		break;
	}
	
	SubtitleBlock->SetOpacity(0.f);
	CorrespondingName->SetOpacity(0.f);
	Colon->SetOpacity(0.f);
	
	TimerDelegates[Index].Unbind();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandles[Index]); 
}

void USubtitles::UpdateText(const int Index, UTextBlock* TextBlock, const FString& CharacterName, const FString& Subtitle, const float Duration)
{
	UTextBlock* CorrespondingName = MapIndexToName(Index);

	SetText(CorrespondingName, TextBlock, CharacterName, Subtitle, Index);
	SetOpacity(Index, CorrespondingName, TextBlock, DefaultOpacity);	

	TimerDelegates[Index].BindUFunction(this, FName("UpdateOpacity"), Index, CorrespondingName, TextBlock);

	if (Duration > 0.f)
	{
		StartFade = Duration;
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandles[Index],
		TimerDelegates[Index],
		StartFade,
		false
	);
}

void USubtitles::UpdateOpacity(const int Index, UTextBlock* NameBlock, UTextBlock* SubtitleBlock)
{	
	SetOpacity(Index, NameBlock, SubtitleBlock, FMath::Lerp(Opacities[Index], TargetOpacity, InterpolationRate));

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandles[Index],
		TimerDelegates[Index],
		FadeSpeed,
		false
	);
}

int USubtitles::GetEarliestZeroOpacityIndex() const
{
	for (int Index = 0; Index < SUBTITLES_TEXT_BLOCK_COUNT; Index++)
	{
		if (IsZeroOpacity(Index))
			return Index;
	}

	return -1;
}

TObjectPtr<UTextBlock> USubtitles::MapIndexToName(const int Index) const
{
	switch (Index)
	{
	case 0: return BottomTextName;
	case 1: return MiddleTextName; 
	case 2: return TopTextName;
	default: return nullptr;
	}
}

TObjectPtr<UTextBlock> USubtitles::MapIndexToSubtitle(const int Index) const
{
	switch (Index)
	{
	case 0: return BottomTextSubtitle;
	case 1: return MiddleTextSubtitle; 
	case 2: return TopTextSubtitle;
	default: return nullptr;
	}
}

void USubtitles::SetText(UTextBlock* NameBlock, UTextBlock* SubtitleBlock, const FString& CharacterName,
	const FString& Subtitle, const int Index)
{
	NameBlock->SetText(FText::FromString(CharacterName));
	SubtitleBlock->SetText(FText::FromString(Subtitle));
	const UHorizontalBox* CurrentHorizontalBox = BottomHorizontalBox;
	const UTextBlock* TextName = BottomTextName;
	const UTextBlock* Colon = BottomColon;

	switch (Index)
	{
	case 1:
		CurrentHorizontalBox = MiddleHorizontalBox;
		TextName = MiddleTextName;
		Colon = MiddleColon;
		break;
	case 2:
		CurrentHorizontalBox = TopHorizontalBox;
		TextName = TopTextName;
		Colon = TopColon;
		break;
	default:
		break;
	}

	HandleTextAlignment(TextName, false);
	HandleTextAlignment(Colon, false);
	
	float HorizontalBoxSizeY = CurrentHorizontalBox->GetCachedGeometry().GetLocalSize().Y;
	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, SubtitleBlock, HorizontalBoxSizeY, TextName, Colon]()
	{
		const float SubtitleBlockSizeY = SubtitleBlock->GetDesiredSize().Y;
		if (SubtitleBlockSizeY >= HorizontalBoxSizeY)
		{
			HandleTextAlignment(TextName, true);
			HandleTextAlignment(Colon, true);
		}
	}, 0.01f, false);
}

void USubtitles::SetOpacity(const int Index, UTextBlock* NameBlock, UTextBlock* SubtitleBlock, const float Amount)
{
	NameBlock->SetOpacity(Amount);
	SubtitleBlock->SetOpacity(Amount);
	Opacities[Index] = Amount;
	switch (Index)
	{
	case 0: BottomColon->SetOpacity(Amount); break;
	case 1: MiddleColon->SetOpacity(Amount); break;
	case 2: TopColon->SetOpacity(Amount); break;
	default: break;
	}
}

bool USubtitles::IsZeroOpacity(const int Index) const
{
	return FMath::IsNearlyZero(Opacities[Index]);
}

void USubtitles::HandleTextAlignment(const UTextBlock* Text, const bool bIsWrapping)
{
	if (UHorizontalBoxSlot* TextSlot = Cast<UHorizontalBoxSlot>(Text->Slot))
	{
		if (!bIsWrapping)
		{
			if (TextSlot->GetVerticalAlignment() != VAlign_Bottom)
			{
				TextSlot->SetVerticalAlignment(VAlign_Bottom);
			}
		}
		else
		{
			TextSlot->SetVerticalAlignment(VAlign_Top);
		}
	}
}
