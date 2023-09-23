// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpinePluginImplement : ModuleRules
{
	public SpinePluginImplement(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add("SpinePlugin/Public");
		PublicIncludePaths.Add("SpinePlugin/Public/spine-cpp/include");


		PrivateIncludePaths.Add("SpinePlugin/Private");
		PrivateIncludePaths.Add("SpinePlugin/Public/spine-cpp/include");


		PublicDependencyModuleNames.AddRange(
			new string[]
			{

			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"LevelSequence",
				"Slate",
				"SlateCore",
				"SpinePlugin",
				"ProceduralMeshComponent",
				// ... add private dependencies that you statically link with here ...	
				"PhysicsCore",
				"MovieScene",
				"MovieSceneTracks",
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
