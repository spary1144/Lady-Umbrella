//  
// GenericProgressBar.h
// 
// Generic progress bar with percentage calculation.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenericProgressBar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UGenericProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UProgressBar* GetGenericProgressBar() const;
	float GetMaximumValue() const;
	float GetPercentageValue() const;
	
	void SetGenericProgressBar(UProgressBar* NewProgressBar);
	void SetMaximumValue(const float Value);
	void SetPercentageValue(const float Value);
	
private:

	UPROPERTY(BlueprintReadWrite, Category = "ProgressBar", meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* ProgressBar;
	
	float MaximumValue;
	float Percentage;
	
protected:

	virtual void UpdateProgressBar(const float Value);
	
};
