// 
// BTT_RunEQS.h
// 
// Task responsible of run EQS depends on AI controlled arquetype
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTT_RunEQS.generated.h"

//forward declaration
class UEnvQuery;
struct FEnvQueryRequest;
struct FEnvQueryResult;
class UBlackboardComponent;
//delegates
DECLARE_DELEGATE_OneParam(FQueryFinishedSignature, TSharedPtr<FEnvQueryResult>);

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_RunEQS : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTT_RunEQS(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// The function that gets called when querry finished
	//void MyQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	UPROPERTY(EditAnywhere,Category = EQS)
	//UEnvQuery* QueryMafia; // set the query in editor
	FEQSParametrizedQueryExecutionRequest QueryMafia;
	UPROPERTY(EditAnywhere,Category = EQS)
	FEQSParametrizedQueryExecutionRequest QueryAgency;
	//UEnvQuery* QueryAgency; // set the query in editor

	UPROPERTY(EditAnywhere, Category = Blackboard)
	struct FBlackboardKeySelector BlackboardKey;

	FQueryFinishedSignature QueryFinishedDelegate;
private:
	void MoveToQueryResult(TSharedPtr<FEnvQueryResult> result);
	//FEnvQueryRequest MyQueryRequest;
	 //Guardar el OwnerComp para terminar la tarea después
	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp;

	UPROPERTY()
	UBlackboardComponent* bb;
};
