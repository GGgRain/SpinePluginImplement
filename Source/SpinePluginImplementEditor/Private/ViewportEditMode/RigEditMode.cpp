// Copyright Epic Games, Inc. All Rights Reserved.

#include "ViewportEditMode/RigEditMode.h"
#include "SceneView.h"
#include "EditorViewportClient.h"
#include "Framework/Commands/UICommandList.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "EditorModeManager.h"
#include "ViewportEditMode/AssetEditorSelectedItem.h"

#include "EngineUtils.h"
#include "ComponentVisualizer.h"
#include "PhysicsEngine/BodySetup.h"

#include "SpineRigDataEditor/EditingShapeComponent/EditingShapeComponent.h"
#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"
#include "Components/ShapeComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "SpineRigDataEditor/EditingShapeComponent/EditorPhysicsConstraintComponent.h"

#include "SSpineRigDataViewport.h"
#include "SpineRigDataEditorViewportClient.h"
#include "FastUpdate/WidgetProxy.h"
#include "EditorModeTools.h"

#define LOCTEXT_NAMESPACE "PaperGeometryEditing"

//////////////////////////////////////////////////////////////////////////
// FRigEditMode

const FEditorModeID FRigEditMode::EM_RigEdit(TEXT("RigEdit"));


FRigEditMode::FRigEditMode()
{
	bDrawPivot = false;
	bDrawGrid = false;

	PermittedComponentClasses.Add(UShapeComponent::StaticClass());
	PermittedComponentClasses.Add(UBoxComponent_Edit::StaticClass());
	PermittedComponentClasses.Add(UCapsuleComponent_Edit::StaticClass());
	PermittedComponentClasses.Add(USphereComponent_Edit::StaticClass());
	PermittedComponentClasses.Add(UEditorRigBoneDriverComponent::StaticClass());
	PermittedComponentClasses.Add(UPhysicsConstraintComponent::StaticClass());
	PermittedComponentClasses.Add(UEditorPhysicsConstraintComponent::StaticClass());
}

void FRigEditMode::Initialize()
{
}

void FRigEditMode::DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	FEdMode::DrawHUD(ViewportClient, Viewport, View, Canvas);
}

void FRigEditMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	FEdMode::Render(View, Viewport, PDI);
}

void FRigEditMode::AddSelectedItems(TArray<UObject*> InSelectedComponentsArr) {
	for (UObject* Test : InSelectedComponentsArr) {
		AddSelectedItem(Test);
	}
}

void FRigEditMode::AddSelectedItem(UObject* InSelectedComponent) {
	if (PermittedComponentClasses.Contains(InSelectedComponent->GetClass())) {
		if (USceneComponent* CastedComp = Cast<USceneComponent>(InSelectedComponent)) {
			SelectedComponentsArr.Add(CastedComp);
		}
	}
}

void FRigEditMode::ClearSelectedItem() {
	SelectedComponentsArr.Empty();
}

bool FRigEditMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{

	FSpineRigDataEditorViewportClient* Client = (FSpineRigDataEditorViewportClient*)InViewportClient;
	FViewport* Viewport = InViewportClient->Viewport;

	const bool bIsCtrlKeyDown = Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);
	const bool bIsShiftKeyDown = Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
	const bool bIsAltKeyDown = Viewport->KeyState(EKeys::LeftAlt) || Viewport->KeyState(EKeys::RightAlt);
	bool bHandled = false;

	if(!(bIsShiftKeyDown || bIsCtrlKeyDown)){
		SelectedComponentsArr.Empty();
	}

	if (HitProxy)
	{
		
		bHandled = true;
		
		GetModeManager()->SetWidgetMode(FWidget::WM_Translate);

		if (HitProxy->IsA(HActor::StaticGetType())) {
			HActor* CastedProxy = (HActor*)(HitProxy);

			if (CastedProxy != nullptr) {

				if (CastedProxy->Actor != nullptr && CastedProxy->PrimComponent != nullptr) {

					TSet<UActorComponent*> CompSet;
					CompSet = CastedProxy->Actor->GetComponents();

					if (CompSet.Contains(CastedProxy->PrimComponent)) {

						if (PermittedComponentClasses.Contains(CastedProxy->PrimComponent->GetClass())) {

							UActorComponent** TestComp = CompSet.Find(CastedProxy->PrimComponent);
		
							if (USceneComponent* CastedComp = Cast<USceneComponent>(*TestComp)) {
								SelectedComponentsArr.Add(CastedComp);
							}
						}
					}

				}
			}
		}
		else if (HitProxy->IsA(HWidgetAxis::StaticGetType())) {
			StartTracking(InViewportClient, Viewport); Click.GetKey();
		}
	}

	Client->OnEditModeSelectionChanged(SelectedComponentsArr);


	return bHandled ? true : FEdMode::HandleClick(InViewportClient, HitProxy, Click);
}

bool FRigEditMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool bHandled = false;
	FInputEventState InputState(Viewport, Key, Event);

	FSpineRigDataEditorViewportClient* Client = (FSpineRigDataEditorViewportClient*)ViewportClient;

	if (Key == EKeys::BackSpace)
	{
		bHandled = true;

		for (USceneComponent* TestComp : SelectedComponentsArr) {
			if (TestComp->GetClass()->IsChildOf(UEditorRigBoneDriverComponent::StaticClass())) {
				SelectedComponentsArr.Remove(TestComp);
			}
		}

		if (SelectedComponentsArr.Num()) {
			Client->OnTryDeleteObjects(SelectedComponentsArr);
			SelectedComponentsArr.Empty();
		}
	}

	if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released) {
		EndTracking(ViewportClient, ViewportClient->Viewport);
	}


	//@TODO: Support select-and-drag in a single operation (may involve InputAxis and StartTracking)

	// Pass keys to standard controls, if we didn't consume input
	return bHandled ? true : FEdMode::InputKey(ViewportClient, Viewport, Key, Event);
}

void FRigEditMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{

	FEdMode::Tick(ViewportClient, DeltaTime);
}

bool FRigEditMode::ShouldDrawWidget() const
{
	bool bReturn = false;
	if (SelectedComponentsArr.Num()) {
		bReturn = true;
	}
	return bReturn;
}

FVector FRigEditMode::GetWidgetLocation() const
{
	FVector SummedPos(ForceInitToZero);

	if (SelectedComponentsArr.Num())
	{
		// Find the center of the selection set
		for (USceneComponent* Comp : SelectedComponentsArr)
		{
			SummedPos += Comp->GetComponentLocation();
		}
		return (SummedPos / SelectedComponentsArr.Num());
	}

	return SummedPos;
}

bool FRigEditMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	bool bHandled = false;

	//const bool bManipulating = Getmodeto
	EAxisList::Type CurrentAxis = GetCurrentWidgetAxis();

	if ((CurrentAxis != EAxisList::None))
	{
		bHandled = true;

		const FWidget::EWidgetMode MoveMode = GetModeManager()->GetWidgetMode();
		switch(MoveMode){
		case FWidget::EWidgetMode::WM_Translate: {
			// Apply the delta to all of the selected objects
			for (USceneComponent* Comp : SelectedComponentsArr)
			{
				Comp->SetRelativeLocation(Comp->GetRelativeLocation() + InDrag);
			}
			break;
		}
		case FWidget::EWidgetMode::WM_Rotate: {
			// Apply the delta to all of the selected objects
			for (USceneComponent* Comp : SelectedComponentsArr)
			{
				Comp->SetRelativeRotation(Comp->GetRelativeRotation() + InRot);
			}
			break;
		}
		case FWidget::EWidgetMode::WM_Scale: {
			// Apply the delta to all of the selected objects
			for (USceneComponent* Comp : SelectedComponentsArr)
			{
				Comp->SetRelativeScale3D(Comp->GetRelativeScale3D() + InScale);
			}
			break;
		}
		}
	}

	return bHandled;
}



bool FRigEditMode::ShowModeWidgets() const
{
	return true;
}

bool FRigEditMode::UsesTransformWidget() const
{
	return true;
}

void FRigEditMode::DrawBoneStats(FViewport& InViewport, FSceneView& View, FCanvas& Canvas, USpineRigData* Data) {
	FCanvasTextItem TextItem(FVector2D(6, 50), LOCTEXT("BoneStats", "Bone Stats"), GEngine->GetSmallFont(), FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);

	TextItem.Draw(&Canvas);
	TextItem.Position += FVector2D(6.0f, 18.0f);


	bool bIsPrinted = false;

	if (!Data->BoneRigDataList.Contains(Data->DefaultTransformUpdateRootBone)) {
		static FText Prompt = LOCTEXT("BoneWarning1", "Bone {0} is marked as a default transform update root bone, but there is no bone for that name!");

		TextItem.Text = FText::Format(Prompt, FText::FromName(Data->DefaultTransformUpdateRootBone));
		TextItem.SetColor(FColor(255, 255, 0));
		TextItem.Draw(&Canvas);
		TextItem.Position.Y += 18.0f;

		bIsPrinted = true;
	}

	if (!bIsPrinted) {
		static FText VaildText = LOCTEXT("BoneWarningClear", "All Bone datas are vaild.");

		TextItem.Text = VaildText;
		TextItem.Draw(&Canvas);
		TextItem.Position.Y += 18.0f;
	}
}



void FRigEditMode::DrawConstraintStats(FViewport& InViewport, FSceneView& View, FCanvas& Canvas , USpineRigData* Data)
{
	FCanvasTextItem TextItem(FVector2D(6, 100), LOCTEXT("ConstraintStats", "Constraint Stats"), GEngine->GetSmallFont(), FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);

	TextItem.Draw(&Canvas);
	TextItem.Position += FVector2D(6.0f, 18.0f);

	TArray<FName> Keys;
	Data->JointList.GetKeys(Keys);

	bool bIsPrinted = false;
	for (FName TestKey : Keys) {
		FJointConstraintData& ConstraintData = Data->JointList[TestKey];
		if (!Data->BoneRigBodySetupList.Contains(ConstraintData.DefaultInstance.ConstraintBone1)) {
			static FText Prompt = LOCTEXT("ConstraintWarning1","Constraint {0} has Bone {1} as a participant bone1, but there is no bodysetup for the constraint.");

			TextItem.Text = FText::Format(Prompt, FText::FromName(TestKey), FText::FromName(ConstraintData.DefaultInstance.ConstraintBone1));
			TextItem.SetColor(FColor(255, 255, 0));
			TextItem.Draw(&Canvas);
			TextItem.Position.Y += 18.0f;

			bIsPrinted = true;
		}
		if (!Data->BoneRigBodySetupList.Contains(ConstraintData.DefaultInstance.ConstraintBone2)) {
			static FText Prompt = LOCTEXT("ConstraintWarning2", "Constraint {0} has Bone {1} as a participant bone2, but there is no bodysetup for the constraint.");

			TextItem.Text = FText::Format(Prompt, FText::FromName(TestKey), FText::FromName(ConstraintData.DefaultInstance.ConstraintBone2));
			TextItem.SetColor(FColor(255, 255, 0));
			TextItem.Draw(&Canvas);
			TextItem.Position.Y += 18.0f;

			bIsPrinted = true;
		}

	}

	if (!bIsPrinted) {
		static FText VaildText = LOCTEXT("ConstraintWarningClear", "All Constraint datas are vaild.");

		TextItem.Text = VaildText;
		TextItem.Draw(&Canvas);
		TextItem.Position.Y += 18.0f;
	}
}



//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
