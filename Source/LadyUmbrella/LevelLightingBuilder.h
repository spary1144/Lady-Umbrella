// LevelLightingBuilder.h

#pragma once

#include "CoreMinimal.h"

#include "LevelLightingBuilder.generated.h"

UCLASS(Blueprintable, Blueprintable)
class ULevelLightingBuilder : public UObject
{
	GENERATED_BODY()
#if WITH_EDITOR
public:

	UFUNCTION(BlueprintCallable, BlueprintType, CallInEditor, Category = "Level Lighting Builder")
	static void BuildLightingForAllLevels(const FString& RootPath);

	UFUNCTION(BlueprintCallable, BlueprintType, CallInEditor, Category="Level Cleanup")
	static void LogActorsWithEmptyStaticMeshes();

	UFUNCTION(BlueprintCallable, BlueprintType, CallInEditor, Category="Level Cleanup")
	static void LogAllBrokenReferencesInWorld();

	UFUNCTION(BlueprintCallable, BlueprintType, CallInEditor, Category="Level Cleanup")
	static void CleanActorsWithEmptyMeshes();

	//UFUNCTION(BlueprintCallable, BlueprintType, CallInEditor, Category="Level Cleanup")
	//static void IterateActorsData();
	
	UFUNCTION(BlueprintCallable, BlueprintType, CallInEditor, Category="Level Cleanup")
	static void ListActorsWithBrokenLightmaps();
	
private:
	
	static TSet<ULevel*> PendingLevels;
	static FTSTicker::FDelegateHandle TickerHandle;
	
	static TArray<FString> GetAllLevelsInPath(const FString& RootPath);

	static void StartProcessingLevels(UWorld* World);

	static void CheckLoadedLevels(UWorld* World);

	static void SaveLevelSilently(ULevel* Level);

	static void CheckIfAllLevelsLoaded(UWorld* World);

#endif
};
