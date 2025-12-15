// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChangeMovementSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/AI/Controllers/AIControllerShieldAgency.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/ShieldAgencyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h"

EBTNodeResult::Type UBTT_ChangeMovementSpeed::SetSpeedMafia() const
{
	float NewSpeed = 0.f;
	//FVector NewVelocity = FVector::ZeroVector;
	AMafiaCharacter* MafiaCharacter = Cast<AMafiaCharacter>(EnemyPawn);
	if (!IsValid(MafiaCharacter) || !MafiaCharacter->GetCharacterMovement())
	{
		return EBTNodeResult::Failed;
	}
	switch (Behaviour)
	{
	case EBehaviorStatus::Flee: NewSpeed = MafiaCharacter->GetSpeedOnFleeing();
		break;
	case EBehaviorStatus::Patrol: NewSpeed = MafiaCharacter->GetSpeedOnPatrolling();
		break;
	case EBehaviorStatus::EnterArena: NewSpeed = MafiaCharacter->GetSpeedOnEnterArena();
		break;
	case EBehaviorStatus::CloseCombat: NewSpeed = MafiaCharacter->GetSpeedOnCloseCombat();
		break;
	case EBehaviorStatus::MoveToCover: NewSpeed = MafiaCharacter->GetSpeedOnMoveToCover();
		break;
	default: NewSpeed = 0.f;
		break;
	}
	MafiaCharacter->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_ChangeMovementSpeed::SetSpeedRegularAgency() const
{
	float NewSpeed = 0.f;
	FVector NewVelocity = FVector::ZeroVector;
	AAgencyCharacter* AgencyCharacter = Cast<AAgencyCharacter>(EnemyPawn);
	if (!IsValid(AgencyCharacter) || !AgencyCharacter->GetCharacterMovement())
	{
		return EBTNodeResult::Failed;
	}
	switch (Behaviour)
	{
	case EBehaviorStatus::MoveBetweenCover: NewSpeed = AgencyCharacter->GetSpeedOnMoveBetweenCovers(); //NewVelocity = FVector(100.f,0.f,0.f);
		break;
	case EBehaviorStatus::MoveAroundCover: NewSpeed = AgencyCharacter->GetSpeedOnMoveAroundCover(); //NewVelocity = FVector(100.f,0.f,0.f);
		break;
	case EBehaviorStatus::CloseCombat: NewSpeed = AgencyCharacter->GetSpeedOnCloseCombat(); //NewVelocity = FVector(1.f,1.f,0.f);
		break;
	case EBehaviorStatus::FindPlayerCovered: NewSpeed = AgencyCharacter->GetSpeedOnFindPlayer(); //NewVelocity = FVector(100.f,0.f,0.f);
		break;
	default: NewSpeed = 0.f;
		break;
	}
	AgencyCharacter->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	//AgencyCharacter->GetCharacterMovement()->Velocity = NewVelocity;
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_ChangeMovementSpeed::SetSpeedShieldAgency() const
{
	float NewSpeed = 0.f;
	AShieldAgencyCharacter* ShieldAgencyCharacterCharacter = Cast<AShieldAgencyCharacter>(EnemyPawn);
	if (!IsValid(ShieldAgencyCharacterCharacter) || !ShieldAgencyCharacterCharacter->GetCharacterMovement())
	{
		return EBTNodeResult::Failed;
	}
	switch (Behaviour)
	{
	case EBehaviorStatus::AproachPlayer: NewSpeed = ShieldAgencyCharacterCharacter->GetSpeedOnAproachPlayer();
		break;
	case EBehaviorStatus::AttackedByPlayer: NewSpeed = ShieldAgencyCharacterCharacter->GetSpeedOnBeingAttacked();
		break;
	case EBehaviorStatus::FindPlayerCovered: NewSpeed = ShieldAgencyCharacterCharacter->GetSpeedOnFindPlayer();
		break;
	default: NewSpeed = 0.f;
		break;
	}
	ShieldAgencyCharacterCharacter->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_ChangeMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	EnemyPawn = Controller->GetPawn();
	if (!IsValid(EnemyPawn))
	{
		return EBTNodeResult::Failed;
	}

	//Agency Regular
	if (OwnerComp.GetAIOwner()->IsA(AAIControllerRegularAgency::StaticClass()))
	{
		return SetSpeedRegularAgency();
	}

	//agency shield
	if (OwnerComp.GetAIOwner()->IsA(AAIControllerShieldAgency::StaticClass()))
	{
		return SetSpeedShieldAgency();
	}
	
	//Mafia
	if (OwnerComp.GetAIOwner()->IsA(AAIControllerMafia::StaticClass()) || OwnerComp.GetAIOwner()->IsA(AAIControllerGeneric::StaticClass()))
	{
		return SetSpeedMafia();
	}
	return EBTNodeResult::Succeeded;
}
