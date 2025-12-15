// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericInteractable.h"
#include "SecurityLever.generated.h"

class ASecuritySystem;

UCLASS()
class LADYUMBRELLA_API ASecurityLever : public AGenericInteractable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Security Lever", meta = (AllowPrivateAccess = "true"))
	ASecuritySystem* SecuritySystem;
	
public:
	ASecurityLever();
	
private:	
	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; };

protected:
	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Temp")
	void OnLeverInteract();
};
