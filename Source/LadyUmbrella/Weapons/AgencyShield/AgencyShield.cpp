// Fill out your copyright notice in the Description page of Project Settings.


#include "AgencyShield.h"

#include "Components/BoxComponent.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"


// Sets default values
AAgencyShield::AAgencyShield()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(RootComponent);
	
 	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(StaticMesh);
	BoxCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetCanRegenerate(false);
	HealthComponent->OnDeathDelegate.AddDynamic(this,&AAgencyShield::OnDestroy);

	DataTableRowHandler.DataTable = AssetUtils::FindObject<UDataTable>("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_AgencyShieldData.DT_LadyUmbrellaSystems_AgencyShieldData'");;
	if (DataTableRowHandler.DataTable)
	{
		DataTableRowHandler.RowName = "BP_AgencyShield";
		LoadShieldFromDataTable();
	}
}

// Called when the game starts or when spawned
void AAgencyShield::BeginPlay()
{
	Super::BeginPlay();
	LoadShieldFromDataTable(); 
}

void AAgencyShield::SubtractHealth(const float HealthToSubtract,enum EWeaponType DamageType)
{
	HealthComponent->SubtractHealth(HealthToSubtract);
}

void AAgencyShield::SetHealth(const float _NewHealth)
{
	HealthComponent->SetHealth(_NewHealth);
}

const float AAgencyShield::GetHealth() const
{
	return HealthComponent->GetHealth();
}

void AAgencyShield::ContactWithCable(FVector& SocketLocation)
{
	//Notify characters that has shield
	OnShieldDestroyedDelegate.ExecuteIfBound();
	
	FVector Direction = FVector::ZeroVector;
	if (GetAttachParentActor())
	{
		Direction = (SocketLocation - GetAttachParentActor()->GetActorLocation()) * 100.f;
	}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SimulatePhysics();
	StaticMesh->AddImpulse(Direction);
	
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "DeactivatePhysics");
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelay, Delegate, 3.f, false);
}

IHookInterface::GrabType AAgencyShield::GetGrabType()
{
	return PULL;
}

void AAgencyShield::SimulatePhysics()
{
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	StaticMesh->SetEnableGravity(true);
}

void AAgencyShield::OnDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed - Agency Shield %s"), *GetName());	
	//Notify characters that has shield
	OnShieldDestroyedDelegate.ExecuteIfBound();
	//Deattach from Agent
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//Simulate physics
	SimulatePhysics();
	//Timer to deactivate physics
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "DeactivatePhysics");
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelay, Delegate, 2.f, false);
}

void AAgencyShield::DeactivatePhysics()
{
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	StaticMesh->SetEnableGravity(false);
	TimerHandle_SpawnDelay.Invalidate();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelay,[this]
	{
		Destroy();
	},1.f,false);
}


void AAgencyShield::LoadShieldFromDataTable()
{
	const FEnemyShieldDataDriven* ShieldData = FDataTableHelpers::ValidateTableAndGetData<FEnemyShieldDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	
	if (!ShieldData)
	{
		FLogger::ErrorLog("Failed to load Data from Shield Table");
		return;
	}
	
	GetHealthComponent()->SetMaxHealth(ShieldData->Endurance);
	GetHealthComponent()->SetHealth(ShieldData->Endurance);
}