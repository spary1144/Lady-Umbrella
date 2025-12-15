// Fill out your copyright notice in the Description page of Project Settings.


#include "EasingLibrary.h"

#include "LadyUmbrella/Util/LoggerUtil.h"

float UEasingLibrary::ApplyEase(float ValueToEase, EEaseType EaseType)
{
#if WITH_EDITORONLY_DATA
	if (ValueToEase < 0 || ValueToEase > 1)
	{
		#if UE_EDITOR
		FLogger::WarningLog("Trying to ease a value of %f. The value must be in the [0, 1] range.", ValueToEase);
		#endif
	}
#endif
	
	ValueToEase = FMath::Clamp(ValueToEase, 0.0f, 1.0f);
	
	switch (EaseType)
	{
	case EEaseType::NoEase:
		return ValueToEase;
		
	case EEaseType::InSine:
		return InSineEase(ValueToEase);    
	case EEaseType::OutSine:
		return OutSineEase(ValueToEase);  
	case EEaseType::InOutSine:
		return InOutSineEase(ValueToEase);
		
	case EEaseType::InQuad:
		return InQuadEase(ValueToEase);   
	case EEaseType::OutQuad:
		return OutQuadEase(ValueToEase);  
	case EEaseType::InOutQuad:
		return InOutQuadEase(ValueToEase);
		
	case EEaseType::InCubic:
		return InCubicEase(ValueToEase);   
	case EEaseType::OutCubic:
		return OutCubicEase(ValueToEase);  
	case EEaseType::InOutCubic:
		return InOutCubicEase(ValueToEase);
		
	case EEaseType::InQuart:
		return InQuartEase(ValueToEase);   
	case EEaseType::OutQuart:
		return OutQuartEase(ValueToEase);  
	case EEaseType::InOutQuart:
		return InOutQuartEase(ValueToEase);
		
	case EEaseType::InQuint:
		return InQuintEase(ValueToEase);   
	case EEaseType::OutQuint:
		return OutQuintEase(ValueToEase);  
	case EEaseType::InOutQuint:
		return InOutQuintEase(ValueToEase);
	
	case EEaseType::InExpo:
		return InExpoEase(ValueToEase);   
	case EEaseType::OutExpo:
		return OutExpoEase(ValueToEase);  
	case EEaseType::InOutExpo:
		return InOutExpoEase(ValueToEase);
		
	case EEaseType::InCirc:
		return InCircEase(ValueToEase);   
	case EEaseType::OutCirc:
		return OutCircEase(ValueToEase);  
	case EEaseType::InOutCirc:
		return InOutCircEase(ValueToEase);
		
	case EEaseType::InBack:
		return InBackEase(ValueToEase);   
	case EEaseType::OutBack:
		return OutBackEase(ValueToEase);  
	case EEaseType::InOutBack:
		return InOutBackEase(ValueToEase);
		
	case EEaseType::InElastic:
		return InElasticEase(ValueToEase);   
	case EEaseType::OutElastic:
		return OutElasticEase(ValueToEase);  
	case EEaseType::InOutElastic:
		return InOutElasticEase(ValueToEase);
		
	case EEaseType::InBounce:
		return InBounceEase(ValueToEase);   
	case EEaseType::OutBounce:
		return OutBounceEase(ValueToEase);  
	case EEaseType::InOutBounce:
		return InOutBounceEase(ValueToEase);
		
	default:
		#if UE_EDITOR
		FLogger::WarningLog("Ease Type [%i] not identified. Please go to Pablo about it and force him to work.", static_cast<int>(EaseType));
		#endif
		
		return ValueToEase;
	}
}
