#include "ANSGameplaySound.h"

#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"

void UANSGameplaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	IPlaySoundInterface* PlaySoundInterface = Cast<IPlaySoundInterface>(MeshComp->GetOwner());
	if (!PlaySoundInterface || FmodSoundEventToPlay.IsEmpty())
	{
		return;
	}

	PlaySoundInterface->PlayNotifiedActionSound(FmodSoundEventToPlay);
}
