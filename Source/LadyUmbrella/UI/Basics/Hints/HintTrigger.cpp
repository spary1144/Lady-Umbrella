#include "HintTrigger.h"

#include "Components/BoxComponent.h"
#include "HintBase.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

AHintTrigger::AHintTrigger()
{
	if (!GetRootComponent())
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	}
	
	HintShowArea = CreateDefaultSubobject<UBoxComponent>(FName("TriggerHit"));
	HintShowArea->SetupAttachment(GetRootComponent());
	bHasBeenShown = false;
}

void AHintTrigger::LoadGame()
{
	bHasBeenShown = false;
}

void AHintTrigger::LoadDataCheckPoint()
{
	LoadGame();
}

void AHintTrigger::SaveGame()
{
}

void AHintTrigger::SaveDataCheckPoint()
{
}

void AHintTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HintShowArea->OnComponentBeginOverlap.IsAlreadyBound(this, &AHintTrigger::OnBeginOverlap))
	{
		HintShowArea->OnComponentBeginOverlap.AddDynamic(this, &AHintTrigger::OnBeginOverlap);
	}
	
	if (!HintShowArea->OnComponentEndOverlap.IsAlreadyBound(this, &AHintTrigger::OnEndOverlap))
	{
		HintShowArea->OnComponentEndOverlap.AddDynamic(this, &AHintTrigger::OnEndOverlap);
	}
}

void AHintTrigger::GenerateHintInstance()
{
	if (IsValid(HintInstance))
	{
		HintInstance->Destruct();
	}
	HintInstance = CreateWidget<UHintBase>(GetWorld(), Hint, FName(TitleText.ToString()));
	if (!IsValid(HintInstance))
	{
		return;
	}
	
	if(HintInstance->GetHasTitle())
	{
		HintInstance->SetHasTitle(!TitleText.IsEmpty());
	}
	
	HintInstance->SetTitleCode(TitleText);
	HintInstance->SetDescriptionCode(DescriptionText);

	HintInstance->OnShowChangeTo.AddDynamic(this, &AHintTrigger::HandleDisappeared);
	
	if (DisplayType == EWidgetDisplay::ONCE_TIMED || DisplayType == EWidgetDisplay::REPEATING_TIMED)
	{
		HintInstance->SetIsTimed(true);
	}
	else
	{
		HintInstance->SetIsTimed(false);
	}
	
	HintInstance->SetHintAlignment(Alignment, ScreenOffset);
}

void AHintTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasBeenShown || !OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}
	if (!IsValid(HintInstance))
	{
		GenerateHintInstance();
	}
	if (!IsValid(HintInstance) || HintInstance->IsShowing())
	{
		return;
	}
	HintInstance->ToggleHint();
	HintInstance->AddToViewport();
	bHasBeenShown = true;
}

void AHintTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(HintInstance) || !OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}
	if (HintInstance->IsShowing())
	{
		HintInstance->ToggleHint();
	}
}

void AHintTrigger::HandleDisappeared(bool bIsShown)
{
	if (!bIsShown)
	{
		HintInstance->Destruct();
		HintInstance->RemoveFromParent();
		
		if (DisplayType == EWidgetDisplay::REPEATING || DisplayType == EWidgetDisplay::REPEATING_TIMED)
		{
			bHasBeenShown = false;
		}
	}
}
