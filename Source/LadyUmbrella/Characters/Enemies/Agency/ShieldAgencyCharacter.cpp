// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldAgencyCharacter.h"

#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Controllers/AIControllerShieldAgency.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Weapons/AgencyShield/AgencyShield.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FEnemyDataDriven.h"
#include "LadyUmbrella/Util/SaveSystem/FChapterNames.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"

AShieldAgencyCharacter::AShieldAgencyCharacter()
{
	SetWeaponType(EWeaponType::Shield);
	StunDurationWhenShieldBreaks = 2.f;
	SpeedAproachPlayer = 300.f;
	SpeedOnBeingAttacked = 100.f;
	SpeedOnFindPlayerCovered = 100.f;
	StunMontagePlayRate = 1.f;
	DataTableRowHandler.RowName = "BP_ShieldAgency";
}

void AShieldAgencyCharacter::HandleShieldDestroyed()
{
	//pause logic
	ShieldController = Cast<AAIControllerShieldAgency>(GetController());
	
	if (!IsValid(ShieldController))
	{
		return;
	}

	PauseAILogic(PauseLogicReason);

	//play stun animation
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	const UWorld* World = GetWorld();
	const FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(World);

	if (IsValid(World) &&
		CurrentLevelName.ToString().Equals(ChapterNames::ChapterList[4]) &&
		(Shield->GetHealth() <= 0))
	{
		Steam::UnlockAchievement(ESteamAchievement::SHIELD_BREAKER);
	}
	
	if (!IsValid(StunAnimMontage))
	{
		return;
	}
	
	if (!IsValid(AnimInstance) || AnimInstance->Montage_IsPlaying(StunAnimMontage) || AnimInstance->OnMontageEnded.IsBound())
	{
		return;
	}
	
	AnimInstance->Montage_Play(StunAnimMontage);
	AnimInstance->Montage_SetPlayRate(StunAnimMontage, StunMontagePlayRate);
	AnimInstance->OnMontageEnded.AddDynamic(this, &AShieldAgencyCharacter::OnStunMontageCompleted);
}

void AShieldAgencyCharacter::OnStunMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsValid(ShieldController))
	{
		return;
	}
	
	//remove delegate
	if (IsValid(Montage) && Montage == StunAnimMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AShieldAgencyCharacter::OnStunMontageCompleted);
	}
	//change animation to pistol
	SetWeaponType(EWeaponType::Pistol);

	//resume logic
	ShieldController->BrainComponent->ResumeLogic(ResumeLogicReason);

	//notify to controller
	ShieldController->NotifyShieldIsBroken();
}

AActor* AShieldAgencyCharacter::GetShield() const
{
	if (IsValid(Shield))
	{
		return Shield;
	}
	return nullptr;
}

void AShieldAgencyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Shield = GetWorld()->SpawnActor<AAgencyShield>(ShieldSubclass);
	if (IsValid(Shield))
	{
		if (USkeletalMeshComponent* Comp = FindComponentByClass<USkeletalMeshComponent>())
		{
			Shield->AttachToComponent(
				Comp,
				FAttachmentTransformRules(
					EAttachmentRule::KeepRelative,
					EAttachmentRule::KeepRelative,
					EAttachmentRule::KeepRelative,
					true),
				"EnemyShieldSocket"
				);
		}
		Shield->OnShieldDestroyedDelegate.BindUFunction(this,FName("HandleShieldDestroyed"));
		GetCapsuleComponent()->IgnoreActorWhenMoving(Shield, true);
	}	
}

void AShieldAgencyCharacter::LoadEnemyFromDataTable()
{
	const FEnemyDataDriven* Data = FDataTableHelpers::ValidateTableAndGetData<FEnemyDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	if (!Data)
	{
		FLogger::ErrorLog("AgencyCharacter Data not loaded from Table [DataTableRowHandler.DataTable == nullptr]");
		return;
	}
	
	SpeedAproachPlayer = Data->AproachPlayerSpeed;
	SpeedOnBeingAttacked = Data->OnBeingAttackedSpeed;
	SpeedOnFindPlayerCovered = Data->OnFindPlayerCoveredSpeed;
}