#include "ANSUmbrellaGadget.h"
#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"

void UANSUmbrellaGadget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	IPlaySoundInterface* PlaySoundInterface = Cast<IPlaySoundInterface>(MeshComp->GetOwner());
	if (!PlaySoundInterface || FmodSoundEventToPlay.IsEmpty())
	{
		return;
	}
	PlaySoundInterface->PlayUmbrellaGadgetSound(FmodSoundEventToPlay);
}
