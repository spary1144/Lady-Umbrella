// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameFreezeLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

FTimerHandle UFrameFreezeLibrary::FrameFreezeTimerHandle;
FTimerHandle UFrameFreezeLibrary::CustomFrameFreezeTimerHandle;

void UFrameFreezeLibrary::FrameFreeze(UObject* WorldContextObject, const float FreezeDilation, const float Time, const bool bHasPriority)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	if (World->GetTimerManager().IsTimerActive(FrameFreezeTimerHandle))
	{
		if (bHasPriority)
		{
			World->GetTimerManager().ClearTimer(FrameFreezeTimerHandle);
		}
		else
		{
			return;
		}
	}
	UGameplayStatics::SetGlobalTimeDilation(World, FreezeDilation);
	
	World->GetTimerManager().SetTimer(FrameFreezeTimerHandle, [World]()
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
	}, FreezeDilation * Time, false);
}

void UFrameFreezeLibrary::CustomFrameFreeze(UObject* WorldContextObject, AActor* FreezedActor, const float FreezeDilation, const float Time)
{
	if (!IsValid(WorldContextObject) || !IsValid(FreezedActor))
	{
		return;
	}
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	if (World->GetTimerManager().TimerExists(CustomFrameFreezeTimerHandle))
	{
		FTimerHandle CustomFrameFreezeTimerHandle2;
		FreezedActor->CustomTimeDilation = FreezeDilation;
	
		World->GetTimerManager().SetTimer(CustomFrameFreezeTimerHandle2, [World, FreezedActor]()
		{
			FreezedActor->CustomTimeDilation = 1.f;
		}, Time, false);
		return;
	}
	
	FreezedActor->CustomTimeDilation = FreezeDilation;
	
	World->GetTimerManager().SetTimer(CustomFrameFreezeTimerHandle, [World, FreezedActor]()
	{
		FreezedActor->CustomTimeDilation = 1.f;
	}, Time, false);
}
