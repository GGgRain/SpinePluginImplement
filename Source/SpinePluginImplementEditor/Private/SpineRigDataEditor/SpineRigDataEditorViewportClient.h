#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "SpineRigData.h"
#include "SSpineRigDataViewport.h"

class FSpineRigDataEditorViewportClient : public FEditorViewportClient, public TSharedFromThis<FSpineRigDataEditorViewportClient>
{
public:
	// 생성자에서 모든 필요한 기본 변수를 설정.
	FSpineRigDataEditorViewportClient(TWeakPtr<class FSpineRigDataEditorToolkit> InParentToolkits, const TSharedRef<class FAdvancedPreviewScene>& AdvPreviewScene, const TSharedRef<class SSpineRigDataViewport>& InSpineRigDataAssetViewport, USpineRigData* ObjectToEdit);
	~FSpineRigDataEditorViewportClient();

	// 에디터에서 사용하는 입력
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