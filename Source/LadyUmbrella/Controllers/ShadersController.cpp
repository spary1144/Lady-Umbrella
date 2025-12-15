// 
// ShadersController.cpp
// 
// Implementation of the ShadersController class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "LadyUmbrella/Controllers/ShadersController.h"
#include "LadyUmbrella/UI/Basics/LoadingShaderScreen.h"
#include "ShaderPipelineCache.h"
#include "Components/TextBlock.h"

AShadersController::AShadersController()
{
	PrimaryActorTick.bCanEverTick = true;
	LoadingScreenInstance = nullptr;
	//For showcasing the Loading screen for 5 seconds
	ElapsedTime = 0.0f;
	DisplayTime = 5.0;
	RemainingShaders = 71;
}

void AShadersController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(LoadingScreenWidget))
	{
		LoadingScreenInstance = CreateWidget<ULoadingShaderScreen>(GetWorld(), LoadingScreenWidget);
		
		// Begin compiling shaders
		FShaderPipelineCache::SetBatchMode(FShaderPipelineCache::BatchMode::Precompile);
		FShaderPipelineCache::ResumeBatching();
		float RealRemainingShaders = FShaderPipelineCache::NumPrecompilesRemaining();
		RemainingShaders = RealRemainingShaders > 0 ? RealRemainingShaders : RemainingShaders;

		LoadingScreenInstance->AddToViewport();
		LoadingScreenInstance->SetMaximumValue(StaticCast<float>(RemainingShaders)); // Max numbers of PSOs when cooked
	}

}

void AShadersController::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	RemainingShaders = FShaderPipelineCache::NumPrecompilesRemaining();
	if (IsValid(LoadingScreenInstance))
	{
		int Compiled = StaticCast<int>(LoadingScreenInstance->GetMaximumValue()) - RemainingShaders;
		Compiled = Compiled < 0 ? LoadingScreenInstance->GetMaximumValue() : Compiled;
		LoadingScreenInstance->UpdateProgressBar(Compiled);
		LoadingScreenInstance->GetShaderNumber()->SetText(FText::FromString(FString::FromInt(Compiled)));

		if (IsLoadingFinished())
		{
			LoadingScreenInstance->PlayStartAnim(GetWorld(), TEXT("L_MainMenu"));
			FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &AShadersController::OnLoadedLevel);
			
			SetActorTickEnabled(false);
		}
	}
}

void AShadersController::OnLoadedLevel(UWorld* World)
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	LoadingScreenInstance->RemoveFromParent();
	LoadingScreenInstance = nullptr;
}

bool AShadersController::IsLoadingFinished()
{
	bool bShouldEnd = ElapsedTime > DisplayTime;
	return bShouldEnd && !FShaderPipelineCache::IsPrecompiling();
}
