// 
// AISpawnPoint.cpp
// 
// Set-up Actor
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "AISpawnPoint.h"

#include "ArenaManager.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "LadyUmbrella/AI/SplinePath.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/ShieldAgencyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

// Sets default values
AAISpawnPoint::AAISpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorHiddenInGame(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Child Static Mesh"));
	StaticMesh->SetupAttachment(Root);
	StaticMesh->SetMobility(EComponentMobility::Static);
	StaticMesh->SetVisibility(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ¡Esto es CLAVE!
	StaticMesh->SetUsingAbsoluteLocation(false);
	StaticMesh->SetUsingAbsoluteRotation(false);
	StaticMesh->SetUsingAbsoluteScale(false);

	StaticMesh->SetRelativeLocation(FVector::ZeroVector);
	StaticMesh->SetRelativeRotation(FRotator::ZeroRotator);
	StaticMesh->SetRelativeScale3D(FVector(1.f));

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureArena(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_EnemyIcon.T_EnemyIcon'"));
	if (TextureArena.Succeeded())
	{
		Billboard->SetSprite(TextureArena.Object);
	}
	Billboard->SetupAttachment(StaticMesh);

#if WITH_EDITORONLY_DATA
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Child Arrow"));
	Arrow->SetupAttachment(StaticMesh);
	Arrow->ArrowColor = FColor::Red;
	Arrow->ArrowLength = 100.f;
	Arrow->bIsEditorOnly = true;

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureMafiaIcon(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_MafiaIcon.T_MafiaIcon'"));
	if (TextureMafiaIcon.Succeeded())
	{
		MafiaIcon = TextureMafiaIcon.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureAgencyIcon(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_AgencyIcon.T_AgencyIcon'"));
	if (TextureAgencyIcon.Succeeded())
	{
		AgencyIcon = TextureAgencyIcon.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureAgencyShieldIcon(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_AgencyShieldIcon.T_AgencyShieldIcon'"));
	if (TextureAgencyShieldIcon.Succeeded())
	{
		AgencyShieldIcon = TextureAgencyShieldIcon.Object;
	}
	//ChangeBillboardSprite();
#endif

	Target = ETarget::Enemy;
	bHasPath = false;
	bIsReinforcement = false;
	EnterArenaGroup = -1;
	SpawnOrder = 0;
	WavesOnBeActivated.Add(0);
	bIsFirstCombatAgent = false;
}


void AAISpawnPoint::SetZone(int newzone)
{
	NumZone = newzone;
}

int AAISpawnPoint::GetZone() const
{
	return NumZone;
}

void AAISpawnPoint::SetArenaReference(AArenaManager* AttachedArena)
{
	OwningArenaManager = AttachedArena;
}

// Called when the game starts or when spawned
void AAISpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(OwningArenaManager) && bIsActive)
	{
		OwningArenaManager->OnRegisterSpawnsDelegate.AddUFunction(this,"RegisterInArenaManager");
	}
}
#if WITH_EDITOR
void AAISpawnPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	// if (PropertyName == GET_MEMBER_NAME_CHECKED(AAISpawnPoint, EnemyType))
	// {
		
		// switch (Target)
		// {
		// case ETarget::Enemy:
		
			
		// 	break;
		// case ETarget::Interactable:
		// 	if (InteractableIcon)
		// 	{
		// 		Billboard->SetSprite(InteractableIcon);
		// 	}
		// 	break;
		// default:
		// 	break;
		// }
	//}

	//enemy type change property
	//if is agency then make IsFlanker property visible
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AAISpawnPoint, EnemyType))
	{
		ChangeBillboardSprite();
		
		const bool bWasVisible = bIsFlankerPropertyVisible;

		if (EnemyType && EnemyType->IsChildOf(AAgencyCharacter::StaticClass()))
		{
			bIsFlankerPropertyVisible = true;
		}
		else
		{
			bIsFlankerPropertyVisible = false;
		}

		if (bIsFlankerPropertyVisible != bWasVisible)
		{
			Modify();
			RerunConstructionScripts();
		}
	}

	// if (PropertyName == GET_MEMBER_NAME_CHECKED(AAISpawnPoint, bIsFlanker))
	// {
	// 	if (bIsFlanker)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Property changed in editor: %s"), *PropertyChangedEvent.Property->GetName());
	// 		OwningArenaManager->SpawnTriggerVolume();
	// 	}
	// }
}

void AAISpawnPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!IsValid(Path) && bHasPath)
	{
		const FVector Location = GetActorLocation() + FVector(0, 200, 0);
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		Path = GetWorld()->SpawnActor<ASplinePath>(SplineClass, Location, FRotator::ZeroRotator, Params);
	}
	ChangeBillboardSprite();
}

void AAISpawnPoint::ChangeBillboardSprite()
{
	if (!EnemyType)
	{
		return;
	}
	if (EnemyType->IsChildOf(AMafiaCharacter::StaticClass())  && IsValid(MafiaIcon))
	{
		Billboard->SetSprite(MafiaIcon);
		Billboard->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
	}
	if (EnemyType->IsChildOf( AAgencyCharacter::StaticClass()) && IsValid(AgencyIcon))
	{
		Billboard->SetSprite(AgencyIcon);
		Billboard->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
	}
	if (EnemyType->IsChildOf( AShieldAgencyCharacter::StaticClass()) && IsValid(AgencyShieldIcon))
	{
		Billboard->SetSprite(AgencyShieldIcon);
		Billboard->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
	}
}

#endif
// Called every frame
void AAISpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAISpawnPoint::Destroyed()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Destroyed - AI Spawn Point %s"), *GetName());	
	Super::Destroyed();
}

void AAISpawnPoint::RegisterInArenaManager()
{
	if (!bIsActive || !IsValid(OwningArenaManager))
	{
		return;
	}
	
	OwningArenaManager->RegisterSpawn(this);
}

