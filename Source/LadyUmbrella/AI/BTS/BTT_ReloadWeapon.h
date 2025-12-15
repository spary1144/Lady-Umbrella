// 
// BTT_ReloadWeapon.h
// 
// BehaviourTree: Task that notify player to reload
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ReloadWeapon.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_ReloadWeapon : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
