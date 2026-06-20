// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TruckLogisticsSimulator : ModuleRules
{
	public TruckLogisticsSimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"Slate",
			"SlateCore",
			"UMG",
			"Json",
			"JsonUtilities",
			"Chaos",
			"PhysicsCore",
			"GameplayTasks",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"InputCore"
		});

		// Enable for debugging
		PublicDefinitions.Add("ENABLE_LOGGING=1");
		PublicDefinitions.Add("WITH_EDITOR_ONLY_DATA=1");
	}
}
