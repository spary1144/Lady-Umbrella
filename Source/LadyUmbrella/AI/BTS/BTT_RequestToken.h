// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RequestToken.generated.h"

UENUM()
enum class ETokenType
{
	FLANK,
	ELECTRIC
};

UCLASS()
class LADYUMBRELLA_API UBTT_RequestToken : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere)
	ETokenType TokenType = ETokenType::ELECTRIC;

	UPROPERTY(EditAnywhere)
	bool bIsRequesting = true;
};
