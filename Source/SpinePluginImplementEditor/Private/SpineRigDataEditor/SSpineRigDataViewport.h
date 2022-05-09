#pragma once

#include "EngineMinimal.h"
#include "SlateFwd.h"
#include "UObject/GCObject.h"
#include "SEditorViewport.h"
#include "SLevelViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class FSpineRigDataEditorToolkit;
class USpineRigData;

class SSpineRigDataViewport : public SEditorViewport, public FGCObject, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SSpineRigDataViewport) {}
	SLATE_ARGUMENT(TWeakPtr<FSpineRigDataEditorToolkit>, InParentToolkits)
		SLATE_ARGUMENT(USpineRigData*, ObjectToEdit)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);
	SSpineRigDataViewport();
	~SSpineRigDataViewport();

	//Toolbar interface
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	// FGCObject interface. U오브젝트가 아니라도 가비지컬렉션 회수 가능.
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	TSharedRef<class FAdvancedPreviewScene> GetPreviewScene();

protected:
	// SEditorViewport interface
	virtual void BindCommands() override;
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	//virtual EVisibility GetTransformToolbarVisibility() const override;
	//virtual void OnFocusViewportToSelection() override;

	//virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void UpdateScene();

public:
	TWeakPtr<FSpineRigDataEditorToolkit> ParentToolkitsPtr;
	TSharedPtr<class FAdvancedPreviewScene> PreviewScene;
	TSharedPtr<class FSpineRigDataEditorViewportClient> SpineRigDataEditorViewportClientptr;
	USpineRigData* EditingSpineRigData;

	UPROPERTY()
		class AEditorSpineRigActor* EditorSpineRigActor;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	UPROPERTY()
		TArray<USceneComponent*> PreviewRegisteredUpdatableComponents;
};