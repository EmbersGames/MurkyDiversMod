// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS_Utility : ModuleRules
{
	public GAS_Utility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// to remove, only for debug
		OptimizeCode = CodeOptimization.Never;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"GAS_Utility/",
				"GAS_Utility/AbilitySystem",
				"GAS_Utility/AbilitySystem/AttributeSets",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"NavigationSystem",
				"HeadMountedDisplay",
				"EnhancedInput",
				"GameplayAbilities",
				"GameplayTasks",
				"GameplayTags",
				"MotionWarping",
				"AIModule",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"GameplayAbilities",
				"GameplayTags",
				"EnhancedInput",
				"GameplayTasks",
				"ModularGameplay",
				"GameFeatures",
				"MotionWarping",
				"NiagaraAnimNotifies",
				"Niagara",
				"AIModule"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
