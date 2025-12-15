#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ANotifyFootSteps.generated.h"

class UFModComponentInterface;

UCLASS()
class LADYUMBRELLA_API UANotifyFootSteps : public UAnimNotify
{
	GENERATED_BODY()
		
	const float LineRange = -200.f;
	const float PlayerHeightOffset = 10.f;
	const FName EventParameterName = "SurfaceStepped";

public:
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
