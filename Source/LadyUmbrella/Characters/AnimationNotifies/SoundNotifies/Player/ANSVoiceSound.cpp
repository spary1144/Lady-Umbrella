#include "ANSVoiceSound.h"

#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"

void UANSVoiceSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	IPlaySoundInterface* PlaySoundInterface = Cast<IPlaySoundInterface>(MeshComp->GetOwner());
	if (!PlaySoundInterface || FmodSoundEventToPlay.IsEmpty())
	{
		return;
	}

	PlaySoundInterface->PlayNotifiedVoiceSound(FmodSoundEventToPlay);
	
}
