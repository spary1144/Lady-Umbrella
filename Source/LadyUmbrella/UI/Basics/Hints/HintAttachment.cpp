#include "HintAttachment.h"

#include "Components/BoxComponent.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

UHintAttachment::UHintAttachment()
{
	HintShowArea = CreateDefaultSubobject<UBoxComponent>("Hint Area");
	HintShowArea->SetupAttachment(this);
	
	ScreenOffset = FVector2D::ZeroVector;

	TitleText = FText::GetEmpty();
	DescriptionText = FText::GetEmpty();

}

void UHintAttachment::LoadGame()
{
	bHasBeenShown = false;
}

void UHintAttachment::LoadDataCheckPoint()
{
	LoadGame();
}

void UHintAttachment::SaveGame()
{
}

void UHintAttachment::SaveDataCheckPoint()
{
}
void UHintAttachment::BeginPlay()
{
	Super::BeginPlay();

	if (!HintShowArea->OnComponentBeginOverlap.IsAlreadyBound(this, &UHintAttachment::OnBeginOverlap))
	{
		HintShowArea->OnComponentBeginOverlap.AddDynamic(this, &UHintAttachment::OnBeginOverlap);

	}
	if (!HintShowArea->OnComponentEndOverlap.IsAlreadyBound(this, &UHintAttachment::OnEndOverlap))
	{
		HintShowArea->OnComponentEndOverlap.AddDynamic(this, &UHintAttachment::OnEndOverlap);
	}
	
	SetRelativeLocation(FVector(0.f, ScreenOffset.X, ScreenOffset.Y));
}

void UHintAttachment::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasBeenShown || !OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}
	RegenerateHintByType();
	GetWidget()->AddToViewport();
	HintInstance->ToggleHint();
	bHasBeenShown = true;
}

void UHintAttachment::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (!IsValid(GetWidget()) || !OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}

	HintInstance->ToggleHint();
}

void UHintAttachment::HandleDisappeared(bool bIsShown)
{
	if (!bIsShown)
	{
		GetWidget()->RemoveFromParent();
		SetWidget(nullptr);
		HintInstance->Destruct();
		
		if (DisplayType == EWidgetDisplay::REPEATING || DisplayType == EWidgetDisplay::REPEATING_TIMED)
		{
			bHasBeenShown = false;
		}
	}
}

void UHintAttachment::RegenerateHintByType()
{
	FName Title = TitleText.IsEmpty() ? FName(GetOwner()->GetName() + FString(" Hint")) : FName(TitleText.ToString());
	HintInstance = Cast<UHintBase>(CreateWidget(GetWorld(), Hint, Title));
	SetWidget(HintInstance);
	if (HintInstance->GetHasTitle())
	{
		HintInstance->SetHasTitle(!TitleText.IsEmpty());
	}
	HintInstance->SetTitleCode(TitleText);
	HintInstance->SetDescriptionCode(DescriptionText);
	SetWidgetSpace(EWidgetSpace::World);
	if (DisplayType == EWidgetDisplay::ONCE_TIMED || DisplayType == EWidgetDisplay::REPEATING_TIMED)
	{
		HintInstance->SetIsTimed(true);
	}
	else
	{
		HintInstance->SetIsTimed(false);
	}
	
	HintInstance->OnShowChangeTo.AddDynamic(this, &UHintAttachment::HandleDisappeared);


	// ScreenOffset;

}
