// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverParent.h"

#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "LadyUmbrella/UI/Basics/ProgressBar/NumericProgressBar.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

// Sets default values
ACoverParent::ACoverParent()
{
	// Variable initialization
	NumZone = 0;
	bCanBeDestroyed = false;
	bNeedsOnConstructReset = false;
	bNeedsOnConstructUpdate = true;
	
	// Rest of Constructor	
#if UE_EDITOR // Doing this to rotate the life widget towards the player.
	PrimaryActorTick.bCanEverTick = true;
#else
	PrimaryActorTick.bCanEverTick = false;
#endif
	
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Stationary);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	BillboardRoot = CreateEditorOnlyDefaultSubobject<USceneComponent>(TEXT("BillboardRoot"));
	Billboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	ZoneTextRender = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("ZoneTextRender"));

	if (IsValid(BillboardRoot) && IsValid(Billboard) && IsValid(ZoneTextRender))
	{
		BillboardRoot->SetRelativeLocation(FVector(0, 0, 50));
		BillboardRoot->SetupAttachment(Root);
		
		Billboard->SetupAttachment(BillboardRoot);
		Billboard->SetVisibility(true);
		Billboard->SetHiddenInGame(true);
		Billboard->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		TObjectPtr<UTexture2D> ShieldSpriteRef = AssetUtils::FindObject<UTexture2D>(TEXT("/Game/Textures/Sprites/Icons/CoverSystem/T_ShieldIcon.T_ShieldIcon"));
		if (ShieldSpriteRef != nullptr)
		{
			Billboard->SetSprite(ShieldSpriteRef);
		}

		ZoneTextRender->SetupAttachment(BillboardRoot);
		ZoneTextRender->SetRelativeLocation(FVector(0, 0, 50)); // Completely arbitrary number to raise it above the shield icon.
		ZoneTextRender->SetVisibility(true);
		ZoneTextRender->SetHiddenInGame(true);
		ZoneTextRender->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ZoneTextRender->Text = FText::FromString(TEXT(""));
		ZoneTextRender->TextRenderColor = FColor::Red;
		ZoneTextRender->XScale = 2;
		ZoneTextRender->YScale = 2;
	}
	
	HealthWidgetRoot = CreateEditorOnlyDefaultSubobject<USceneComponent>(TEXT("HealthWidgetRoot"));
	HealthWidget = CreateEditorOnlyDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));

	if (IsValid(HealthWidgetRoot) && IsValid(HealthWidget))
	{
		HealthWidgetRoot->SetupAttachment(Root);
		HealthWidgetRoot->SetMobility(EComponentMobility::Type::Movable);

		HealthWidget->SetMobility(EComponentMobility::Type::Movable);
		HealthWidget->SetupAttachment(HealthWidgetRoot);
		HealthWidget->SetHiddenInGame(bCanBeDestroyed);
		HealthWidget->CastShadow = false;
		HealthWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

#pragma region Public Functions

void ACoverParent::DestroyCover()
{
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (IsValid(Cover))
		{
			Cover->DestroyCover();
		}
	}

	Destroy();
}

void ACoverParent::SetZone(const int NewZone)
{
	NumZone = NewZone;

	if (IsValid(ZoneTextRender))
	{
		FText ZoneName = FText::FromString(FString::FromInt(NumZone));
		ZoneTextRender->SetText(ZoneName);
	}
}

void ACoverParent::ResetIaEntryPointsOwningSplines() const
{
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr)
		{
			Cover->ResetIaEntryPointsOwningSplines();
		}
	}
}

#pragma endregion

#pragma region Editor-Only Functions

void ACoverParent::ResetAllSplines()
{
	bNeedsOnConstructReset = true;

#if WITH_EDITOR
	RerunConstructionScripts();
#endif
}

void ACoverParent::ForceUpdateAllSplines()
{
	bNeedsOnConstructUpdate = true;
	
#if WITH_EDITOR
	RerunConstructionScripts();
#endif
}

void ACoverParent::ShowAllPlayerEntryPoints() const
{
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr)
		{
			Cover->ShowPlayerEntryPoints();
		}
	}
}

void ACoverParent::HideAllPlayerEntryPoints() const
{
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr)
		{
			Cover->HidePlayerEntryPoints();
		}
	}
}

void ACoverParent::DeleteAllAiEntryPoints() const
{
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr)
		{
			Cover->DestroyAllAttachedIaEntryPoints();
		}
	}
}

void ACoverParent::CleanUnusedAttachedIaEntryPoints()
{
	ResetIaEntryPointsOwningSplines();
	
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		const AAICoverEntryPoint* EntryPoint = Cast<AAICoverEntryPoint>(Actor);
		if (IsValid(EntryPoint)) // If the Entry Point is not connected to any USplineComponent.
		{
			const UCoverSpline* OwningCover = EntryPoint->GetOwningSpline();
			if (OwningCover == nullptr)
			{
				Actor->Destroy();
				continue;
			}

			ACoverParent* ParentActor = OwningCover->TryGetParent();
			if (!IsValid(ParentActor) || ParentActor != this) // If we have an entry point attached that doesn't belong to us. (ex. When duplicating objects)
			{
				Actor->Destroy();
				ParentActor->ForceUpdateAllSplines();
			}
		}
	}
}

void ACoverParent::FindConnectableEntryPoints(TArray<AAICoverEntryPoint*>& BufferArray) const
{
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr && !Cover->IsLoop())
		{
			TArray<AAICoverEntryPoint*> CoverEntryPoints = Cover->GetIaEntryPoints();

			for (AAICoverEntryPoint* EntryPoint : CoverEntryPoints)
			{
				if (!IsValid(EntryPoint)) // I don't why a point could be nullptr HERE but it happened.
				{
					continue;
				}
				
				EntryPoint->EnsureOwningSplineIsSet();
				
				if (EntryPoint->GetSection() == 0 || EntryPoint->GetSection() == 1)
				{
					BufferArray.Add(EntryPoint);
				}
			}
		}
	}
}

#pragma endregion

#pragma region Private Functions

void ACoverParent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HealthWidget) && IsValid(HealthWidgetBP))
	{
		HealthWidget->SetWidgetClass(HealthWidgetBP);

		UNumericProgressBar* CoverHealthEditorWidget = Cast<UNumericProgressBar>(HealthWidget->GetWidget());

		if (IsValid(CoverHealthEditorWidget))
		{
			CoverHealthEditorWidget->SetMaximumValue(static_cast<float>(HealthComponent->GetMaxHealth()));
			CoverHealthEditorWidget->UpdateProgressBar(static_cast<float>(HealthComponent->GetHealth()));
		}
	}
}

void ACoverParent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsValid(HealthWidgetRoot))
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
		if (IsValid(PlayerPawn))
		{
			HealthWidgetRoot->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
			HealthWidgetRoot->GetComponentLocation(),
				PlayerPawn->GetActorLocation()));
		}
	}
}

void ACoverParent::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!IsValid(this))
	{
		return;
	}

	CleanUnusedAttachedIaEntryPoints(); // This also Ensures that our Entry Points have the OwningSpline value set.

	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	// Checking if we need to reset or update our covers (Resets & Updates must be made in separate loops so that all resets have ocurred before any updates).
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr && Cover->HasBeenCreated())
		{
			if (bNeedsOnConstructReset || Cover->NeedsOnConstructReset())
			{
				Cover->ResetCoverSpline();
			}
		}
	}
	
	for (UCoverSpline* Cover : Components)
	{
		if (Cover != nullptr && Cover->HasBeenCreated())
		{
			if (bNeedsOnConstructUpdate || Cover->NeedsOnConstructUpdate())
			{
				Cover->UpdateCoverSpline();
			}
		}
	}

	// If we needed to Update as a whole Cover Parent, we already did.
	bNeedsOnConstructReset = false;
	bNeedsOnConstructUpdate = false;
	
	// Updating the Health Widget Visibility based on whether it can be destroyed & Updating its MaxHealth.
	if(IsValid(HealthWidget))
	{
		if (bCanBeDestroyed)
		{
			HealthWidget->SetVisibility(true);
		}
		else
		{
			HealthWidget->SetVisibility(false);
		}
	}

	// Updating the Text in our Zone Text Render
	if (IsValid(ZoneTextRender))
	{
		ZoneTextRender->Text = FText::FromString(FString::FromInt(NumZone));
	}
}

void ACoverParent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogTemp, Warning, TEXT("EndPlay - Cover Parent %s with reason %d"), *GetName(), EndPlayReason);	
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (IsValid(Cover))
		{
			Cover->DestroyCover();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ACoverParent::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed - Cover Parent %s"), *GetName());	
	TArray<UCoverSpline*> Components;
	GetComponents(UCoverSpline::StaticClass(), Components);
	
	for (UCoverSpline* Cover : Components)
	{
		if (IsValid(Cover))
		{
			Cover->DestroyCover();
		}
	}
	
	Super::Destroyed();
}

#pragma endregion

#pragma region Health Interface

void ACoverParent::SubtractHealth(const float HealthToSubstract, EWeaponType DamageType)
{
	if (!IsValid(HealthComponent))
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Substract from Actor's Health but could not find its HealthComponent.");
#endif
		return;	
	}
	
	if (bCanBeDestroyed)
	{
		HealthComponent->SubtractHealth(HealthToSubstract);
	}
	
	if(IsValid(HealthWidget))
	{
		UNumericProgressBar* CastedHealthWidget = Cast<UNumericProgressBar>(HealthWidget->GetWidget());
		
		if (IsValid(CastedHealthWidget))
		{
			CastedHealthWidget->UpdateProgressBar(HealthComponent->GetHealth());
		}
	}

	if (bCanBeDestroyed && HealthComponent->GetHealth() == 0)
	{
		DestroyCover();
	}
}

void ACoverParent::SetHealth(const float NewHealth)
{
	if (!IsValid(HealthComponent))
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Set Actor's Health but could not find its HealthComponent.");
#endif
		return;	
	}
	
	HealthComponent->SetHealth(NewHealth);
	
	if(IsValid(HealthWidget))
	{
		UNumericProgressBar* CastedHealthWidget = Cast<UNumericProgressBar>(HealthWidget->GetWidget());
		
		if (IsValid(CastedHealthWidget))
		{
			CastedHealthWidget->UpdateProgressBar(HealthComponent->GetHealth());
		}
	}
	
	if (bCanBeDestroyed && HealthComponent->GetHealth() == 0)
	{
		DestroyCover();
	}
}

#pragma endregion
