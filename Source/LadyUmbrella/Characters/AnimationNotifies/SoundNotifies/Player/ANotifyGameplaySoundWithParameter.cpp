#include "ANotifyGameplaySoundWithParameter.h"

#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"

void UANotifyGameplaySoundWithParameter::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp) || !IsValid(Animation))
	{
		return;
	}

	AActor* MeshCompOwner = MeshComp->GetOwner();
	if (!IsValid(MeshCompOwner))
	{
		return;
	}
	constexpr float ForwardOffsetForPlayerPosition = 50.f;
	const FVector OwnerForward  = MeshCompOwner->GetActorForwardVector();
	const FVector StartLocation	= MeshCompOwner->GetActorLocation() + OwnerForward * ForwardOffsetForPlayerPosition;
	const FVector EndLocation	= StartLocation + MeshCompOwner->GetActorUpVector() * LineRange;
	
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshCompOwner);
	Params.bReturnPhysicalMaterial = true;
	
	const UWorld* World = MeshCompOwner->GetWorld();
	const bool bHit = World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Visibility, Params);
	
	const UPhysicalMaterial* PhysicalMaterialHit = OutHit.PhysMaterial.Get();
	if (!bHit || !IsValid(PhysicalMaterialHit))
	{
		return;
	}
	/**
 * Types of Surface:
 * SurfaceType1: Wood
 * SurfaceType2: Concrete
 * SurfaceType3: MetalThick
 * SurfaceType4: Tiles
 * SurfaceType5: MetalThin
 */

	int8 ValueForVaultSoundEvent;
	
	switch (PhysicalMaterialHit->SurfaceType)
	{
		case SurfaceType3:
			ValueForVaultSoundEvent = 1; //FLogger::DebugLog("Metalthick");
			break;
		case SurfaceType5:
			// change this value later to 5 when we have metal thin
			ValueForVaultSoundEvent = 1; //FLogger::DebugLog("Metalthin");
			break;
		default:
			ValueForVaultSoundEvent = 0;
			break;
	}
	
	if (!MeshCompOwner->Implements<UPlaySoundInterface>())
	{
		return;
	}
	
	IPlaySoundInterface* SoundInterface = Cast<IPlaySoundInterface>(MeshCompOwner);
	if (!SoundInterface)
	{
		return;
	}
	SoundInterface->PlayNotifiedActionSoundWithParameter(FmodSoundEventToPlay, ValueForVaultSoundEvent);
}
