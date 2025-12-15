#include "NotifyEndMeleeAttack.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

void UNotifyEndMeleeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return;
	}
	FGlobalPointers::PlayerCharacter->ResetPlayerStateToFreeMovementAndCheckAim();
}
