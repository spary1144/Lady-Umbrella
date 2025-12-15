#pragma once

#include "CoreMinimal.h"
#include "HookableParent.h"
#include "HookableInterpolation.generated.h"

UCLASS()
class LADYUMBRELLA_API AHookableInterpolation : public AHookableParent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Static Mesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Curve", meta = (DisplayName = "Acceleration Curve", AllowPrivateAccess = "true"))
	UCurveFloat* RotationCurve;

	const float RotateActorDynamicRate = 0.01f;

	float RotationElapsed;

	float RotationDuration;

	FQuat InitialQuat;
	
	FQuat TargetQuat;

	FQuat FinalRotation;

	FTimerHandle RotationTimer;

public:
	
	AHookableInterpolation();
	
	virtual void RotateFromChildDynamic(const float DegreesOfRotation, const float SpeedOfRotation, const FRotator& RotatorForParent) override;
	void RotateActor();
};


