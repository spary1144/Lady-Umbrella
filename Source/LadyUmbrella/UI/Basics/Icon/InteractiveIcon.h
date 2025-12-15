// 
// InteractiveIcon.h
// 
// Interactive icons for player interactions. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractiveIcon.generated.h"

class APlayerCharacter;
class UCanvasPanel;
class UImage;
class UPlatformDependantWidget;

UCLASS()
class LADYUMBRELLA_API UInteractiveIcon : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* CircleImage;

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* ProhibitedImage;
	
	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UPlatformDependantWidget* PlatformKey;
	
	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* ArrowImage;

	UPROPERTY(BlueprintReadWrite, Category = "Image", meta = (BindWidget, AllowPrivateAccess = "true"))
	UCanvasPanel* InteractiveIconPanel;

	UPROPERTY(EditAnywhere, Category="Arrow Offset", meta = (AllowPrivateAccess = true))
	float MinimumOffset;

	UPROPERTY(EditAnywhere, Category="Arrow Offset", meta = (AllowPrivateAccess = true))
	float MaximumOffset;
	
	// NOTE: The default values cannot be inlined in the constructor as UUserWidget demands control of the constructor.
	bool bShouldShow      = true;
	bool bShouldOffset    = false;
	float InterpolateSize = 1.0f;
	FVector TraceOffset   = FVector(0.0f, 0.0f, 10.0f);

	int32 ViewportWidth      = 0, ViewportHeight = 0;
	float OffscreenOffset    = 25.0f;
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	
public:

	bool Update(const AActor* Owner, const float OuterSphereSize, const float OuterSphereTriggerRadius,
		const float InnerSphereSize, const float InnerSphereTriggerRadius, const float AnimationSpeed,
		const bool bCanInteract, const FVector RootOffset);

	UImage* GetCircleImage() const;
	UPlatformDependantWidget* GetPlatformKey() const;
	UImage* GetProhibitedImage() const;
	UImage* GetArrowImage() const;

	bool IsIconOnScreen(APlayerController* PlayerController, FVector WidgetWorldPos);

	FORCEINLINE bool ShouldShow() const { return bShouldShow; }
	FORCEINLINE void SetShouldShow(const bool Value) { bShouldShow = Value; }
	FORCEINLINE void SetShouldOffset(const bool Value) { bShouldOffset = Value; }
	FORCEINLINE void SetTraceOffset(const FVector& Value) { TraceOffset = Value; }

private:
	
	virtual void NativeConstruct() override;

	bool ShowInnerIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter, const float InnerSphereSize,
		const float AnimationSpeed, const bool bCanInteract);
	bool ShowOuterIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter, const float OuterSphereSize,
		const float AnimationSpeed);

	bool AnimateIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter, const float DesiredSize,
		const float AnimationSpeed);

	bool CheckCollisionBeforeCamera(const AActor* Owner, const APlayerCharacter* PlayerCharacter) const;
	bool IsOutsideFrustum(const AActor* Owner, const float IconFrustumMargin);
	
};
