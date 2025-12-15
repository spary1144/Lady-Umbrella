// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericPickable.h"
#include "UpgradePieces.generated.h"

class UBoxComponent;
class UFModComponentInterface;

const uint8 TOTAL_PIECES_IN_GAME = 14;

UCLASS()
class LADYUMBRELLA_API AUpgradePieces : public AGenericPickable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;


protected:
	virtual void BeginPlay() override final;
	
public: 
	AUpgradePieces();

private:
	
	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	
};
