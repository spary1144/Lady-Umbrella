// 
// LadyUmbrellaEditor.h
// 
// Main LadyUmbrella *editor* class, initializes additional functionality to the LadyUmbrella Project Editor.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreUObject.h"
#include "Modules/ModuleManager.h"

class FLadyUmbrellaEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void ProcessLevelChangeNames(const FAssetData& AssetData, const FString& OldPath);
	
};