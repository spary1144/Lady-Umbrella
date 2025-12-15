// 
// BTT_MoveToward.h
// 
// BehaviourTree: Task responsible for move character toward location provided by blackboard
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTT_MoveToward.generated.h"

class APlayerCharacter;
class AAIController;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_MoveToward : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	EPathFollowingRequestResult::Type RequestResult;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	ACharacter* Pawn;
	
	UPROPERTY()
	AAIController* MyController;

	UPROPERTY()
	UBlackboardComponent* Blackboard;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerReference;
	
    UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0"))
    float AcceptableRadius;

    UPROPERTY(Category = Node, EditAnywhere)
    uint32 bAllowStrafe : 1;
	
    UPROPERTY(Category = Node, EditAnywhere)
    TSubclassOf<class UNavigationQueryFilter> FilterClass;
	UBTT_MoveToward();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual FString GetStaticDescription() const override;
};
