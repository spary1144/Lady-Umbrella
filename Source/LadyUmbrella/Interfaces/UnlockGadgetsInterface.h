#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Weapons/Umbrella/GadgetType.h"
#include "UObject/Interface.h"
#include "UnlockGadgetsInterface.generated.h"

UINTERFACE(MinimalAPI)
class UUnlockGadgetsInterface : public UInterface
{
	GENERATED_BODY()
};

class LADYUMBRELLA_API IUnlockGadgetsInterface
{
	GENERATED_BODY()

public:
	virtual void UnlockGadgets(EGadgetType GadgetToUnlock) = 0;
	
};
