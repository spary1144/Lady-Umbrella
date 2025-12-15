#pragma once

#include "CoreMinimal.h"
#include "ShapeToVis.h"
#include "ShapeInformation.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "FShapeInformation"))
struct FShapeInformation
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EShapeToVis ShapeToShow;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVector Origin;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(EditCondition = "ShapeToShow==EShapeToVis::LINE", EditConditionHides = true))
	FVector End;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(EditCondition = "ShapeToShow==EShapeToVis::BOX", EditConditionHides = true, Units = "Centimeters"))
	FVector Size;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(EditCondition = "ShapeToShow==EShapeToVis::BOX", EditConditionHides = true, Units = "Degrees"))
	FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(EditCondition = "ShapeToShow==EShapeToVis::SPHERE", EditConditionHides = true, Units = "Centimeters"))
	float Radius;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FColor Color;
	
	FShapeInformation(EShapeToVis Shape, FVector From, FVector To, FColor Col = FColor::White)
	{
		ShapeToShow = Shape;
		Origin = From;
		End = To;
		Color = Col;

		// Unused but initialized
		Size = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		Radius = 1.f;
	}
	
	FShapeInformation(EShapeToVis Shape, FVector From, FVector Bounds, FRotator Rot, FColor Col = FColor::White)
	{
		ShapeToShow = Shape;
		Origin = From;
		Size = Bounds;
		Rotation = Rot;
		Color = Col;
		
		// Unused but initialized
		End = FVector::ZeroVector;
		Radius = 1.f;
	}

	FShapeInformation(EShapeToVis Shape, FVector From, float Rad, FColor Col = FColor::White)
	{
		ShapeToShow = Shape;
		Origin = From;
		Radius = Rad;
		Color = Col;
		
		// Unused but initialized
		End = FVector::ZeroVector;
		Size = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
	}

	FShapeInformation()
	{
		ShapeToShow = EShapeToVis::SPHERE;
		Origin = FVector::ZeroVector;
		End = FVector::ZeroVector;
		Size = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		Radius = 1.f;
		Color = FColor::White;
	}
};