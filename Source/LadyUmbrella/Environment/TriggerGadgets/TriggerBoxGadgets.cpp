#include "TriggerBoxGadgets.h"

#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Environment/TriggerLevelChange/TriggerLevelChange.h"
#include "LadyUmbrella/Interfaces/UnlockGadgetsInterface.h"
#include "LadyUmbrella/UI/Basics/SkipButtom.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

ATriggerBoxGadgets::ATriggerBoxGadgets()
{
	CollisionTriggerBox = CreateDefaultSubobject<UBoxComponent>("Collision Box Component");
	CollisionTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	CinematicTriggered = false;
	IsActive = true;
}

void ATriggerBoxGadgets::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !OtherActor->Implements<UUnlockGadgetsInterface>() || CinematicTriggered || !IsActive)
	{
		return;
	}
	
	IUnlockGadgetsInterface* GadgetsInterface = Cast<IUnlockGadgetsInterface>(OtherActor);
	if (GadgetsInterface && UnlockType != EGadgetType::None)	
	{
		GadgetsInterface->UnlockGadgets(UnlockType);
	}

	if (!IsValid(LevelSequence))
	{
		// Maybe crashing the game here. We don't want the game to move on without animation
		Destroy();
		return;
	}
	
	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	PlaybackSettings.bAutoPlay = false;

	ALevelSequenceActor* OutActor;
	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		LevelSequence,				// Secuencia a reproducir
		PlaybackSettings,
		OutActor                 // Devuelve el actor que contiene la secuencia
	);

	if (!IsValid(SequencePlayer) || !IsValid(LevelSequence))
	{
		// Again maybe crash the game
		return;
	}

	SequencePlayer->OnFinished.AddDynamic(this, &ATriggerBoxGadgets::HandleSequenceFinished);
	SequencePlayer->OnStop.AddDynamic(this, &ATriggerBoxGadgets::HandleSequenceStop);
	
	SequencePlayer->Play();

	CinematicTriggered = true;
	
	if (WidgetSkipClass)
	{
		ActiveWidget = CreateWidget<USkipButtom>(GetWorld(), WidgetSkipClass);
		if (ActiveWidget)
		{
			ActiveWidget->AddToViewport();
			ActiveWidget->SetFocus();
			ActiveWidget->SetKeyboardFocus();
			ActiveWidget->SetSequencePlayer(SequencePlayer);
			ActiveWidget->SetTriggerChangeLevel(LevelChangeRef);
		}
	}

	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(FGlobalPointers::MainController))
	{
		return;
	}

	FGlobalPointers::PlayerCharacter->SetPlayerGeneralState(EPlayerState::ControlsDisabled);
	FGlobalPointers::PlayerCharacter->GetMovementComponent()->Velocity = FVector::ZeroVector;
	FGlobalPointers::PlayerCharacter->SetActorHiddenInGame(true);
	
	FGlobalPointers::MainController->FlushPressedKeys();
	FGlobalPointers::MainController->bShowMouseCursor = false;
	FGlobalPointers::MainController->SetInputMode(FInputModeUIOnly());
	FGlobalPointers::MainController->SetIgnoreMoveInput(true);

	FGlobalPointers::Umbrella->SetActorHiddenInGame(true);
}

void ATriggerBoxGadgets::HandleSequenceFinished()
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		FGlobalPointers::PlayerCharacter->ResetPlayerStateToFreeMovementAndCheckAim();
		FGlobalPointers::PlayerCharacter->GetMovementComponent()->Velocity = FVector::ZeroVector;
		FGlobalPointers::PlayerCharacter->SetActorHiddenInGame(false);
	}	
	
	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->SetIgnoreMoveInput(false);
		FGlobalPointers::MainController->SetInputMode(FInputModeGameOnly());
	}

	FGlobalPointers::Umbrella->SetActorHiddenInGame(false);

	if (!IsValid(LevelChangeRef))
	{
		Destroy();
		return;
	}
	
	LevelChangeRef->OnBeginOverlap(nullptr,FGlobalPointers::PlayerCharacter,nullptr,0,false,FHitResult());
}

void ATriggerBoxGadgets::HandleSequenceStop()
{
	HandleSequenceFinished();
}
