// 
// ShadersController.h
// 
// Controller for pre compiling shaders while showing a loading screen. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShadersController.generated.h"

class ULoadingShaderScreen;

UCLASS()
class LADYUMBRELLA_API AShadersController : public APlayerController
{
	GENERATED_BODY()
	
	AShadersController();

	UPROPERTY()
	ULoadingShaderScreen* LoadingScreenInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ULoadingShaderScreen> LoadingScreenWidget;

	//To see the loading screen because shaders compile in less than a second
	float ElapsedTime;
	float DisplayTime;
	int32 RemainingShaders;
	// remove this once we have more shaders

	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

	UFUNCTION()
	void OnLoadedLevel(UWorld* World);
	bool IsLoadingFinished();
};
