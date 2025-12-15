// 
// TriggerBoxArena.cpp
// 
// TriggerBox that notify to the Arena linked if need to be Activated, StartCombat or SelectZones for Covers
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "TriggerBoxArena.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "Components/BillboardComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

ATriggerBoxArena::ATriggerBoxArena()
{
#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UTexture2D> SpawnSprite(
		TEXT("/Game/Textures/Sprites/Icons/ArenaManager/T_SpawnTrigger.T_SpawnTrigger"));
	if (SpawnSprite.Object != nullptr)
	{
		SpriteActivate = SpawnSprite.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> StartCombatrSprite(
		TEXT("/Game/Textures/Sprites/Icons/ArenaManager/T_Startfight.T_Startfight"));
	if (StartCombatrSprite.Object != nullptr)
	{
		SpriteCombat = StartCombatrSprite.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> EnterEventrSprite(
		TEXT("/Game/Textures/Sprites/Icons/ArenaManager/T_EventSprite.T_EventSprite"));
	if (EnterEventrSprite.Object != nullptr)
	{
		SpriteEnter = EnterEventrSprite.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> ZoneSprite(
		TEXT("/Game/Textures/Sprites/Icons/ArenaManager/T_SetZoneSprite.T_SetZoneSprite"));
	if (ZoneSprite.Object != nullptr)
	{
		SpriteZones = ZoneSprite.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> ExitSprite(
		TEXT("/Game/Textures/Sprites/Icons/ArenaManager/T_ExitDoor.T_ExitDoor"));
	if (ExitSprite.Object != nullptr)
	{
		SpriteExit = ExitSprite.Object;
	}

	ResetBillboardSprite();
#endif
}

void ATriggerBoxArena::BeginPlay()
{
	Super::BeginPlay();

	if (OnActorBeginOverlap.IsBound())
	{
		return;
	}
	
	OnActorBeginOverlap.AddDynamic(this, &ATriggerBoxArena::NotifyArenaPurpose);
}

void ATriggerBoxArena::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetCollisionComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ATriggerBoxArena::SetArenaReference(AArenaManager* AttachedArena)
{
	ArenaManagerToActivate = AttachedArena;
}

void ATriggerBoxArena::NotifyArenaPurpose(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor || !OtherActor->ActorHasTag("Player"))
	{
		return;
	}

	switch (Efunc)
	{
	case EPurpose::ActivateArena:
		ActiveArena(OverlappedActor, OtherActor);
		break;

	case EPurpose::StartCombatArena:
		StartCombatArena(OverlappedActor, OtherActor);
		break;

	case EPurpose::EnterArenaEvent:
		EnterArenaEvent(OverlappedActor, OtherActor);
		break;

	case EPurpose::ExitArenaEvent:
		ExitArenaEvent(OverlappedActor, OtherActor);
		break;

	case EPurpose::SelectZones:
		Destroy();
		break;

	default:
		break;
	}
}

void ATriggerBoxArena::ActiveArena(AActor* OverlappedActor, AActor* OtherActor) const
{
	if (!IsValid(ArenaManagerToActivate) || ArenaManagerToActivate->IsActive())
	{
		return;
	}
	
	if (ULU_GameInstance* Ulu_GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		Ulu_GameInstance->SaveCheckPoint();
	}
	
	ArenaManagerToActivate->Activate();
}

void ATriggerBoxArena::StartCombatArena(AActor* OverlappedActor, AActor* OtherActor) const
{
	if (!IsValid(ArenaManagerToActivate) || !ArenaManagerToActivate->IsActive() || ArenaManagerToActivate->IsCombatStarted())
	{
		return;
	}
	
	ArenaManagerToActivate->StartCombat();
}

void ATriggerBoxArena::EnterArenaEvent(AActor* OverlappedActor, AActor* OtherActor) const
{
	if (!IsValid(ArenaManagerToActivate) || !ArenaManagerToActivate->IsActive())
	{
		return;
	}
	
	ArenaManagerToActivate->EnterToArena(NumGroup);
}

void ATriggerBoxArena::ExitArenaEvent(AActor* OverlappedActor, AActor* OtherActor) const
{
	if (!IsValid(ArenaManagerToActivate))
	{
		return;
	}
	
	ArenaManagerToActivate->OnPlayerSkipArena();
	if (ArenaManagerToActivate->IsActive())
	{
		ArenaManagerToActivate->UnlockBlockingElement();
	}
}

#if WITH_EDITORONLY_DATA
void ATriggerBoxArena::SetEFunc(const EPurpose& NewPurpose)
{
	Efunc = NewPurpose;
	ResetBillboardSprite();
}

void ATriggerBoxArena::ResetBillboardSprite()
{
	UBillboardComponent* Billboard = GetSpriteComponent();

	if (Billboard != nullptr)
	{
		switch (Efunc)
		{
		case EPurpose::StartCombatArena:
			Billboard->SetSprite(SpriteCombat);
			Billboard->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
			break;
		case EPurpose::ActivateArena:
			Billboard->SetSprite(SpriteActivate);
			Billboard->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
			break;
		case EPurpose::SelectZones:
			Billboard->SetSprite(SpriteZones);
			Billboard->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			break;
		case EPurpose::EnterArenaEvent:
			Billboard->SetSprite(SpriteEnter);
			Billboard->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			break;
		case EPurpose::ExitArenaEvent:
			Billboard->SetSprite(SpriteExit);
			Billboard->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			break;
		default:
			Billboard->SetSprite(nullptr);
			break;
		}
	}
}
#endif



void ATriggerBoxArena::SetZones() const
{
	FBox TriggerBox = GetComponentsBoundingBox();
	if (IsValid(ArenaManagerToActivate) && IsValid(CoverClass) && TriggerBox.IsValid)
	{
		TArray<AActor*> FoundCoversParents;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), CoverClass, FoundCoversParents);

		//fill
		for (AActor* CoverParent : FoundCoversParents)
		{
			//Check if are inside trigger box
			FVector CoverLocation = CoverParent->GetActorLocation();

			if (TriggerBox.IsInside(CoverLocation))
			{
				ACoverParent* CastedCoverParent = Cast<ACoverParent>(CoverParent);
				if (IsValid(CastedCoverParent))
				{
					CastedCoverParent->SetZone(NumZone);
					ArenaManagerToActivate->AddCoverZone(NumZone);
				}
			}
		}
		TArray<AActor*> FoundSpawns;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAISpawnPoint::StaticClass(), FoundSpawns);

		for (AActor* Spawn : FoundSpawns)
		{
			//Check if are inside trigger box
			FVector SpawnLocation = Spawn->GetActorLocation();

			if (TriggerBox.IsInside(SpawnLocation))
			{
				AAISpawnPoint* SpawnActor = Cast<AAISpawnPoint>(Spawn);
				if (IsValid(SpawnActor))
				{
					if (SpawnActor->bIsActive)
					{
						SpawnActor->SetZone(NumZone);
						//ArenaManagerToActivate->AddEnemyZone(NumZone);
					}
				}
			}
		}
	}
}

void ATriggerBoxArena::ClearZones() const
{
	if (IsValid(ArenaManagerToActivate))
	{
		ArenaManagerToActivate->ClearZone(NumZone);
	}
}

#if WITH_EDITORONLY_DATA
void ATriggerBoxArena::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr)
							? PropertyChangedEvent.Property->GetFName()
							: NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ATriggerBoxArena, Efunc))
	{
		ResetBillboardSprite();
	}
}
#endif
