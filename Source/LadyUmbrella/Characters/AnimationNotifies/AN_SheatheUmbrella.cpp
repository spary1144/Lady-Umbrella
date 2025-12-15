// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_SheatheUmbrella.h"

#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

void UAN_SheatheUmbrella::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		FGlobalPointers::PlayerCharacter->ChangeUmbrellaLocation(EUmbrellaLocation::Back, true, false);
		FGlobalPointers::PlayerCharacter->SetCurrentUmbrellaLocation(EUmbrellaLocation::Back);
		FGlobalPointers::Umbrella->TryToCloseShield();
	}
}
