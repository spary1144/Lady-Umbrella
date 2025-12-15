#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LadyUmbrella/Characters/AnimationNotifies/SoundNotifies/ANSSoundParent.h"
#include "ANotifyGameplaySoundWithParameter.generated.h"

class UFModComponentInterface;

UCLASS()
class UANotifyGameplaySoundWithParameter : public UANSSoundParent
{
	GENERATED_BODY()

	const float LineRange = -300.f;
		
public:
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
