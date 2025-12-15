// 
// EQC_Player.cpp
// 
// ProvideContext implementation
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "EQC_Player.h"
#include "Kismet/GameplayStatics.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Character.h"

void UEQC_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	//get player pawn
	APawn* playerpawn = Cast<APawn>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (playerpawn)
	{
		//Set the context playerPawn to the provided context data
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, playerpawn);
	}
}
