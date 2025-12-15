// 
// FocusPlayer.h
// 
// Service to focus agent into player
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_DefaultFocus.h"
#include "FocusPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UFocusPlayer : public UBTService_DefaultFocus
{
	GENERATED_BODY()

	UFocusPlayer();
};
