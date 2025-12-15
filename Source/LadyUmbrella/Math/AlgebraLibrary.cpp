// Fill out your copyright notice in the Description page of Project Settings.


#include "AlgebraLibrary.h"

float UAlgebraLibrary::GetXYDegreeAngleFromVectorToVector(const FVector& FromVector, const FVector& ToVector)
{
	if (FromVector.IsZero() || ToVector.IsZero())
	{
		return 0.0f;
	}

	const FVector2D NormalizedFromVector = FVector2D(FromVector.X, FromVector.Y).GetSafeNormal();
	const FVector2D NormalizedToVector = FVector2D(ToVector.X, ToVector.Y).GetSafeNormal();

	return FMath::RadiansToDegrees(FMath::Atan2(NormalizedToVector.X * NormalizedFromVector.Y - NormalizedToVector.Y * NormalizedFromVector.X, FVector2D::DotProduct(NormalizedToVector, NormalizedFromVector)));
}
