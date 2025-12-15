#include "ModalWindow.h"
#include "Components/Button.h"

void UModalWindow::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConfirmButton->OnPressed.AddDynamic(this, &UModalWindow::ConfirmClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &UModalWindow::ConfirmClicked);
	
	DenyButton->OnPressed.AddDynamic(this, &UModalWindow::DenyClicked);
	DenyButton->OnClicked.AddDynamic(this, &UModalWindow::DenyClicked);
}
