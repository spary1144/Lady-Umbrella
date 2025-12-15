// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_Jump.h"

#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/GlobalUtil.h"


void UAN_Jump::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		FGlobalPointers::PlayerCharacter->Jump();
	}
}
