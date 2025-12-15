#include "LadyUmbrellaEditor.h"
#include "LadyUmbrella/Math/SecondOrderSystems/SecondOrderStructs.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Customization/SecondOrderStructsCustomization.h"


IMPLEMENT_GAME_MODULE(FLadyUmbrellaEditorModule, LadyUmbrellaEditor);

#define LOCTEXT_NAMESPACE "LadyUmbrellaEditor"

void FLadyUmbrellaEditorModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("LadyUmbrella Custom Editor Module Loaded."));

	if ( FModuleManager::Get().IsModuleLoaded("AssetRegistry") )
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

		AssetRegistryModule.Get().OnAssetRenamed().AddRaw(this, &FLadyUmbrellaEditorModule::ProcessLevelChangeNames);
	}

	if (false) // Enable Custom Editor
	{
		// We load the Property Editor Module.
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		// Registering every custom property.
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			FSecondOrderParams::StaticStruct()->GetFName(), // The name of the Struct that will be customized.
			FOnGetPropertyTypeCustomizationInstance::CreateStatic // Using a delegate to record which function to call when creating the Customized Inspector.
			(&FSecondOrderParamsCustomization::MakeInstance)  // This is the function that creates the inspector.
		);
		// Add new Custom Properties here and on ShutdownModule():

		// Notifying the module to implement the changes.
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}
}

void FLadyUmbrellaEditorModule::ShutdownModule()
{
	if (false) // Enable Custom Editor
	{
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			// Unregistering the properties when shutting down the module.
			FPropertyEditorModule& PropertyEditorModule =
				FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

			// Unregistering all of our custom properties.
			PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
				FSecondOrderParams::StaticStruct()->GetFName());
			// Add new Custom Properties here and on StartupModule():

			// Notifying the module to implement the changes.
			PropertyEditorModule.NotifyCustomizationModuleChanged();
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("LadyUmbrella Custom Editor Module Unloaded."));
}

void FLadyUmbrellaEditorModule::ProcessLevelChangeNames(const FAssetData& AssetData, const FString& OldPath)
{
	if (AssetData.GetClass() == ULevel::StaticClass())
	{
		UE_LOG(LogTemp, Log, TEXT("Renombrado: %s → %s"), *OldPath, *AssetData.ObjectPath.ToString());
		
	}
}

#undef LOCTEXT_NAMESPACE
