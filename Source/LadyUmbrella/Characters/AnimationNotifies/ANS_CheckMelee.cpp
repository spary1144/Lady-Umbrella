#include "ANS_CheckMelee.h"
#include "LadyUmbrella/Components/MeleeComponent/MeleeComponent.h"
#include "LadyUmbrella/Interfaces/GetWeaponInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

void UANS_CheckMelee::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    if (!MeshComp)
    {
        return;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    // Check if the owner implements the GetWeaponInterface
    if (Owner->Implements<UGetWeaponInterface>())
    {
        AUmbrella* Umbrella = Cast<IGetWeaponInterface>(Owner)->GetWeapon();
        if (IsValid(Umbrella))
        {
            Umbrella->GetMeleeComponent()->ProcessCollision(Umbrella->GetActorLocation(),Umbrella->GetActorForwardVector());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner does not implement GetWeaponInterface"));
    }
}
