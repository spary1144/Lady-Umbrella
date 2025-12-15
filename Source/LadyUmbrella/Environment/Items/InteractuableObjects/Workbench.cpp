/*
 * Zulo Interactive
 */

#include "Workbench.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "LadyUmbrella/UI/Menus/WorkbenchMenu/WorkBenchMenu.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

FOnWorkBenchInteract AWorkbench::OnWorkBenchInteract;

AWorkbench::AWorkbench()
{
	CameraComponent		  = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	StaticMeshComponent	  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	PlayerPosition		  =	CreateDefaultSubobject<USceneComponent>(TEXT("PlayerPosition"));
	
	CameraComponent->SetupAttachment(GetSphereComponent());
	StaticMeshComponent->SetupAttachment(GetSphereComponent());
	PlayerPosition->SetupAttachment(GetSphereComponent());
	
	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}

	FmodComponent = CreateDefaultSubobject<UFModComponentInterface>("Fmod Component Interface");

	UFMODEvent* OpenWorkbench = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/WORKBENCHOPEN.WORKBENCHOPEN'");; 
	UFMODEvent* CloseWorkbench = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/WORKBENCHCLOSE.WORKBENCHCLOSE'"); 
	FmodUtils::TryAddEvent(OpenWorkbench, FmodComponent);
	FmodUtils::TryAddEvent(CloseWorkbench, FmodComponent);
}

void AWorkbench::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

// void AWorkbench::OnConstruction(const FTransform& Transform)
// {
// 	Super::OnConstruction(Transform);
// 	if (IsValid(CollisionSphereComponent))
// 	{
// 		CollisionSphereComponent->SetSphereRadius(SphereComponentRadius);
// 	}
// }

void AWorkbench::PlayFModEvent(const FString& EventToPlay) const
{
	if (!IsValid(FmodComponent))
	{
		return;
	}
	if (FmodComponent->SetCurrentEvent(EventToPlay))
	{
		FmodComponent->PlayEvent();	
	}
}

int32 AWorkbench::Interacting()
{
	PlayFModEvent("WORKBENCHOPEN");
	DisableInputCustom();
	SetUpWidget();
	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		const FVector TargetLocation = PlayerPosition->GetComponentLocation();
		const FRotator TargetRotation = PlayerPosition->GetComponentRotation();
		FGlobalPointers::PlayerCharacter->SetActorLocationAndRotation(TargetLocation, TargetRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	OnWorkBenchInteract.Broadcast(EUmbrellaLocation::WorkBench, true, false);
	// Does not give anything back (ammo or pieces)
	return 0;
}

int32 AWorkbench::Interacting(APlayerCharacter* PlayerCharacternInteractor)
{
	return 0;
}

void AWorkbench::DisableInputCustom()
{
	// Disable input
	PlayerController->SetViewTargetWithBlend(this, 0.5f, VTBlend_Linear, 0, false);
	//PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->FlushPressedKeys();
	PlayerController->SetShowMouseCursor(true);
	//PlayerController->Pause();
}


void AWorkbench::WorkbenchExit(EUmbrellaLocation UmbrellaLocation, bool bIsImmediate, bool bPlayAnim)
{
	PlayFModEvent("WORKBENCHCLOSE");
}

void AWorkbench::SetUpWidget()
{
	if (IsValid(WorkbenchWidgetReference))
	{
		if (!IsValid(WorkbenchCreatedWidget))
		{
			WorkbenchCreatedWidget = CreateWidget<UWorkBenchMenu>(GetWorld(), WorkbenchWidgetReference);

			if (IsValid(WorkbenchCreatedWidget))
			{
				WorkbenchCreatedWidget->AddToViewport();
				WorkbenchCreatedWidget->SetVisibility(ESlateVisibility::Visible);
				WorkbenchCreatedWidget->OnWorkBenchExit.AddDynamic(this, &AWorkbench::WorkbenchExit);
				if (IsValid(FGlobalPointers::MainController))
				{
					FGlobalPointers::MainController->UnTogglePlayerHUD();
				}
				if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::Umbrella))
				{
					// WorkbenchWidget Setup
					// WorkbenchCreatedWidget->PlayFadeIn();
					// Set UI For pieces
					WorkbenchCreatedWidget->SetPiecesElement(FGlobalPointers::PlayerCharacter->GetCurrentPiecesForUpgrades());
					UUpgradeComponent* UpgradesComponentUmbrella = FGlobalPointers::Umbrella->GetUpgradeComponent();
					if (IsValid(UpgradesComponentUmbrella))
					{
						WorkbenchCreatedWidget->ReadUmbrellaUpgrades(*UpgradesComponentUmbrella->GetUpgrades());
					}
					WorkbenchCreatedWidget->SetUpInitialFocus();
				}
			}
		}
		else
		{
			WorkbenchCreatedWidget->SetVisibility(ESlateVisibility::Visible);
			
			if (IsValid(FGlobalPointers::MainController))
			{
				FGlobalPointers::MainController->SetInputMode(FInputModeUIOnly());
				FGlobalPointers::MainController->UnTogglePlayerHUD();
			}
			if (IsValid(FGlobalPointers::PlayerCharacter))
			{
				WorkbenchCreatedWidget->SetPiecesElement(FGlobalPointers::PlayerCharacter->GetCurrentPiecesForUpgrades());
				WorkbenchCreatedWidget->ReadUmbrellaUpgrades(*FGlobalPointers::Umbrella->GetUpgradeComponent()->GetUpgrades());
				WorkbenchCreatedWidget->SetUpInitialFocus();
			}
		}
	}
}
