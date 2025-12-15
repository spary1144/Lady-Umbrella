// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class LadyUmbrella : ModuleRules
{
	public LadyUmbrella(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "FMODStudio" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "AIModule", "NavigationSystem", "CableComponent", "UMG", "Niagara", "MotionWarping",
			"RenderCore", "Slate", "SlateCore", "GameplayTags", "AdvancedWidgets", "FMODStudio", "MovieScene", "MovieSceneTracks",
			"LevelSequence", "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "Steamworks" });

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
		
		var SteamworksBinary = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "Steamworks", "bin");
		var SteamworksInclude = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "Steamworks", "include");
		
		PublicAdditionalLibraries.Add(Path.Combine(SteamworksBinary, "steam_api64.lib"));
		RuntimeDependencies.Add(Path.Combine(SteamworksBinary, "steam_api64.dll"));

		PublicIncludePaths.Add(SteamworksInclude);
		PublicDelayLoadDLLs.Add("steam_api64.dll");
	}
}
