// 
// BTT_SetBoolValue.cpp
// 
// Task executed by Behaviour Tree : SetBoolValue
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "BTT_SetBoolValue.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_SetBoolValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* character = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (character)
	{
		UBlackboardComponent* bb = character->GetBlackboardComponent();
		bb->SetValueAsBool(BoolValue.SelectedKeyName, value);
	}
	return EBTNodeResult::Succeeded;
}
