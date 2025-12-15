#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/InteractiveMovement.h"
#include "SwingBar.generated.h"

enum class EUmbrellaLocation : uint8;
class UFModComponentInterface;
class UBoxComponent;
class UBillboardComponent;
class UTextRenderComponent;
class UCharacterMovementComponent;
class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwingStateChanged, EUmbrellaLocation, UmbrellaLocation, bool, bIsImmediate, bool, bPlayAnim);

UCLASS()
class LADYUMBRELLA_API ASwingBar : public AActor, public IInteractiveMovement
{
	GENERATED_BODY()
	
public:
	static FOnSwingStateChanged OnSwingStateChanged;
	// Sets default values for this actor's properties
	ASwingBar();

	virtual bool Interact(AActor* OtherActor) override;

	UFUNCTION()
	void SetIsActive(bool IsActive);
	
	UFUNCTION()
	bool GetIsActive() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element")
	USceneComponent* VisibleRoot;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBoxComponent* BoxCollider1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBoxComponent* BoxCollider2;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBoxComponent* CloseBoxCollider1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBoxComponent* CloseBoxCollider2;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* SwingLocation1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* SwingLocation2;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* SwingLocation1Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* SwingLocation2Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* TargetPoint1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* TargetPoint2;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* TargetPoint1Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* TargetPoint2Label;
	
	UPROPERTY(EditInstanceOnly, Category = "Swing Element", meta = (ToolTip = "Whether to display debug lines for the swing system in the editor."))
	bool bShowDebugLines;

private:
	const float MIN_TRAVEL_TIME_RATE = 0.01;

	UPROPERTY()
	UFModComponentInterface* SwingSoundFmodComponentInterface;
	
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Whether the swing bar is active or not."))
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Time taken to move to the swing location."))
	float TravelTimeToSwingLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Shortened travel time for close swing locations."))
	float CloseTravelTimeToSwingLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Time before reaching the swing position at which the swing animation will start."))
	float SwingStartTimeOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Time to travel from the swing location to the final target location."))
	float TravelTimeToTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Final location the character will move to after swinging."))
	FVector TargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ChosenSwingLocation;

	UPROPERTY()
	FTimerHandle ParabolicLaunchTimerHandler;

	UPROPERTY()
	FTimerHandle ParabolicLaunchToSwingLocationTimerHandler;

	UPROPERTY()
	FTimerHandle CooldownTimerHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Cooldown time before another swing can be initiated."))
	float Cooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Whether the swing ability is currently in cooldown."))
	bool bIsInCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Indicates if the current swing is performed with a fast animation."))
	bool bIsFastSwing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum dot product threshold required to consider the character is facing the swing direction."))
	float MinFacingDotThreshold;
	
	UFUNCTION()
	bool TryMoveToClosestLocation();

	UFUNCTION()
	void Swing();

	UFUNCTION()
	void OnSwingCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	
	UFUNCTION()
	void LaunchParabolicTo(float Time, const FVector& Start, const FVector& End);
	
	UFUNCTION()
	void TryToClearParabolicLaunchTimer();

	UFUNCTION()
	void CooldownReset();

	UFUNCTION()
	bool CheckIfPlayerFacingBar(const FVector& RefLocation) const;

	UFUNCTION()
	void SetupSwing();
	
	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
