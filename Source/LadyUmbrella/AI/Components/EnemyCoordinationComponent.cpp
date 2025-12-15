// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCoordinationComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

// Sets default values for this component's properties
UEnemyCoordinationComponent::UEnemyCoordinationComponent()
	: TokenUpdateTime(2.f)
	, NumberOfTokens(2)
	, NumberOfGrenadeTokens(1)
	, TimeInCoverUntilThrowGrenade(2.f)
	, ThrowGrenadeRate(5.f)
	, PlayerPositionUpdateTime(5.f)
	, MinDistanceToAvoidGrenade(200)
	, LastPlayerPosition(FVector::ZeroVector)
	, NumberOfElectricBulletTokens(1)
	, NumberOfFlankTokens(1.f)
	, MaxTimeWithoutToken(0)
	, WeightedDistance(0.8f)
	, WeightedAngle(0.9f)
	, WeightedTimeWithoutToken(0.01)
	, TimeToDropElectricToken(4.f)
    , NumPlayerKillsToReduceToken(2)
	, CurrentPlayerKills(0)
	, bInitialized(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UEnemyCoordinationComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (IsValid(World) && IsValid(World->GetFirstPlayerController())) 
	{
		CamManager = World->GetFirstPlayerController()->PlayerCameraManager;
	}
}

void UEnemyCoordinationComponent::StartCombat()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(ArenaManager))
	{
		FLogger::DebugLog("[CoordinationComponent]: Null");
		return;
	}
	
	if (IsValid(FGlobalPointers::PlayerCharacter->GetCoverMovementComponent()) && !ArenaManager->GetIsAgencyArena())
	{
		if (!FGlobalPointers::PlayerCharacter->GetCoverMovementComponent()->OnIsCoveredChanged.IsAlreadyBound(this, &UEnemyCoordinationComponent::CheckThrowGrenade))
		{
			FGlobalPointers::PlayerCharacter->GetCoverMovementComponent()->OnIsCoveredChanged.AddDynamic(this, &UEnemyCoordinationComponent::CheckThrowGrenade);
		}
	
		GrenadeTokenPool = NumberOfGrenadeTokens;

		AMafiaCharacter::OnGrenadeTokenReleased.AddDynamic(this, &UEnemyCoordinationComponent::AddGrenadeTokenToPool);
		if (IsValid(World))
		{
			World->GetTimerManager().SetTimer(CheckPlayerPositionTimerHandler, this, &UEnemyCoordinationComponent::CheckPlayerPosition, PlayerPositionUpdateTime, true, 0);
		}
	}

	if (ArenaManager->GetIsAgencyArena())
	{
		ElectricBulletTokenPool = NumberOfElectricBulletTokens;
		FlankTokenPool = NumberOfFlankTokens;
		AAgencyCharacter::OnElectricBulletTokenRequest.AddDynamic(this, &UEnemyCoordinationComponent::ChangeElectricBulletTokenPool);
	}
	if (IsValid(World))
	{
		World->GetTimerManager().SetTimer(CalculateTokensTimerHandler, this, &UEnemyCoordinationComponent::CalculateTokenUsers, TokenUpdateTime, true, 0);
	}
}

void UEnemyCoordinationComponent::Initialize()
{
	bInitialized = true;
	
	if (!GetOwner())
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: OWNER NULL");
	}
	
	ArenaManager = Cast<AArenaManager>(GetOwner());
	if (!IsValid(ArenaManager))
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: ARENA MANAGER NULL");
		return;
	}
	
	if (!IsValid(World))
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::Initialize]: WORLD NULL");
		return;
	}
}

void UEnemyCoordinationComponent::ClearTimers()
{
	if (!IsValid(World))
	{
		FLogger::ErrorLog("[EnemyCoordinationComponent::ClearTimer]: World NULL");
		return;
	}
	if (CalculateTokensTimerHandler.IsValid())
	{
		World->GetTimerManager().ClearTimer(CalculateTokensTimerHandler);
	}
	if (CheckPlayerPositionTimerHandler.IsValid())
	{
		World->GetTimerManager().ClearTimer(CheckPlayerPositionTimerHandler);
	}
	
	if (CalculateGrenadeTokensTimerHandler.IsValid())
	{
		World->GetTimerManager().ClearTimer(CalculateGrenadeTokensTimerHandler);
	}
}

void UEnemyCoordinationComponent::StopAndCleanup()
{
	// TODO Move this to Cover System
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			continue;
		}
		if (It.Key()->GetCoverMovementComponent()->IsMovementControlledByCover())
		{
			It.Key()->GetCoverMovementComponent()->NotifyExitCover();
		}
	}
	TokenScoreMap.Empty();
	//EnemyPool.Empty();
	MaxTimeWithoutToken = 0;
	LastPlayerPosition = FVector::ZeroVector;

	//Clear Timers
	ClearTimers();
	
	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		if (IsValid(FGlobalPointers::PlayerCharacter->GetCoverMovementComponent()))
		{
			FGlobalPointers::PlayerCharacter->GetCoverMovementComponent()->OnIsCoveredChanged.RemoveDynamic(this, &UEnemyCoordinationComponent::CheckThrowGrenade);
		}
	}

	AMafiaCharacter::OnGrenadeTokenReleased.RemoveDynamic(this, &UEnemyCoordinationComponent::AddGrenadeTokenToPool);
	AAgencyCharacter::OnElectricBulletTokenRequest.RemoveDynamic(this, &UEnemyCoordinationComponent::ChangeElectricBulletTokenPool);
}

void UEnemyCoordinationComponent::SetPoolReference(const TArray<FEnemyPoolEntry>& EnemyPool)
{
	for (FEnemyPoolEntry Enemy : EnemyPool)
	{
		FTokenScore AuxScore;
		TokenScoreMap.Add(Enemy.Enemy, AuxScore);
	}
}

void UEnemyCoordinationComponent::HandleAgencyCharacterDeath(AEnemyCharacter* DeadCharacter)
{
	AAgencyCharacter* Enemy = Cast<AAgencyCharacter>(DeadCharacter);
	if (IsValid(Enemy))
	{
		// if (Enemy->GetHasFlankToken())
		// {
		// 	ArenaManager->RemoveSpecialistEnemies();
		// }
		if (Enemy->GetHasElectricBullet())
		{
			ChangeElectricBulletTokenPool(false, Enemy);
		}
	}
}

void UEnemyCoordinationComponent::RemoveEnemy(AEnemyCharacter* DeadCharacter)
{
	if (!IsValid(DeadCharacter) && !IsValid(ArenaManager))
	{
		return;
	}
	if (ArenaManager->GetIsAgencyArena())
	{
		HandleAgencyCharacterDeath(DeadCharacter);
	}
	else
	{
		AMafiaCharacter* MafiaCharacter = Cast<AMafiaCharacter>(DeadCharacter);
		if (IsValid(MafiaCharacter) && MafiaCharacter->GetHasGrenadeToken())
		{
			AddGrenadeTokenToPool();
		}
	}
	TokenScoreMap.Remove(DeadCharacter);
}

void UEnemyCoordinationComponent::AddEnemy(FEnemyPoolEntry Enemy)
{
	FTokenScore AuxScore;
	TokenScoreMap.FindOrAdd(Enemy.Enemy, AuxScore);
}

void UEnemyCoordinationComponent::TryReduceTokens()
{
	//reduce difficulty one time
	if (NumberOfTokens <= 1)
	{
		return;
	}
	
	CurrentPlayerKills++;
	if (CurrentPlayerKills == NumPlayerKillsToReduceToken) 
	{
		NumberOfTokens--;
	}
}

void UEnemyCoordinationComponent::CalculateTokenUsers()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("[UEnemyCoordinationComponent::CalculateTokenUsers]: FGlobalPointers::PlayerCharacter NULL");
		return;
	}
	float MaxDistance = -1000000000;
	TArray<float> DistanceArray;

	int i = 0;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			continue;	
		}
		DistanceArray.Add(FVector::Dist(FGlobalPointers::PlayerCharacter->GetActorLocation(), It.Key()->GetActorLocation()));
		if (MaxDistance < DistanceArray[i])
		{
			MaxDistance = DistanceArray[i];
		}
		i++;
	}

	i = 0;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			continue;
		}
		// Distance
		TokenScoreMap[It.Key()].TokenScore = (1 - (DistanceArray[i] / MaxDistance)) * WeightedDistance;
		TokenScoreMap[It.Key()].Distance = DistanceArray[i];
		
		// Angle From Player
		TokenScoreMap[It.Key()].TokenScore += (1 - (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CamManager->GetActorForwardVector().GetSafeNormal(), (It.Key()->GetActorLocation() - FGlobalPointers::PlayerCharacter->GetActorLocation()).GetSafeNormal()))) / 180)) * WeightedAngle;
	
		// Time Without Token
		if (MaxTimeWithoutToken != 0)
		{
			TokenScoreMap[It.Key()].TokenScore += (TokenScoreMap[It.Key()].TimeWithoutToken / MaxTimeWithoutToken) * WeightedTimeWithoutToken;
		}

		TokenScoreMap[It.Key()].TokenScore /= CALCULATE_TOKENS_NUMBER_OF_PARAMS;
		i++;
	}

	TokenScoreMap.ValueStableSort([](FTokenScore A, FTokenScore B) {
		return A.TokenScore > B.TokenScore; // sort keys in reverse
		});


	i = 0;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			continue;	
		}
		if (i < NumberOfTokens)
		{
			It.Key()->SetHasToken(true);
			TokenScoreMap[It.Key()].TimeWithoutToken = 0;
		}
		else
		{
			It.Key()->SetHasToken(false);
			TokenScoreMap[It.Key()].TimeWithoutToken += TokenUpdateTime;

			if (TokenScoreMap[It.Key()].TimeWithoutToken > MaxTimeWithoutToken)
			{
				MaxTimeWithoutToken = TokenScoreMap[It.Key()].TimeWithoutToken;
			}
		}
		i++;
	}
}

void UEnemyCoordinationComponent::CalculateGrenadeTokenUsers()
{
	int32 TokenScoreSize = TokenScoreMap.Num();
	if (!NumberOfGrenadeTokens || TokenScoreSize == 0 || GrenadeTokenPool <= 0)
	{
		return;
	}
	
	AMafiaCharacter* GrenadeMafiaCharacter;
	for (auto It = TokenScoreMap.CreateConstIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			continue;
		}
		GrenadeMafiaCharacter = Cast<AMafiaCharacter>(It.Key());

		if (!IsValid(GrenadeMafiaCharacter))
		{
			continue;
		}
		
		if (It.Value().Distance < GrenadeMafiaCharacter->GetMinThrowGrenadeDistance() || It.Value().Distance > GrenadeMafiaCharacter->GetMaxThrowGrenadeDistance())
		{
			continue;
		}
		if (GrenadeTokenPool <= 0)
		{
			break;
		}
		

		if (IsValid(GrenadeMafiaCharacter))
		{
			GrenadeMafiaCharacter->SetHasGrenadeToken(true);
			GrenadeTokenPool--;
		}
	}
}

void UEnemyCoordinationComponent::CalculateFlankTokenUsers()
{
	
	if (TokenScoreMap.Num() < MIN_ENEMIES_ALIVE_TO_GET_FLANK_TOKEN)
	{
		return;
	}
	
	ArenaManager->AddSpecialistEnemies();

	TArray<AEnemyCharacter*> Keys;
	TokenScoreMap.GetKeys(Keys);
	AAgencyCharacter* AgencyCharacter = nullptr;
	if (Keys.IsValidIndex(TokenScoreMap.Num() - 1))
	{
		AgencyCharacter = Cast<AAgencyCharacter>(Keys[TokenScoreMap.Num() - 1]);
	}
	
	if (IsValid(AgencyCharacter))
	{
		ChangeFlankTokenPool(true, AgencyCharacter);
	}
}

void UEnemyCoordinationComponent::CheckPlayerPosition()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return;
	}

	if (FGlobalPointers::PlayerCharacter->GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		return;
	}
	
	const float DistSinceLastCheck = FVector::Dist(FGlobalPointers::PlayerCharacter->GetActorLocation(), LastPlayerPosition);
	if (DistSinceLastCheck < MinDistanceToAvoidGrenade)
	{
		CheckThrowGrenade(true);
	}
	else
	{
		if (CalculateGrenadeTokensTimerHandler.IsValid() && IsValid(World))
		{
			World->GetTimerManager().ClearTimer(CalculateGrenadeTokensTimerHandler);
		}
	}

	LastPlayerPosition = FGlobalPointers::PlayerCharacter->GetActorLocation();
}

void UEnemyCoordinationComponent::CheckThrowGrenade(const bool bNewState)
{
	if (!IsValid(World))
	{
		return;
	}
	
	if (bNewState)
	{
		if (World->GetTimerManager().IsTimerActive(CalculateGrenadeTokensTimerHandler))
		{
			return;
		}
		World->GetTimerManager().SetTimer(CalculateGrenadeTokensTimerHandler, this, &UEnemyCoordinationComponent::CalculateGrenadeTokenUsers, ThrowGrenadeRate, true, TimeInCoverUntilThrowGrenade);
	}
	else
	{
		if (CalculateGrenadeTokensTimerHandler.IsValid())
		{
			World->GetTimerManager().ClearTimer(CalculateGrenadeTokensTimerHandler);
		}
	}
}

void UEnemyCoordinationComponent::AddGrenadeTokenToPool()
{
	if (!IsValid(World))
	{
		return;
	}
	FTimerHandle GrenadeCooldownTimerHandler;
	World->GetTimerManager().SetTimer(GrenadeCooldownTimerHandler, [this]() -> void
	{
		//if (IsValid(this))
			GrenadeTokenPool++;
	}, ThrowGrenadeRate, false);
}

void UEnemyCoordinationComponent::ChangeElectricBulletTokenPool(const bool bIsRequesting, AAgencyCharacter* AgencyCharacter)
{
	if (bIsRequesting)
	{
		if (ElectricBulletTokenPool > 0)
		{
			ElectricBulletTokenPool--;
			AgencyCharacter->SetHasElectricBullet(true);
		}
	}
	else
	{
		if (ElectricBulletTokenPool < NumberOfElectricBulletTokens && IsValid(World))
		{
			World->GetTimerManager().SetTimer(DropFlankTokenTimerHandler, [this]()
			{
				ElectricBulletTokenPool++;
			},TimeToDropElectricToken,false);
		}
		AgencyCharacter->SetHasElectricBullet(false);
	}
}

void UEnemyCoordinationComponent::ChangeFlankTokenPool(bool bIsRequesting, AAgencyCharacter* AgencyCharacter)
{
	AAIControllerRegularAgency* Controller = Cast<AAIControllerRegularAgency>(AgencyCharacter->GetController());
	if (!IsValid(Controller))
	{
		return;
	}
	UBlackboardComponent* BlackboardComponent = Controller->GetBlackboardComponent();
	if (!IsValid(BlackboardComponent))
	{
		return;
	}

	if (bIsRequesting && FlankTokenPool > 0)
	{
		FlankTokenPool--;
		AgencyCharacter->SetHasFlankToken(true);

		BlackboardComponent->SetValueAsBool("IsFlanker", true);
	}
	else
	{
		if (FlankTokenPool < NumberOfFlankTokens)
		{
			FlankTokenPool++;
			//CalculateFlankTokenUsers();
		}
		AgencyCharacter->SetHasFlankToken(false);
		
		BlackboardComponent->SetValueAsBool("IsFlanker", false);
	}
	
}



