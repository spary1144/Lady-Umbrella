// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EasingLibrary.generated.h"

UENUM(BlueprintType)
enum class EEaseType : uint8
{
	NoEase	   UMETA(DisplayName = "NoEase"),
	
	InSine     UMETA(DisplayName = "InSine"),
	OutSine     UMETA(DisplayName = "OutSine"),
	InOutSine     UMETA(DisplayName = "InOutSine"),

	InQuad     UMETA(DisplayName = "InQuad"),
	OutQuad     UMETA(DisplayName = "OutQuad"),
	InOutQuad     UMETA(DisplayName = "InOutQuad"),
	
	InCubic     UMETA(DisplayName = "InCubic"),
	OutCubic     UMETA(DisplayName = "OutCubic"),
	InOutCubic     UMETA(DisplayName = "InOutCubic"),
	
	InQuart     UMETA(DisplayName = "InQuart"),
	OutQuart     UMETA(DisplayName = "OutQuart"),
	InOutQuart     UMETA(DisplayName = "InOutQuart"),

	InQuint     UMETA(DisplayName = "InQuint"),
	OutQuint     UMETA(DisplayName = "OutQuint"),
	InOutQuint     UMETA(DisplayName = "InOutQuint"),

	InExpo     UMETA(DisplayName = "InExpo"),
	OutExpo     UMETA(DisplayName = "OutExpo"),
	InOutExpo     UMETA(DisplayName = "InOutExpo"),

	InCirc     UMETA(DisplayName = "InCirc"),
	OutCirc     UMETA(DisplayName = "OutCirc"),
	InOutCirc     UMETA(DisplayName = "InOutCirc"),

	InBack     UMETA(DisplayName = "InBack"),
	OutBack     UMETA(DisplayName = "OutBack"),
	InOutBack     UMETA(DisplayName = "InOutBack"),

	InElastic     UMETA(DisplayName = "InElastic"),
	OutElastic     UMETA(DisplayName = "OutElastic"),
	InOutElastic     UMETA(DisplayName = "InOutElastic"),

	InBounce     UMETA(DisplayName = "InBounce"),
	OutBounce     UMETA(DisplayName = "OutBounce"),
	InOutBounce     UMETA(DisplayName = "InOutBounce"),
};

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UEasingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

#pragma region CONSTANTS

	// Back Easing
	FORCEINLINE consteval static float Back_Const1() { return 1.70158f; }
	FORCEINLINE consteval static float Back_Const2() { return Back_Const1() * 1.525f; }
	FORCEINLINE consteval static float Back_Const3() { return Back_Const1() + 1.0f; }

	// Elastic Easing
	FORCEINLINE consteval static float Elastic_PI() { return 3.1415926535897932f; }
	FORCEINLINE consteval static float Elastic_Const1() { return 2.0f * Elastic_PI() / 3.0f; }
	FORCEINLINE consteval static float Elastic_Const2() { return 2.0f * Elastic_PI() / 4.5f; }

	// Bounce Easing
	FORCEINLINE consteval static float Bounce_Const1() { return 7.5625f; }
	FORCEINLINE consteval static float Bounce_Const2() { return 2.75f; }

#pragma endregion 
	
public:
	static float ApplyEase(float ValueToEase, EEaseType EaseType);


#pragma region EASE FUNCTIONS
	
private:
	FORCEINLINE static float InSineEase(const float ValueToEase) { return 1 - FMath::Cos(ValueToEase * PI / 2.0f); };
	FORCEINLINE static float OutSineEase(const float ValueToEase) { return FMath::Sin(ValueToEase * PI / 2.0f); };
	FORCEINLINE static float InOutSineEase(const float ValueToEase) { return -(FMath::Cos(PI * ValueToEase) - 1) / 2; };
	
	FORCEINLINE static float InQuadEase(const float ValueToEase) { return ValueToEase * ValueToEase; };
	FORCEINLINE static float OutQuadEase(const float ValueToEase) { return 1 - (1 - ValueToEase) * (1 - ValueToEase); };
	FORCEINLINE static float InOutQuadEase(const float ValueToEase) { return (ValueToEase < 0.5) ? 2 * InQuadEase(ValueToEase) : 1 - FMath::Pow(-2 * ValueToEase + 2, 2) / 2; };
		
	FORCEINLINE static float InCubicEase(const float ValueToEase) { return ValueToEase * ValueToEase * ValueToEase; };
	FORCEINLINE static float OutCubicEase(const float ValueToEase) { return 1 - FMath::Pow(1 - ValueToEase, 3); };
	FORCEINLINE static float InOutCubicEase(const float ValueToEase) { return ValueToEase < 0.5 ? 4 * InCubicEase(ValueToEase) : 1 - FMath::Pow(-2 * ValueToEase + 2, 3) / 2; };
		
	FORCEINLINE static float InQuartEase(const float ValueToEase) { return ValueToEase * ValueToEase * ValueToEase * ValueToEase; };
	FORCEINLINE static float OutQuartEase(const float ValueToEase) { return 1 - FMath::Pow(1 - ValueToEase, 4); };
	FORCEINLINE static float InOutQuartEase(const float ValueToEase) { return (ValueToEase < 0.5) ? 8 * InQuartEase(ValueToEase) : 1 - FMath::Pow(-2 * ValueToEase + 2, 4) / 2; };
	
	FORCEINLINE static float InQuintEase(const float ValueToEase) { return ValueToEase * ValueToEase * ValueToEase * ValueToEase * ValueToEase; };
	FORCEINLINE static float OutQuintEase(const float ValueToEase) { return 1 - FMath::Pow(1 - ValueToEase, 5); };
	FORCEINLINE static float InOutQuintEase(const float ValueToEase) { return ValueToEase < 0.5 ? 16 * InQuintEase(ValueToEase) : 1 - FMath::Pow(-2 * ValueToEase + 2, 5) / 2; };
	
	FORCEINLINE static float InExpoEase(const float ValueToEase) { return ValueToEase == 0 ? 0 : FMath::Pow(2.0f, 10.0f * ValueToEase - 10.0f); };
	FORCEINLINE static float OutExpoEase(const float ValueToEase) { return ValueToEase == 1 ? 1 : 1.0f - FMath::Pow(2.0f, -10.0f * ValueToEase); };
	FORCEINLINE static float InOutExpoEase(const float ValueToEase)
	{
		return ValueToEase == 0 || ValueToEase == 1 ? ValueToEase : 
			(ValueToEase < 0.5 ? FMath::Pow(2.0f, 20.0f * ValueToEase - 10.0f) / 2.0f
				: (2.0f - FMath::Pow(2.0f, -20.0f * ValueToEase + 10.0f)) / 2.0f);
	};
	
	FORCEINLINE static float InCircEase(const float ValueToEase) { return 1 - FMath::Sqrt(1.0f - FMath::Pow(ValueToEase, 2.0f)); };
	FORCEINLINE static float OutCircEase(const float ValueToEase) { return FMath::Sqrt(1.0f - FMath::Pow(ValueToEase - 1.0f, 2.0f)); };
	FORCEINLINE static float InOutCircEase(const float ValueToEase)
	{
		return ValueToEase < 0.5
			? (1.0f - FMath::Sqrt(1.0f - FMath::Pow(2.0f * ValueToEase, 2.0f))) / 2.0f
			: (FMath::Sqrt(1.0f - FMath::Pow(-2.0f * ValueToEase + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	};
	
	FORCEINLINE static float InBackEase(const float ValueToEase)
	{
		return Back_Const3() * InCubicEase(ValueToEase) - Back_Const1() * InQuadEase(ValueToEase);
	};
	FORCEINLINE static float OutBackEase(const float ValueToEase)
	{
		return 1.0f + Back_Const3() * FMath::Pow(ValueToEase - 1.0f, 3.0f) + Back_Const1() * FMath::Pow(ValueToEase - 1.0f, 2.0f);
	};
	FORCEINLINE static float InOutBackEase(const float ValueToEase)
	{
		return ValueToEase < 0.5
			? FMath::Pow(2.0f * ValueToEase, 2.0f) * ((Back_Const2() + 1.0f) * 2.0f * ValueToEase - Back_Const2()) / 2.0f
			: (FMath::Pow(2.0f * ValueToEase - 2.0f, 2.0f) * ((Back_Const2() + 1.0f) * (ValueToEase * 2.0f - 2.0f) + Back_Const2()) + 2) / 2.0f;
	};
	
	FORCEINLINE static float InElasticEase(const float ValueToEase)
	{
		return ValueToEase == 0 || ValueToEase == 1 ? ValueToEase
			: -FMath::Pow(2.0f, 10.0f * ValueToEase - 10.0f) * FMath::Sin((ValueToEase * 10.0f - 10.75f) * Elastic_Const1());
	};
	FORCEINLINE static float OutElasticEase(const float ValueToEase)
	{
		return ValueToEase == 0 || ValueToEase == 1 ? ValueToEase
			: FMath::Pow(2.0f, -10.0f * ValueToEase) * FMath::Sin((ValueToEase * 10.0f - 0.75f) * Elastic_Const1()) + 1.0f;
	};
	FORCEINLINE static float InOutElasticEase(const float ValueToEase)
	{
		return ValueToEase == 0 || ValueToEase == 1 ? ValueToEase
			: ValueToEase < 0.5
				? -(FMath::Pow(2.0f, 20.0f * ValueToEase - 10.0f) * FMath::Sin((20.0f * ValueToEase - 11.125f) * Elastic_Const2())) / 2.0f
				: FMath::Pow(2.0f, -20.0f * ValueToEase + 10.0f) * FMath::Sin((20.0f * ValueToEase - 11.125f) * Elastic_Const2()) / 2.0f + 1.0f;
	};
	
	FORCEINLINE static float InBounceEase(const float ValueToEase) { return 1 - OutBounceEase(1 - ValueToEase); };
	FORCEINLINE static float OutBounceEase(const float ValueToEase)
	{			// If 1
		return ValueToEase < 1 / Bounce_Const2()
				// Then 1
				? Bounce_Const1() * ValueToEase * ValueToEase

				// If 2
				: ValueToEase < 2 / Bounce_Const2()
				// Then 2
				? Bounce_Const1() * (ValueToEase - 1.5f / Bounce_Const2()) * ValueToEase + 0.75f

				// If 3
				: ValueToEase < 2.5 / Bounce_Const2()
				// Then 3
				? Bounce_Const1() * (ValueToEase - 2.25f / Bounce_Const2()) * ValueToEase + 0.9375f

				// Else
				: Bounce_Const1() * (ValueToEase - 2.625f / Bounce_Const2()) * ValueToEase + 0.984375f;
	};
	FORCEINLINE static float InOutBounceEase(const float ValueToEase)
	{
		return ValueToEase < 0.5
  			? (1.0f - OutBounceEase(1.0f - 2.0f * ValueToEase)) / 2.0f
  			: (1.0f + OutBounceEase(2.0f * ValueToEase - 1.0f)) / 2.0f;
	};

#pragma endregion 
};
