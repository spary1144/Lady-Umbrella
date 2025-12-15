// 
// SplinePath.h
// 
// Spline that represents path followed by AI
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "SplinePath.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ASplinePath : public AActor
{
	GENERATED_BODY()

public:
	ASplinePath();
	
	UPROPERTY(EditDefaultsOnly)
	class USplineComponent* Spline;
	
	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* Billboard;
	
	USplineComponent* GetSplineComponent() const{ return Spline; }
};
