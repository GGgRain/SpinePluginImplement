// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "UObject/GCObject.h"

class FCanvas;
class FPrimitiveDrawInterface;
class FSceneView;
class FSelectedItem;
class FViewport;

//////////////////////////////////////////////////////////////////////////
// IDestructibleSpriteSelectionContext

class IRigEditingSelectionContext
{
public:
	virtual void BeginTransaction(const FText& SessionName) = 0;
	virtual void MarkTransactionAsDirty() = 0;
	virtual void EndTransaction() = 0;
	virtual void InvalidateViewportAndHitProxies() = 0;
};

//////////////////////////////////////////////////////////////////////////
// FDestructibleSpriteSelectionHelper

class FRigEditingSelectionHelper
{
public:
	bool HasAnySelectedItems() const
	{
		return SelectedItemSet.Num() > 0;
	}

	const TSet< TSharedPtr<FSelectedItem> >& GetSelectionSet() const
	{
		return SelectedItemSet;
	}

	void SelectItem(TSharedPtr<FSelectedItem> NewItem);

	virtual void ClearSelectionSet();

	virtual bool CanDeleteSelection() const;

	virtual ~FRigEditingSelectionHelper()
	{
	}

private:
	// Set of selected objects
	TSet< TSharedPtr<FSelectedItem> > SelectedItemSet;
};

//////////////////////////////////////////////////////////////////////////
// FDestructibleSpriteGeometryEditingHelper

class FRigEditingSelectionEditingHelper : public FRigEditingSelectionHelper
{
public:
	FRigEditingSelectionEditingHelper();

	// FDestructibleSpriteSelectionHelper interface
	virtual void ClearSelectionSet() override;
	// End of FDestructibleSpriteSelectionHelper interface

	void DeleteSelectedItems();
};
