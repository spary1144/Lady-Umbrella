#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/UI/Basics/State/EScreenPosition.h"
#include "LadyUmbrella/UI/Basics/State/EWidgetDisplay.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "HintTrigger.generated.h"

class UBoxComponent;
class UHintBase;
class UWidgetComponent;

UCLASS()
class LADYUMBRELLA_API AHintTrigger : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(AllowPrivateAccess=true))
	TSubclassOf<UHintBase> Hint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	UBoxComponent* HintShowArea;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(AllowPrivateAccess=true))
	FText TitleText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(AllowPrivateAccess=true, MultiLine=true))
	FText DescriptionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(AllowPrivateAccess=true))
	EWidgetDisplay DisplayType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(AllowPrivateAccess=true))
	EScreenPosition Alignment;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behaviour", meta=(AllowPrivateAccess=true))
	FVector2D ScreenOffset;
	
public:
	
	AHintTrigger();
	
	void GenerateHintInstance();

	FORCEINLINE TObjectPtr<UHintBase> GetHint() { return HintInstance; }
	FORCEINLINE TObjectPtr<UBoxComponent> GetShowArea() { return HintShowArea; }
	FORCEINLINE TSubclassOf<UHintBase> GetHintType() { return Hint; }
	FORCEINLINE void SetHintType(TSubclassOf<UHintBase> HintType) { Hint = HintType; }

	virtual void LoadGame() override;
	virtual void LoadDataCheckPoint() override;
	virtual void SaveGame() override;
	virtual void SaveDataCheckPoint() override;

protected:
	
	UPROPERTY() UHintBase* HintInstance;

	bool bHasBeenShown;
	
	virtual void BeginPlay() override;

	UFUNCTION() void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION() void HandleDisappeared(bool bIsShown);

};
