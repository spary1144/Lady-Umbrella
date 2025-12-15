// Fill out your copyright notice in the Description page of Project Settings.


#include "ParabolicMovement.h"

FVector ParabolicMovement::CalculateParabolicLaunchVelocity(const FVector& Start, const FVector& End, const float Time, const float GravityZ)
{
	FVector Delta = End - Start;

	FVector Velocity;
	Velocity.X = Delta.X / Time;
	Velocity.Y = Delta.Y / Time;
	Velocity.Z = (Delta.Z - 0.5f * GravityZ * Time * Time) / Time;

	return Velocity;
}

void ParabolicMovement::DrawDebugParabola(UWorld* World, const FVector& Start, const FVector& InitialVelocity, const float GravityZ, const float TotalTime)
{
	const float TimeStep = 0.05f;
	FVector PreviousPosition = Start;

	for (float t = 0.0f; t <= TotalTime; t += TimeStep)
	{
		FVector Position = Start + InitialVelocity * t + FVector(0, 0, 0.5f * GravityZ * t * t);

		DrawDebugLine(World, PreviousPosition, Position, FColor::Green, false, 2.0f, 0, 2.0f);
		
		PreviousPosition = Position;
	}
}
