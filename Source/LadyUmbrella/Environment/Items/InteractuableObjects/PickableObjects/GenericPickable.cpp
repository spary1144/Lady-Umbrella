// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericPickable.h"
#include <LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h>
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "Components/WidgetComponent.h"

AGenericPickable::AGenericPickable()
{
	AmountToPickup = 0;
	ConstructorHelpers::FObjectFinder<UMaterialInstance> OverlayMat (TEXT("/Game/Materials/Instancias/Translucent/MI_PickableShader.MI_PickableShader"));
	if (IsValid(OverlayMat.Object))
	{
		OverlayMaterialInstance = OverlayMat.Object;
	}
	
	PickUpSoundComponent = CreateDefaultSubobject<UFModComponentInterface>("PickUp FMod Sound Component Interface");

	UFMODEvent* PickUpEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/PICK_UP_ELEMENT.PICK_UP_ELEMENT'");
	FmodUtils::TryAddEvent(PickUpEvent, PickUpSoundComponent);
	
}

void AGenericPickable::BeginPlay()
{
	Super::BeginPlay();
	
	if (OverlayMaterialInstance)
	{
		SetOverlayMaterialForMeshes(OverlayMaterialInstance);
	}
}

void AGenericPickable::SetOverlayMaterialForMeshes(UMaterialInstance* OverlayMaterial) const
{
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UActorComponent* Component : MeshComponents)
	{
		if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Component))
		{
			MeshComp->SetOverlayMaterial(OverlayMaterial);
		}
	}
}

int32 AGenericPickable::Interacting()
{
	Super::Interacting();

	if (IsValid(FGlobalPointers::MainController))
	{
		if (IsValid(FGlobalPointers::MainController->GetMainHUD()))
		{
			FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
			if (!FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState())
			{
				FGlobalPointers::MainController->TogglePlayerHUD();
			}
			if (IsValid(FGlobalPointers::PlayerCharacter))
			{
				if (FGlobalPointers::PlayerCharacter->GetCurrentUmbrellaLocation() != EUmbrellaLocation::Hand)
				{
					FGlobalPointers::MainController->DeactivatePlayerHUDOnTimer();
				}
			}
		}
	}
	
	return 0;
}

int32 AGenericPickable::Interacting(APlayerCharacter* Interactor)
{
	Super::Interacting(Interactor);
	
	return 0;
}

void AGenericPickable::DestroyInteractable()
{
	if (IsValid(GetWidgetComponent()))
	{
		if (UUserWidget* UserWidget = Cast<UUserWidget>(GetWidgetComponent()->GetWidget()))
		{
			if (UserWidget->IsInViewport())
			{
				UserWidget->RemoveFromParent();
			}
		}
	}
	Destroy();
}