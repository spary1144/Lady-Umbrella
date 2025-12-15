// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradePieces.h"

#include "Components/StaticMeshComponent.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"

AUpgradePieces::AUpgradePieces()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetupAttachment(RootComponent);

	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}
	
	SetAmountToPickup(1);

	TypeOfPickable = true;
}

void AUpgradePieces::BeginPlay()
{
	Super::BeginPlay();
}

int32 AUpgradePieces::Interacting()
{
	GetPickUpSoundComponent()->SetParameter(PickUpEventParameterName, TypeOfPickable);
	
	GetPickUpSoundComponent()->PlayEvent();

	SetActorHiddenInGame(true);

	GetWorldTimerManager().SetTimer(PickUpTimerHandle, this, &AGenericPickable::DestroyInteractable, 2.f, false);
	
	//Destroy();

	if (IsValid(FGlobalPointers::GameInstance))
	{
		const uint8 CurrentPieces = FGlobalPointers::GameInstance->GetSaveGameFile()->GetTotalPiecesPickedUp();
		FGlobalPointers::GameInstance->GetSaveGameFile()->SetTotalPiecesPickedUp(CurrentPieces + 1);

		if (CurrentPieces == 0)
			Steam::UnlockAchievement(ESteamAchievement::THE_SHINING);

		if ((CurrentPieces + 1) == TOTAL_PIECES_IN_GAME)
			Steam::UnlockAchievement(ESteamAchievement::GOTTA_PICK_EM_ALL);
	}
	
	return GetAmountToPickup();
}

int32 AUpgradePieces::Interacting(APlayerCharacter* Interactor)
{
	Super::Interacting(Interactor);
	
	return 0;
}