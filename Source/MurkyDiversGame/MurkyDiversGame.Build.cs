// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MurkyDiversGame : ModuleRules
{
	public MurkyDiversGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"MurkyDiversGame"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreOnline",
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				"PhysicsCore",
				"GameplayTags",
				"GameplayTasks",
				"GameplayAbilities",
				"AIModule",
				"ModularGameplay",
				"DataRegistry",
				"ReplicationGraph",
				"GameFeatures",
				"SignificanceManager",
				"Hotfix",
				"Niagara",
				"ControlFlows",
				"PropertyPath", 
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"Slate",
				"SlateCore",
				"RenderCore",
				"DeveloperSettings",
				"EnhancedInput",
				"NetCore",
				"RHI",
				"Engine",
				"Projects",
				"Gauntlet",
				"NavigationSystem",
				"UMG",
				"CommonUI",
				"CommonInput",
				"AudioMixer",
				"NetworkReplayStreaming",
				"AudioModulation",
				"EngineSettings",
				"CommonUI",
				"CinematicCamera",
				"AIModule",
				"ModularGameplayActors", "GAS_Utility",
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				
			}
		);
		
		// Generate compile errors if using DrawDebug functions in test/shipping builds.
		PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
		//PublicDefinitions.Add("ALLOW_CONSOLE=0");
		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
