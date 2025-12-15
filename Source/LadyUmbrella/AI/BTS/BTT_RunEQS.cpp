// 
// BTT_RunEQS.cpp
// 
// Run EQS and handle the result
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "BTT_RunEQS.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include <BehaviorTree/Tasks/BTTask_RunEQSQuery.h>


UBTT_RunEQS::UBTT_RunEQS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	QueryFinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &UBTT_RunEQS::MoveToQueryResult);
}

void UBTT_RunEQS::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	QueryMafia.InitForOwnerAndBlackboard(*this, GetBlackboardAsset());
	QueryAgency.InitForOwnerAndBlackboard(*this, GetBlackboardAsset());
}


EBTNodeResult::Type UBTT_RunEQS::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//save owner ref
	MyOwnerComp = &OwnerComp;
	//get owner
	AActor* QueryOwner = OwnerComp.GetOwner();
	AAIController* MyController = Cast<AAIController>(OwnerComp.GetOwner());
	if (MyController)
	{
		QueryOwner = MyController->GetPawn();
	}
	//save bb
	bb = OwnerComp.GetBlackboardComponent();
	//get name arquetype
	FString name = QueryOwner->GetClass()->GetName();
	//choose eqs depends of arquetype
	FEnvQueryRequest MyQueryRequest;
	//FBTEnvQueryTaskMemory* MyMemory = CastInstanceNodeMemory<FBTEnvQueryTaskMemory>(NodeMemory);
	if (QueryOwner)
	{
		if (name == "BP_MafiaCharacter_C" && QueryMafia.IsValid())
		{
			//MyQueryRequest = FEnvQueryRequest(QueryMafia, MyController);
			QueryMafia.Execute(*QueryOwner, bb, QueryFinishedDelegate);
		}
		if (name == "BP_AgencyCharacter_C" && QueryAgency.IsValid())
		{
			//MyQueryRequest = FEnvQueryRequest(QueryAgency, MyController);
			QueryAgency.Execute(*QueryOwner, bb, QueryFinishedDelegate);
		}
	}
		//WaitForMessage(OwnerComp, UBrainComponent::AIMessage_QueryFinished, MyMemory->RequestID);
		//return EBTNodeResult::InProgress;
	//execute eqs
	MyQueryRequest.Execute(EEnvQueryRunMode::SingleResult, this,&UBTT_RunEQS::MoveToQueryResult);

	return EBTNodeResult::InProgress;
}
	

void UBTT_RunEQS::MoveToQueryResult(TSharedPtr<FEnvQueryResult> result)
{
	if (!MyOwnerComp || !bb)
	{
		return;
	}

	if (!result->IsSuccessful() || result->Items.Num() == 0)
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Failed);
		return;
	}
	AActor* BestCover = result->GetItemAsActor(0);
	if (!BestCover)
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Failed);
		return;
	}

	bb->SetValueAsObject(BlackboardKey.SelectedKeyName, BestCover);

	/*QueryMafia->AbortInsideMemberFunction();*/

	FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
}
