#include "AnimatedModalButton.h"

void UAnimatedModalButton::NativeOnInitialized() // this is called only once for the UUserWidget. If you have one-time things to establish up-front (like binding callbacks to events on BindWidget properties), do so here
{
	Super::NativeOnInitialized();

	ModalWindow->OnChoice.AddDynamic(this,&UAnimatedModalButton::ManageChoice);
}

/*void UAnimatedModalButton::GoToPressed()
{
	Super::GoToPressed();
	// Handle modal && navigation
}*/

void UAnimatedModalButton::ActivateButton()
{
	Super::ActivateButton();
	// Any additional logic
}

void UAnimatedModalButton::ManageChoice(bool bConfirmed)
{
	if (bConfirmed)
	{
		ActivateButton();
		return;
	}
	//GoToIdle();
}
