#include "SpineRigDataEditorViewportClient.h"
#include "SSpineRigDataViewport.h"
#include "SpineRigDataEditorToolkit.h"
#include "AdvancedPreviewScene.h"

#include "EditorStyleSet.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "AssetViewerSettings.h"

#include "EditorModes.h"
#include "AssetEditorModeManager.h"
#include "ViewportEditMode/RigEditMode.h"


FSpineRigDataEditorViewportClient::FSpineRigDataEditorViewportClient(TWeakPtr<class FSpineRigDataEditorToolkit> InParentToolkits, const TSharedRef<class FAdvancedPreviewScene>& AdvPreviewScene, const TSharedRef<class SSpineRigDataViewport>& InSpineRigDataAssetViewport, USpineRigData* ObjectToEdit)
	: FEditorViewportClient(nullptr, &AdvPreviewScene.Get(), StaticCastSharedRef<SEditorViewport>(InSpineRigDataAssetViewport))
	, ParentToolkitsPtr(InParentToolkits)
	, SpineRigDataAssetViewportPtr(InSpineRigDataAssetViewport)
	, EditingSpineRigData(ObjectToEdit)
{
	SetViewMode(VMI_Lit);

	AdvancedPreviewScene = static_cast<FAdvancedPreviewScene*>(PreviewScene);

	//Widget->SetUsesEditorModeTools(ModeTools);
	((FAssetEditorModeManager*)ModeTools)->SetPreviewScene(PreviewScene);
}

FSpineRigDataEditorViewportClient::~FSpineRigDataEditorViewportClient()
{
}

void FSpineRigDataEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	//if (AdvancedPreviewScene) {
	//	AdvancedPreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	//}
}

void FSpineRigDataEditorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);
}

void FSpineRigDataEditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) {

	if (EditingSpineRigData != nullptr) {
		FRigEditMode::DrawBoneStats(InViewport, View, Canvas, EditingSpineRigData);
		FRigEditMode::DrawConstraintStats(InViewport, View, Canvas, EditingSpineRigData);
	}
}


void FSpineRigDataEditorViewportClient::HandleSelectionChanged(const TArrayView<UObject*>& InSelectedItems, ESelectInfo::Type InSelectInfo) {

}

void FSpineRigDataEditorViewportClient::ActivateEditMode()
{
	// Activate the sprite geometry edit mode

	ModeTools->SetToolkitHost(ParentToolkitsPtr.Pin()->GetToolkitHost());
	ModeTools->SetDefaultMode(FRigEditMode::EM_RigEdit);
	ModeTools->ActivateDefaultMode();

	FRigEditMode* EditMode = ModeTools->GetActiveModeTyped<FRigEditMode>(FRigEditMode::EM_RigEdit);
	check(EditMode);
	ModeTools->SetWidgetMode(FWidget::WM_None);

}

void FSpineRigDataEditorViewportClient::OnEditModeSelectionChanged(TArray<USceneComponent*> SelectedComponentsArr) {
	ParentToolkitsPtr.Pin()->OnEditModeSelectionChanged(SelectedComponentsArr);
}

void FSpineRigDataEditorViewportClient::OnTryDeleteObjects(TArray<USceneComponent*> SelectedComponentsArr) {
	ParentToolkitsPtr.Pin()->OnTryDeleteObjects(SelectedComponentsArr);
}


void FSpineRigDataEditorViewportClient::SelectObjects(TArray<UObject*> SelectedComponentsArr) {
	FRigEditMode* EditMode = ModeTools->GetActiveModeTyped<FRigEditMode>(FRigEditMode::EM_RigEdit);
	check(EditMode);

	EditMode->ClearSelectedItem();
	EditMode->AddSelectedItems(SelectedComponentsArr);
}