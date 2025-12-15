// 
// InputAssets.h
// 
// Collection of pre-cached input uassets.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

class UInputAction;

namespace FInputAssets
{
	inline TObjectPtr<UInputAction> AimAction      = nullptr;
	inline TObjectPtr<UInputAction> AttackAction   = nullptr;
	inline TObjectPtr<UInputAction> CoverAction    = nullptr;
	inline TObjectPtr<UInputAction> GadgetAction   = nullptr;
	inline TObjectPtr<UInputAction> JumpAction     = nullptr;
	inline TObjectPtr<UInputAction> InteractAction = nullptr;
	inline TObjectPtr<UInputAction> LookAction     = nullptr;
	inline TObjectPtr<UInputAction> MoveAction     = nullptr;
	inline TObjectPtr<UInputAction> PauseAction    = nullptr;
	inline TObjectPtr<UInputAction> ReloadAction   = nullptr;

	inline TObjectPtr<UInputAction> SelectNextGadgetAction   = nullptr;
	inline TObjectPtr<UInputAction> SelectFirstGadgetAction  = nullptr;
	inline TObjectPtr<UInputAction> SelectSecondGadgetAction = nullptr;
	inline TObjectPtr<UInputAction> ShowPlayerHUD = nullptr;

	// TODO: These inputs need to be converted to a settings option and can then be removed.
	inline TObjectPtr<UInputAction> ShieldHoldAction   = nullptr;
	inline TObjectPtr<UInputAction> ShieldToggleAction = nullptr;
	inline TObjectPtr<UInputAction> ShoulderSwapAction = nullptr;

	// TODO: These inputs need to be removed when releasing the final production build.
	inline TObjectPtr<UInputAction> CheatAction = nullptr;
	inline TObjectPtr<UInputAction> ActivateNextControllerMappingContextAction = nullptr;
	inline TObjectPtr<UInputAction> ActivateNextKeyboardMappingContextAction   = nullptr;
};
