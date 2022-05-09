// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoneRigComponentSequenceActions.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Sequencer/BoneRigComponentSequence.h"
#include "BoneRigComponentSequenceEditorToolkit.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

FBoneRigComponentSequenceActions::FBoneRigComponentSequenceActions(const TSharedRef<ISlateStyle>& InStyle) : Style(InStyle)
{
}

uint32 FBoneRigComponentSequenceActions::GetCategories()
{
	return EAssetTypeCategories::Animation;
}


FText FBoneRigComponentSequenceActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_BoneRigComponentSequence", "Bone Rig Component Sequence");
}


UClass* FBoneRigComponentSequenceActions::GetSupportedClass() const
{
	return UBoneRigComponentSequence::StaticClass();
}


FColor FBoneRigComponentSequenceActions::GetTypeColor() const
{
	return FColor(108, 53, 0);
}


void FBoneRigComponentSequenceActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	
	// forward to level sequence asset actions
	UWorld* WorldContext = nullptr;
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.WorldType == EWorldType::Editor)
		{
			WorldContext = Context.World();
			break;
		}
	}

	if (!ensure(WorldContext))
	{
		return;
	}

	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid()
		? EToolkitMode::WorldCentric
		: EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		ULevelSequence* LevelSequence = Cast<ULevelSequence>(*ObjIt);

		if (LevelSequence != nullptr)
		{
			// Legacy upgrade
			LevelSequence->ConvertPersistentBindingsToDefault(WorldContext);

			TSharedRef<FBoneRigComponentSequenceEditorToolkit> Toolkit = MakeShareable(new FBoneRigComponentSequenceEditorToolkit(Style));
			Toolkit->Initialize(Mode, EditWithinLevelEditor, LevelSequence);
		}
	}
	
}


bool FBoneRigComponentSequenceActions::ShouldForceWorldCentric()
{
	// @todo sequencer: Hack to force world-centric mode for Sequencer
	return true;
}

#undef LOCTEXT_NAMESPACE
