#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "EmissiveBounceCard.generated.h"

UCLASS()
class LADYUMBRELLA_API AEmissiveBounceCard : public AStaticMeshActor
{
	GENERATED_BODY()

	// Constants
	const FString EMISSIVE_BOUNCE_CARD_MATERIAL_OBJECT_PATH = TEXT("/Game/Materials/RegularMaterials/M_EmissiveBounceCards.M_EmissiveBounceCards");
	const FString ENGINE_SPHERE_OBJECT_PATH = TEXT("/Engine/BasicShapes/Sphere.Sphere");
	
public:	
	// Sets default values for this actor's properties
	AEmissiveBounceCard();
};
