#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/GenericInteractable.h"
#include "ZiplineElement.generated.h"

class UFModComponentInterface;
class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class USplineComponent;
class USplineMeshComponent;
class UParticleSystemComponent;
class UWidgetComponent;
class UInputAction;
class APlayerCharacter;
class UEnhancedInputComponent;

UCLASS()
class LADYUMBRELLA_API AZiplineElement : public AGenericInteractable
{
	GENERATED_BODY()

public:
	AZiplineElement();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	// Components
	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	USceneComponent* DefaultRoot;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	UStaticMeshComponent* Cube1;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	UStaticMeshComponent* Cube2;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	USceneComponent* StartPoint;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	USceneComponent* EndPoint;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	USplineComponent* ZipLineSpline;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	USplineMeshComponent* ZiplineMesh;

	UPROPERTY(VisibleAnywhere, Category="Zipline|Components")
	UBoxComponent* Cube1CollisionBox;

	UPROPERTY(VisibleAnywhere, Category="Zipline|VFX")
	UParticleSystemComponent* ZiplineEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FModSoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FModHookingComponent;
	
	// Tuning
	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FVector Cube1Position;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FVector Cube2Position;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FVector Cube1Scale;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FVector Cube2Scale;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FRotator Cube1Rotation;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FRotator Cube2Rotation;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FVector StartOffset;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	FVector EndOffset;

	UPROPERTY(EditAnywhere, Category="Zipline|Setup")
	float CableHeightOffset;

	UPROPERTY(EditAnywhere, Category="Zipline|Collision")
	FVector Box1Extent;

	UPROPERTY(EditAnywhere, Category="Zipline|Collision")
	FVector CollisionOffset;

	UPROPERTY(EditAnywhere, Category="Zipline|Movement")
	float ZipLineSpeed;

	UPROPERTY(EditAnywhere, Category="Zipline|Movement")
	float SplineTimeOffset;

	UPROPERTY(EditAnywhere, Category="Zipline|Movement")
	float SplineMaxTime;

	UPROPERTY(EditAnywhere, Category="Zipline|Movement")
	float MaxStartTime;

	UPROPERTY(EditAnywhere, Category="Zipline|Snapping")
	float SnappingInterpSpeed;

	UPROPERTY(EditAnywhere, Category="Zipline|Snapping")
	float SnapMinDistanceThreshold;

	UPROPERTY(EditAnywhere, Category="Zipline|UI")
	float UIHeightOffset;

	UPROPERTY(EditAnywhere, Category="Zipline|SplineMesh")
	FVector2D V_MeshScale;

	UPROPERTY(EditAnywhere, Category="Zipline|Camera")
	float CameraLookClampMin;

	UPROPERTY(EditAnywhere, Category="Zipline|Camera")
	float CameraLookClampMax;

	// Input
	UPROPERTY(EditAnywhere, Category="Zipline|Input")
	UInputAction* JumpAction = nullptr;

	// Runtime state
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerCharacter> PlayerCharacterZiplineRef;

	UPROPERTY(Transient)
	bool IsInRange;

	UPROPERTY(Transient)
	bool IsSnapping;

	UPROPERTY(Transient)
	float CurrentSplineTime;

	UPROPERTY(Transient)
	FVector SnapLocation = FVector::ZeroVector;

	const FName ParameterName = "ZiplineHasWind";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMod Parameters")
	bool bIsInAir;
	
	FTimerHandle MovementTimerHandle;

	// THE ONE POINTER ALLOWED FOR THE PLAYER
	static TWeakObjectPtr<AZiplineElement> ActiveZipline;

	// Helpers
	bool IsActiveThis() const { return ActiveZipline.Get() == this; }
	void BecomeActive();
	void ResignIfActive();

	void AdjustSnappingPoint();
	void MoveCharacterAlongSpline();
	void ProcessOverlap(AActor* OtherActor);
	void ClearLocalPlayerRef();
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	UFUNCTION()
	void OnCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OffCube1Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnSplineOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSplineOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void Jump();
};
