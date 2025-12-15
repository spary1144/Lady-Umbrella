
#pragma once

#include "CoreMinimal.h"
#include "GrapplingObject.h"
#include "GrapplingCustom.generated.h"

/**
 * Grappling Custom: Element of the world that allows to customize a Hookable Element
 * with custom params.
 */

UCLASS()
class LADYUMBRELLA_API AGrapplingCustom : public AGrapplingObject
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", DisplayName = "PITCH SPEED",  meta = (AllowPrivateAccess = "true"))
	float SpeedOfRotationPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", DisplayName = "YAW SPEED", meta = (AllowPrivateAccess = "true"))
	float SpeedOfRotationYaw;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", DisplayName = "ROLL SPEED",meta = (AllowPrivateAccess = "true"))
	float SpeedOfRotationRoll;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", meta = (AllowPrivateAccess = "true"))
	float DegreesOfRotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", DisplayName = "ORDER: ROLL - PITCH - YAW",  meta = (AllowPrivateAccess = "true"))
	FRotator RotationAxis;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", meta = (AllowPrivateAccess = "true"))
	bool InterpolateRotation;
	
public:
	
	AGrapplingCustom();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override;
	virtual GrabType GetGrabType() override;

	UFUNCTION()
	void RotateElement();

	FTimerHandle RotateTimer;
	
};


