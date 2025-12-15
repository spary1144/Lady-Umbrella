#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EWidgetDisplay : uint8
{
	ONCE,
	REPEATING,
	ONCE_TIMED,
	REPEATING_TIMED,
};
