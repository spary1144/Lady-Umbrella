#include "ANSSoundParent.h"

#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"

void UANSSoundParent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
							   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (
				!IsValid(MeshComp)
			||	!IsValid(Animation)
			||	!IsValid(MeshComp->GetOwner())
			||	!MeshComp->GetOwner()->Implements<UPlaySoundInterface>()
		)
	{
		return;
	}
}
