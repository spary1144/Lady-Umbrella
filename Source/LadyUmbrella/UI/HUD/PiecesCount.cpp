#include "PiecesCount.h"

#include "Components/TextBlock.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/GlobalUtil.h"

FAcquiredPieces UPiecesCount::AcquiredPieces;

void UPiecesCount::NativeConstruct()
{
	Super::NativeConstruct();
	AcquiredPieces.AddDynamic(this, &UPiecesCount::ShowAdding);
}

void UPiecesCount::ShowAdding(uint32 ToAdd, uint32 CurrTotal)
{
	uint32 ActualTotal = CurrTotal + ToAdd;
	Added->SetText(FText::FromString(FString::FromInt(ToAdd)));
	Total->SetText(FText::FromString(FString::FromInt(ActualTotal)));
	PrevTotal->SetText(FText::FromString(FString::FromInt(CurrTotal)));
	
	PlayAnimationTimeRange(AddPiecesAnim);
}
