// 
// EQC_QuerierActor.h
// 
// ProvideContext Implementation -> get AI pawn as context in EQS
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "EQC_QuerierActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

void UEQC_QuerierActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	UObject* Querier = QueryInstance.Owner.Get();
	if (!IsValid(Querier))
	{
		return;
	}
	AActor* QuerierActor = Cast<AActor>(Querier);
	if (!IsValid(QuerierActor))
	{
		return;
	}

	AAIController* AIController = Cast<AAIController>(QuerierActor->GetInstigatorController());
	if (!IsValid(AIController))
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
	{
		return;
	}

	UObject* BlackboardValue = BlackboardComp->GetValueAsObject(BlackboardKeyName);
	const AAICoverEntryPoint* TargetActor = Cast<AAICoverEntryPoint>(BlackboardValue);

	
	if (IsValid(TargetActor))
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor->GetOwner());
	}
}
