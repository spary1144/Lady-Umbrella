#include "ANotify_CanAttackMelee.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/MeleeComponent/MeleeComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Interfaces/GetWeaponInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

void UANotify_CanAttackMelee::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	// Check if the owner implements the GetWeaponInterface
	if (Owner->Implements<UGetWeaponInterface>())
	{
		AUmbrella* Umbrella = Cast<IGetWeaponInterface>(Owner)->GetWeapon();
		if (IsValid(Umbrella) && IsValid(Umbrella->GetMeleeComponent()))
		{
			Umbrella->GetMeleeComponent()->SetIsAttacking(false);
			Umbrella->GetMeleeComponent()->SetCanDetect(true);
			FGlobalPointers::PlayerCharacter->ResetPlayerStateToFreeMovementAndCheckAim();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner does not implement GetWeaponInterface"));
	}
}
