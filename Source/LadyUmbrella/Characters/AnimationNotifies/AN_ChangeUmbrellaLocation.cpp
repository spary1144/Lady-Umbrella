// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_ChangeUmbrellaLocation.h"

#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

void UAN_ChangeUmbrellaLocation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		FGlobalPointers::PlayerCharacter->ChangeUmbrellaLocation(UmbrellaLocation, true, false);
	}
}
