// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpineRigDataEditor/SSpineRigDataEditorViewportToolbar.h"
#include "SEditorViewport.h"

#define LOCTEXT_NAMESPACE "SSpineRigDataEditorViewportToolbar"

///////////////////////////////////////////////////////////
// SSpriteEditorViewportToolbar

void SSpineRigDataEditorViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InInfoProvider);
}

TSharedRef<SWidget> SSpineRigDataEditorViewportToolbar::GenerateShowMenu() const
{
	GetInfoProvider().OnFloatingButtonClicked();
	
	TSharedRef<SEditorViewport> ViewportRef = GetInfoProvider().GetViewportWidget();


	const bool bInShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder ShowMenuBuilder(bInShouldCloseWindowAfterMenuSelection, ViewportRef->GetCommandList());
	{
		/*
		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowSockets);
		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowBaseSpritePoints);
		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowPivot);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowGrid);
		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowBounds);
		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteGeometryEditCommands::Get().SetShowNormals);

		ShowMenuBuilder.AddMenuSeparator();

		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowCollision);
		ShowMenuBuilder.AddMenuEntry(FDestructibleSpriteEditorCommands::Get().SetShowMeshEdges);
		*/
	}

	return ShowMenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
