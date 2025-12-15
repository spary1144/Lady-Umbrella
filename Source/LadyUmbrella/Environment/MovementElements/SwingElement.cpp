#include "SwingElement.h"

#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"

ASwingElement::ASwingElement()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/StructuralGeometry/Beams/SM_PH_Beam_300.SM_PH_Beam_300'"));
	UStaticMesh* Asset = MeshAsset.Object;
	StaticMeshComponent->SetStaticMesh(Asset);
	StaticMeshComponent->AddLocalRotation(FRotator(0, 90, 0));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("thirdOne"));
	BoxComponent->SetupAttachment(StaticMeshComponent);
	BoxComponent->SetRelativeTransform(FTransform(FRotator(0, 0, 0), FVector(0, 0, 200), FVector(1.0, 1.0, 1.0)));
	BoxComponent->SetBoxExtent(FVector(30, 30, 150), false);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASwingElement::OnOverlap);

	ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("AnimMontage'/Game/Animations/Montages/AllCharacters/Movement/AM_Swing.AM_Swing'"));
	if (MontageAsset.Succeeded())
	{
		SwingMontage = MontageAsset.Object;
	}

	MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MovementTimeline"));
	ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("CurveFloat'/Game/DataStructures/Curves/Combat/C_MeleeRotation.C_MeleeRotation'"));
	if (CurveAsset.Succeeded())
	{
		MovementCurve = CurveAsset.Object;
	}
}

void ASwingElement::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		if (!PlayerCharacter->GetInteractiveMovementComponent()->IsSwinging())
		{
			PlayerCharacter->GetInteractiveMovementComponent()->SetIsSwinging(true);

			APlayerController* PP = GetWorld()->GetFirstPlayerController();
			if (PP)
			{
				PlayerCharacter->DisableInput(PP);
			}

			MovePlayerToClosestLocation(PlayerCharacter);

			PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

			TimelineFinishedEvent.BindUFunction(this, FName("OnTimelineFinished"));
			MovementTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent);
			if (MovementTimeline)
			{
				MovementTimeline->PlayFromStart();
			}
		}
	}
}

void ASwingElement::MovePlayerToClosestLocation(APlayerCharacter* PlayerCharacter)
{
	FTransform SwingTransform = GetActorTransform();
	FVector WorldLocation1 = SwingTransform.TransformPosition(Location1);
	FVector WorldLocation2 = SwingTransform.TransformPosition(Location2);

	OriginalLocation = PlayerCharacter->GetActorLocation();
	OriginalRotation = PlayerCharacter->GetActorRotation();
	float DistanceToLocation1 = FVector::Dist(PlayerCharacter->GetActorLocation(), WorldLocation1);
	float DistanceToLocation2 = FVector::Dist(PlayerCharacter->GetActorLocation(), WorldLocation2);
	TargetLocation = (DistanceToLocation1 < DistanceToLocation2) ? WorldLocation1 : WorldLocation2;
	TargetRotation = this->GetActorRotation() + FRotator(0, 0, 0);
	if (TargetLocation == WorldLocation2) {TargetRotation = this->GetActorRotation() + FRotator(0, 180.0f, 0);};
	if (MovementCurve)
	{
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this, FName("UpdateTimeline"));
		MovementTimeline->AddInterpFloat(MovementCurve, ProgressUpdate);

		MovementTimeline->PlayFromStart();
	}
	PlayMontage();
}

void ASwingElement::UpdateTimeline(float Alpha)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerCharacter)
	{
		FVector NewLocation = FMath::Lerp(OriginalLocation, TargetLocation, Alpha);
		FRotator NewRotation = FMath::Lerp(OriginalRotation,FRotator(OriginalRotation.Pitch, TargetRotation.Yaw, OriginalRotation.Roll), Alpha);
		PlayerCharacter->SetActorLocation(NewLocation);
		PlayerCharacter->SetActorRotation(NewRotation);
	}
}

void ASwingElement::PlayMontage()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerCharacter)
	{
		if (SwingMontage)
		{
			UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(SwingMontage, MontagePlayRate);

				FOnMontageEnded MontageEndedDelegate;
				MontageEndedDelegate.BindUObject(this, &ASwingElement::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, SwingMontage);
			}
		}
	}
}
void ASwingElement::OnTimelineFinished()
{
	// no need for now, used to call the montage here
}

void ASwingElement::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == SwingMontage)
	{
		APlayerController* PP = GetWorld()->GetFirstPlayerController();
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PP->GetPawn()))
		{
			PlayerCharacter->SetActorRotation(FRotator(OriginalRotation.Pitch, TargetRotation.Yaw, OriginalRotation.Roll));
			PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			PlayerCharacter->EnableInput(PP);
			PlayerCharacter->GetInteractiveMovementComponent()->SetIsSwinging(false);

			FVector direction = (TargetLocation - PlayerCharacter->GetActorLocation()).GetSafeNormal();

			FVector throwVelocity = -1.0f * direction * HorizontalThrowSpeed;
			throwVelocity.Z = VerticalThrowSpeed;

			UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
			if (MovementComponent)
			{
				MovementComponent->Launch(throwVelocity);
			}

			PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}
}
