// 
// StatusEffectDTO.h
// 
// Contains all data related to the different status effects available in the game.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

class UNiagaraComponent;
class UNiagaraSystem;

struct FStatusEffectDTO
{
	FTimerHandle TimerHandle;

	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	TObjectPtr<UNiagaraSystem>    NiagaraSystem;
};
