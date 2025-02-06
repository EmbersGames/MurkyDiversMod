// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS_UtilityEditor : ModuleRules
{
	public GAS_UtilityEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"GAS_UtilityEditor/"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GAS_Utility",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"HeadMountedDisplay",
				"EnhancedInput",
				"GameplayAbilities",
				"GameplayTasks",
				"GameplayTags"
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
				"InputCore",
				"AssetTools",
				"UnrealEd",     // for FAssetEditorManager
				"KismetWidgets",
				"KismetCompiler",
				"BlueprintGraph",
				"GraphEditor",
				"Kismet",       // for FWorkflowCentricApplication
				"PropertyEditor",
				"EditorStyle",
				"Sequencer",
				"DetailCustomizations",
				"Settings",
				"RenderCore",
				"GameplayAbilities",
				"GameplayTags",
				"EnhancedInput",
				"GameplayTasks",
				"ModularGameplay",
				"GameFeatures", "EditorScriptingUtilities", "Blutility","UMGEditor", "GameplayTagsEditor",
				"UMG",
				"ApplicationCore"
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
