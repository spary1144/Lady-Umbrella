// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DrawDebugFlankingPath.h"

#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"

EBTNodeResult::Type UBTT_DrawDebugFlankingPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerRegularAgency* Controller = Cast<AAIControllerRegularAgency>(OwnerComp.GetAIOwner());
	if (!Controller || Controller->GetCoversSelected().Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	Covers = Controller->GetCoversSelected(); // Guarda una copia
	CurrentIndex = 0;
	World = Controller->GetWorld();
	BehaviorTreeComp = &OwnerComp;

	// Inicia el timer
	World->GetTimerManager().SetTimer(TimerHandle, this, &UBTT_DrawDebugFlankingPath::DrawNextCover, 1.0f, true);

	return EBTNodeResult::InProgress;
}

void UBTT_DrawDebugFlankingPath::DrawNextCover()
{
	if (!World || CurrentIndex >= Covers.Num())
	{
		// Termina la tarea cuando acabes
		World->GetTimerManager().ClearTimer(TimerHandle);
		FinishLatentTask(*BehaviorTreeComp, EBTNodeResult::Succeeded);
		return;
	}

	AAICoverEntryPoint* Cover = Covers[CurrentIndex];
	if (Cover)
	{
		if (CurrentIndex == 0)
		{
			DrawDebugSphere(World, Cover->GetActorLocation(), 30.f, 12, FColor::Blue, false, 2.0f);
		}
		else
		{
			DrawDebugSphere(World, Cover->GetActorLocation(), 20.f, 12, FColor::Red, false, 2.0f);
		}
	}

	CurrentIndex++;
}
