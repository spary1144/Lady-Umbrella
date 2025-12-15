// 
// MainController.cpp
// 
// Implementation of the MainController class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MainController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/AnimationAssets.h"
#include "LadyUmbrella/Assets/BlueprintAssets.h"
#include "LadyUmbrella/Assets/CurveAssets.h"
#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Assets/InputAssets.h"
#include "LadyUmbrella/Assets/SkeletalMeshAssets.h"
#include "LadyUmbrella/Assets/VFXAssets.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Characters/Other/MarcoCharacter.h"
#include "LadyUmbrella/Characters/Other/VincenzoCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Components/TimeDilationComponent/TimeDilationComponent.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeComponent.h"
#include "LadyUmbrella/UI/HUD/MainHUD.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

AMainController::AMainController()
{
	ActiveControllerInputMappingContextIndex = 0;
	ActiveKeyboardInputMappingContextIndex   = 0;

	bSubtitlesEnabled = true;
	MainHUD      = nullptr;
	NoClipSpeed  = 2.0f;
	
	TimeDilationComponent = CreateDefaultSubobject<UTimeDilationComponent>(TEXT("TimeDilationComponent"));

	// TODO: Move ALL asset loading to the ShaderMap.
	/**/ 
	/**/ //FAnimationAssets::PlayerGrappleMontage = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/RetargetedAnimations/Montages/AM_Grapple.AM_Grapple'"));
	/**/ FAnimationAssets::PlayerGrappleMontage = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/V16/Montages/Others/AM_Francesca_Other_ShootHoook_InPlace.AM_Francesca_Other_ShootHoook_InPlace'"));
	/**/ FAnimationAssets::PlayerRecoilMontage  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/V16/Montages/Combat/AM_Recoil.AM_Recoil'"));
	/**/ FAnimationAssets::PlayerSheethUmbrellaMontage  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FrancescaRetargetRootMotion/AS_Sheeth_v14_Montage_v16.AS_Sheeth_v14_Montage_v16'"));
	/**/ FAnimationAssets::PlayerMeleeMontage   = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/V16/Montages/Combat/AM_Francesca_Combat_Melee1.AM_Francesca_Combat_Melee1'"));
	/**/ FAnimationAssets::PlayerMeleeMontage2  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/V16/Montages/Combat/AM_Francesca_Combat_Melee2.AM_Francesca_Combat_Melee2'"));
	/**/ FAnimationAssets::PlayerMeleeMontage3  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/V16/Montages/Combat/AM_Francesca_Combat_Melee3.AM_Francesca_Combat_Melee3'"));

	/**/ FAnimationAssets::EnemyDeath1  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/Enemies/Montages/Combat/AM_agency_combat_death_01.AM_agency_combat_death_01'"));
	/**/ FAnimationAssets::EnemyDeath2  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/Enemies/Montages/Combat/AM_Mafia_combat_death_01.AM_Mafia_combat_death_01'"));

	
	/**/ FAnimationAssets::EnemyKickKnockbackMontage  = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/Enemies/Montages/Combat/AM_Mafia_Combate_Being_Kicked.AM_Mafia_Combate_Being_Kicked'"));
	/**/ FAnimationAssets::EnemyHookedMontage = AssetUtils::FindObject<UAnimMontage>(TEXT("/Script/Engine.AnimMontage'/Game/Animations/FinalAnimations/Enemies/Montages/Combat/AM_Mafia_Combate_Getting_Hooked.AM_Mafia_Combate_Getting_Hooked'"));
	/**/
	/**/ FBlueprintAssets::MarcoCharacterBlueprint = AssetUtils::FindClass<AMarcoCharacter>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Spawnables/Characters/BP_MarcoCharacter2.BP_MarcoCharacter2_C'"));
	/**/ FBlueprintAssets::UmbrellaBlueprint = AssetUtils::FindClass<AUmbrella>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Attachments/Interactive/Weapons/BP_Umbrella.BP_Umbrella_C'"));
	/**/ FBlueprintAssets::VincenzoCharacterBlueprint = AssetUtils::FindClass<AVincenzoCharacter>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Spawnables/Characters/BP_VincenzoCharacter.BP_VincenzoCharacter_C'"));
	/**/ FBlueprintAssets::AgencyCharacterBlueprint = AssetUtils::FindClass<AAgencyCharacter>( TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Spawnables/Characters/BP_AgencyCinematic.BP_AgencyCinematic_C'"));
	/**/
	/**/ FCurveAssets::MeleeRotationCurve = AssetUtils::FindObject<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_MeleeRotation.C_MeleeRotation'"));
	/**/
	/**/ FDatatableAssets::PlayerSystemData = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_FrancescaData.DT_LadyUmbrellaSystems_FrancescaData'"));
	/**/ FDatatableAssets::PlayerVoiceLines = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_FrancescaVoiceLines.DT_FrancescaVoiceLines'"));
	/**/ FDatatableAssets::BossVoiceLines   = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_BossVoiceLines.DT_BossVoiceLines'"));
	/**/ FDatatableAssets::MarcoVoiceLines    = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_MarcoVoiceLines.DT_MarcoVoiceLines'"));
	/**/ FDatatableAssets::LeonardoVoiceLines = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_LeonardoVoiceLines.DT_LeonardoVoiceLines'"));
	/**/ FDatatableAssets::VincenzoVoiceLines = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_VincenzoVoiceLines.DT_VincenzoVoiceLines'"));
	/**/ FDatatableAssets::UmbrellaSystemData = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_Umbrella.DT_LadyUmbrellaSystems_Umbrella'"));
	/**/ FDatatableAssets::UpgradesDatatable  = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_UpgradeData.DT_LadyUmbrellaSystems_UpgradeData'"));
	/**/ 
	/**/ FDatatableAssets::MafiaVoiceLines = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_MafiaVoiceLines.DT_MafiaVoiceLines'"));
	/**/ FDatatableAssets::AgencyVoiceLines = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/VoiceLines/DT_AgencyVoiceLines.DT_AgencyVoiceLines'"));
	/**/
	/**/ FInputAssets::AimAction      = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Aim.IA_Aim'"));
	/**/ FInputAssets::AttackAction   = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Attack.IA_Attack'"));
	/**/ FInputAssets::CoverAction    = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Cover.IA_Cover'"));
	/**/ FInputAssets::GadgetAction   = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Gadget.IA_Gadget'"));
	/**/ FInputAssets::JumpAction     = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Jump.IA_Jump'"));
	/**/ FInputAssets::LookAction     = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Look.IA_Look'"));
	/**/ FInputAssets::InteractAction = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Interact.IA_Interact'"));
	/**/ FInputAssets::MoveAction     = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Move.IA_Move'"));
	/**/ FInputAssets::PauseAction    = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Pause.IA_Pause'"));
	/**/ FInputAssets::ReloadAction   = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Reload.IA_Reload'"));
	/**/ FInputAssets::SelectNextGadgetAction   = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_SelectNextGadget.IA_SelectNextGadget'"));
	/**/ FInputAssets::SelectFirstGadgetAction  = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_SelectFirstGadget.IA_SelectFirstGadget'"));
	/**/ FInputAssets::SelectSecondGadgetAction = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_SelectSecondGadget.IA_SelectSecondGadget'"));
	/**/ FInputAssets::ShieldHoldAction   = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ShieldHold.IA_ShieldHold'"));
	/**/ FInputAssets::ShieldToggleAction = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ShieldToggle.IA_ShieldToggle'"));
	/**/ FInputAssets::ShoulderSwapAction = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ShoulderSwap.IA_ShoulderSwap'"));
	/**/ FInputAssets::CheatAction = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Cheats.IA_Cheats'"));
	/**/ FInputAssets::ShowPlayerHUD = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_Toggle_PlayerHUD.IA_Toggle_PlayerHUD'"));
	/**/ FInputAssets::ActivateNextControllerMappingContextAction = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ActivateNextControllerMappingContext.IA_ActivateNextControllerMappingContext'"));
	/**/ FInputAssets::ActivateNextKeyboardMappingContextAction   = AssetUtils::FindObject<UInputAction>(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputActions/IA_ActivateNextKeyboardMappingContext.IA_ActivateNextKeyboardMappingContext'"));
	/**/ 
	/**/ FSkeletalMeshAssets::PlayerSkeletalMesh = AssetUtils::FindObject<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_FrancesaZ.SK_FrancesaZ'"));
	/**/ 
	/**/ FVFXAssets::BulletCasingNiagaraSystem = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_BulletCasing.NS_BulletCasing'"));
	/**/ FVFXAssets::DartEquipNiagaraSystem    = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_EquipElectricDart.NS_EquipElectricDart'"));
	/**/ FVFXAssets::DustImpactNiagaraSystem   = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_DustImpact.NS_DustImpact'"));
	/**/ FVFXAssets::ElectricNiagaraSystem = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_ElectricArcs.NS_ElectricArcs'"));
	/**/ FVFXAssets::HookEquipNiagaraSystem = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_HookShoot.NS_HookShoot'"));
	/**/ FVFXAssets::HookHitNiagaraSystem   = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_HookEnemyHit.NS_HookEnemyHit'"));
	/**/ FVFXAssets::HookShootNiagaraSystem = FVFXAssets::HookEquipNiagaraSystem;
	/**/ FVFXAssets::LethalAmmoHitNiagaraSystem = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_LethalHit.NS_LethalHit'"));
	/**/ FVFXAssets::NonLethalAmmoHitNiagaraSystem = FVFXAssets::LethalAmmoHitNiagaraSystem;
	/**/ FVFXAssets::SmokeExplosionNiagaraSystem = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_SmokeExplosion.NS_SmokeExplosion'"));
	/**/ FVFXAssets::UmbrellaMeleeNiagaraSystem  = AssetUtils::FindObject<UParticleSystem>(TEXT("/Game/Particles/CascadeParticleSystems/CPS_Explosion_Melee.CPS_Explosion_Melee"));
	/**/ FVFXAssets::MuzzleflashUmbrella = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_Muzzleflash.NS_Muzzleflash'"));
	/**/ FVFXAssets::ElectricEnemiesNiagaraSystem = AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_ElectricArcsEnemies.NS_ElectricArcsEnemies'"));

	/**/ FWidgetAssets::InitializeWidgetAssets();
	
}

void AMainController::BeginPlay()
{
	Super::BeginPlay();

	MainHUD = Cast<AMainHUD>(GetHUD());

	// NOTE: There is no IsValid checks for the global variables, since we WANT the game to
	//       crash here: no player means the game is unplayable anyway, for example.
	FGlobalPointers::MainController = this;
	FGlobalPointers::GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	FGlobalPointers::PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	FGlobalPointers::Umbrella = GetWorld()->SpawnActor<AUmbrella>(FBlueprintAssets::UmbrellaBlueprint);
	
	/////////////////////
	//Aqui llamar a funcion de cargar datos del umbrella
	////////////////////
	// if (IsValid(FGlobalPointers::PlayerCharacter))
	// {
	// 	FGlobalPointers::PlayerCharacter->LoadUmbrellaData();
	// }
	
	if (IsValid(MainHUD) && IsValid(FGlobalPointers::Umbrella))
	{
		MainHUD->OnUmbrellaSpawned();
	}

	// TODO: This is part of development stuff and needs to be removed when releasing the final production build.
	/**/ const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	/**/ 
	/**/ if (!IsValid(Subsystem))
	/**/ {
	/**/ 	FLogger::ErrorLog("Invalid UEnhancedInputLocalPlayerSubsystem pointer in AMainController.BeginPlay()");
	/**/ 	return;
	/**/ }
	/**/
	// WARNING: This is used to set stuff on controller settings
	/**/ Subsystem->AddMappingContext(ControllerInputMappingContexts[0], 0);
	/**/ Subsystem->AddMappingContext(KeyboardInputMappingContexts[0], 0);

	FmodUtils::InitializeFmodUtil();

// Comment this Whenever settings don't want to be scaled to 
//   Value 0:low, 1:medium, 2:high, 3:epic, 4:cinematic (gets clamped if needed)
// 	
// 	int32 QualitySettings = 3;
// 	
// // WINDOWS
// #if PLATFORM_WINDOWS
// 	
// 	QualitySettings = 1;
// 	
// #endif
// // LINUX
// #if PLATFORM_LINUX
// 	
// 	QualitySettings = 1;
// 	
// #endif
// 
// 	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
// 	if (IsValid(Settings))
// 	{
// 		Settings->SetAntiAliasingQuality(QualitySettings);
// 		Settings->SetAudioQualityLevel(QualitySettings);
// 		Settings->SetFoliageQuality(QualitySettings);
// 		Settings->SetGlobalIlluminationQuality(QualitySettings);
// 		Settings->SetReflectionQuality(QualitySettings);
// 		Settings->SetPostProcessingQuality(QualitySettings);
// 		Settings->SetOverallScalabilityLevel(QualitySettings);
// 		Settings->SetShadingQuality(QualitySettings);
// 		Settings->SetShadowQuality(QualitySettings);
// 		Settings->SetTextureQuality(QualitySettings);
// 		Settings->SetViewDistanceQuality(QualitySettings);
// 		Settings->SetVisualEffectQuality(QualitySettings);
// 		Settings->ApplySettings(false);
// 	}
// 	
//

	if (!ControllerConnectionHandle.IsValid())
	{
		ControllerConnectionHandle = FCoreDelegates::OnControllerConnectionChange.AddLambda(
		[this](bool bCon, FPlatformUserId UId, int32 CId)
		{
			if (!bCon) TogglePauseMenu();
		});
	}
}

void AMainController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (ControllerConnectionHandle.IsValid())
	{
		FCoreDelegates::OnControllerConnectionChange.Remove(ControllerConnectionHandle);
		ControllerConnectionHandle.Reset();
	}

//	FGlobalPointers::ResetGlobalPointers();
}

void AMainController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const TObjectPtr<UEnhancedInputComponent> EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInput))
	{
		FLogger::ErrorLog("Invalid EnhancedInput pointer in AMainController.SetupInputComponent()");
		return;
	}

	EnhancedInput->BindAction(FInputAssets::PauseAction, ETriggerEvent::Triggered, this, &AMainController::TogglePauseMenu);
	EnhancedInput->BindAction(FInputAssets::ShowPlayerHUD, ETriggerEvent::Triggered, this, &AMainController::ShowPlayerHUDOnKeyPressed);
	
	// TODO: This is part of development stuff and needs to be removed when releasing the final production build.
	/**/ 
	/**/ EnhancedInput->BindAction(FInputAssets::ActivateNextControllerMappingContextAction, ETriggerEvent::Started, this, &AMainController::ActivateNextControllerInputMappingContext);
	/**/ EnhancedInput->BindAction(FInputAssets::ActivateNextKeyboardMappingContextAction,   ETriggerEvent::Started, this, &AMainController::ActivateNextKeyboardInputMappingContext);
	/**/ EnhancedInput->BindAction(FInputAssets::CheatAction, ETriggerEvent::Triggered, this, &AMainController::ToggleCheatMenu);
	/**/ EnhancedInput->BindAction(FInputAssets::CheatAction, ETriggerEvent::Completed, this, &AMainController::EnableToggleCheatMenu);
}

void AMainController::ParseInputDevice()
{
	FPlatformUserId PlayerID = FPlatformUserId::CreateFromInternalId(0);
	
	if (const UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
	{
		FHardwareDeviceIdentifier DeviceIdentifier = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(PlayerID);
		FString DeviceName = DeviceIdentifier.ToString();
		TArray<FString> ParsedDeviceName;
		DeviceName.ParseIntoArray(ParsedDeviceName, TEXT("::"));
		if (ParsedDeviceName.Num() > 0)
		{
			DeviceName = ParsedDeviceName.Last();
		}
		if (!InputDevice.Equals(DeviceName))
		{
			InputDevice = DeviceName;
			if (ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				EPlatform UpdatedPlatform =
					*FWidgetAssets::PlatformsKeys.FindKey(*InputDevice) == EPlatform::PC ?
					EPlatform::PC : EPlatform::PS;
				GameInstance->SetPlatformName(UpdatedPlatform);
			}
		}
	}

	
}

void AMainController::TogglePauseMenu()
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.TogglePauseMenu()");
		return;
	}

	MainHUD->UnTogglePlayerHUD();
	MainHUD->TogglePauseMenu();
}

void AMainController::ToggleSettingsMenu() const
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.ToggleSettingsMenu()");
		return;
	}
	
	MainHUD->ToggleSettingsMenu();
}

void AMainController::TogglePlayerHUD() const
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.TogglePlayerHUD()");
		return;
	}
	
	MainHUD->TogglePlayerHUD();
}

void AMainController::UnTogglePlayerHUD() const
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.UnTogglePlayerHUD()");
		return;
	}

	MainHUD->UnTogglePlayerHUD();
}

void AMainController::DeactivatePlayerHUDOnTimer() const
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.UnTogglePlayerHUD()");
		return;
	}
	MainHUD->HidePlayerHUDOnTimer();
}

void AMainController::ActivateResetHidePlayerHUDTimer() const
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.UnTogglePlayerHUD()");
		return;
	}
	MainHUD->ResetHidePlayerHUDTimer();
}

void AMainController::ShowPlayerHUDOnKeyPressed()
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.UnTogglePlayerHUD()");
		return;
	}
	
	ActivateResetHidePlayerHUDTimer();
	TogglePlayerHUD();
	DeactivatePlayerHUDOnTimer();
}

void AMainController::UmbrellaSpawned() const
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.UmbrellaSpawned()");
		return;
	}
	
	MainHUD->OnUmbrellaSpawned();
}

void AMainController::HandleToggleGrenadeIndicator(AGrenade* Grenade) const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->ToggleGrenadeIndicator(Grenade);
	}
}

void AMainController::HandleUnToggleGrenadeIndicator() const
{
	if (IsValid(GetHUD()))
	{
		GetMainHUD()->UnToggleGrenadeIndicator();
	}
}

// +-------------------------------------------------+
// |                                                 |
// |                DEVELOPMENT STUFF                |
// |                                                 |
// +-------------------------------------------------+
void AMainController::ActivateNextControllerInputMappingContext()
{
	if (ActiveControllerInputMappingContextIndex == ControllerInputMappingContexts.Num() - 1)
	{
		SetActiveControllerInputMappingContext(0);
	}
	else
	{
		SetActiveControllerInputMappingContext(ActiveControllerInputMappingContextIndex + 1);
	}
}

void AMainController::ActivateNextKeyboardInputMappingContext()
{
	if (ActiveKeyboardInputMappingContextIndex == KeyboardInputMappingContexts.Num() - 1)
	{
		SetActiveKeyboardInputMappingContext(0);
	}
	else
	{
		SetActiveKeyboardInputMappingContext(ActiveKeyboardInputMappingContextIndex + 1);
	}
}

TWeakObjectPtr<UInputMappingContext> AMainController::GetControllerMappingContextAtIndex(int32 _InputMappingContextIndex) const
{
	// If we tried to get an invalid Input Mapping Context.
	if (_InputMappingContextIndex >= ControllerInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Get Controller Input Mapping Nº%i, but the MainController only has %i available Controller Mapping Contexts.", _InputMappingContextIndex, ControllerInputMappingContexts.Num());
#endif
		
		return nullptr;
	}

	return ControllerInputMappingContexts[_InputMappingContextIndex];
}

TWeakObjectPtr<UInputMappingContext> AMainController::GetKeyboardMappingContextAtIndex(int32 _InputMappingContextIndex) const
{
	// If we tried to get an invalid Input Mapping Context.
	if (_InputMappingContextIndex >= KeyboardInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to Get Keyboard Input Mapping Nº%i, but the MainController only has %i available Keyboard Mapping Contexts.", _InputMappingContextIndex, KeyboardInputMappingContexts.Num());
#endif
		
		return nullptr;
	}

	return KeyboardInputMappingContexts[_InputMappingContextIndex];
}

void AMainController::SetActiveControllerInputMappingContext(int32 _NewInputMappingContextIndex)
{
	// If we are trying to activate the Mapping Context that is already on.
	if (ActiveControllerInputMappingContextIndex == _NewInputMappingContextIndex)
	{
		return;
	}
	// If we tried to activate an invalid Input Mapping Context.
	if (_NewInputMappingContextIndex >= ControllerInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to activate Controller Input Mapping N.%i, but the MainController only has %i available Controller Mapping Contexts.", _NewInputMappingContextIndex, ControllerInputMappingContexts.Num());
#endif
		
		return;
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex]);
		ActiveControllerInputMappingContextIndex = _NewInputMappingContextIndex;
		Subsystem->AddMappingContext(ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex], 0);

#if UE_EDITOR
		FString message = FString("Activating Controller Input Mapping N.") + FString::FromInt(ActiveControllerInputMappingContextIndex) + FString(", ") + ControllerInputMappingContexts[ActiveControllerInputMappingContextIndex]->GetName();
		FLogger::WarningLog(message);
#endif
	}
}

void AMainController::SetActiveKeyboardInputMappingContext(int32 _NewInputMappingContextIndex)
{
	// If we are trying to activate the Mapping Context that is already on.
	if (ActiveKeyboardInputMappingContextIndex == _NewInputMappingContextIndex)
	{
		return;
	}
	// If we tried to activate an invalid Input Mapping Context.
	if (_NewInputMappingContextIndex >= KeyboardInputMappingContexts.Num())
	{
#if UE_EDITOR
		FLogger::ErrorLog("Trying to activate Keyboard Input Mapping N.%i, but the MainController only has %i available Keyboard Mapping Contexts.", _NewInputMappingContextIndex, KeyboardInputMappingContexts.Num());
#endif
		
		return;
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex]);
		ActiveKeyboardInputMappingContextIndex = _NewInputMappingContextIndex;
		Subsystem->AddMappingContext(KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex], 0);

#if UE_EDITOR
		FString message = FString("Activating Keyboard Input Mapping N.") + FString::FromInt(ActiveKeyboardInputMappingContextIndex) + FString(", ") + KeyboardInputMappingContexts[ActiveKeyboardInputMappingContextIndex]->GetName();
		FLogger::WarningLog(message);
#endif
	}
}

void AMainController::ToggleCheatMenu()
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.ToggleCheatMenu()");
		return;
	}

	MainHUD->ToggleCheatMenu();
}

void AMainController::EnableToggleCheatMenu()
{
	if (!IsValid(MainHUD))
	{
		FLogger::ErrorLog("Invalid MainHUD pointer in MainController.EnableToggleCheatMenu()");
		return;
	}
	
	MainHUD->EnableToggleCheatMenu();
}

void AMainController::Godmode() const
{
	FGlobalPointers::PlayerCharacter->SetGodmode(!FGlobalPointers::PlayerCharacter->IsGodmode());
}

void AMainController::NoClip() const
{
	FGlobalPointers::PlayerCharacter->SetNoClip(!FGlobalPointers::PlayerCharacter->IsNoClip());
		
	// NOTE: This doesn't work for some reason? Maybe it's multithreaded?
	// FGlobalPointers::PlayerCharacter->SetActorEnableCollision(!FGlobalPointers::PlayerCharacter->IsNoClip());
		
	if (FGlobalPointers::PlayerCharacter->IsNoClip())
	{
		FGlobalPointers::PlayerCharacter->GetCharacterMovement()->MaxFlySpeed *= NoClipSpeed;
		FGlobalPointers::PlayerCharacter->ClientCheatFly();
		FGlobalPointers::PlayerCharacter->SetActorEnableCollision(false);
	}
	else
	{
		FGlobalPointers::PlayerCharacter->GetCharacterMovement()->MaxFlySpeed /= NoClipSpeed;
		FGlobalPointers::PlayerCharacter->ClientCheatWalk();
		FGlobalPointers::PlayerCharacter->SetActorEnableCollision(true);
	}
		
	FGlobalPointers::PlayerCharacter->bUseControllerRotationPitch = FGlobalPointers::PlayerCharacter->IsNoClip();
	FGlobalPointers::PlayerCharacter->bUseControllerRotationYaw = FGlobalPointers::PlayerCharacter->IsNoClip();
}

void AMainController::InfiniteAmmo()
{
	if (IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->SetInfiniteAmmo(!FGlobalPointers::Umbrella->IsInfiniteAmmo());			
	}
}

void AMainController::ReloadLevel()
{
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName));
}

void AMainController::SetAmmoCurrent(const int Amount)
{
	if (IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->SetAmmoCurrent(Amount);
	}
}

void AMainController::SetAmmoReserve(const int Amount)
{
	if (IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->SetAmmoReserve(Amount);
	}
}

void AMainController::SetComponents(const int Amount)
{
	if (IsValid(FGlobalPointers::PlayerCharacter))
	{
		FGlobalPointers::PlayerCharacter->SetCurrentPiecesForUpgrades(Amount);
	}
}

void AMainController::UnlockGadgets()
{
	FGlobalPointers::PlayerCharacter->UnlockGadgets(EGadgetType::Electric);
	FGlobalPointers::PlayerCharacter->UnlockGadgets(EGadgetType::Grapple);
}

void AMainController::UnlockUpgrades()
{
	for (const TPair Pair : *FGlobalPointers::Umbrella->GetUpgradeComponent()->GetUpgrades())
		FGlobalPointers::Umbrella->ApplyUpgrade(Pair.Key);
}
