
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "SecuritySystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecuritySystemActivate);

class ALaserElement;

UENUM(BlueprintType)
enum class ESecurityState : uint8
{
	NotTriggered	UMETA(DisplayName = "NotTriggered"),
	Activated		UMETA(DisplayName = "Activated"),
	Deactivated		UMETA(DisplayName = "Deactivated"),
};

UCLASS()
class LADYUMBRELLA_API ASecuritySystem : public AActor, public ISaveInterface
{
	
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SecuritySystem", meta = (AllowPrivateAccess = true))
	ESecurityState SecurityState;
	
	UPROPERTY(VisibleAnywhere, Category = "SecuritySystem")
	TArray<AActor*> TriggerActivableActors;
	
	UPROPERTY(VisibleAnywhere, Category = "SecuritySystem")
	TArray<AActor*> LaserActors;
	
	UPROPERTY(VisibleAnywhere, Category = "SecuritySystem")
	TArray<ALaserElement*> LaserElementsArray;

	UPROPERTY(VisibleAnywhere, Category = "SecuritySystem")
	TArray<AActor*> ActorsToHideWhenActivating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sublevels", meta = (AllowPrivateAccess = true))
	TArray<FString> Sublevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sublevels", meta = (AllowPrivateAccess = true))
	bool bShouldLoadSublevels;

	UFUNCTION()
	void LoadSublevels();

	
public:
	
	ASecuritySystem();
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void ActivateSecuritySystem();

	UFUNCTION(BlueprintCallable)
	void DeactivateSecuritySystem();

	UFUNCTION(BlueprintCallable)
	void BroadcastSecurityState();

	UFUNCTION(BlueprintCallable)
	void StopLoadingSublevels();
	
	virtual void SaveGame() override;
	
	virtual void LoadGame() override;
	
	virtual void SaveDataCheckPoint() override;
	
	virtual void LoadDataCheckPoint() override;
	bool InitializeTaggedActors();


	UFUNCTION(BlueprintImplementableEvent, Category = "SecuritySystem")
	void OnSystemActivate();

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnSecuritySystemActivate OnSecuritySystemActivate;
	
protected:
	
	virtual void BeginPlay() override;
	

};


