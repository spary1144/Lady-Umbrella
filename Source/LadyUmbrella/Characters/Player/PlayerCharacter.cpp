// 
// PlayerCharacter.cpp
// 
// Implementation of the PlayerCharacter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "PlayerCharacter.h"

// TODO: There are way too many includes here. This is a sign that this class should be
//       broken up into smaller classes (components).
#include "CableComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Assets/AnimationAssets.h"
#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Assets/InputAssets.h"
#include "LadyUmbrella/Assets/SkeletalMeshAssets.h"
#include "LadyUmbrella/Characters/Other/MarcoCharacter.h"
#include "LadyUmbrella/Components/AimAssistComponent/AimAssistComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"
#include "LadyUmbrella/Components/MeleeComponent/MeleeComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineActor.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineManager.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateComponent.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Components/CoverComponent/PlayerCoverMovementComponent.h"
#include "LadyUmbrella/Components/FreeRoamCoverComponent/FreeRoamCoverComponent.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/PickableObjects/Ammo.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/PickableObjects/UpgradePieces.h"
#include "LadyUmbrella/Interfaces/InteractInterface.h"
#include "LadyUmbrella/Math/EasingLibrary.h"
#include "LadyUmbrella/UI/HitReactionComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FPlayerDataDriven.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/SaveSystem/LU_SaveGame.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "MotionWarpingComponent.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/CameraInputSpeed.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/CameraStateKey.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/DeathScreenComponent/DeathScreenComponent.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeComponent.h"
#include "LadyUmbrella/Environment/CheckPointTrigger.h"
#include "LadyUmbrella/Environment/LandmarkFocus.h"
#include "LadyUmbrella/Environment/SwingRope.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/VerticalHookMovement.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/Workbench.h"
#include "LadyUmbrella/Environment/MovementElements/SwingBar.h"
#include "LadyUmbrella/UI/HUD/PiecesCount.h"
#include "LadyUmbrella/UI/Menus/WorkbenchMenu/WorkBenchMenu.h"
#include "LadyUmbrella/Util/SaveSystem/FChapterNames.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"

class UVoiceLineManager;

APlayerCharacter::APlayerCharacter()
{
	bWantsToJump							= false;
	bIsAimingButtonPressed		 			= false;
	bUseControllerRotationPitch  			= false;
	bUseControllerRotationYaw	 			= false;
	bUseControllerRotationRoll	 			= false;
	CloseRadius                  			= 400.f;
	CurrentPiecesForUpgrades     			= 0;
	FleeRadius                   			= 1500.f;
	InitialMeshRelativeTransform 			= FTransform::Identity;
	RecoilAnimSpeedOffset        			= 0.0f;
	TimeToResetUmbrellaPosition	 			= 5.f;
	bPlayerInCombat				 			= false;
	DelayToPutUmbrellaOnBackAfterArenaEnded = 2.f;
	PreviousTickLocation					= FVector::ZeroVector;
	CurrentUmbrellaLocation					= EUmbrellaLocation::Hand;

	AimAssistComponent      = CreateDefaultSubobject<UAimAssistComponent>(TEXT("AimAssistComponent"));
	CameraSystemComponent   = CreateDefaultSubobject<UCustomCameraSystemComponent>(TEXT("CameraSystemComponent"));
	FmodComponentSteps	    = CreateDefaultSubobject<UFModComponentInterface>("Fmod Audio Steps Interface Component");
	FmodComponentVoiceLines = CreateDefaultSubobject<UFModComponentInterface>("Fmod Audio VoiceLine Interface Component");
	FmodThirdCinematic		= CreateDefaultSubobject<UFModComponentInterface>("Fmod Third Cinematic Interface Component");
	InGameVoicesComponent	= CreateDefaultSubobject<UFModComponentInterface>("Fmod Audio InGame Voices Interface Component");
	FmodHealthSnapshot		= CreateDefaultSubobject<UFModComponentInterface>("Fmod Snapshot Interface Component");
	AnimationsFmodComponent = CreateDefaultSubobject<UFModComponentInterface>("Fmod Animations Interface Component");
	FreeRoamCoverComponent  = CreateDefaultSubobject<UFreeRoamCoverComponent>(TEXT("FreeRoamCoverComponent"));
	HitReactionComponent    = CreateDefaultSubobject<UHitReactionComponent>(TEXT("HitReactionComponent"));
	MotionWarpingComponent  = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	PlayerStateComponent    = CreateDefaultSubobject<UPlayerStateComponent>(TEXT("PlayerStateComponent"));
	DeathScreenComponent	= CreateDefaultSubobject<UDeathScreenComponent>(TEXT("DeathScreenComponent"));
	
	FmodComponentSteps->SetAutoActivate(false);
	FmodComponentVoiceLines->SetAutoActivate(false);

	static UFMODEvent* MeleeContactSound = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_MELEE.UMBRELLA_MELEE'"); 
	FmodUtils::TryAddEvent(MeleeContactSound, AnimationsFmodComponent);

	static UFMODEvent* LandingEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/FRANCESCA_LANDING.FRANCESCA_LANDING'");
	FmodUtils::TryAddEvent(LandingEvent, AnimationsFmodComponent);

	static UFMODEvent* VaultShortEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaGameplaySounds/FRANCESCA_VAULT_SHORT.FRANCESCA_VAULT_SHORT'");
	FmodUtils::TryAddEvent(VaultShortEvent, AnimationsFmodComponent);

	static UFMODEvent* VaultLongEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaGameplaySounds/FRANCESCA_VAULT_LONG.FRANCESCA_VAULT_LONG'");
	FmodUtils::TryAddEvent(VaultLongEvent, AnimationsFmodComponent);
	
	static UFMODEvent* JumpEffect = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/FRANCESCA_JUMP_EFFECT.FRANCESCA_JUMP_EFFECT'");
	FmodUtils::TryAddEvent(JumpEffect,AnimationsFmodComponent);

	static UFMODEvent* FallEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaGameplaySounds/FRANCESCA_FALL_SOUND.FRANCESCA_FALL_SOUND'");
	FmodUtils::TryAddEvent(FallEvent,AnimationsFmodComponent);
	
	static UFMODEvent* FallRollEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/FRANCESCA_FALLING.FRANCESCA_FALLING'");
	FmodUtils::TryAddEvent(FallRollEvent,InGameVoicesComponent);
	
	static UFMODEvent* PumpActionEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_PUMPACTION.UMBRELLA_PUMPACTION'");
	FmodUtils::TryAddEvent(PumpActionEvent,AnimationsFmodComponent);
	
	static UFMODEvent* ReloadEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_RELOAD.UMBRELLA_RELOAD'");
	FmodUtils::TryAddEvent(ReloadEvent,AnimationsFmodComponent);
	
	static UFMODEvent* SeathingEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_SEATHING.UMBRELLA_SEATHING'");
	FmodUtils::TryAddEvent(SeathingEvent,AnimationsFmodComponent);
	
	static UFMODEvent* FmodStepsEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaFootSteps/FrancescaFootSteps.FrancescaFootSteps'");
	FmodUtils::TryAddEvent(FmodStepsEvent,FmodComponentSteps);

	static UFMODEvent* FmodVaultEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_VAULTING.FRANCESCA_VAULTING'");
	FmodUtils::TryAddEvent(FmodVaultEvent,FmodComponentVoiceLines);
	
	static UFMODEvent* DyingEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/FRANCESCA_DYING.FRANCESCA_DYING'");
	FmodUtils::TryAddEvent(DyingEvent,FmodComponentVoiceLines);

	static UFMODEvent* FallingEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/FRANCESCA_FALLING.FRANCESCA_FALLING'");
	FmodUtils::TryAddEvent(FallingEvent,InGameVoicesComponent);
	
	static UFMODEvent* FmodCoverEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_TAKING_COVER.FRANCESCA_TAKING_COVER'");
	FmodUtils::TryAddEvent(FmodCoverEvent, FmodComponentVoiceLines);

	static UFMODEvent* FmodReload = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_RELOADING.FRANCESCA_RELOADING'");
	FmodUtils::TryAddEvent(FmodReload,FmodComponentVoiceLines);
	
	static UFMODEvent* FmodDefeatingEnemy = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_DEFEAT_ENEMY.FRANCESCA_DEFEAT_ENEMY'");
	FmodUtils::TryAddEvent(FmodDefeatingEnemy,FmodComponentVoiceLines);

	static UFMODEvent* FmodInCover = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_IN_COVER.FRANCESCA_IN_COVER'");
	FmodUtils::TryAddEvent(FmodInCover,FmodComponentVoiceLines);

	static UFMODEvent* FmodUsingHook = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_USING_HOOK.FRANCESCA_USING_HOOK'");
	FmodUtils::TryAddEvent(FmodUsingHook,FmodComponentVoiceLines);

	static UFMODEvent* FmodUsingElectricDard = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Combat/FRANCESCA_USING_ELECTRIC.FRANCESCA_USING_ELECTRIC'");
	FmodUtils::TryAddEvent(FmodUsingElectricDard,FmodComponentVoiceLines);

	static UFMODEvent* Chapter1GameplayVoices	 = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/Chapter_1/FRANCESCA_CINEMATIC_CHAPTER_1.FRANCESCA_CINEMATIC_CHAPTER_1'");
	FmodUtils::TryAddEvent(Chapter1GameplayVoices, InGameVoicesComponent);

	static UFMODEvent* Chapter2GameplayVoices	 = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/Chapter_2/FRANCESCA_CINEMATIC_CHAPTER_2.FRANCESCA_CINEMATIC_CHAPTER_2'");
	FmodUtils::TryAddEvent(Chapter2GameplayVoices,InGameVoicesComponent);	

	static UFMODEvent* Chapter3GameplayVoices	 = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/Chapter_3/FRANCESCA_CINEMATIC_CHAPTER_3.FRANCESCA_CINEMATIC_CHAPTER_3'");
	FmodUtils::TryAddEvent(Chapter3GameplayVoices,InGameVoicesComponent);	

	static UFMODEvent* Chapter4GameplayVoices	 = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/Chapter_4/FRANCESCA_CINEMATIC_CHAPTER_4.FRANCESCA_CINEMATIC_CHAPTER_4'");
	FmodUtils::TryAddEvent(Chapter4GameplayVoices,InGameVoicesComponent);

	static UFMODEvent* Chapter5GameplayVoices	 = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Gameplay/Chapter_5/FRANCESCA_CINEMATIC_CHAPTER_5.FRANCESCA_CINEMATIC_CHAPTER_5'");
	FmodUtils::TryAddEvent(Chapter5GameplayVoices,InGameVoicesComponent);
	
	static UFMODEvent* FirstCinematic		= AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Cinematics/FRANCESCA_SEQUENCER_CHAPTER_0.FRANCESCA_SEQUENCER_CHAPTER_0'");
	FmodUtils::TryAddEvent(FirstCinematic,FmodThirdCinematic);

	static UFMODEvent* SecondCinematic		= AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Cinematics/FRANCESCA_SEQUENCER_CHAPTER_3.FRANCESCA_SEQUENCER_CHAPTER_3'");
	FmodUtils::TryAddEvent(SecondCinematic,FmodThirdCinematic);

	static UFMODEvent* ThirdCinematicEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/FrancescaVoices/Cinematics/FRANCESCA_SEQUENCER_CHAPTER_5.FRANCESCA_SEQUENCER_CHAPTER_5'");
	FmodUtils::TryAddEvent(ThirdCinematicEvent,FmodThirdCinematic);

	static UFMODEvent* JumpingSounds = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/FrancescaVoices/Gameplay/FRANCESCA_JUMP.FRANCESCA_JUMP'");
	FmodUtils::TryAddEvent(JumpingSounds,InGameVoicesComponent);

	static UFMODEvent* Climb = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/FrancescaVoices/Gameplay/FRANCESCA_CLIMB.FRANCESCA_CLIMB'");
	FmodUtils::TryAddEvent(Climb,InGameVoicesComponent);
	
	FmodThirdCinematic->SetVoiceLineIdentifier("FRANCESCA_");
	FmodComponentVoiceLines->SetVoiceLineIdentifier("FRANCESCA_");
	InGameVoicesComponent->SetVoiceLineIdentifier("FRANCESCA_");
	
	static UFMODEvent* HealthSnapshotEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODSnapshot'/Game/FMOD/Snapshots/HealthSnapshot.HealthSnapshot'");
	FmodUtils::TryAddEvent(HealthSnapshotEvent, FmodHealthSnapshot);
	FmodHealthSnapshot->bAutoActivate = false;

	ReloadFmodComponent = CreateDefaultSubobject<UFModComponentInterface>("Fmod Reload Component Interface");
	static UFMODEvent* ReloadUmbrellaEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_RELOAD.UMBRELLA_RELOAD'");
	FmodUtils::TryAddEvent(ReloadUmbrellaEvent,ReloadFmodComponent);
	ReloadFmodComponent->bAutoActivate = false;

	GetCameraSystemComponent()->GetCameraBoom()->SetupAttachment(RootComponent);
	
	GetCameraSystemComponent()->GetFollowCamera()->SetupAttachment(
		GetCameraSystemComponent()->GetCameraBoom().Get(),
		USpringArmComponent::SocketName
	);
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCharacterMovement()->AirControl                 = 0.35f;
	GetCharacterMovement()->bOrientRotationToMovement  = true;	
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->JumpZVelocity              = 700.f;
	GetCharacterMovement()->MaxWalkSpeed               = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed         = 20.f;
	GetCharacterMovement()->RotationRate               = FRotator(0.0f, 500.0f, 0.0f);

	GetDataTableRowHandle().DataTable = FDatatableAssets::PlayerSystemData;
	GetDataTableRowHandle().RowName   = "BP_Character";

	GetHealthComponent()->SetCanRegenerate(true);
	GetMesh()->SetSkeletalMesh(FSkeletalMeshAssets::PlayerSkeletalMesh);
	
	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::PlayerVoiceLines);
	GetVoiceLineComponent()->SetOwnerName(PLAYER_DEFAULT_NAME);

	SetCoverMovementComponent(CreateDefaultSubobject<UPlayerCoverMovementComponent>(TEXT("CoverMovementComponent")));
	SetWeaponType(EWeaponType::Umbrella);
}

void APlayerCharacter::SwapGameplayVoices(const FString& NewChapter) const
{
	if (!InGameVoicesComponent->GetFMODAudioEventsMap().Find(NewChapter))
	{
		return;
	}
	
	const UFMODEvent* Event = *InGameVoicesComponent->GetFMODAudioEventsMap().Find(NewChapter);
	if (!IsValid(Event))
	{
		return;
	}

	if (!InGameVoicesComponent->SetCurrentEvent(Event->GetName()))
	{
		FLogger::ErrorLog("APlayerCharacter::SwapGameplayVoices: Error setting NewChapter Event");
	}
}

void APlayerCharacter::AttachUmbrellaToSocket(const FName& NewPosition)
{
	FGlobalPointers::Umbrella->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		NewPosition
	);
}

void APlayerCharacter::HideHUD(const EUmbrellaLocation UmbrellaPosition)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		return;		
	}
	if (IsValid(FGlobalPointers::MainController->GetMainHUD()))
	{
		FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
		if (!FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState() && UmbrellaPosition == EUmbrellaLocation::Hand)
		{
			FGlobalPointers::MainController->TogglePlayerHUD();
		}
		else if(FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState() &&
			UmbrellaPosition == EUmbrellaLocation::UmbrellaCenter && !IsPlayerInCombat())
		{
			FGlobalPointers::MainController->UnTogglePlayerHUD();
		}
	}
}

void APlayerCharacter::ChangeUmbrellaLocation(const EUmbrellaLocation UmbrellaPosition, const bool bImmediateChange = true, const bool bPlayAnim = false)
{
	if (!IsValid(this) || IsActorBeingDestroyed())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	FName NewPosition;
	switch (UmbrellaPosition)
	{
	case EUmbrellaLocation::Back:
		NewPosition = "OnBackWeaponSocket";
		break;
	case EUmbrellaLocation::Hand:
		NewPosition = "WeaponSocket";
		break;
	case EUmbrellaLocation::UmbrellaCenter:
		NewPosition = "UmbrellaCenter";
		break;
	case EUmbrellaLocation::LeftHand:
		NewPosition = "LeftHandSocket";
		break;
	case EUmbrellaLocation::WorkBench:
		NewPosition = "WorkBenchSocket";
		break;
	default:
		NewPosition = "OnBackWeaponSocket";
		break;
	}
	
	FTimerManager& TimerManager = World->GetTimerManager();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	TimerManager.ClearTimer(ResetUmbrellaPositionTimerHandle);
	if (UmbrellaPosition != EUmbrellaLocation::Back)
	{
		TimerManager.ClearTimer(CombatStateTimerHandle);
		if (IsValid(AnimInstance) && IsValid(FAnimationAssets::PlayerSheethUmbrellaMontage) && AnimInstance->Montage_IsPlaying(FAnimationAssets::PlayerSheethUmbrellaMontage))
		{
			AnimInstance->Montage_Stop(0.f, FAnimationAssets::PlayerSheethUmbrellaMontage);
		}
	}
	
	if (CurrentUmbrellaLocation == UmbrellaPosition)
	{
		return;
	}

	if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover() && CurrentUmbrellaLocation == EUmbrellaLocation::Hand)
	{
		return;
	}
	if (bImmediateChange)
	{
		if (bPlayerInCombat && UmbrellaPosition == EUmbrellaLocation::Back)
		{
			FTimerDelegate SetUmbrellaOnHandDelegate;
			SetUmbrellaOnHandDelegate.BindUFunction(this, "ChangeUmbrellaLocation", EUmbrellaLocation::Hand, true, false);
			TimerManager.SetTimer(ResetUmbrellaPositionTimerHandle, SetUmbrellaOnHandDelegate,MAX_TIME_WITH_UMBRELLA_ON_BACK_IN_COMBAT, false, MAX_TIME_WITH_UMBRELLA_ON_BACK_IN_COMBAT);
		}

		if (UmbrellaPosition != EUmbrellaLocation::Back)
		{
			AttachUmbrellaToSocket(NewPosition);
			CurrentUmbrellaLocation = UmbrellaPosition;
			HideHUD(UmbrellaPosition);
			
			return;
		}

		// EUmbrellaLocation::Back
		if (bPlayAnim && GetPlayerGeneralState() != EPlayerState::InTraversalNoControls && !IsDead())
		{
			if (GetPlayerGeneralState() == EPlayerState::AimingUmbrella || GetPlayerGeneralState() == EPlayerState::OpenShield || GetPlayerGeneralState() == EPlayerState::InCoverAiming)
			{
				TimerManager.ClearTimer(ResetUmbrellaPositionTimerHandle);
				return;
			}
			PlayMontage(FAnimationAssets::PlayerSheethUmbrellaMontage, 1.0f);
			if (IsValid(AnimInstance) && IsValid(FAnimationAssets::PlayerSheethUmbrellaMontage) && !AnimInstance->OnMontageEnded.IsAlreadyBound(this, &APlayerCharacter::OnUmbrellaSheathEnded))
			{
				AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::OnUmbrellaSheathEnded);
			}
		}
		else
		{
			CurrentUmbrellaLocation = UmbrellaPosition;
			TryToCloseShield();
			AttachUmbrellaToSocket(NewPosition);
			if (IsValid(FGlobalPointers::MainController) && IsValid(FGlobalPointers::MainController->GetMainHUD()))
			{
				if (FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState() && !IsPlayerInCombat())
				{
					FGlobalPointers::MainController->UnTogglePlayerHUD();
				}
			}
		}
	}
	else
	{
		if (UmbrellaPosition == EUmbrellaLocation::Back && bPlayerInCombat)
		{
			return;
		}
		
		FTimerDelegate ResetUmbrellaPositionDelegate;
		ResetUmbrellaPositionDelegate.BindLambda([this, UmbrellaPosition]()
		{
			ChangeUmbrellaLocation(UmbrellaPosition, true, true);
			//TryToCloseShield();
			
			if (FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState())
			{
				FGlobalPointers::MainController->UnTogglePlayerHUD();
			}
		});
		TimerManager.SetTimer(ResetUmbrellaPositionTimerHandle, ResetUmbrellaPositionDelegate,TimeToResetUmbrellaPosition, false, TimeToResetUmbrellaPosition);
	}
}

void APlayerCharacter::OnUmbrellaSheathEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(Montage) && (Montage == FAnimationAssets::PlayerSheethUmbrellaMontage))
	{
		if (IsValid(AnimInstance) && IsValid(FAnimationAssets::PlayerSheethUmbrellaMontage))
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::OnUmbrellaSheathEnded);
		}
	}
}

void APlayerCharacter::ChangePlayerInCombatState(bool NewCombatState)
{
	SetPlayerInCombat(NewCombatState);
	if (NewCombatState)
	{
		ChangeUmbrellaLocation(EUmbrellaLocation::Hand, true, false);
	}
	else
	{
		if (GetPlayerGeneralState() != EPlayerState::AimingUmbrella && GetPlayerGeneralState() != EPlayerState::InCoverAiming)
		{
			GetWorld()->GetTimerManager().SetTimer(CombatStateTimerHandle, [this]() { ChangeUmbrellaLocation(EUmbrellaLocation::Back, true, true); }, DelayToPutUmbrellaOnBackAfterArenaEnded, false, DelayToPutUmbrellaOnBackAfterArenaEnded);
		}
	}
}

// void APlayerCharacter::PostInitProperties()
// {
// 	Super::PostInitProperties();
// 	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::PlayerVoiceLines);
// 	GetVoiceLineComponent()->SetOwnerName(PLAYER_DEFAULT_NAME);
// }

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(DataTableRowHandler.DataTable))
	{
		LoadPlayerFromDataTable();
	}

	PreviousTickLocation = GetActorLocation();
	
	if (IsValid(GetController()))
	{
		//PlayerSaveStruct.WorldTransform = GetController()->GetLevelTransform();
	}

	if (!IsValid(FGlobalPointers::Umbrella))
	{
		return;
	}

	// NOTE: The umbrella CANNOT be spawned in the constructor, as we need to make sure the player is fully
	//       created before being able to spawn child actors. Also, unlike the other global pointers, this
	//       can't go in AMainController.BeginPlay() since it's spawned dynamically when the Player spawns.
	//       However, the "crash-when-fail" policy still applies, since the game NEEDS the umbrella in order
	//       to run successfully, so if this fails we crash the game.
	FGlobalPointers::Umbrella->SetOwner(this);

	FGlobalPointers::Umbrella->UpdateVisualAmmoOnLoad();
	FGlobalPointers::MainController->TogglePlayerHUD();
	FGlobalPointers::Umbrella->OnUmbrellaRangeAndSpread.Broadcast(FGlobalPointers::Umbrella->GetBulletRange(), FGlobalPointers::Umbrella->GetSpreadAngleX(), FGlobalPointers::Umbrella->GetSpreadAngleZ());
	(void) FGlobalPointers::Umbrella->OnMaxPelletsPerShot.ExecuteIfBound(FGlobalPointers::Umbrella->GetMaxNrBulletPerShot());
	
	FGlobalPointers::Umbrella->Tags.Add(TEXT("PlayerUmbrella"));
	
	FGlobalPointers::Umbrella->OnIsCoveredByUmbrellaChanged.AddDynamic(this, &APlayerCharacter::OnUmbrellaOpenStateChange);
	FGlobalPointers::Umbrella->GetMeleeComponent()->OnMeleeAttack.BindUFunction(this, "OnMeleeAttackListener");
	if (!FGlobalPointers::Umbrella->OnUmbrellaReloadStateChanged.IsBound())
	{
		FGlobalPointers::Umbrella->OnUmbrellaReloadStateChanged.BindUFunction(this, "ChangeUmbrellaLocation");
	}
	GetWorld()->GetTimerManager().ClearTimer(ResetUmbrellaPositionTimerHandle);
	AArenaManager::OnArenaStarted.AddDynamic(this, &APlayerCharacter::ChangePlayerInCombatState);
	AArenaManager::OnArenaEnded.AddDynamic(this, &APlayerCharacter::ChangePlayerInCombatState);
	AVerticalHookMovement::OnVerticalHookStateChanged.AddDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	ASwingBar::OnSwingStateChanged.AddDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	ASwingRope::OnSwingRopeStateChanged.AddDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	AWorkbench::OnWorkBenchInteract.AddDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	UWorkBenchMenu::OnWorkBenchExit.AddDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	if (IsValid(GetInteractiveMovementComponent()))
	{
		GetInteractiveMovementComponent()->OnMantleStarted.BindUFunction(this, "ChangeUmbrellaLocation");
		GetInteractiveMovementComponent()->OnZiplineStateChanged.BindUFunction(this, "ChangeUmbrellaLocation");
		//GetInteractiveMovementComponent()->OnSwingStarted.BindUFunction(this, "ChangeUmbrellaLocation");
	}
	ChangeUmbrellaLocation(EUmbrellaLocation::Back, true, false);

	if (IsValid(GetMesh()))
	{
		InitialMeshRelativeTransform = GetMesh()->GetRelativeTransform();
	}

	HitReactionComponent->Activate();

	//if (IsValid(FreeRoamCoverComponent) && !FreeRoamCoverComponent->OnCoverExited.IsBound())
	//{
	//	FreeRoamCoverComponent->OnCoverExited.AddDynamic(this, &APlayerCharacter::ResetWalkSpeedAfterFreeCover);
	//}
	
	/*const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetWorld()->GetGameInstance());
	if (!IsValid(GameInstance))
	{
		return;
	}*/
	
	/*ULU_SaveGame* SaveGameFile = GameInstance->GetSaveGameFile();
	if (!IsValid(SaveGameFile))
	{
		return;
	}*/

	/*AUmbrella* Umbrella = GetWeapon();
	if (!IsValid(Umbrella))
	{
		return;
	}*/
	
	//Umbrella->SetAmmoCurrent(SaveGameFile->GetPlayerData().UmbrellaCurrentMagAmmo);
	//Umbrella->SetAmmoReserve(SaveGameFile->GetPlayerData().UmbrellaInventoryAmmo);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	//reset voice line cooldown
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	World->GetTimerManager().ClearTimer(ResetUmbrellaPositionTimerHandle);
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		return;
	}
	
	UVoiceLineManager* Manager = GameInstance->GetSubsystem<UVoiceLineManager>();
	if (!IsValid(Manager))
	{
		return;//return Manager->ClearVoiceLineBuffer();
	}
	
	Manager->ClearVoiceLineBuffer();

	AArenaManager::OnArenaStarted.RemoveDynamic(this, &APlayerCharacter::ChangePlayerInCombatState);
	AArenaManager::OnArenaEnded.RemoveDynamic(this, &APlayerCharacter::ChangePlayerInCombatState);
	AVerticalHookMovement::OnVerticalHookStateChanged.RemoveDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	ASwingBar::OnSwingStateChanged.RemoveDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	ASwingRope::OnSwingRopeStateChanged.RemoveDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	AWorkbench::OnWorkBenchInteract.RemoveDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	UWorkBenchMenu::OnWorkBenchExit.RemoveDynamic(this, &APlayerCharacter::ChangeUmbrellaLocation);
	
	if (IsValid(GetInteractiveMovementComponent()))
	{
		GetInteractiveMovementComponent()->OnMantleStarted.Unbind();
		GetInteractiveMovementComponent()->OnZiplineStateChanged.Unbind();
		//GetInteractiveMovementComponent()->OnSwingStarted.BindUFunction(this, "ChangeUmbrellaLocation");
	}
	World->GetTimerManager().ClearTimer(ResetUmbrellaPositionTimerHandle);
	World->GetTimerManager().ClearTimer(CombatStateTimerHandle);

	if (EndPlayReason != EEndPlayReason::Type::Quit && EndPlayReason != EEndPlayReason::Type::EndPlayInEditor)
	{
		if (IsValid(FGlobalPointers::GameInstance))
			FGlobalPointers::GameInstance->GetSaveStructPlayer().CurrentUpgradePieces = GetCurrentPiecesForUpgrades();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	const TObjectPtr<UEnhancedInputComponent> EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInput))
	{
		FLogger::ErrorLog("Invalid UEnhancedInputComponent pointer in PlayerCharacter.SetupPlayerInputComponent()");
		return;
	}
	
	EnhancedInput->BindAction(FInputAssets::AimAction,      ETriggerEvent::Started,   this, &APlayerCharacter::StartedHoldingAimButton);
	EnhancedInput->BindAction(FInputAssets::AimAction,      ETriggerEvent::Completed, this, &APlayerCharacter::ReleasedAimButton);
	EnhancedInput->BindAction(FInputAssets::AttackAction,   ETriggerEvent::Started,   this, &APlayerCharacter::ShootingUmbrella);
	EnhancedInput->BindAction(FInputAssets::CoverAction,    ETriggerEvent::Triggered, this, &APlayerCharacter::ToggleCover);
	EnhancedInput->BindAction(FInputAssets::GadgetAction,   ETriggerEvent::Started,   this, &APlayerCharacter::GadgetShoot);
	EnhancedInput->BindAction(FInputAssets::InteractAction, ETriggerEvent::Started,   this, &APlayerCharacter::InteractingWithOverlappingElements);
	EnhancedInput->BindAction(FInputAssets::JumpAction,     ETriggerEvent::Started,   this, &APlayerCharacter::Jump);
	EnhancedInput->BindAction(FInputAssets::LookAction,     ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	EnhancedInput->BindAction(FInputAssets::LookAction,     ETriggerEvent::Completed, this, &APlayerCharacter::StopLook);
	EnhancedInput->BindAction(FInputAssets::MoveAction,     ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EnhancedInput->BindAction(FInputAssets::MoveAction,     ETriggerEvent::Completed, this, &APlayerCharacter::StopMovingPlayer);
	EnhancedInput->BindAction(FInputAssets::ReloadAction,   ETriggerEvent::Started,   this, &APlayerCharacter::Reload);

	EnhancedInput->BindAction(FInputAssets::SelectNextGadgetAction,   ETriggerEvent::Started, this, &APlayerCharacter::CycleUmbrellaBullets);
	EnhancedInput->BindAction(FInputAssets::SelectFirstGadgetAction,  ETriggerEvent::Started, this, &APlayerCharacter::SwapToHook);
	EnhancedInput->BindAction(FInputAssets::SelectSecondGadgetAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToElectric);
	
	EnhancedInput->BindAction(FInputAssets::ShieldHoldAction,   ETriggerEvent::Started,   this, &APlayerCharacter::HoldShieldInput);
	EnhancedInput->BindAction(FInputAssets::ShieldHoldAction,   ETriggerEvent::Completed, this, &APlayerCharacter::StopShieldInput);
	EnhancedInput->BindAction(FInputAssets::ShieldToggleAction, ETriggerEvent::Started,   this, &APlayerCharacter::ToggleShieldInput);
	EnhancedInput->BindAction(FInputAssets::ShoulderSwapAction, ETriggerEvent::Started,   this, &APlayerCharacter::ShoulderSwap);

}

void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsValid(CameraSystemComponent))
	{
		GetCameraSystemComponent()->CameraTick(DeltaTime);
		if(GetCameraSystemComponent()->GetCameraInputMotion() == ECameraInputSpeed::DECELERATING)
		{
			FVector2D DeceleratingInput = CameraSystemComponent->GetDeceleratingLookVector(DeltaTime);
			AddControllerYawInput(DeceleratingInput.X);
			AddControllerPitchInput(DeceleratingInput.Y);
		}
		if (PreviousTickLocation == GetActorLocation())
		{
			GetCameraSystemComponent()->MakeCameraShake(EShakeSource::IDLE);
		}
	}
	PreviousTickLocation = GetActorLocation();
	
	if (IsAiming())
	{
		SetAimOffsetPitchVal(GetBaseAimRotation().Pitch);
	}
	
	if (GetCharacterMovement()->IsFalling())
	{
		if (GetPlayerGeneralState() != EPlayerState::Falling)
		{
			GetInteractiveMovementComponent()->SetFallInitialHeight(GetActorLocation().Z);
			bWantsToJump = false;
		}
		
		if (IsAiming())
		{
			ResetAiming();
		}

		if (IsReloading())
		{
			FGlobalPointers::Umbrella->InterruptReload();
		}
		
		if (GetPlayerGeneralState() != EPlayerState::InTraversalNoControls
			&& GetPlayerGeneralState() != EPlayerState::ControlsDisabled
			&& GetPlayerGeneralState() != EPlayerState::InComms
			&& CanChangeToPlayerState(EPlayerState::Falling))
		{
			SetPlayerGeneralState(EPlayerState::Falling);
		}
		
		if (GetInteractiveMovementComponent()->TryToMantle())
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::PlayFrancescaClimbing, .2f, false);
		}
	}
	else if (GetPlayerGeneralState() == EPlayerState::Falling) // If we are no longer falling but our PlayerState is Falling
	{
		ResetPlayerStateToFreeMovementAndCheckAim();
		
		GetInteractiveMovementComponent()->TryToRoll();
		GetInteractiveMovementComponent()->SetFallInitialHeight(0);
	}

	GetInteractiveMovementComponent()->TryToApplyJumpBoost(DeltaTime);
}

void APlayerCharacter::PlayFrancescaClimbing() const
{
	if (InGameVoicesComponent->SetCurrentEvent("FRANCESCA_CLIMB"))
	{
		InGameVoicesComponent->PlayEvent();
	}
}

void APlayerCharacter::Jump()
{
	FGlobalPointers::MainController->ParseInputDevice();
	const bool bIsAttacking = FGlobalPointers::Umbrella->GetMeleeComponent()->IsAttacking();
	const bool bIsInAStateThatCanJump = GetPlayerGeneralState() != EPlayerState::LimitedMovement
		&& GetPlayerGeneralState() != EPlayerState::InComms
		&& GetPlayerGeneralState() != EPlayerState::ControlsDisabled;
	
	if (IsAiming() || bIsAttacking || !bIsInAStateThatCanJump || GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying() || GetController()->IsMoveInputIgnored())
	{
		return;
	}
	
	if (IsReloading())
	{
		FGlobalPointers::Umbrella->InterruptReload();
	}

	if (GetCoverMovementComponent()->GetCoveredState() == ECoveredState::InCover)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			GetCoverMovementComponent()->NotifyExitCover();
			Jump();
		});
	}
	else if (bWantsToJump)
	{
		bWantsToJump = false;
		ACharacter::Jump();
		if ( !IsValid(AnimationsFmodComponent)
			|| !IsValid(InGameVoicesComponent)
			|| InGameVoicesComponent->IsEventPlaying()
			|| AnimationsFmodComponent->IsEventPlaying()
			|| bWantsToJump
			|| !InGameVoicesComponent->SetCurrentEvent("FRANCESCA_JUMP")
			|| !AnimationsFmodComponent->SetCurrentEvent("FRANCESCA_JUMP_EFFECT"))
		{
			return;
		}
		InGameVoicesComponent->PlayEvent();
		AnimationsFmodComponent->PlayEvent();
	}
	else
	{
		if (!GetInteractiveMovementComponent()->CheckSwing() && !GetInteractiveMovementComponent()->CheckVault() && GetPlayerGeneralState() != EPlayerState::InTraversalNoControls)
		{
			const FVector&  InputVector = GetLastMovementInputVector();
			const FVector2D InputXY(InputVector.X, InputVector.Y);
			 
			if (InputXY.SizeSquared() > KINDA_SMALL_NUMBER)
			{
				GetInteractiveMovementComponent()->SetIsApplyingJumpBoost(true);
				GetInteractiveMovementComponent()->SetJumpBoostRemaining(GetInteractiveMovementComponent()->GetJumpBoostDistance());
			}
			
			if (GetInteractiveMovementComponent()->CheckFloor(InputXY.SizeSquared()))
			{
				GetCharacterMovement()->StopActiveMovement();
				bWantsToJump = true;
				//FTimerHandle WantsToJumpTimer;
				//GetWorld()->GetTimerManager().SetTimer(WantsToJumpTimer, [this]() { bWantsToJump = false; }, 0.11f, false);
			}
			else
			{
				ACharacter::Jump();
			}
		}
	}
}

// TODO: This function is too big. Split up.
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.Move()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();

	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !GetPlayerStateComponent()->CanPlayerMove() || !IsValid(Controller) || GetStatusEffectComponent()->IsElectrified())
	{
		return;
	}

    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (MovementVector.Length() < 0.1f)
    {
    	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Move - Movement Vector is smaller than 0.1!"));
        return;
    }
	
    const FRotator& Rotation = FGlobalPointers::MainController->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector& RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (IsNoClip())
    {
    	if (!IsValid(GetCameraSystemComponent()) || !IsValid(GetCameraSystemComponent()->GetFollowCamera().Get()))
    	{
    		FLogger::ErrorLog("Invalid CameraSystemComponent pointer in PlayerCharacter.Move()");
    	}
    	
        ForwardDirection = GetCameraSystemComponent()->GetFollowCamera()->GetForwardVector();
    }

	if (!IsAiming())
	{
		GetCameraSystemComponent()->MakeCameraShake(EShakeSource::SWAY);
	}
	
    const FVector& CompositeDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;

    bool bFreeRoamBlocking = false;
    //if (IsValid(FreeRoamCoverComponent) && FreeRoamCoverComponent->ShouldBlockMovement())
    //{
    //    bFreeRoamBlocking = true;
    //    
    //    const FRotator TargetRotation = CompositeDirection.Rotation();
    //    SetActorRotation(FMath::RInterpTo(
    //        GetActorRotation(),
    //        TargetRotation,
    //        GetWorld()->GetDeltaSeconds(),
    //        FreeRoamCoverComponent->RotationInterpSpeed
    //    ));
    //}

    if (IsValid(GetCoverMovementComponent()) && (GetCoverMovementComponent()->IsMovementControlledByCover() || GetCoverMovementComponent()->IsTravellingToCover()))
    {
        if (!GetCoverMovementComponent()->IsTravellingToCover() && !bFreeRoamBlocking)
        {
            const float InCoverTraversingXInput = FVector::DotProduct(GetActorRightVector(), CompositeDirection);
            const float InCoverTraversingYInput = FVector::DotProduct(GetActorForwardVector(), CompositeDirection);
            
            GetCoverMovementComponent()->StoreInCoverInput(InCoverTraversingYInput, InCoverTraversingXInput);
        }
    }
    else if (!bFreeRoamBlocking)
    {
        if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->GetCoveredState() == ECoveredState::ExitingCover)
        {
            AddMovementInput(ForwardDirection, MovementVector.Y * UEasingLibrary::ApplyEase(GetCoverMovementComponent()->GetExitCoverTransitionCompletePercentage(), EEaseType::InQuart));
            AddMovementInput(RightDirection, MovementVector.X * UEasingLibrary::ApplyEase(GetCoverMovementComponent()->GetExitCoverTransitionCompletePercentage(), EEaseType::InQuart));
        }
        else
        {
            AddMovementInput(ForwardDirection, MovementVector.Y);
            AddMovementInput(RightDirection, MovementVector.X);
        }
        CameraSystemComponent->MoveAssistance(MovementVector, GetCoverMovementComponent()->IsMovementControlledByCover());
    }
}

void APlayerCharacter::StopMovingPlayer(const FInputActionValue& Value)
{
	if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		GetCoverMovementComponent()->StoreInCoverInput(0,0);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FGlobalPointers::MainController->ParseInputDevice();
	
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !IsValid(Controller))
	{
		return;
	}

	const FVector2D LookAxisVector = CameraSystemComponent->GetAdjustedLookVector(FGlobalPointers::MainController->GetInputDevice(), Value.Get<FVector2D>());
	AddControllerYawInput(LookAxisVector.X * GetCameraSystemComponent()->GetCameraLookMultiplierAimAssist());
	AddControllerPitchInput(LookAxisVector.Y * GetCameraSystemComponent()->GetCameraLookMultiplierAimAssist());
}

void APlayerCharacter::StopLook(const FInputActionValue& Value)
{
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !IsValid(CameraSystemComponent))
	{
		return;
	}
	
	CameraSystemComponent->SetCameraInputMotion(ECameraInputSpeed::DECELERATING);
}

void APlayerCharacter::StartedHoldingAimButton()
{
	bIsAimingButtonPressed = true;

	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return;
	}
	
	StartAiming();
}

void APlayerCharacter::ReleasedAimButton()
{
	bIsAimingButtonPressed = false;

	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return;
	}
	
	ResetAiming();
}

void APlayerCharacter::StartAiming()
{
	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->ParseInputDevice();
		FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
		if (IsValid(FGlobalPointers::MainController->GetMainHUD()))
		{
			if (!FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState() && CurrentUmbrellaLocation != EUmbrellaLocation::UmbrellaCenter)
			{
				FGlobalPointers::MainController->TogglePlayerHUD();
			}
		}
	}
	
	const bool bCanEnterAimingMode = (GetPlayerGeneralState() == EPlayerState::InCover && CanChangeToPlayerState(EPlayerState::InCoverAiming))
		|| (CanChangeToPlayerState(EPlayerState::AimingUmbrella) && GetPlayerGeneralState() != EPlayerState::InComms);
	
	const bool bIsTravellingToCover = IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsTravellingToCover();

	if (IsAiming() || !bCanEnterAimingMode || bIsTravellingToCover)
	{
		return;
	}
	
	CameraSystemComponent->SetCameraState(ECameraStateKey::AIM);
	UpdateAimState(true);
	GetInteractiveMovementComponent()->SetStrafing(true);
	PlayMontage(IdleToAimMontage, IdleToAimMontagePlayRate);

	if (UCharacterMovementComponent* UCharacterMovementComponent = GetCharacterMovement())
	{
		UCharacterMovementComponent->bUseControllerDesiredRotation = true;
		UCharacterMovementComponent->bOrientRotationToMovement = false;
		UCharacterMovementComponent->MaxWalkSpeed = GetInteractiveMovementComponent()->GetStrafeWalkSpeed();

		if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover())
		{
			SetPlayerGeneralState(EPlayerState::InCoverAiming);

			// If we are at the left-edge of a tall cover.
			UPlayerCoverMovementComponent* PlayerCoverMovementComponent = Cast<UPlayerCoverMovementComponent>(GetCoverMovementComponent());
			if (IsValid(PlayerCoverMovementComponent) && PlayerCoverMovementComponent->ShouldAimInCoverOverLeftShoulder())
			{
				// Setting the shoulder camera to be inverted.
				GetCameraSystemComponent()->SetCameraStateShoulder(ECameraStateKey::AIM, true);
			}
		}
		else
		{
			SetPlayerGeneralState(EPlayerState::AimingUmbrella);
		}
	}

	if (IsValid(AimAssistComponent))
	{
		AimAssistComponent->OnAimInputPressed();
	}
	ChangeUmbrellaLocation(EUmbrellaLocation::Hand, true, false);
}

void APlayerCharacter::ResetAiming()
{
	if (!IsAiming() || !CameraSystemComponent)
	{
		return;
	}
	
	const bool bHoldShieldActionHeld = IsValid(FGlobalPointers::Umbrella) ? FGlobalPointers::Umbrella->IsOpen() : false;
	AMainController* PC = Cast<AMainController>(GetWorld()->GetFirstPlayerController());
	
	if (bHoldShieldActionHeld && FGlobalPointers::MainController->GetInputDevice() != "KBM" && IsValid(PC) && PC->GetControllerInputMappingContextIndex() == 1)
	{
		return;
	}
	
	if (IsValid(FGlobalPointers::Umbrella) && FGlobalPointers::Umbrella->IsOpen())
	{
		TryToCloseShield();
	}
	
	UpdateAimState(false);
	
	PlayMontage(AimToIdleMontage, IdleToAimMontagePlayRate);

	if (IsValid(GetCoverMovementComponent()))
	{
		const ECameraStateKey NewCameraState = GetCoverMovementComponent()->IsMovementControlledByCover() ? ECameraStateKey::COVER : ECameraStateKey::DEFAULT;
		CameraSystemComponent->SetCameraState(NewCameraState);

		if (GetPlayerGeneralState() != EPlayerState::ControlsDisabled)
		{
			const EPlayerState NewPlayerState = GetCoverMovementComponent()->IsMovementControlledByCover() ? EPlayerState::InCover : EPlayerState::FreeMovement;
			SetPlayerGeneralState(NewPlayerState);
		}
		
		// Resetting the shoulder camera to be on the right.
		GetCameraSystemComponent()->SetCameraStateShoulder(ECameraStateKey::AIM, false);
	}
	
	UCharacterMovementComponent* UCharacterMovementComponent = GetCharacterMovement();
	if (IsValid(UCharacterMovementComponent))
	{
		UCharacterMovementComponent->MaxWalkSpeed = GetInteractiveMovementComponent()->GetMaxWalkSpeed();
		UCharacterMovementComponent->bUseControllerDesiredRotation = false;
	}
	if (!bIsInCombatVolume)
	{
		GetInteractiveMovementComponent()->SetStrafing(false);
		
		if (IsValid(UCharacterMovementComponent))
		{
			UCharacterMovementComponent->bOrientRotationToMovement = true;
			UCharacterMovementComponent->bUseControllerDesiredRotation = false;
		}
	}

	if (IsValid(AimAssistComponent))
	{
		AimAssistComponent->OnAimInputReleased();
		GetCameraSystemComponent()->SetCameraStateShoulder(ECameraStateKey::AIM, false);
		GetCameraSystemComponent()->ResetCameraPosition();
	}
	
	ChangeUmbrellaLocation(EUmbrellaLocation::Back, false, true);
}

void APlayerCharacter::Shoot() const
{
	if (!IsValid(FGlobalPointers::Umbrella) || GetStatusEffectComponent()->IsStunned() || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return;
	}
	
	if (FGlobalPointers::Umbrella->TryToShoot())
	{
		if (IsValid(CameraSystemComponent) && !bIsInSecurityCinematic)
		{
			CameraSystemComponent->MakeCameraShake(EShakeSource::RECOIL);
		}

		const float RecoilPlayRate = 1.0f / (GetWeapon()->GetShootingCadence() + RecoilAnimSpeedOffset);
		PlayMontage(FAnimationAssets::PlayerRecoilMontage, RecoilPlayRate);
					
		(void) OnPlayerShot.ExecuteIfBound();
	}
}

void APlayerCharacter::ShootingUmbrella()
{
	// TODO: Remove reference to MainController from player.
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.ShootingUmbrella()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();

	if (!GetPlayerStateComponent()->CanShootUmbrella())
	{
		return;
	}

	if (IsValid(FGlobalPointers::Umbrella))
	{
		if (IsAiming())
		{
			Shoot();
		}
		else if (!GetWeapon()->GetMeleeComponent()->IsAttacking())
		{
			GetWeapon()->GetMeleeComponent()->Attack();
		}
	}
}

void APlayerCharacter::Reload()
{
	// TODO: Remove reference to MainController from player.
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.Reload()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();

	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return;
	}
	
	if (GetPlayerStateComponent()->CanPlayerReload() && IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->StartReloading();
	}
}

void APlayerCharacter::PlayStepsSounds(const uint8 StepSurface)
{
	if (!IsValid(FmodComponentSteps) || !FmodComponentSteps->SetCurrentEvent(FmodComponentStepsEventName))
	{
		FLogger::ErrorLog("APlayerCharacter::PlayStepsSounds - Could not set current event to FmodComponentSteps");
		return;
	}
	FmodComponentSteps->SetParameter(FmodComponentStepsParameterName, StepSurface);
	FmodComponentSteps->PlayEvent();
}

void APlayerCharacter::PlayNotifiedVoiceSound(const FString& EventToPlay)
{
	if (!InGameVoicesComponent->SetCurrentEvent(EventToPlay))
	{
		FLogger::ErrorLog("APlayerCharacter::PlayNotifiedSound - Could not set current event to: " + EventToPlay + " in AnimationsFmodComponent.");
		return;
	}
	InGameVoicesComponent->PlayEvent();
}

void APlayerCharacter::PlayNotifiedActionSound(const FString& EventToPlay)
{
	if (!AnimationsFmodComponent->SetCurrentEvent(EventToPlay))
	{
		FLogger::ErrorLog("APlayerCharacter::PlayNotifiedSound - Could not set current event to: " + EventToPlay + " in AnimationsFmodComponent.");
		return;
	}
	AnimationsFmodComponent->PlayEvent();
}

void APlayerCharacter::PlayNotifiedActionSoundWithParameter(const FString& EventToPlay, const float NewParameter)
{
	if (!AnimationsFmodComponent->SetCurrentEvent(EventToPlay))
	{
		FLogger::ErrorLog("APlayerCharacter::PlayNotifiedSound - Could not set current event to: " + EventToPlay + " in AnimationsFmodComponent.");
		return;
	}
	AnimationsFmodComponent->SetParameter(*EventToPlay, NewParameter);
	AnimationsFmodComponent->PlayEvent();
}

void APlayerCharacter::PlayUmbrellaGadgetSound(const FString& EventToPlay)
{
	if (!IsValid(GetWeapon()->GetFmodGadgetsComponent()) || !GetWeapon()->GetFmodGadgetsComponent()->SetCurrentEvent(EventToPlay))
	{
		return;
	}
	GetWeapon()->GetFmodGadgetsComponent()->PlayEvent();
}

void APlayerCharacter::ResetSnapshot()
{
	if (GetHealthComponent()->GetHealth() < HealthSnapshotThreshold)
	{
		return;
	}
	
	FmodHealthSnapshot->StopEvent();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleHealthSnapshot);
}

void APlayerCharacter::ToggleShieldInput()
{
	// TODO: Remove reference to MainController from player.
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.ToggleShieldInput()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();
	
	if (!IsAiming() || !IsValid(FGlobalPointers::Umbrella))
	{
		return;
	}

	FGlobalPointers::Umbrella->IsOpen() ? TryToCloseShield() : TryToOpenShield();
}

void APlayerCharacter::ShoulderSwap()
{
	if (!IsValid(FGlobalPointers::MainController) || !IsValid(FGlobalPointers::Umbrella))
	{
		FLogger::ErrorLog("Invalid MainController or Umbrella pointer in PlayerCharacter.ToggleShieldInput()");
		return;
	}

	const bool bIsInCover = IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover();

	if (!IsAiming() || FGlobalPointers::Umbrella->IsOpen() || bIsInCover)
	{
		return;
	}
	
	GetCameraSystemComponent()->SetCameraStateShoulder(ECameraStateKey::AIM, !GetCameraSystemComponent()->GetCameraState(ECameraStateKey::AIM).bScreenRatioMirrored);
	FGlobalPointers::MainController->ParseInputDevice();
}

void APlayerCharacter::HoldShieldInput()
{
	// TODO: Remove reference to MainController from player.
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.HoldShieldInput()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();

	if (!IsAiming())
	{
		StartAiming();
	}
	
	if (!TryToOpenShield() && !bIsAimingButtonPressed)
	{
		ResetAiming();
	}
}

void APlayerCharacter::StopShieldInput()
{
	// TODO: Remove reference to MainController from player.
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.StopShieldInput()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();
	
	if (!TryToCloseShield())
	{
		return;
	}

	if (bIsAimingButtonPressed)
	{
		SetPlayerGeneralState(EPlayerState::AimingUmbrella);
	}
	else
	{
		ResetAiming();
	}
}

void APlayerCharacter::CheckForFloor()
{
	const UWorld* World = GetWorld();
	const FName CurrentLevelName = *UGameplayStatics::GetCurrentLevelName(World);

	if (IsValid(World))
	{
		FHitResult HitResult;
		
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		
		bool bHasHitSomething = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			GetActorLocation(),
			GetActorLocation() + (GetActorUpVector() * -FALLING_DISTANCE),
			ECC_Camera,
			CollisionQueryParams
		);

		if (bHasHitSomething &&
			CurrentLevelName.ToString().Equals(ChapterNames::ChapterList[3]) &&
			(HitResult.Distance > FALLING_ACHIEVE)) 
		{
			Steam::UnlockAchievement(ESteamAchievement::MORY_POPPIN);
		}
	}
}

bool APlayerCharacter::TryToOpenShield()
{
	const bool bIsFalling = GetCharacterMovement()->IsFalling();
	const bool bCanChangeToOpenShieldState = GetPlayerGeneralState() != EPlayerState::ControlsDisabled && CanChangeToPlayerState(EPlayerState::OpenShield);
	const bool bIsMovingBetweenCovers = IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->GetCoveredState() == ECoveredState::InCover && GetCoverMovementComponent()->GetInsideCoverState() == EInsideCoverState::MovingBetweenCovers;
	const bool bIsInComms = GetPlayerGeneralState() == EPlayerState::InComms;

	CheckForFloor();
	
	if (!IsValid(FGlobalPointers::Umbrella) || bIsFalling || !bCanChangeToOpenShieldState || bIsMovingBetweenCovers || bIsInComms)
	{
		return false;
	}

	// Trying to open the Shield
	if (!FGlobalPointers::Umbrella->TryToOpenShield())
	{
		return false;
	}
	
	SetPlayerGeneralState(EPlayerState::OpenShield);
	
	//if (FreeRoamCoverComponent && IsInFreeRoamCover())
	//{
	//	FreeRoamCoverComponent->ResetCoverVariables();
	//}
	return true;
}

bool APlayerCharacter::TryToCloseShield()
{
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !CanChangeToPlayerState(EPlayerState::FreeMovement) || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return false;
	}
	
	if (!FGlobalPointers::Umbrella->TryToCloseShield())
	{
		return false;
	}

	if (IsAiming())
	{
		if (IsValid(AimAssistComponent))
		{
			AimAssistComponent->OnAimInputPressed();
		}
		SetPlayerGeneralState(EPlayerState::AimingUmbrella);
	}
	else
	{
		SetPlayerGeneralState(EPlayerState::FreeMovement);
	}
	
	return true;
}

void APlayerCharacter::OnUmbrellaOpenStateChange(const bool bIsOpen)
{
	if (bIsOpen)
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = GetInteractiveMovementComponent()->GetOpenShieldWalkSpeed();
		}

		if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover())
		{
			GetCoverMovementComponent()->NotifyExitCover();
		}
	}
	else
	{
		if (IsAiming())
		{
			GetCharacterMovement()->MaxWalkSpeed = GetInteractiveMovementComponent()->GetStrafeWalkSpeed();

			if (!bIsAimingButtonPressed)
			{
				ResetAiming();
			}
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = GetInteractiveMovementComponent()->GetMaxWalkSpeed();
		}
	}
}

void APlayerCharacter::ActivateInComsMode(const bool IsCommunicationCinematic)
{
	if (!IsCommunicationCinematic)
	{
		return;
	}
	
	ResetAiming();
	//limited movement and Decrease velocity
	SetPlayerGeneralState(EPlayerState::InComms);
	SaveSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = VELOCITY_IN_GAME_CINEMATIC;
	
	GetStatusEffectComponent()->SetInComms(IsCommunicationCinematic);
	EUmbrellaLocation UmbrellaLocation = IsCommunicationCinematic ? EUmbrellaLocation::Back : EUmbrellaLocation::Hand;
	ChangeUmbrellaLocation(UmbrellaLocation, true, false);
}

void APlayerCharacter::DeActivateInComsMode(const bool IsCommunicationCinematic)
{
	if (!IsCommunicationCinematic)
	{
		return;
	}
	
	GetStatusEffectComponent()->SetInComms(false);
	ResetPlayerStateToFreeMovementAndCheckAim();
	GetCharacterMovement()->MaxWalkSpeed = SaveSpeed;
}

void APlayerCharacter::ChangeHandling(const float Value)
{
	GetInteractiveMovementComponent()->SetStrafeWalkSpeed(Value);
}

void APlayerCharacter::ChangeShieldMotion(const float Value)
{
	GetInteractiveMovementComponent()->SetOpenShieldWalkSpeed(Value);
}

void APlayerCharacter::PlayVoiceLine(const EVoiceLineState& VoiceLineState, const bool HasPriority, const bool queue)
{
	if (IsValid(GetVoiceLineComponent()))
	{
		if (queue)
		{
			GetVoiceLineComponent()->QueueVoiceLine(FVoiceLineDTO{VoiceLineState,EVoiceLineActor::PLAYER,this,HasPriority});
			return;
		}
		GetVoiceLineComponent()->PlayRandomVoiceLine(FVoiceLineDTO{VoiceLineState,EVoiceLineActor::PLAYER,this,HasPriority});
	}
}

void APlayerCharacter::UnlockGadgets(const EGadgetType GadgetToUnlock)
{
	GetWeapon()->UnlockGadgets(GadgetToUnlock);
}

void APlayerCharacter::GadgetShoot()
{
	const bool bCanShoot = GetPlayerGeneralState() == EPlayerState::AimingUmbrella
		|| GetPlayerGeneralState() == EPlayerState::InCoverAiming
		|| GetPlayerGeneralState() == EPlayerState::OpenShield;

	if (!IsValid(FGlobalPointers::Umbrella) || !bCanShoot || !IsAiming())
	{
		return;
	}
	
	const bool GadgetShot = FGlobalPointers::Umbrella->GadgetShoot();
	
	if (!GadgetShot || !bPlayerInCombat)
	{
		return;
	}

	FGlobalPointers::Umbrella->GetCurrentGadgetType() == EGadgetType::Grapple ? PlayVoiceLine(EVoiceLineState::GADGET_HOOK) : PlayVoiceLine(EVoiceLineState::GADGET_ELECTRIC);
}

void APlayerCharacter::CycleUmbrellaBullets()
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid MainController pointer in PlayerCharacter.CycleUmbrellaBullets()");
		return;
	}
	
	FGlobalPointers::MainController->ParseInputDevice();

	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled
		|| GetPlayerGeneralState() == EPlayerState::LimitedMovement || !IsValid(FGlobalPointers::Umbrella) || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return;
	}
	
	FGlobalPointers::Umbrella->CycleBulletType();
}

bool APlayerCharacter::CanSwapGadget() const
{
	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled
		|| GetPlayerGeneralState() == EPlayerState::LimitedMovement
		|| GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return false;
	}
	
	return true;
}

void APlayerCharacter::SwapToHook()
{
	// Reverse animation
	
	FGlobalPointers::MainController->ParseInputDevice();

	if (!CanSwapGadget() || !IsValid(FGlobalPointers::Umbrella))
	{
		return;
	}

	if (FGlobalPointers::Umbrella->GetGadgetTypeStructMap()[EGadgetType::Grapple])
	{
		if (IsValid(FGlobalPointers::MainController))
		{
			if (IsValid(FGlobalPointers::MainController->GetMainHUD()))
			{
				FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
				if (!FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState())
				{
					FGlobalPointers::MainController->TogglePlayerHUD();
				}
			}
		}
		FGlobalPointers::Umbrella->SetBulletType(EGadgetType::Grapple);
		(void) FGlobalPointers::Umbrella->GetFmodGadgetsComponent()->SetCurrentEvent(FString("GADGET_HOOK"));
		FGlobalPointers::Umbrella->GetFmodGadgetsComponent()->PlayEvent();
		FGlobalPointers::Umbrella->SpawnGadgetCycleParticle();
		if (!IsAiming())
		{
			FGlobalPointers::MainController->DeactivatePlayerHUDOnTimer();
		}
	}
}
void APlayerCharacter::SwapToElectric()
{
	// Forward animation
	
	FGlobalPointers::MainController->ParseInputDevice();
	
	if (!CanSwapGadget() || !IsValid(FGlobalPointers::Umbrella))
	{
		return;
	}
	if (FGlobalPointers::Umbrella->GetGadgetTypeStructMap()[EGadgetType::Electric])
	{
		if (IsValid(FGlobalPointers::MainController))
		{
			if (IsValid(FGlobalPointers::MainController->GetMainHUD()))
			{
				FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
				if (!FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState())
				{
					FGlobalPointers::MainController->TogglePlayerHUD();
				}
			}
		}
		FGlobalPointers::Umbrella->SetBulletType(EGadgetType::Electric);
		(void) FGlobalPointers::Umbrella->GetFmodGadgetsComponent()->SetCurrentEvent(FString("GADGET_ELECTRIC"));
		FGlobalPointers::Umbrella->GetFmodGadgetsComponent()->PlayEvent();
		FGlobalPointers::Umbrella->SpawnGadgetCycleParticle();
		if (!IsAiming())
		{
			FGlobalPointers::MainController->DeactivatePlayerHUDOnTimer();
		}
	}
}

void APlayerCharacter::MovePlayerViaGrapple(const FVector& EndPosition)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying, 0);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetInteractiveMovementComponent()->SetIsGrappling(true);
	
	FTimerDelegate Grappling_Delegate;
	Grappling_Delegate.BindUFunction(this, "Grappling", EndPosition); // Character is the parameter we wish to pass with the function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GrapplingDelay, Grappling_Delegate, GetWorld()->DeltaTimeSeconds, true);
}

void APlayerCharacter::Grappling(const FVector& EndPosition)
{
	// TODO: Remove reference to MainController from player.
	if (!IsValid(GetCharacterMovement()))
	{
		FLogger::ErrorLog("Invalid CharacterMovement pointer in PlayerCharacter.CycleUmbrellaBullets()");
		return;
	}
	
	if (GetInteractiveMovementComponent()->IsGrappling())
	{
		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		const FVector ActorLocation = GetActorLocation();
		MovementComponent->Velocity = VelocityMultiplier * UKismetMathLibrary::GetDirectionUnitVector(ActorLocation, EndPosition);
		
		const FVector CalculusVector = ActorLocation - EndPosition;
		FGlobalPointers::Umbrella->SetCableEndLocation( UKismetMathLibrary::InverseTransformLocation(FGlobalPointers::Umbrella->GetActorTransform(), GetActorLocation()));

		if (UKismetMathLibrary::NearlyEqual_FloatFloat(CalculusVector.Length(),0,100))
		{
			MovementComponent->StopMovementImmediately();
			GetInteractiveMovementComponent()->SetIsGrappling(false);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_GrapplingDelay);

			FGlobalPointers::Umbrella->GetCableComponent()->SetVisibility(false);
			FGlobalPointers::Umbrella->GetCableComponent()->SetComponentTickEnabled(false);
		}
	}
}

void APlayerCharacter::PlayGrappleAnim() const
{	
	PlayMontage(FAnimationAssets::PlayerGrappleMontage, GrappleMontagePlayRate);
}

// void APlayerCharacter::SaveUmbrellaData() const
// {
// 	ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance;
// 	if ( !IsValid(GameInstance))
// 	{
// 		return;
// 	}
//
// 	AUmbrella* Umbrella = GetWeapon();
// 	if (!IsValid(Umbrella))
// 	{
// 		return;
// 	}
// 	
// 	GameInstance->GetSaveStructPlayer().UmbrellaCurrentMagAmmo	= Umbrella->GetAmmoCurrent();
// 	GameInstance->GetSaveStructPlayer().UmbrellaInventoryAmmo	= Umbrella->GetAmmoReserve();
// 	GameInstance->GetSaveStructPlayer().CurrentUpgradePieces	= GetCurrentPiecesForUpgrades();
// }

// void APlayerCharacter::LoadUmbrellaData() const
// {
// 	ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance;
// 	if ( !IsValid(GameInstance))
// 	{
// 		return;
// 	}
//
// 	AUmbrella* Umbrella = GetWeapon();
// 	if (!IsValid(Umbrella))
// 	{
// 		return;
// 	}
//
// 	GetWeapon()->SetAmmoCurrent(GameInstance->GetSaveStructPlayer().UmbrellaCurrentMagAmmo);
// 	GetWeapon()->SetAmmoReserve(GameInstance->GetSaveStructPlayer().UmbrellaInventoryAmmo);
// }

void APlayerCharacter::SaveGame()
{
	ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance;

	if ( !IsValid(GameInstance) || !IsValid(GetHealthComponent())
		|| !IsValid(GetController()) || !IsValid(GetWeapon())
		|| !IsValid(GameInstance->GetSaveGameFile()) || !IsValid(GetWeapon()->GetUmbrellaUpgradeComponent()))
	{
		return;
	}
	
	FSaveStructPlayer PlayerSaveData;
	
	if (IsValid(GameInstance->GetLastCheckpointTrigger()))
	{
		PlayerSaveData.SaveFileLastPlayerTransform = GameInstance->GetLastCheckpointTrigger()->GetArrowTransform();
		PlayerSaveData.LastRightVector			   = GameInstance->GetLastCheckpointTrigger()->GetActorRightVector();
		PlayerSaveData.LastForwardVector		   = GameInstance->GetLastCheckpointTrigger()->GetArrowForwardVector();
	}
	
	// PlayerSaveData.UmbrellaCurrentMagAmmo = GameInstance->GetSaveStructPlayer().UmbrellaCurrentMagAmmo;
	// PlayerSaveData.UmbrellaInventoryAmmo = GameInstance->GetSaveStructPlayer().UmbrellaInventoryAmmo;
	// PlayerSaveData.CurrentUpgradePieces = GameInstance->GetSaveStructPlayer().CurrentUpgradePieces;
	
	//GameInstance->SetSaveStructPlayer(PlayerSaveData);
	
	PlayerSaveData.UmbrellaCurrentMaxAmmo	   = GetWeapon()->GetAmmoCurrentMax();
	PlayerSaveData.UmbrellaCurrentMagAmmo	   = GetWeapon()->GetAmmoCurrent();
	PlayerSaveData.UmbrellaInventoryAmmo       = GetWeapon()->GetAmmoReserve();
	PlayerSaveData.CurrentUpgradePieces		   = GetCurrentPiecesForUpgrades();
	
	GameInstance->GetSaveGameFile()->SetPlayerData(PlayerSaveData);
	
	GameInstance->GetSaveGameFile()->SetUpgradesMap(*GetWeapon()->GetUmbrellaUpgradeComponent()->GetUpgrades());
	GameInstance->GetSaveGameFile()->SetGadgetsUnlocked(FGlobalPointers::Umbrella->GetGadgetTypeStructMap());
}

void APlayerCharacter::LoadGame()
{
	const ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance;
	if (!IsValid(GameInstance) || !IsValid(GetHealthComponent()) || !IsValid(GetWeapon()))
	{
		return;
	}
	
	ULU_SaveGame* SaveGameFile = GameInstance->GetSaveGameFile();
	if (!IsValid(SaveGameFile))
	{
		FLogger::ErrorLog("APlayerCharacter::LoadGame: Invalid SaveGameFile");
		return;
	}

	AUmbrella* Umbrella = GetWeapon();
	if (!IsValid(Umbrella))
	{
		return;
	}

	Umbrella->SetGadgetsUnlocked(SaveGameFile->GetGadgetsUnlocked());
	Umbrella->UpdateVisualGadgetsOnLoad();
	Umbrella->SetAmmoCurrentMax(GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMaxAmmo);
	Umbrella->SetAmmoCurrent(GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMagAmmo);
	Umbrella->SetAmmoReserve(GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaInventoryAmmo);
	Umbrella->UpdateVisualAmmoOnLoad();
	Umbrella->GetUpgradeComponent()->SetUpgrades(SaveGameFile->GetUpgradesMap());

	ACheckPointTrigger* LastCheckPoint = GameInstance->GetLastCheckpointTrigger();
	if (IsValid(LastCheckPoint))
	{
		const FTransform LastActorTransform = SaveGameFile->GetPlayerData().SaveFileLastPlayerTransform;
		SetActorTransform(LastActorTransform);
	}

	CurrentPiecesForUpgrades = GameInstance->GetSaveGameFile()->GetPlayerData().CurrentUpgradePieces;

	// SetActorRotation(GetActorRotation() + FRotator(0,0,0));
	// if (IsValid(FGlobalPointers::MainController))
	// {
	// 	FGlobalPointers::MainController->SetControlRotation(FRotator(0,90,0));
	// }
}

void APlayerCharacter::SaveDataCheckPoint()
{
	ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance;
	if (!IsValid(GameInstance) || !IsValid(GetHealthComponent()) || !IsValid(FGlobalPointers::Umbrella))
	{
		return;
	}
	
	//GameInstance->GetSaveStructPlayer().SaveFileLastPlayerTransform = GetActorTransform();
	GameInstance->GetSaveStructPlayer().UmbrellaCurrentMaxAmmo		= FGlobalPointers::Umbrella->GetAmmoCurrentMax();
	GameInstance->GetSaveStructPlayer().UmbrellaCurrentMagAmmo		= FGlobalPointers::Umbrella->GetAmmoCurrent();
	GameInstance->GetSaveStructPlayer().UmbrellaInventoryAmmo		= FGlobalPointers::Umbrella->GetAmmoReserve();
	
}

void APlayerCharacter::LoadDataCheckPoint()
{
	ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance;
	if (!IsValid(GameInstance))
	{
		return;
	}
	
	FTransform LastActorTransform = GameInstance->GetSaveGameFile()->GetPlayerData().SaveFileLastPlayerTransform;

	const FTransform Offset = FTransform(FRotator(0,0,/*-9*/0), FVector::ZeroVector, FVector::ZeroVector);
	LastActorTransform = LastActorTransform + Offset;
	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->SetControlRotation(FRotator(0,90,0));
	}
	SetActorTransform(LastActorTransform);
	//GetController()->SetControlRotation(GetActorForwardVector().Rotation());
	if (IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->SetAmmoCurrentMax(GameInstance->GetSaveStructPlayer().UmbrellaCurrentMaxAmmo);
		FGlobalPointers::Umbrella->SetAmmoCurrent(GameInstance->GetSaveStructPlayer().UmbrellaCurrentMagAmmo);
		FGlobalPointers::Umbrella->SetAmmoReserve(GameInstance->GetSaveStructPlayer().UmbrellaInventoryAmmo);
		FGlobalPointers::Umbrella->UpdateVisualAmmoOnLoad();
	}
}

void APlayerCharacter::LoadPlayerFromDataTable()
{
	const FPlayerDataDriven* PlayerData = FDataTableHelpers::ValidateTableAndGetData<FPlayerDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);

	if (!PlayerData)
	{
		FLogger::ErrorLog("Player Data not loaded from Table [DataTableRowHandler.DataTable == nullptr]");
		return;
	}
	
	if (!IsValid(GetHealthComponent()))
	{
		return;
	}
	
	if (!IsValid(GetCoverMovementComponent()))
    {
    	return;
    }
	
	GetInteractiveMovementComponent()->SetMaxWalkSpeed(PlayerData->WalkSpeed);
	GetInteractiveMovementComponent()->SetStrafeWalkSpeed(PlayerData->StrafeAimingSpeed);
	GetInteractiveMovementComponent()->SetOpenShieldWalkSpeed(PlayerData->StrafeShieldingSpeed);
	
	GetHealthComponent()->SetMaxHealth(PlayerData->MaxHealth);
	GetHealthComponent()->SetRegenerationSpeed(PlayerData->RegenerationSpeed);
	GetHealthComponent()->SetRegenerationCooldown(PlayerData->RegenerationCoolDown);
	
	GetCoverMovementComponent()->MaxCoverBasicMovementSpeed = PlayerData->OnCoverSpeed;
	
	PlayerData->OnTalkingSpeed;
	PlayerData->MaxJumpHeight;
	PlayerData->MaxJumpDistance;
	PlayerData->AirControl;
	PlayerData->KickStaggerTime;
	PlayerData->DartStunTime;
}

bool APlayerCharacter::LandmarkInteractUnderComms(const TArray<AActor*>& InteractingActors) const
{
	bool bIsInComms = GetStatusEffectComponent()->IsInComms();
	bool bLandmarkInteract = InteractingActors.ContainsByPredicate([](const AActor* Actor)
		{
			return IsValid(Actor) && Actor->IsA(ALandmarkFocus::StaticClass());
		});
	
	return bLandmarkInteract && bIsInComms;
}

void APlayerCharacter::InteractingWithOverlappingElements(const FInputActionValue& Value)
{
	FGlobalPointers::MainController->ParseInputDevice();
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	
	if (!LandmarkInteractUnderComms(OverlappingActors) && (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || !GetPlayerStateComponent()->CanInteractWithOverlappingElements()))
	{
		return;
	}
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		IInteractInterface* InteractInterface = Cast<IInteractInterface>(OverlappingActor);
		if (!InteractInterface)
		{
			continue;
		}
		
		if (const int32 Currency = InteractInterface->Interacting())
		{
			if (OverlappingActor->IsA(AAmmo::StaticClass()))
			{
				return;
			}
			if (OverlappingActor->IsA(AUpgradePieces::StaticClass()))
			{
				UPiecesCount::AcquiredPieces.Broadcast(Currency, CurrentPiecesForUpgrades);
				CurrentPiecesForUpgrades += Currency;
				return;
			}
		}
		else if(InteractInterface->GetTypeOfInteraction() == PLAYER_CHARACTER)
		{
			InteractInterface->Interacting(this);
		}
	}
}

void APlayerCharacter::ToggleCover()
{
	FGlobalPointers::MainController->ParseInputDevice();

	if (GetPlayerGeneralState() == EPlayerState::ControlsDisabled || GetPlayerGeneralState() == EPlayerState::InComms)
	{
		return;
	}
	
	UPlayerCoverMovementComponent* PlayerCoverMovementComponent = Cast<UPlayerCoverMovementComponent>(GetCoverMovementComponent());
	if (!IsValid(PlayerCoverMovementComponent))
	{
		return;
	}
	
	if (GetCoverMovementComponent()->IsMovementControlledByCover() || GetCoverMovementComponent()->IsTravellingToCover())
	{
		if (IsValid(GetCameraSystemComponent()->GetCameraBoom().Get()))
		{
			GetCameraSystemComponent()->GetCameraBoom()->SetRelativeLocation(FVector(0,0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.75f));
		}
		GetCoverMovementComponent()->NotifyExitCover();
		ChangeUmbrellaLocation(EUmbrellaLocation::Back, false, true);
	}
	else if (CanChangeToPlayerState(EPlayerState::InCover))
	{
		if (PlayerCoverMovementComponent->PlayerTryToEnterCover())
		{
			if (IsValid(FGlobalPointers::Umbrella) && FGlobalPointers::Umbrella->IsOpen())
			{
				FGlobalPointers::Umbrella->TryToCloseShield();
			}
			
			//if (FreeRoamCoverComponent && IsInFreeRoamCover())
			//{
			//	FreeRoamCoverComponent->ResetCoverVariables();
			//}

			if (IsValid(GetCameraSystemComponent()->GetCameraBoom().Get()))
			{
				GetCameraSystemComponent()->GetCameraBoom()->SetRelativeLocation(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.75f));
				//GetCameraSystemComponent()->GetCameraBoom()->SetRelativeLocation(FVector(0,0, GetMesh()->GetLocalBounds().GetBox().GetSize().Z / 2.5f));
			}

			// If we are at the left-edge of a tall cover.
			if (IsAiming())
			{
				const bool bShouldAimOverLeftShoulder = PlayerCoverMovementComponent->ShouldAimInCoverOverLeftShoulder();
				
				// Setting the shoulder camera to be inverted.
				GetCameraSystemComponent()->SetCameraStateShoulder(ECameraStateKey::AIM, bShouldAimOverLeftShoulder);
			}
			
			ChangeUmbrellaLocation(EUmbrellaLocation::Hand, true, false);
		}
		//else
		//{
		//	if (FreeRoamCoverComponent)
		//	{
		//		FreeRoamCoverComponent->StartCover();
		//	}
		//}
	}
}

void APlayerCharacter::ResetWalkSpeedAfterFreeCover()
{
	switch (GetPlayerGeneralState()){
		case EPlayerState::OpenShield:
			GetCharacterMovement()->MaxWalkSpeed = GetInteractiveMovementComponent()->GetOpenShieldWalkSpeed();
			break;
		case EPlayerState::AimingUmbrella:
			GetCharacterMovement()->MaxWalkSpeed = GetInteractiveMovementComponent()->GetStrafeWalkSpeed();
			break;
		default:
			GetCharacterMovement()->MaxWalkSpeed = GetInteractiveMovementComponent()->GetMaxWalkSpeed();
			break;
	}
}

void APlayerCharacter::OnMeleeAttackListener()
{
	if (!CanChangeToPlayerState(EPlayerState::InMeleeAnimation))
	{
		return;
	}
	ChangeUmbrellaLocation(EUmbrellaLocation::Hand, true, false);
	if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		GetCoverMovementComponent()->NotifyExitCover();
	}
	
	SetPlayerGeneralState(EPlayerState::InMeleeAnimation);

	if (IsValid(GetMesh()))
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance) && IsValid(FAnimationAssets::PlayerMeleeMontage) && !AnimInstance->OnMontageEnded.IsAlreadyBound(this, &APlayerCharacter::OnMeleeCompleted))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::OnMeleeCompleted);
		}
	}
}

void APlayerCharacter::OnMeleeCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(Montage) && Montage == FAnimationAssets::PlayerMeleeMontage || Montage == FAnimationAssets::PlayerMeleeMontage2 || Montage == FAnimationAssets::PlayerMeleeMontage3)
	{
		if (IsValid(AnimInstance) && IsValid(FAnimationAssets::PlayerMeleeMontage))
		{
			ChangeUmbrellaLocation(EUmbrellaLocation::Back, false, true);
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::OnMeleeCompleted);
			if (IsValid(FGlobalPointers::Umbrella->GetMeleeComponent()))
			{
				FGlobalPointers::Umbrella->GetMeleeComponent()->SetIsAttacking(false);
				FGlobalPointers::Umbrella->GetMeleeComponent()->SetCanDetect(true);
			}
		}
		if (bInterrupted)
		{
			if (!IsDead() && CanChangeToPlayerState(EPlayerState::FreeMovement)
				&& GetPlayerGeneralState() != EPlayerState::OpenShield
				&& GetPlayerGeneralState() != EPlayerState::AimingUmbrella
				&& GetPlayerGeneralState() != EPlayerState::InTraversalNoControls)
			{
				SetPlayerGeneralState(EPlayerState::FreeMovement);
			}
			if (IsValid(FGlobalPointers::Umbrella->GetMeleeComponent()))
			{
				FGlobalPointers::Umbrella->GetMeleeComponent()->SetIsAttacking(false);
				FGlobalPointers::Umbrella->GetMeleeComponent()->SetCanDetect(true);
			}
		}
	}
}

void APlayerCharacter::OnVaultListener()
{
	Super::OnVaultListener();

	if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		GetCoverMovementComponent()->NotifyExitCover();
	}

	ResetAiming();

	if (!bPlayerInCombat)
	{
		return;
	}
	
	PlayVoiceLine(EVoiceLineState::VAULTING);
}

void APlayerCharacter::OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned)
{
	Super::OnStatusEffectChangeListener(StatusEffect, bStunned);

	if (!IsValid(GetStatusEffectComponent()))
	{
		FLogger::ErrorLog("Invalid StatusEffectComponent pointer in PlayerCharacter.OnStatusEffectChangeListener()");
	}
	
	if (GetStatusEffectComponent()->IsStunned())
	{
		ResetAiming();
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->StopAllMontages(0);
		SetPlayerGeneralState(EPlayerState::ControlsDisabled);

		return;
	}
	
	ResetPlayerStateToFreeMovementAndCheckAim();
}

// TODO: Move to HealthComponent.

void APlayerCharacter::SubtractHealth(const float HealthToSubtract, const EWeaponType DamageType, const FVector& LastDamageSourceLocation)
{
	if (IsGodmode() || GetPlayerGeneralState() == EPlayerState::InTraversalNoControls)
	{
		return;
	}
	if (HitReactionComponent->IsActive())
	{
		HitReactionComponent->ShowHitMarker(LastDamageSourceLocation);
	}
	
	// Snapshot
	UWorld* World = GetWorld();
	const bool bTimerActive = World->GetTimerManager().IsTimerActive(TimerHandleHealthSnapshot);
	if (GetHealth() < HealthSnapshotThreshold && !bTimerActive)
	{
			FmodHealthSnapshot->PlayEvent();
		World->GetTimerManager().SetTimer(TimerHandleHealthSnapshot, this, &ThisClass::ResetSnapshot, HealthSnapshotCallingRate, true);
	}
	Super::SubtractHealth(HealthToSubtract,DamageType);
}

void APlayerCharacter::KillPlayer()
{
	if (IsValid(GetHealthComponent()))
	{
		GetHealthComponent()->SubtractHealth(GetHealthComponent()->GetHealth());
	}
}

void APlayerCharacter::SetDead()
{
	Super::SetDead();
	
	FGlobalPointers::GameInstance->GetSubsystem<UVoiceLineManager>()->ResetManager();
	
	if (IsAiming())
	{
		ResetAiming();
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(Controller))
	{
		AnimInstance->StopAllMontages(0.f);
		Controller->SetIgnoreMoveInput(false);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->DisableInput(PlayerController);
	}
	
	SetPlayerGeneralState(EPlayerState::ControlsDisabled);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);

	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->StopMovementImmediately();
	}

	if (IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->AutoReload();
		FGlobalPointers::Umbrella->SetVisibility(false);
		FGlobalPointers::Umbrella->SetHealth(FGlobalPointers::Umbrella->GetMaxHealth());
	}
	
	if (IsValid(GetCoverMovementComponent()) && GetCoverMovementComponent()->IsMovementControlledByCover() || GetCoverMovementComponent()->IsTravellingToCover()) 
	{ 
		GetCoverMovementComponent()->NotifyExitCover(); 
	}
	
	CameraSystemComponent->ResetCameraPosition();
	CameraSystemComponent->ResetCameraParams();

	HitReactionComponent->Deactivate();

	if (IsValid(FGlobalPointers::GameInstance))
		FGlobalPointers::GameInstance->AddDeathsInLevel();

	if (IsValid(FmodComponentVoiceLines))
	{
		if (!FmodComponentVoiceLines->SetCurrentEvent("FRANCESCA_DYING"))
		{
			return;
		}
		FmodComponentVoiceLines->PlayEvent();
	}
	
	if (IsValid(DeathScreenComponent))
	{
		DeathScreenComponent->TriggerDeathScreen();
	}
	else
	{
		// If we can't find a Death Screen Component, we Respawn immediately.
		Respawn();
		// We also reset our input immediately.
		ResetInputAfterRespawnFadeFinishes();	
	}

	FGlobalPointers::Umbrella->GetMeleeComponent()->SetIsAttacking(false);
	FGlobalPointers::Umbrella->GetMeleeComponent()->SetCanDetect(true);
}

void APlayerCharacter::Respawn()
{
	HitReactionComponent->Activate();
	
	if (IsValid(GetMesh()))
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetRelativeTransform(InitialMeshRelativeTransform);
	}

	if (!IsValid(FGlobalPointers::Umbrella) || !IsValid(FGlobalPointers::GameInstance))
	{
		FLogger::ErrorLog("Invalid global pointers in APlayerCharacter.Respawn()");
		return;
	}
	
	FGlobalPointers::Umbrella->SetVisibility(true);
	FGlobalPointers::GameInstance->LoadCheckpoint();
	CheckpointCameraReset();
	
	GetHealthComponent()->SetHealth(GetHealthComponent()->GetMaxHealth());
	ResetSnapshot();
}

void APlayerCharacter::CheckpointCameraReset()
{
	if (!IsValid(FGlobalPointers::MainController) || !IsValid(FGlobalPointers::GameInstance))
	{
		FLogger::ErrorLog("Invalid global pointers in APlayerCharacter.Respawn()");
		return;
	}

	FVector& Forward = FGlobalPointers::GameInstance->GetSaveStructPlayer().LastForwardVector;
	SetActorRotation(Forward.Rotation());

	 FVector CameraForward = GetCameraSystemComponent()->GetFollowCamera()->GetForwardVector();
	
	Forward.Z = 0.0f;
	CameraForward.Z = 0.0f;
		
	// FLogger::DebugLog("[%f, %f, %f]", Forward.Y, CameraForward.Y, FVector::DotProduct(Forward, CameraForward));

	if (FVector::DotProduct(Forward, CameraForward) < 0.0f)
	{
		CameraForward = -CameraForward;
	}
	
	Forward.Normalize();
	CameraForward.Normalize();
	
	FRotator TargetRot = Forward.Rotation();

	FRotator ControlRotation = FGlobalPointers::MainController->GetControlRotation();
	ControlRotation.Yaw = TargetRot.Yaw;
	FGlobalPointers::MainController->SetControlRotation(ControlRotation);
}

void APlayerCharacter::ResetInputAfterRespawnFadeFinishes()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->EnableInput(PlayerController);
	}
 
 	ResetPlayerStateToFreeMovementAndCheckAim();
}

EPlayerState APlayerCharacter::GetPlayerGeneralState() const
{
	if (IsValid(GetPlayerStateComponent()))
	{
		return GetPlayerStateComponent()->GetPlayerState();
	}

#if UE_EDITOR
	FLogger::ErrorLog("Couldn't Find Player State Component.");
#endif
	
	return EPlayerState::FreeMovement;
}

bool APlayerCharacter::SetPlayerGeneralState(EPlayerState NewPlayerState)
{
	if (IsValid(GetPlayerStateComponent()))
	{
		return GetPlayerStateComponent()->SetPlayerState(NewPlayerState);
	}

#if UE_EDITOR
	FLogger::ErrorLog("Couldn't Find Player State Component.");
#endif

	return false;
}

bool APlayerCharacter::CanChangeToPlayerState(EPlayerState NewPlayerState)
{
	if (IsValid(GetPlayerStateComponent()))
	{
		return GetPlayerStateComponent()->CanChangeToPlayerState(NewPlayerState);
	}

#if UE_EDITOR
	FLogger::ErrorLog("Couldn't Find Player State Component.");
#endif

	return false;
}

void APlayerCharacter::ResetPlayerStateToFreeMovementAndCheckAim()
{
	SetPlayerGeneralState(EPlayerState::FreeMovement);

	if (IsHoldingAimButton())
	{
		StartAiming();
	}
}

bool APlayerCharacter::PlayMotionWarpingMontage(UAnimMontage* Montage, const float PlayRate, FMotionWarpingTarget MotionWarpingTarget) const
{
	MotionWarpingComponent->AddOrUpdateWarpTarget(MotionWarpingTarget);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(Montage))
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_SetPlayRate(Montage, PlayRate);
		return true;
	}
	return false;
}