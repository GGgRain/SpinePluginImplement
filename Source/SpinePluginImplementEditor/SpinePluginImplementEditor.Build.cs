// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpinePluginImplementEditor : ModuleRules
{
	public SpinePluginImplementEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {
			"SpinePluginImplementEditor",
			"SpinePluginImplementEditor/Private",
			"SpinePluginImplementEditor/Private/Factory",
			"SpinePluginImplementEditor/Private/AssetTool",
			"SpinePluginImplementEditor/Private/Style",
			"SpinePluginImplementEditor/Private/SpineRigDataEditor",
		});


		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"UnrealEd",
			"SpinePlugin",
			"SpinePluginImplement",
			"EditorStyle",                 
			"ContentBrowser",	
			"DesktopWidgets",		
			"InputCore",		
			"Projects",			
			"Slate",
			"SlateCore",
			"EditorWidgets",	
			"KismetWidgets",
			"Kismet",

			"LevelSequenceEditor",

			"MovieScene",
			"MovieSceneTracks",
			"MovieSceneTools",
			"MovieSceneCaptureDialog",

			"VREditor",
			"Sequencer",

			"CinematicCamera",

			"AdvancedPreviewScene",
			"CommonMenuExtensions",

			"LevelEditor",

			"PropertyEditor",

			"DeveloperSettings",
		});

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",

			"LevelSequence",
		});
	}
}
