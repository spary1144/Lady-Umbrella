// Fill out your copyright notice in the Description page of Project Settings.

#include "LU_GameInstance.h"

#include "LU_SaveGame.h"
#include "SaveGameSlotNames.h"
#include "SaveInterface.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Environment/CheckPointTrigger.h"
#include "LadyUmbrella/Environment/TriggerLevelChange/EChapterNames.h"
#include "LadyUmbrella/UI/HUD/LoadingScreen.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

class FAssetRegistryModule;

ULU_GameInstance::ULU_GameInstance()
{
    LoadVerificationLoopTime = 0.1f;
    CurrentSaveSlot = SaveGameSlots::SaveSlot1;
    LoadingScreenClass = AssetUtils::FindClass<ULoadingScreen>(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Menu/Components/WBP_LoadingScreen.WBP_LoadingScreen_C'"));
    CreditsClass = AssetUtils::FindClass<UCredits>(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Menu/Components/WBP_Credits.WBP_Credits_C'"));
    SaveGameFlag = false;
    LanguageCode = ELanguageCode::EN_US;

    MeleeKills = 0;
    VaultStuns = 0;
    GrenadeRebounds = 0;
    BulletsShotInLevel = 0;
}

void ULU_GameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init: %p"), this);

    Steam::Initialize();

    ControlTypeCounter = -1;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), ActorsWithSaveData);
    LoadingScreenWidget = CreateWidget<ULoadingScreen>(this, LoadingScreenClass);
    CreditsWidget = CreateWidget<UCredits>(this, CreditsClass);
    
    if (!ReadSaveGameFile())
    {
        UE_LOG(LogTemp, Warning, TEXT("ULU_GameInstance::Init: No SaveGameFile found. Creating new one"));
        CreateGameSaveObject();
        GetSaveGameFile()->InitializeDefaults();
        SaveGameFlag = false;
        return;
    }
    
    SaveGameFlag = true;
}

void ULU_GameInstance::BeginDestroy()
{
    Super::BeginDestroy();
    FGlobalPointers::ResetGlobalPointers();
}

FVector ULU_GameInstance::GetLastArenaPosition()
{
    AArenaManager* LastArena = GetLastArena();
    if (!IsValid(LastArena))
    {
        return FVector::ZeroVector;
    }
    return LastArena->GetActorLocation();
}

AArenaManager* ULU_GameInstance::GetLastArena()
{
    if (ActivatedArenas.IsEmpty())
    {
        return nullptr;
    }
		
    AArenaManager* Arena = ActivatedArenas.Last();
    if(IsValid(Arena))
    {
        return Arena;
    }
    return nullptr;
}

void ULU_GameInstance::CreateGameSaveObject()
{
    if (IsValid(GetSaveGameFile()))
    {
        return;
    }
    
    ULU_SaveGame* SaveFileGame = Cast<ULU_SaveGame>(UGameplayStatics::CreateSaveGameObject(ULU_SaveGame::StaticClass()));

    if (IsValid(SaveFileGame))
    {
        SetSaveFile(SaveFileGame);
    }
}

FBox GetLevelBounds(const ULevel* Level)
{
    if (!IsValid(Level))
    {
        return FBox(ForceInit);
    }

    FBox Bounds(ForceInit);

    for (const AActor* Actor : Level->Actors)
    {
        if (!IsValid(Actor))
        {
            continue;
        }
        Bounds += Actor->GetComponentsBoundingBox(true);
    }
    return Bounds;
}

FString GetCleanSubLevelName(const FString& FullName)
{
    FString ShortName = FPackageName::GetShortName(FullName);
    
    int32 FirstUnderscore;
    if (ShortName.FindChar('_', FirstUnderscore))
    {
        int32 SecondUnderscore = ShortName.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstUnderscore + 1);
        if (SecondUnderscore != INDEX_NONE)
        {
            ShortName = ShortName.Mid(SecondUnderscore + 1);
        }
        else
        {
            ShortName = ShortName.Mid(FirstUnderscore + 1);
        }
    }
    return ShortName;
}

ULevelStreaming* GetStreamingLevelByName(UWorld* World, const FString& SubLevelName)
{
    if (!World)
    {
        return nullptr;
    }

    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (!StreamingLevel) continue;

        FString LevelName = StreamingLevel->GetWorldAssetPackageName();
        if (FPackageName::GetShortName(LevelName) == SubLevelName)
        {
            return StreamingLevel;
        }
    }

    return nullptr;
}

void ULU_GameInstance::SaveSublevelToGameSaveFile()
{
    UWorld* World = GetWorld();
    
    if (!IsValid(World))
    {
        FLogger::ErrorLog("ULU_GameInstance::SaveSublevelToGameSaveFile: Invalid World pointer");
        return;
    }

    if (!IsValid(LastCheckpointTrigger))
    {
        //FLogger::ErrorLog("ULU_GameInstance::SaveSublevelToGameSaveFile: Invalid LastCheckpointTrigger Pointer");
        return;
    }

    const FString LastCheckpoint = LastCheckpointTrigger->GetLevel()->GetOutermost()->GetName();

    if (LastCheckpoint.IsEmpty())
    {
        FLogger::ErrorLog("ULU_GameInstance::SaveSublevelToGameSaveFile - Failed trying to save checkpoint to SaveGameFile");
        return; 
    }

    const FString ShortName = FPackageName::GetShortName(LastCheckpoint);
    
    if (!IsValid(GetSaveGameFile()))
    {
        FLogger::ErrorLog("ULU_GameInstance::SaveSublevelToGameSaveFile: Invalid SaveGameFile Pointer");
        return;
    }
    
    GetSaveGameFile()->SetLastSublevelName(ShortName);
    
}

void ULU_GameInstance::SaveGame()
{
     checkf(!CurrentSaveSlot.IsEmpty(), TEXT("SaveSlot (Save File Name) is empty when calling ULU_GameInstance::SaveGame"));
    SaveGameFlag = true;
    if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveSlot, 0) || !IsValid(GetSaveGameFile()))
    {
        CreateGameSaveObject();
    }
    
    if (IsValid(GetSaveGameFile()))
    {
        SaveDataToGameInstance();
        SaveSublevelToGameSaveFile();
        // GetSaveGameFile()->SetPlayerData( GI_SaveStructPlayer );
        check(UGameplayStatics::SaveGameToSlot(GetSaveGameFile(), CurrentSaveSlot, 0));
    }
}

bool ULU_GameInstance::DestroySaveGameFile() const
{
    if (!IsValid(GetSaveGameFile()))
    {
        return false;
    }
    return UGameplayStatics::DeleteGameInSlot(CurrentSaveSlot, 0);
}

bool ULU_GameInstance::ReadSaveGameFile()
{
    // We check just in case this is being called with a GameSaveFile already created.
    ULU_SaveGame* SaveFileGame = Cast<ULU_SaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameSlots::SaveSlot1, 0));

    if (IsValid(SaveFileGame))
    {
        SetSaveFile(SaveFileGame);
        return true;
    }
    return false;
}

void ULU_GameInstance::LoadGame()
{
    if (!IsValid(GetSaveGameFile()))
    {
        return;
    }
    
    SubLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), *GetSaveGameFile()->GetLastSublevelName());
    if (IsValid(SubLevel))
    {
        SubLevel->bDisableDistanceStreaming = false;
    }
    
    GI_SaveStructPlayer = SaveFile->GetPlayerData();
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), ActorsWithSaveData);
    
    for (AActor* Actor : ActorsWithSaveData)
    {
        ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
        if (SaveInterface)
        {
            SaveInterface->LoadGame();
        }
    }
    
    GetWorld()->GetTimerManager().SetTimer(
        LoadVerificationHandle,
        this,
        &ThisClass::LoadAfterVerifyingSublevel,
        LoadVerificationLoopTime,
        false
    );
}

void ULU_GameInstance::LoadAfterVerifyingSublevel()
{
    if (!IsValid(SubLevel))
    {
        //FLogger::ErrorLog("Invalid Sublevel pointer in LU_GameInstance.LoadAfterVerifyingSublevel()");
        return;
    }

    if (!SubLevel->IsLevelLoaded() || !SubLevel->IsLevelVisible())
    {
        if (IsValid(FGlobalPointers::PlayerCharacter))
        {
            FGlobalPointers::PlayerCharacter->SetActorTransform(GI_SaveStructPlayer.SaveFileLastPlayerTransform);
            LoadGame();
        }
        return;
    }
    
    GetLoadingScreenWidget()->Hide();
    GetLoadingScreenWidget()->SetMapPath("");
    GetLoadingScreenWidget()->SetSublevelPath("");

    if (GetLoadingScreenWidget()->IsVisible())
    {
        LoadGame();
    }

    if (!IsValid(FGlobalPointers::PlayerCharacter))
    {
        FLogger::ErrorLog("Invalid PlayerCharacter pointer in LU_GameInstance.LoadAfterVerifyingSublevel()");
        return;
    }
    
    FGlobalPointers::PlayerCharacter->CheckpointCameraReset();
}

void ULU_GameInstance::SaveDataToGameInstance()
{
    // This has to be encapsulated to a func
    UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), ActorsWithSaveData);
    
    for (AActor* Actor : ActorsWithSaveData)
     {
         ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
         if (SaveInterface)
         {
             SaveInterface->SaveGame();
         }
     }
}

void ULU_GameInstance::SaveCheckPoint()
{
    UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), ActorsWithSaveData);
    
    for (AActor* Actor : ActorsWithSaveData)
    {
        ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
        if (SaveInterface)
        {
            SaveInterface->SaveDataCheckPoint();
        }
    }
}

void ULU_GameInstance::LoadCheckpoint()
{
    UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), ActorsWithSaveData);
    
    for (AActor* Actor : ActorsWithSaveData)
    {
        ISaveInterface* SaveInterface = ValidateSaveInterface(Actor);
        if (SaveInterface)
        {
            SaveInterface->LoadDataCheckPoint();
        }
    }
    
    OnPlayerRespawned.Broadcast();
}

const FTransform& ULU_GameInstance::GetPlayerStartTransform() const
{
    TArray<AActor*> Actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
    if (Actors.Num() > 0 && IsValid(Actors[0]))
    {
        return Actors[0]->GetTransform();
    }
    return FTransform::Identity;
}

bool ULU_GameInstance::UnlockLevelProgress(const FName& ChapterToUnlockProgress)
{
    if (!IsValid(GetSaveGameFile()))
    {
        return false;
    }
    if (!GetSaveGameFile()->GetChaptersProgress().Contains(ChapterToUnlockProgress))
    {
        return false;
    }
    FChaptersProgress* LevelToUnlock = GetSaveGameFile()->GetChaptersProgress().Find(ChapterToUnlockProgress);
    if (!LevelToUnlock)
    {
        return false;
    }
    
    LevelToUnlock->bLastChapterPlayed   = true;
    LevelToUnlock->bChapterUnlocked     = false;
    LevelToUnlock->bLastChapterUnlocked = true;
    
    GI_SaveStructPlayer.SaveFileLastPlayerTransform = GetPlayerStartTransform();

    return true;
}

void ULU_GameInstance::OpenLevelFromLoadGame()
{
    FName LastLevelPlayed;

    /*
    if (!IsValid(SaveFile))
    {
        UGameplayStatics::OpenLevel(GetWorld(), ToName(EChapterNames::Chapter1));
        SaveGame();
        return;
    }*/
    
    for (TMap<FName, FChaptersProgress>::TConstIterator It = SaveFile->GetChaptersProgress().CreateConstIterator(); It; ++It)
    {
        if (It.Value().bLastChapterPlayed)
        {
            LastLevelPlayed = It.Key();
            break;
        }
    }
    OpenLevelWithLoad(LastLevelPlayed.ToString());
}

void ULU_GameInstance::OpenLevelFromLoadGameDelegate(UWorld* World)
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

    FString Sublevel = GetSaveGameFile()->GetLastSublevelName();
    if (!IsValid(GetLoadingScreenWidget()))
    {
        return;
    }
    GetLoadingScreenWidget()->AddToViewport(INT_MAX);
    GetLoadingScreenWidget()->SetRenderOpacity(1.0);
/*
    if (!DoesPersistentLevelContainSublevel(World, GetCleanSubLevelName(LastSublevelName)))
    {
        PostLoadingLevel(World);
        GetLoadingScreenWidget()->Hide();
        return;
    }
*/
    
    const FString LastSublevelName = *GetSaveGameFile()->GetLastSublevelName();
    FLatentActionInfo LatentActionInfo;
    LatentActionInfo.CallbackTarget    = this;
    LatentActionInfo.ExecutionFunction = FName(TEXT("LoadGame"));
    LatentActionInfo.Linkage           = 0;
    LatentActionInfo.UUID              = FMath::Rand();

    UGameplayStatics::LoadStreamLevel(World, *LastSublevelName, false, true, LatentActionInfo);
    SubLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), *LastSublevelName);
    if (IsValid(SubLevel))
    {
        SubLevel->bDisableDistanceStreaming = true;
        SubLevel->SetShouldBeLoaded(true);
        SubLevel->SetShouldBeVisible(true);
    }
    
    if (Sublevel.IsEmpty())
    {
        GetLoadingScreenWidget()->Hide();
    }
}

ISaveInterface* ULU_GameInstance::ValidateSaveInterface(AActor*& ActorToValidate)
{
    ISaveInterface* ReturnPtr = nullptr;
    if (ActorToValidate->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
    {
        ReturnPtr = Cast<ISaveInterface>(ActorToValidate);
    }
    return ReturnPtr;
}

void ULU_GameInstance::CheckLevelReady()
{
    const UWorld* World = GetWorld();
    FTimerManager& WorldTimerManager = World->GetTimerManager();
    if (!IsValid(World))
    {
        WorldTimerManager.ClearTimer(LevelReadyCheckHandle);
        return;
    }
    const FString LastSublevelName = *GetSaveGameFile()->GetLastSublevelName();
    ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(World, FName(*LastSublevelName));
    if (!IsValid(Streaming))
    {
        return;
    }

    if (!Streaming->HasLoadedLevel() || !Streaming->IsLevelVisible() && !Streaming->GetLoadedLevel()) 
    {
        return;
    }

    ULevel* LoadedLevel = Streaming->GetLoadedLevel();
    if (!IsValid(LoadedLevel))
    {
        return;
    }
    
    bool bAllCollisionReady = true;
    for (AActor* Actor : LoadedLevel->Actors)
    {
        if (!IsValid(Actor))
        {
            continue;
        }
        TArray<UPrimitiveComponent*> PrimComps;
        Actor->GetComponents<UPrimitiveComponent>(PrimComps);
        for (UPrimitiveComponent* Comp : PrimComps)
        {
            if (!IsValid(Comp)) continue;
            if (!Comp->IsCollisionEnabled()) continue;
            
            FBodyInstance* Body = Comp->GetBodyInstance();
            if (!Body || !Body->IsValidBodyInstance())
            {
                bAllCollisionReady = false;
                break;
            }
        }
        if (!bAllCollisionReady) break;
    }

    if (bAllCollisionReady)
    {
        WorldTimerManager.ClearTimer(LevelReadyCheckHandle);
        Streaming->SetShouldBeVisible(true);
        LoadGame();
    }
}


bool ULU_GameInstance::DoesPersistentLevelContainSublevel(UWorld* World, const FString& SublevelName) const
{
    if (SublevelName.IsEmpty() || !IsValid(World))
    {
        return false;
    }

    for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
    {
        if (!IsValid(StreamingLevel))
        {
            continue;
        }
        
        FString PackageName = StreamingLevel->GetWorldAssetPackageName();
        FString ShortName   = UWorld::RemovePIEPrefix(FPackageName::GetShortName(PackageName));

        if (ShortName.Equals(SublevelName))
        {
            return true;
        }
    }

    return false;
}

void ULU_GameInstance::ShowCredits() const
{
    CreditsWidget->Show();
}

void ULU_GameInstance::OpenLevelWithLoad(const FString& MapName)
{
    if (IsValid(FGlobalPointers::GameInstance))
    {
        FGlobalPointers::GameInstance->SetBulletsShotInLevel(0);
        FGlobalPointers::GameInstance->SetDeathsInLevel(0);
    }
    
    GetLoadingScreenWidget()->SetMapPath(MapName);
    GetLoadingScreenWidget()->SetSublevelPath(GetCleanSubLevelName(*GetSaveGameFile()->GetLastSublevelName()));
    GetLoadingScreenWidget()->Show();
    GetLoadingScreenWidget()->AddToViewport(INT_MAX);
    
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULU_GameInstance::OpenLevelFromLoadGameDelegate);
}

void ULU_GameInstance::PostLoadingLevel(UWorld* World)
{
    if (!IsValid(World))
    {
        return;
    }
    
    const FName LevelName = *UGameplayStatics::GetCurrentLevelName(World);
    
    if (UnlockLevelProgress(LevelName))
    {
        GetSaveGameFile()->SetLastSublevelName("");
       //SaveGame();
        LoadGame();
    }
}

void ULU_GameInstance::Shutdown()
{
    Super::Shutdown();
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: %p"), this);
}


EPlatform ULU_GameInstance::GetPlatformName() const
{
    return PlatformName;
}

void ULU_GameInstance::SetPlatformName(EPlatform NewPlatformName)
{
    PlatformName = NewPlatformName;
    if(OnPlatformUpdated.IsBound())
    {
        OnPlatformUpdated.Broadcast(NewPlatformName);
    }
}

int32 ULU_GameInstance::GetControlTypeCounter() const
{
    return ControlTypeCounter;
}

void ULU_GameInstance::SetControlTypeCounter(const int32 NewControlTypeCounter)
{
    ControlTypeCounter = NewControlTypeCounter;
}

void ULU_GameInstance::AddMeleeKill()
{
    MeleeKills++;
}

void ULU_GameInstance::AddVaultStuns()
{
    VaultStuns++;
}

void ULU_GameInstance::AddGrenadeRebounds()
{
    GrenadeRebounds++;
}

void ULU_GameInstance::AddBulletShotInLevel()
{
    BulletsShotInLevel++;
}

void ULU_GameInstance::AddDeathsInLevel()
{
    DeathsInLevel++;
}

void ULU_GameInstance::UnlockAchievement(const ESteamAchievement Achievement)
{
    Steam::UnlockAchievement(Achievement);
}
