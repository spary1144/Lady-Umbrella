// 
// GenericInteractable.cpp
// 
// Implementation of the GenericInteractable class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "GenericInteractable.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "LadyUmbrella/Util/LoggerUtil.h"


AGenericInteractable::AGenericInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(RootComponent);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(150.0f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetGenerateOverlapEvents(true);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetCastShadow(false);
	WidgetComponent->SetVisibility(true);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	WidgetRootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("WidgetRootLocation"));
	WidgetRootLocation->SetRelativeLocation(FVector::ZeroVector);
	WidgetRootLocation->SetVisibility(false);

	InnerIconSize  = 0.5f;
	OuterIconSize  = 0.2f;
	AnimationSpeed = 0.1f;

	WidgetRootLocation->SetupAttachment(GetRootComponent());
	SphereComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetupAttachment(GetRootComponent());
	
	WidgetOffset = WidgetComponent->GetComponentLocation();
	InnerSphereTriggerRadius = SphereComponent->GetUnscaledSphereRadius() + 5.0f;
	OuterSphereTriggerRadius = SphereComponent->GetUnscaledSphereRadius() * 6.0f;

	bCanInteract = true;
}

void AGenericInteractable::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(WidgetComponent))
	{
		WidgetComponent->SetWidgetClass(InteractiveIcon);
	}
}

void AGenericInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(PlayerCharacter) || !IsValid(WidgetComponent))
	{
		FLogger::ErrorLog("Bad pointers in AGenericInteractable.Tick()");
		return;
	}

	UInteractiveIcon* Icon = Cast<UInteractiveIcon>(WidgetComponent->GetWidget());

	if (!IsValid(Icon))
	{
		FLogger::ErrorLog("Invalid InteractiveIcon pointer in AGenericInteractable.Tick()");
		return;
	}

	if (!IsValid(WidgetRootLocation))
	{
		FLogger::ErrorLog("Invalid WidgetRootLocation pointer in AGenericInteractable.Tick()");
		return;
	}
	
	Icon->Update(this, OuterIconSize, OuterSphereTriggerRadius, InnerIconSize,
		InnerSphereTriggerRadius, AnimationSpeed, bCanInteract, WidgetRootLocation->GetComponentLocation());
}

int32 AGenericInteractable::Interacting()
{
	return 0;
}

int32 AGenericInteractable::Interacting(APlayerCharacter* Interactor)
{
	return 0;
}

USphereComponent* AGenericInteractable::GetSphereComponent() const
{
	return SphereComponent;
}

UWidgetComponent* AGenericInteractable::GetWidgetComponent() const
{
	return WidgetComponent;
}

UInteractiveIcon* AGenericInteractable::GetInteractiveIcon() const
{
	return Cast<UInteractiveIcon>(GetWidgetComponent()->GetWidget());
}

void AGenericInteractable::SetWidgetClass(TSubclassOf<UInteractiveIcon> WidgetClass)
{
	InteractiveIcon = WidgetClass;
}
