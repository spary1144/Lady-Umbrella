#pragma once
 
#include "CoreMinimal.h"
#include "CameraInterpolation.h"
#include "CameraAttachment.h"
#include "FCameraState.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "FCameraState"))
struct FCameraState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFocusDistanceDynamic = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector2D DesiredScreenRatio = FVector2D(0.3f, 0.5f);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bScreenRatioMirrored = false;  
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Units="Centimeters"))
	float Distance = 400.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 0.01f, ClampMax = 1.f, UIMin = 0.01f, UIMax = 1.f))
	float WallHitThreshold   = 0.5f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 0.01f, ClampMax = 1.f, UIMin = 0.01f, UIMax = 1.f))
	float SideHitThreshold   = 0.5f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 0.01f, ClampMax = 1.f, UIMin = 0.01f, UIMax = 1.f))
	float BottomHitThreshold = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ECameraInterpolation InterpolationType = ECameraInterpolation::LINEAR;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ECameraAttachment AttachmentLocation = ECameraAttachment::CAPSULE_TOP;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 0.01f, ClampMax = 10.f, UIMin = 0.01f, UIMax = 10.f))
	float InterpolationSpeed = 1.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (EditCondition = "InterpolationType == ECameraInterpolation::EASE_I || InterpolationType == ECameraInterpolation::EASE_O || InterpolationType == ECameraInterpolation::EASE_IO", Delta = 1.f))
	float EaseExponent = 2.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (EditCondition = "InterpolationType == ECameraInterpolation::CUSTOM"))
	FVector4 BezierPoints = FVector4(0.5f, 0.f, 0.5f, 1.f); // References: https://www.easing.dev/create/cubic-bezier | https://joshcollinsworth.com/blog/easing-curves | https://blog.maximeheckel.com/posts/cubic-bezier-from-math-to-motion/
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bAfterCollisionDelay = false;  
	
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (EditCondition = "bAfterCollisionDelay", Units = "Seconds"))
	float DelaySeconds = 0.5f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bCameraLag = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bUseFoVForLOD = true;
};

