#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LadyUmbrella/Characters/AnimationNotifies/SoundNotifies/ANSSoundParent.h"
#include "ANSGameplaySound.generated.h"

class UFModComponentInterface;

UCLASS()
class UANSGameplaySound : public UANSSoundParent
{
	GENERATED_BODY()

public:
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
