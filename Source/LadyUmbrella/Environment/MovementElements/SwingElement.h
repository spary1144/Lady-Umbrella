#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "LadyUmbrella/Environment/InteractiveElement.h"
#include "SwingElement.generated.h"

class APlayerCharacter;
class UBoxComponent;

UCLASS()
class LADYUMBRELLA_API ASwingElement : public AInteractiveElement
{
	GENERATED_BODY()

public:
	ASwingElement();
	UPROPERTY(EditAnywhere, Category = "Swing Element")
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(EditAnywhere, Category = "Swing Element")
	UBoxComponent* BoxComponent;
	UPROPERTY()
	FVector3f V_PosToUse;
	UPROPERTY()
	FVector OriginalLocation;
	UPROPERTY()
	FVector TargetLocation;
	
	UPROPERTY()
	FRotator OriginalRotation;
	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing")
	float HorizontalThrowSpeed = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing")
	float VerticalThrowSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element")
	UTimelineComponent* MovementTimeline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* MovementCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element")
	UAnimMontage* SwingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element")
	float MontagePlayRate = 1.0f; // Default play rate for the swing montage
	UPROPERTY(EditAnywhere, Category = "Swing Element")
	FVector Location1 = FVector(-60.0f, 90.0f, 190.0f);
	UPROPERTY(EditAnywhere, Category = "Swing Element")
	FVector Location2 = FVector(60.0f, 90.0f, 190.0f);

protected:
	FOnTimelineEvent TimelineFinishedEvent;

public:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void MovePlayerToClosestLocation(APlayerCharacter* PlayerCharacter);
	UFUNCTION()
	void UpdateTimeline(float Alpha);
	void PlayMontage();
	UFUNCTION()
	void OnTimelineFinished();
};
