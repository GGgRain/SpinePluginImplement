// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "ViewportEditMode/RigEditing.h"
#include "EdMode.h"

class FCanvas;
class FEditorViewportClient;
class FPrimitiveDrawInterface;
class FSceneView;
class FUICommandList;
class FViewport;
struct FViewportClick;

//////////////////////////////////////////////////////////////////////////
// FDestructibleSpriteGeometryEditMode

class FRigEditMode : public FEdMode
{
public:
	static const FEditorModeID EM_RigEdit;
public:

	FRigEditMode();

	// FEdMode interface
	virtual void Initialize() override;
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	virtual bool ShouldDrawWidget() const override;
	// End of FEdMode interface

	virtual bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	virtual bool ShowModeWidgets() const override;
	virtual bool UsesTransformWidget() const override;
	virtual FVector GetWidgetLocation() const override;

	static void DrawBoneStats(FViewport& InViewport, FSceneView& View, FCanvas& Canvas, USpineRigData* Data);
	static void DrawConstraintStats(FViewport& InViewport, FSceneView& View, FCanvas& Canvas ,USpineRigData* Data);

public:

	TArray<USceneComponent*> SelectedComponentsArr;

	TArray<UClass*> PermittedComponentClasses;

	void AddSelectedItems(TArray<UObject*> InSelectedComponentsArr);

	void AddSelectedItem(UObject* InSelectedComponent);

	void ClearSelectedItem();

};
