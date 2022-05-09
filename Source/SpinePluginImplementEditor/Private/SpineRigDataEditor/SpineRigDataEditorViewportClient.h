#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "SpineRigData.h"
#include "SSpineRigDataViewport.h"

class FSpineRigDataEditorViewportClient : public FEditorViewportClient, public TSharedFromThis<FSpineRigDataEditorViewportClient>
{
public:
	// �����ڿ��� ��� �ʿ��� �⺻ ������ ����.
	FSpineRigDataEditorViewportClient(TWeakPtr<class FSpineRigDataEditorToolkit> InParentToolkits, const TSharedRef<class FAdvancedPreviewScene>& AdvPreviewScene, const TSharedRef<class SSpineRigDataViewport>& InSpineRigDataAssetViewport, USpineRigData* ObjectToEdit);
	~FSpineRigDataEditorViewportClient();

	// �����Ϳ��� ����ϴ� �Է�
	virtual void Tick(float DeltaSeconds) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;

	void HandleSelectionChanged(const TArrayView<UObject*>& InSelectedItems, ESelectInfo::Type InSelectInfo);

	TWeakPtr<class FSpineRigDataEditorToolkit> ParentToolkitsPtr;
	TWeakPtr<class SSpineRigDataViewport> SpineRigDataAssetViewportPtr;
	USpineRigData* EditingSpineRigData;
	class FAdvancedPreviewScene* AdvancedPreviewScene;

	void ActivateEditMode();
	
	void OnEditModeSelectionChanged(TArray<USceneComponent*> SelectedComponentsArr);

	void OnTryDeleteObjects(TArray<USceneComponent*> SelectedComponentsArr);

	void SelectObjects(TArray<UObject*> SelectedComponentsArr);

};