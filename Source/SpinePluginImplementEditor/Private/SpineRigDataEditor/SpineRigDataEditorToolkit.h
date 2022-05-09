// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EditorUndoClient.h"
#include "Templates/SharedPointer.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "SpineRigData.h"
#include "SSCSEditor.h"
#include "SSpineRigDataViewport.h"

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "UObject/GCObject.h"

class FSpawnTabArgs;
class ISlateStyle;
class IToolkitHost;
class SDockTab;
class UTextAsset;
class SSpineRigDataViewport;


namespace ERigDataEditorMode
{
	enum Type
	{
		RigEditMode,
		RagdollTestMode
	};
}

/**
 * Implements an Editor toolkit for textures.
 */
class FSpineRigDataEditorToolkit
	: public FAssetEditorToolkit
	, public FEditorUndoClient
	, public FGCObject
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InStyle The style set to use.
	 */
	FSpineRigDataEditorToolkit(const TSharedRef<ISlateStyle>& InStyle);

	/** Virtual destructor. */
	virtual ~FSpineRigDataEditorToolkit();

public:

	/**
	 * Initializes the editor tool kit.
	 *
	 * @param InTextAsset The UTextAsset asset to edit.
	 * @param InMode The mode to create the toolkit in.
	 * @param InToolkitHost The toolkit host.
	 */
	void Initialize(USpineRigData* InSpineRigData, const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost>& InToolkitHost);
	void ExtendToolbar();

public:

	//~ FAssetEditorToolkit interface

	virtual FString GetDocumentationLink() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

public:

	//~ IToolkit interface

	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;

public:

	//~ FGCObject interface

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

protected:

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

private:

	/** Callback for spawning the Properties tab. */
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args, FName TabIdentifier);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args, FName TabIdentifier);
	TSharedRef<SDockTab> SpawnTab_HierarchyPreview(const FSpawnTabArgs& Args, FName TabIdentifier);

public:

	TSharedRef<ISlateStyle> Style;

	TSharedPtr<SWidget> PreviewSettingsWidget;

	UPROPERTY()
		USpineRigData* EdtingSpineRigData;

	USpineRigData* GetEditingAsset() { return EdtingSpineRigData; }

public:

	UPROPERTY()
		TSharedPtr<class SSCSEditor> SSCSEditorPtr;
	UPROPERTY()
		TSharedPtr<class SSpineRigDataViewport> ViewportPtr;
	UPROPERTY()
		TSharedPtr<class IDetailsView> DetailPtr;


public:

	UFUNCTION()
		void OnEditingRigDataPropertyChanged();

	UFUNCTION()
		AActor* GetActorContext() const;

	UFUNCTION()
		bool GetAllowComponentTreeEditing() const;

	UFUNCTION()
		void OnSelectionUpdate(const TArray<FSCSEditorTreeNodePtrType>& Arr);

	UFUNCTION()
		void OnItemDoubleClickedOnTreeView(const FSCSEditorTreeNodePtrType Type);

	UFUNCTION()
		void OnEditModeSelectionChanged(TArray<USceneComponent*> SelectedComponentsArr);

	UFUNCTION()
		void OnTryDeleteObjects(TArray<USceneComponent*> SelectedComponentsArr);

public:
	
	void RefreshEditorData();

	void UpdateDetails();


	ERigDataEditorMode::Type CurrentEditorMode;

	void SetCurrentEditorMode(ERigDataEditorMode::Type NewType) {
		CurrentEditorMode = NewType;
	}
	ERigDataEditorMode::Type GetCurrentEditorMode() {
		return CurrentEditorMode;
	}
	
};

