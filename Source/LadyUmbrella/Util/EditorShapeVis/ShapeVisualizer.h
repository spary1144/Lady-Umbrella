#pragma once
#include "CoreMinimal.h"
#include "ShapeInformation.h"
#include "ShapeVisualizer.generated.h"

UCLASS(Blueprintable, BlueprintType, hidecategories=(HLOD,Rendering,Collision,Physics,Object,LOD,Lighting,TextureStreaming,ComponentTick), meta = (BlueprintSpawnableComponent))
class LADYUMBRELLA_API UShapeVisualizer : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:
	// What shape to print
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FShapeInformation Shape;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowInGame = false; // Not Really showing ingame
	
	UShapeVisualizer(const FObjectInitializer& ObjectInitializer);
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	
	virtual bool ShouldRecreateProxyOnUpdateTransform() const override { return true; }
};
