#pragma once

#include "CoreMinimal.h"
#include "ElectricElement.h"
#include "PuddleElement.generated.h"

UCLASS()
class LADYUMBRELLA_API APuddleElement : public AElectricElement
{
	GENERATED_BODY()

public:
	APuddleElement();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PerformElectrification() override;
	virtual void ResetElectrifiedState() override;
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	


	
};
