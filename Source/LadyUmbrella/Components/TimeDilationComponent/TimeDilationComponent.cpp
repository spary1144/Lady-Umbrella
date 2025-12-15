#include "TimeDilationComponent.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UTimeDilationComponent::UTimeDilationComponent()
{
	PreviousRealTime = 0.f;
	CurrentTimeDilation = 1.f;
	CurrentTimeDilationCurveTime = INVALID_CURVE_TIME;
	PrimaryComponentTick.bCanEverTick = true;
}


void UTimeDilationComponent::ActivateTimeDilationCurve(const UCurveFloat* TimeDilationCurve)
{
	CurrentTimeDilationCurve = TimeDilationCurve;
	CurrentTimeDilationCurveTime = 0.f;
	CurrentTimeDilation = CurrentTimeDilationCurve->FloatCurve.Eval(CurrentTimeDilationCurveTime);

	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UGameplayStatics::SetGlobalTimeDilation(World, CurrentTimeDilation);
	}
}

void UTimeDilationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const UWorld* World = GetWorld();
	// If we are not currently using our TimeDilationCurve or if the World is invalid.
	if (!CurrentTimeDilationCurve || !IsValid(World))
	{
		return;
	}

	const float CurrentRealTime = UGameplayStatics::GetRealTimeSeconds(World);
	const float RealDeltaTime = CurrentRealTime - PreviousRealTime;
	PreviousRealTime = CurrentRealTime;
	
	CurrentTimeDilationCurveTime += RealDeltaTime;
	
	CurrentTimeDilation = CurrentTimeDilationCurve->FloatCurve.Eval(CurrentTimeDilationCurveTime);
	UGameplayStatics::SetGlobalTimeDilation(World, CurrentTimeDilation);

	// If we reached the end of our curve.
	if (CurrentTimeDilationCurveTime >= CurrentTimeDilationCurve->FloatCurve.GetLastKey().Time)
	{
		// This disables the Time Dilation Curve.
		CurrentTimeDilationCurve = nullptr;
		CurrentTimeDilationCurveTime = INVALID_CURVE_TIME;
	}
}
