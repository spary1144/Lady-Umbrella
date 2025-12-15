// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayVoiceLine.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UBTT_PlayVoiceLine::UBTT_PlayVoiceLine()
{
	IsPrioritary = false;
}

EBTNodeResult::Type UBTT_PlayVoiceLine::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}

	//trigger voice line
	UVoiceLineComponent* VoiceLineComponent = Character->GetVoiceLineComponent();
	if (!IsValid(VoiceLineComponent))
	{
		return EBTNodeResult::Failed;
	}

	VoiceLineComponent->QueueVoiceLine(FVoiceLineDTO
	{
		VoiceLineState, VoiceLineActor,Character,IsPrioritary
	});

	return EBTNodeResult::Succeeded;
}
