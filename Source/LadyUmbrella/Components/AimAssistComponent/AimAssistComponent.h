#pragma once 
 
#include "CoreMinimal.h" 
#include "Components/ActorComponent.h" 
#include "AimAssistComponent.generated.h" 
 
class AGenericCharacter; 
 
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) 
class LADYUMBRELLA_API UAimAssistComponent : public UActorComponent 
{ 
    GENERATED_BODY() 
    const float BASE_RANGE = 2000.0f;
public:
    float GetUmbrellaRangeFromData() const;
    UAimAssistComponent(); 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float CapsuleRadius; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float CapsuleLength; 
     
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float AimAssistSpeed; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float AimAssistSpeedMultiplier; 
     
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float AimAngleThreshold; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    float ObjectTargetSpeedMultiplier; 
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    FVector TargetOffset; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Snapping") 
    bool bDoObjectSnapping;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockAngle;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockSlowMultiplier; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockMultiplierActors;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    bool bDoObjectSoftLock;
    
    UPROPERTY(BlueprintReadOnly, Category = "Aim Assist|Soft Lock") 
    bool bShouldSlowAiming; 
 
    UPROPERTY(BlueprintReadOnly, Category = "Aim Assist|Soft Lock") 
    bool bIsAiming; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockRadius; 
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Soft Lock") 
    float SoftLockLength; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|General")
    float CameraSnapStartOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|General")
    float StopFollowingMaxTime;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bShowDebugSnap;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bShowDebugSoftLock;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bShowDebugVisibility;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Assist|Debug") 
    bool bDebugMessages;

    UPROPERTY(EditAnywhere, Category="Aim Assist|Break")
    float CameraInputBreakThreshold;


    
protected: 
    virtual void BeginPlay() override; 
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    auto IsTargetDead() -> bool;
    void StartFollowTimer();
    bool bIsHoldingAimInput = false;
public:  
    UFUNCTION(BlueprintCallable) 
    void OnStartAiming();
    void OnEndAiming();
    UFUNCTION(BlueprintCallable) 
    void StopRotation(); 
    UFUNCTION(BlueprintCallable) 
    void CheckForSoftLock();
    float GetSoftLockAngle(const AActor* TargetActor, const FVector& CameraLocation, const FVector& ViewDirection) const;
    void SetCameraMultiplier(APlayerController* PC, float NewMult); 

    void OnAimInputPressed();
    void OnAimInputReleased();
private: 
    TArray<FHitResult> PerformSphereTrace(float TraceRadius, float TraceLength, bool ShowDebug); 
    TArray<AGenericCharacter*> FilterUniqueEnemies(const TArray<FHitResult>& HitResults);
    //TArray<AActor*> FilterUniqueTargets(const TArray<FHitResult>& HitResults);
    AGenericCharacter* FindBestTarget(const TArray<AGenericCharacter*>& Enemies, const FVector& ViewLocation, const FVector& ViewDirection); 
    void RotateViewToTarget(APlayerController* PC, const FVector& TargetLocation, float DeltaTime); 
    bool DidPlayerMoveCamera(class APlayerController* PC) const;
    void ClearLockAndTimers();

private: 
    UPROPERTY()
    AActor* CurrentTarget = nullptr;
    UPROPERTY()
    UPrimitiveComponent* CurrentHookPoint = nullptr;
    UPROPERTY()
    FTimerHandle AimFollowTimer;

    bool bTargetIsActor;
    
}; 