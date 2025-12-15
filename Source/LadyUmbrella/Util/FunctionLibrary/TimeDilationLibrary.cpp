// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeDilationLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Components/TimeDilationComponent/TimeDilationComponent.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/LoggerUtil.h"


void UTimeDilationLibrary::ApplyTimeDilationCurveToWorld(APlayerController* PlayerController, const UCurveFloat* TimeDilationCurve)
{
	AMainController* MainController = Cast<AMainController>(PlayerController);
	if (!IsValid(MainController) || !IsValid(TimeDilationCurve))
	{
		return;
	}

	UTimeDilationComponent* TimeDilationComponent = MainController->GetTimeDilationComponent();
	if (IsValid(TimeDilationComponent))
	{
		TimeDilationComponent->ActivateTimeDilationCurve(TimeDilationCurve);
	}
}
