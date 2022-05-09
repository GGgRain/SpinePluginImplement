// Copyright Epic Games, Inc. All Rights Reserved.

#include "ViewportEditMode/RigEditing.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Engine/Engine.h"
#include "EngineGlobals.h"
#include "SceneManagement.h"
#include "UnrealWidget.h"
#include "ViewportEditMode/AssetEditorSelectedItem.h"


#define LOCTEXT_NAMESPACE "RigEditing"

//////////////////////////////////////////////////////////////////////////
// FDestructibleSpriteSelectionHelper

void FRigEditingSelectionHelper::ClearSelectionSet()
{
	SelectedItemSet.Empty();
}

void FRigEditingSelectionHelper::SelectItem(TSharedPtr<FSelectedItem> NewItem)
{
	SelectedItemSet.Add(NewItem);
}

bool FRigEditingSelectionHelper::CanDeleteSelection() const
{
	return SelectedItemSet.Num() > 0;
}

//////////////////////////////////////////////////////////////////////////
// FDestructibleSpriteGeometryEditingHelper

FRigEditingSelectionEditingHelper::FRigEditingSelectionEditingHelper() {

}

void FRigEditingSelectionEditingHelper::ClearSelectionSet()
{
	FRigEditingSelectionHelper::ClearSelectionSet();
}

void FRigEditingSelectionEditingHelper::DeleteSelectedItems()
{

	for (TSharedPtr<FSelectedItem> SelectedItem : GetSelectionSet())
	{
		if (SelectedItem->CanBeDeleted())
		{
			SelectedItem->DeleteThisItem();
		}
	}
	
	ClearSelectionSet();
}


//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
