// Fill out your copyright notice in the Description page of Project Settings.


#include "SecuritySystem.h"

#include "LaserElement.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"


void ASecuritySystem::LoadSublevels()
{
	for (const FString& Sublevel : Sublevels)
	{
		if (ULevelStreaming* SubLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName(*Sublevel)))
		{
			SubLevel->bDisableDistanceStreaming = true;
			SubLevel->SetShouldBeLoaded(true);
			SubLevel->SetShouldBeVisible(true);
		}
	}
}

ASecuritySystem::ASecuritySystem()
{
	TriggerActivableActors = TArray<AActor*>{};
	LaserActors = TArray<AActor*>{};
	LaserElementsArray = TArray<ALaserElement*>{};
	ActorsToHideWhenActivating = TArray<AActor*>{};
	bShouldLoadSublevels = false;
	SecurityState = ESecurityState::NotTriggered;
	
	PrimaryActorTick.bCanEverTick = true;
}

void ASecuritySystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bShouldLoadSublevels)
	{
		LoadSublevels();
	}
}

void ASecuritySystem::ActivateSecuritySystem()
{
	SecurityState = ESecurityState::Activated;
	
	StopLoadingSublevels();
	OnSystemActivate();
	for (AActor* Actor : TriggerActivableActors)
	{
		if (!IsValid(Actor) || Actor->ActorHasTag("Spawner"))
		{
			continue;
		}
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
	}
		
	
	for (ALaserElement* LaserElement : LaserElementsArray)
	{
		if (!IsValid(LaserElement))
		{
			continue;
		}
		LaserElement->ActivateLaser();
	}
	
	for (AActor* Actor : ActorsToHideWhenActivating)
	{
		if (!IsValid(Actor))
		{
			continue;
		}
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
	}
}

void ASecuritySystem::DeactivateSecuritySystem()
{
	SecurityState = ESecurityState::Deactivated;

	bShouldLoadSublevels = true;
	
	for (ALaserElement* LaserElement : LaserElementsArray)
	{
		if (!IsValid(LaserElement))
		{
			continue;
		}
		LaserElement->DeactivateLaser();
	}

	for (AActor* Actor : TriggerActivableActors)
	{
		if (!IsValid(Actor) || Actor->ActorHasTag("LiftableSecurity") || Actor->ActorHasTag("Lever"))
		{
			continue;
		}
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
	}

	TArray<AActor*> SpawnerActors;
	UGameplayStatics::GetAllActorsWithTag(this, "Spawner", SpawnerActors);
	for (AActor* Actor : SpawnerActors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(true);
	}

	for (AActor* Actor : ActorsToHideWhenActivating)
	{
		if (!IsValid(Actor))
		{
			continue;
		}
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
	}
}

void ASecuritySystem::BroadcastSecurityState()
{
	bShouldLoadSublevels = true;
	
	OnSecuritySystemActivate.Broadcast();
}

void ASecuritySystem::StopLoadingSublevels()
{
	bShouldLoadSublevels = false;
}

void ASecuritySystem::SaveGame()
{
	FGlobalPointers::GameInstance.Get()->GetSaveGameFile()->SetSecurityState(SecurityState);
}

void ASecuritySystem::LoadGame()
{
	ULU_SaveGame* SaveGame = FGlobalPointers::GameInstance.Get()->GetSaveGameFile();
	if (!IsValid(SaveGame))
	{
		FLogger::ErrorLog("ASecuritySystem::LoadGame - Invalid SaveGame Ptr");
		return;
	}

	SecurityState = SaveGame->GetSecurityState();
	switch (SecurityState)
	{
	case ESecurityState::NotTriggered:
		InitializeTaggedActors();
		break;
	case ESecurityState::Activated:
		ActivateSecuritySystem();
		break;
	case ESecurityState::Deactivated:
		DeactivateSecuritySystem();
		break;
	}
}

void ASecuritySystem::SaveDataCheckPoint()
{
	SaveGame();
}

void ASecuritySystem::LoadDataCheckPoint()
{
	LoadGame();
}

bool ASecuritySystem::InitializeTaggedActors()
{
	UGameplayStatics::GetAllActorsWithTag(this, "ActivablePorTrigger", TriggerActivableActors);
	for (AActor* Actor : TriggerActivableActors)
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
	}
	
	UGameplayStatics::GetAllActorsOfClass(this, ALaserElement::StaticClass(), LaserActors);
	if (SecurityState == ESecurityState::Activated)
	{
		return true;
	}
	
	for (AActor* Actor : LaserActors)
	{
		ALaserElement* LaserElement = Cast<ALaserElement>(Actor);
		if (IsValid(LaserElement))
		{
			LaserElement->DeactivateLaser();
			LaserElementsArray.Add(LaserElement);
		}
	}

	UGameplayStatics::GetAllActorsWithTag(this, "HideOnActivation", ActorsToHideWhenActivating);
	for (AActor* Actor : ActorsToHideWhenActivating)
	{
		if (!IsValid(Actor))
		{
			continue;
		}
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
	}
	return false;
}

void ASecuritySystem::BeginPlay()
{
	Super::BeginPlay();
	switch (SecurityState)
	{
	case ESecurityState::NotTriggered:
		InitializeTaggedActors();
		break;
	case ESecurityState::Activated:
		ActivateSecuritySystem();
		break;
	case ESecurityState::Deactivated:
		DeactivateSecuritySystem();
		break;
	}
}
