#include "LandmarkFocus.h"

#include "EnhancedInputComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/CameraStateKey.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"

ALandmarkFocus::ALandmarkFocus()
{
	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));
	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}

	LookTime = 2.f;
	
#if WITH_EDITOR
	FocusPointIcon = CreateDefaultSubobject<UBillboardComponent>("Focus Point");
	FocusPointIcon->SetupAttachment(RootComponent);
	FocusPointIcon->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EditorResources/LevelInstance.LevelInstance")));

	OriginPointIcon = CreateDefaultSubobject<UBillboardComponent>("Origin Point");
	OriginPointIcon->SetupAttachment(RootComponent);
	OriginPointIcon->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Engine/EditorResources/S_SceneCaptureIcon.S_SceneCaptureIcon")));
	
	FocusPointLocation = FTransform(
		GlobalVectorConstants::Float0001,
		GlobalVectorConstants::FloatOne,
		GlobalVectorConstants::Float1110);
#endif
	
}


void ALandmarkFocus::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("EndPlay - Landmark Focus %s with reason %d"), *GetName(), EndPlayReason);
	if (EndPlayReason != EEndPlayReason::Type::RemovedFromWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("ALandmarkFocus unexpected EndPlay"));
		Super::EndPlay(EndPlayReason);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(ActionHandler);
	Super::EndPlay(EndPlayReason);
}

void ALandmarkFocus::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		return;
	}

	const float DistanceToActor = FVector::Dist(FGlobalPointers::PlayerCharacter->GetActorLocation(), GetWidgetRootLocation()->GetComponentLocation());
	if (GetOuterRadius() <= DistanceToActor)
	{
		return;
	}
	
	const bool bInRange = GetInnerRadius() > DistanceToActor;
	if (!bInRange && FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->GetCurrentCameraStateKey() == ECameraStateKey::LOOKAT)
	{
		StopFocusing();
		GetWorld()->GetTimerManager().ClearTimer(ActionHandler);
	}
}

#if WITH_EDITOR
void ALandmarkFocus::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FocusPointIcon->SetRelativeTransform(FocusPointLocation);
	GetWidgetComponent()->SetRelativeTransform(FocusPointLocation);
}
#endif

int32 ALandmarkFocus::Interacting(APlayerCharacter* Interactor)
{
	if (IsValid(Interactor))
	{
		Interactor->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::LOOKAT);
		Interactor->GetCameraSystemComponent()->SetFocusLocation(FocusPointLocation.GetLocation() + GetActorLocation());
		GetWorld()->GetTimerManager().SetTimer(
			ActionHandler,
			this, &ALandmarkFocus::StopFocusing,
			LookTime,
			false
		);
	}

	GetInteractiveIcon()->SetShouldShow(false);
	
	return Super::Interacting(Interactor);
}
void ALandmarkFocus::StopFocusing()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		return;
	}

	if (FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->GetCurrentCameraStateKey() == ECameraStateKey::LOOKAT)
	{
		FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::DEFAULT);
	}

	FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->SetFocusLocation(FVector::ZeroVector);
	
	GetInteractiveIcon()->SetShouldShow(true);
}
