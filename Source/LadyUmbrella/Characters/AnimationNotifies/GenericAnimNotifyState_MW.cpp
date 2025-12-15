// Fill out your copyright notice in the Description page of Project Settings.

#include "GenericAnimNotifyState_MW.h"

#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"

void UGenericAnimNotifyState_MW::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (AGenericCharacter* GenericCharacter = Cast<AGenericCharacter>(Owner))
		{
			GenericCharacter->GetInteractiveMovementComponent()->OnMotionWarpStarted(NotifyName);
		}
	}
}

void UGenericAnimNotifyState_MW::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (AGenericCharacter* GenericCharacter = Cast<AGenericCharacter>(Owner))
		{
			GenericCharacter->GetInteractiveMovementComponent()->OnMotionWarpFinished(NotifyName);
		}
	}
}