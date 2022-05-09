// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Style/SpinePluginImplementEditorStyle.h"

class FSpineRigDataEditorCommands : public TCommands<FSpineRigDataEditorCommands>
{
public:
	FSpineRigDataEditorCommands()
		: TCommands<FSpineRigDataEditorCommands>(
			TEXT("SpineRigDataEditor"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "SpineRigDataEditor", "Spine Rig Data Editor"), // Localized context name for displaying
			NAME_None, // Parent
			FSpinePluginImplementEditorStyle::Get()->GetStyleSetName() // Icon Style Set
			)
	{
	}

	// TCommand<> interface
	virtual void RegisterCommands() override;
	// End of TCommand<> interface

public:
	// Show toggles
	//TSharedPtr<FUICommandInfo> SetShowGrid;

};
