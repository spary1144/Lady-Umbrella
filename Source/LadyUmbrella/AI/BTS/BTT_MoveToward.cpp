//  
// BTT_MoveToward.cpp
// 
// Implementation of Task BTT_MoveToward: Action executed by behaviourTree
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#include "BTT_MoveToward.h"

#include <string>

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UBTT_MoveToward::UBTT_MoveToward()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_MoveToward::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    RequestResult = EPathFollowingRequestResult::Failed;

    MyController = Cast<AAIController>(OwnerComp.GetOwner());
    if (!MyController) return EBTNodeResult::Failed;
    
    Pawn = Cast<ACharacter>(MyController->GetPawn());
    if (!Pawn) return EBTNodeResult::Failed;
    
    Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard) return EBTNodeResult::Failed;
    
    PlayerCharacter = Cast<APlayerCharacter>(Blackboard->GetValueAsObject(PlayerReference.SelectedKeyName));
    if (!IsValid(PlayerCharacter))
    {
        return EBTNodeResult::Failed;
    }
    
    return EBTNodeResult::InProgress;
}

void UBTT_MoveToward::TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    
    if (!IsValid(PlayerCharacter))
    {
        return;
    }
    
    // if (PlayerCharacter->IsAiming())
    // {
    //     Pawn->GetCharacterMovement()->MaxWalkSpeed = 100.f;
    // }
    // else
    // {
    //     Pawn->GetCharacterMovement()->MaxWalkSpeed = 300.f;
    // }
    
    RequestResult = MyController->MoveToActor(PlayerCharacter, AcceptableRadius, true, true, bAllowStrafe, nullptr);
    
    if (RequestResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        //FLogger::DebugLog("He llegado");
        //FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
        //Blackboard->SetValueAsBool(TEXT(""))
        Blackboard->SetValueAsBool(FName("IsReadyToShootElectricProjectile"), true);
    }
    
    if (RequestResult == EPathFollowingRequestResult::Failed)
    {
        FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
    }
}

FString UBTT_MoveToward::GetStaticDescription() const
{
    return  "Moves the character toward the specified point\n" + BlackboardKey.SelectedKeyName.ToString();   
}
