// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityLever.h"

#include "Components/SphereComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/SecuritySystem.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"

#define SM_LEVER TEXT("StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Icons/SM_LeverSmall.SM_LeverSmall'")
#define M1_LEVER TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Editor/LevelDesignKit/MI_Color_GreyDark.MI_Color_GreyDark'")
#define M2_LEVER TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Editor/LevelDesignKit/MI_Color_Silver.MI_Color_Silver'")
#define M3_LEVER TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/Instancias/Editor/LevelDesignKit/MI_Color_Orange.MI_Color_Orange'")


ASecurityLever::ASecurityLever()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterialInstance>(M1_LEVER).Object);
	StaticMeshComponent->SetMaterial(1, ConstructorHelpers::FObjectFinder<UMaterialInstance>(M2_LEVER).Object);
	StaticMeshComponent->SetMaterial(2, ConstructorHelpers::FObjectFinder<UMaterialInstance>(M3_LEVER).Object);
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(SM_LEVER).Object);
	StaticMeshComponent->SetupAttachment(GetSphereComponent());
	//StaticMeshComponent->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}
}

int32 ASecurityLever::Interacting()
{	
	Super::Interacting();
	
	if (!IsValid(SecuritySystem))
	{
		return false;
	}

	OnLeverInteract();
	//SecuritySystem->DeactivateSecuritySystem();

	if (IsValid(FGlobalPointers::GameInstance) && (FGlobalPointers::GameInstance->GetDeathsInLevel() == 0))
		Steam::UnlockAchievement(ESteamAchievement::THE_FLOOR_IS_LAVA);
	
	return true;
}

int32 ASecurityLever::Interacting(APlayerCharacter* Interactor)
{
	return Super::Interacting(Interactor);
}
