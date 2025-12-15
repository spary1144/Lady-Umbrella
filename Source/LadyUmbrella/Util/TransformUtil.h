// 
// TransformUtils.h
// 
// Utility functions to quickly create an FTransform object. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

namespace TransformUtils
{
	inline FTransform TransformEmpty()
	{
		return FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
	}

	inline FTransform TransformLocation(const FVector& Location)
	{
		return FTransform(FRotator::ZeroRotator, Location, FVector::OneVector);
	}

	inline FTransform TransformLocation(const float X, const float Y, const float Z)
	{
		return TransformLocation(FVector(X, Y, Z));
	}
	
	inline FTransform TransformRotation(const FRotator& Rotator)
	{
		return FTransform(Rotator, FVector::ZeroVector, FVector::OneVector);
	}
	
	inline FTransform TransformRotation(const float X, const float Y, const float Z)
	{
		return TransformRotation(FRotator(X, Y, Z));
	}

	inline FTransform TransformScale(const FVector& Scale)
	{
		return FTransform(FRotator::ZeroRotator,  FVector::ZeroVector, Scale);
	}
	
	inline FTransform TransformScale(const float X, const float Y, const float Z)
	{
		return TransformScale(FVector(X, Y, Z));
	}
}
