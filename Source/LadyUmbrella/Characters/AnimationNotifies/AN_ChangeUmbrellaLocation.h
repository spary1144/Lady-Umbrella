// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ChangeUmbrellaLocation.generated.h"

enum class EUmbrellaLocation : uint8;

UCLASS()
class LADYUMBRELLA_API UAN_ChangeUmbrellaLocation : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Notify, meta = (AllowPrivateAccess = true))
	EUmbrellaLocation UmbrellaLocation;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;	
};
