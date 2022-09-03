// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpinePluginImplement : ModuleRules
{
	public SpinePluginImplement(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"LevelSequence",
				"Slate",
				"SlateCore",

				"ProceduralMeshComponent",
				"PhysicsCore",
				"MovieScene",
				"MovieSceneTracks",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"SpinePlugin",
				
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
