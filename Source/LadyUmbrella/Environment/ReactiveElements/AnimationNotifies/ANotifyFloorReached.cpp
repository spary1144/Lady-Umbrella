#include "ANotifyFloorReached.h"

#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Util/GlobalUtil.h"

void UANotifyFloorReached::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		return;
	}
	FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::ENVIRONMENT);

}
