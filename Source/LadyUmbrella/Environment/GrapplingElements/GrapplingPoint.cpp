// Fill out your copyright notice in the Description page of Project Settings.

#include "GrapplingPoint.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"

// Sets default values
AGrapplingPoint::AGrapplingPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxComponent  = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
	CollisionBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrapplingPoint::OnBeginOverlap);
	CollisionBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGrapplingPoint::OnEndOverlap);
	
	SM_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SM_StaticMeshComponent->SetupAttachment(CollisionBoxComponent);
	CollisionBoxComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AGrapplingPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrapplingPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrapplingPoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

void AGrapplingPoint::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AGrapplingPoint::ContactWithCable(FVector& Direction)
{
	
}

IHookInterface::GrabType AGrapplingPoint::GetGrabType()
{
	return PUSH;
}