#include "ANotifyFootSteps.h"

#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

void UANotifyFootSteps::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp) || !IsValid(Animation))
	{
		FLogger::ErrorLog("Failed in ANotifyFootSteps");
		FLogger::ErrorLog("MesComp: %i", IsValid(MeshComp));
		FLogger::ErrorLog("Animation: %i", IsValid(Animation));

		return;
	}

	AActor* MeshCompOwner = MeshComp->GetOwner();
	if (!IsValid(MeshCompOwner))
	{
		return;
	}

	const FVector& StartLocation = MeshCompOwner->GetActorLocation();
	const FVector EndLocation	 = StartLocation + MeshCompOwner->GetActorUpVector() * LineRange;

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshCompOwner);
	Params.bReturnPhysicalMaterial = true;
	
	const UWorld* World = MeshCompOwner->GetWorld();

	const bool bHit = World->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECC_Visibility, Params);

//	DrawDebugLine(World, StartLocation, EndLocation, FColor::Yellow, false, 10, 100);

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
	
	int8 ValueForFmodStepsEvent;
	switch (PhysicalMaterialHit->SurfaceType)
	{
		case SurfaceType1:
			ValueForFmodStepsEvent = 0; //FLogger::DebugLog("Wood");
			break;
		case SurfaceType2:
			ValueForFmodStepsEvent = 1; //FLogger::DebugLog("Concrete");
			break;
		case SurfaceType3:
			ValueForFmodStepsEvent = 3; //FLogger::DebugLog("Metalthick");
			break;
		case SurfaceType4:
			ValueForFmodStepsEvent = 2; //FLogger::DebugLog("Tiles");
			break;
		case SurfaceType5:
			// change this value later to 5 when we have metal thin
			ValueForFmodStepsEvent = 3; //FLogger::DebugLog("Metalthin");
			break;
		default:
			ValueForFmodStepsEvent = 1;  //FLogger::DebugLog("No Material");
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
	SoundInterface->PlayStepsSounds(ValueForFmodStepsEvent);
}