#include "EmissiveBounceCard.h"

#include "LadyUmbrella/Util/AssetUtil.h"

// Sets default values
AEmissiveBounceCard::AEmissiveBounceCard()
{
	PrimaryActorTick.bCanEverTick = false;

	if (IsValid(RootComponent))
	{
		RootComponent->SetMobility(EComponentMobility::Movable);
	}
	
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	TObjectPtr<UStaticMesh> SphereStaticMesh = AssetUtils::FindObject<UStaticMesh>(ENGINE_SPHERE_OBJECT_PATH);
	TObjectPtr<UMaterial> EmissiveBounceCardsMaterial = AssetUtils::FindObject<UMaterial>(EMISSIVE_BOUNCE_CARD_MATERIAL_OBJECT_PATH);
	
	GetStaticMeshComponent()->SetEmissiveLightSource(true);
	GetStaticMeshComponent()->SetAffectIndirectLightingWhileHidden(true);
	GetStaticMeshComponent()->SetCastShadow(false);
	GetStaticMeshComponent()->bVisibleInReflectionCaptures = false;
	GetStaticMeshComponent()->SetDefaultCustomPrimitiveDataVector4(0, FVector4::One());
	GetStaticMeshComponent()->SetDefaultCustomPrimitiveDataFloat(4, 1.f);

	if (SphereStaticMesh)
	{
		GetStaticMeshComponent()->SetStaticMesh(SphereStaticMesh);
	}
	if (EmissiveBounceCardsMaterial)
	{
		GetStaticMeshComponent()->SetMaterial(0, EmissiveBounceCardsMaterial);
	}
}