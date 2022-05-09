#include "SSpineRigDataViewport.h"
#include "SpineRigDataEditorViewportClient.h"

#include "ShowFlagMenuCommands.h"
#include "SCommonEditorViewportToolbarBase.h"

#include "SpineRigDataEditor/EditorSpineRigActor.h"
#include "SpineRigDataEditor/SSpineRigDataEditorViewportToolbar.h"
#include "SpineRigDataEditorToolkit.h"
#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"

#include "AdvancedPreviewScene.h"

#include "DrawDebugHelpers.h"
#include "SpineRigDataEditor/EditingShapeComponent/EditingShapeComponent.h"

#define LOCTEXT_NAMESPACE "SpineRigDataAssetViewport"

SSpineRigDataViewport::SSpineRigDataViewport()
	: PreviewScene(MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues())))
{

}

SSpineRigDataViewport::~SSpineRigDataViewport()
{
	if (SpineRigDataEditorViewportClientptr.IsValid())
	{
		SpineRigDataEditorViewportClientptr->Viewport = NULL;
	}
}

#include "SpineRigDataEditor/SSpineRigDataEditorViewportToolbar.h"

void SSpineRigDataViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingSpineRigData);
	Collector.AddReferencedObject(EditorSpineRigActor);
}

TSharedRef<class FAdvancedPreviewScene> SSpineRigDataViewport::GetPreviewScene()
{
	return PreviewScene.ToSharedRef();
}

TSharedRef<FEditorViewportClient> SSpineRigDataViewport::MakeEditorViewportClient()
{
	SpineRigDataEditorViewportClientptr = MakeShareable(new FSpineRigDataEditorViewportClient(ParentToolkitsPtr, PreviewScene.ToSharedRef(), SharedThis(this), EditingSpineRigData));

	SpineRigDataEditorViewportClientptr->SetRealtime(true);

	return SpineRigDataEditorViewportClientptr.ToSharedRef();
}


TSharedRef<class SEditorViewport> SSpineRigDataViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SSpineRigDataViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SSpineRigDataViewport::OnFloatingButtonClicked()
{
}
void SSpineRigDataViewport::BindCommands() {
	SEditorViewport::BindCommands();


	FUICommandList& UICommandListRef = *CommandList;

	//UICommandListRef.MapAction(
	//	LevelViewportCommands.UseDefaultShowFlags,
	//	FExecuteAction::CreateSP(this, &SLevelViewport::OnUseDefaultShowFlags, false));

	FShowFlagMenuCommands::Get().BindCommands(UICommandListRef, Client);

	UE_LOG(LogTemp, Log, TEXT("SSpineRigDataViewport::BindCommands()"));

}

//EVisibility SSpineRigDataViewport::GetTransformToolbarVisibility() const {

//}
//void SSpineRigDataViewport::OnFocusViewportToSelection() {

//}


TSharedPtr<SWidget> SSpineRigDataViewport::MakeViewportToolbar() {
	return SNew(SSpineRigDataEditorViewportToolbar, SharedThis(this));
}

void SSpineRigDataViewport::Construct(const FArguments& InArgs)
{
	ParentToolkitsPtr = InArgs._InParentToolkits;
	EditingSpineRigData = InArgs._ObjectToEdit;

	SEditorViewport::Construct(SEditorViewport::FArguments());

	

	SpineRigDataEditorViewportClientptr->SetViewRotation(FRotator(0, -90, 0));

	
	//ParentToolkitsPtr.Pin()->deta

	if (EditingSpineRigData != nullptr) {
		//에디팅용 데이터들 초기화

		EditingSpineRigData->FixupInstances();


		FTransform Transform = FTransform::Identity;
		Transform.SetLocation(Transform.GetLocation() + FVector(0, 0, 500));

		PreviewScene->GetWorld()->SetShouldTick(true);

		EditorSpineRigActor = PreviewScene->GetWorld()->SpawnActor<AEditorSpineRigActor>(AEditorSpineRigActor::StaticClass(), Transform);
		if (EditorSpineRigActor) {
			EditorSpineRigActor->SetActorTickEnabled(true);
			EditorSpineRigActor->SetTickableWhenPaused(true);

			//TODO : Disable proxy collision
			EditorSpineRigActor->SetActorEnableCollision(false);
			EditorSpineRigActor->SetAutonomousProxy(false);
			EditorSpineRigActor->AtomicallySetFlags(EObjectFlags::RF_Transactional);

			SpawnedActors.Add(EditorSpineRigActor);

			PreviewScene->AddComponent(EditorSpineRigActor->HandlerComponent, Transform, true);
			PreviewScene->AddComponent(EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp, Transform, true);

			PreviewScene->AddComponent(EditorSpineRigActor->EditorBoneRigComponent, Transform, true);
			EditorSpineRigActor->EditorBoneRigComponent->ParentToolkit = InArgs._InParentToolkits;

	
			EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->Atlas = EditingSpineRigData->PreviewAtlas;
			EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->SkeletonData = EditingSpineRigData->PreviewSkeletonData;

			EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->SetToSetupPose();
			EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->MarkRenderStateDirty();
			EditorSpineRigActor->HandlerComponent->MarkRenderStateDirty();
			EditorSpineRigActor->HandlerComponent->UpdateRenderer(EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp);
			EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->SetToSetupPose();

			EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->TickComponent(0, ELevelTick::LEVELTICK_All, &EditorSpineRigActor->HandlerComponent->SkeletonAnimationComp->PrimaryComponentTick);
			EditorSpineRigActor->HandlerComponent->TickComponent(0, ELevelTick::LEVELTICK_All, &EditorSpineRigActor->HandlerComponent->PrimaryComponentTick);





			EditorSpineRigActor->EditorBoneRigComponent->BaseData = EditingSpineRigData;
			
			UpdateScene();

			SpineRigDataEditorViewportClientptr->SetViewLocation(EditorSpineRigActor->GetActorLocation() + FVector(0, 200, 0));

		
			TArray<UActorComponent*> CompArr;
			CompArr = EditorSpineRigActor->GetComponentsByClass(USpineRigBoneDriverComponent::StaticClass());
			for (UActorComponent* Uncasted : CompArr) {
				if (USpineRigBoneDriverComponent* CastedComp = Cast<USpineRigBoneDriverComponent>(Uncasted)) {
					PreviewScene->AddComponent(CastedComp, CastedComp->GetComponentTransform(), false);
					PreviewRegisteredUpdatableComponents.Add(CastedComp);
				}
			}

		}
	}

	MakeViewportToolbar();
}

void SSpineRigDataViewport::UpdateScene() {
	if (EditorSpineRigActor != nullptr) {

		for (USceneComponent* Uncasted : PreviewRegisteredUpdatableComponents) {
			PreviewScene->RemoveComponent(Uncasted);
		}
		PreviewRegisteredUpdatableComponents.Empty();


		EditorSpineRigActor->EditorBoneRigComponent->RecreateRigWithoutFlushingAllComp(EditorSpineRigActor->HandlerComponent);

		EditorSpineRigActor->EditorBoneRigComponent->RefreshCollisionComponents();

		EditorSpineRigActor->EditorBoneRigComponent->RefreshAllJointConstraintComponents();


		TArray<UActorComponent*> BoneCompArr;
		BoneCompArr = EditorSpineRigActor->GetComponentsByClass(UEditorRigBoneDriverComponent::StaticClass());

		for (UActorComponent* Uncasted : BoneCompArr) {
			if (UEditorRigBoneDriverComponent* CastedComp = Cast<UEditorRigBoneDriverComponent>(Uncasted)) {
				PreviewScene->AddComponent(CastedComp, CastedComp->GetComponentTransform(), false);
				PreviewRegisteredUpdatableComponents.Add(CastedComp);

				for (UBoxComponent_Edit* Box : CastedComp->CollisionRefBoxs) {
					PreviewScene->AddComponent(Box, Box->GetComponentTransform(), false);
					PreviewRegisteredUpdatableComponents.Add(Box);
				}
				for (UCapsuleComponent_Edit* Capsule : CastedComp->CollisionRefCapsules) {
					PreviewScene->AddComponent(Capsule, Capsule->GetComponentTransform(), false);
					PreviewRegisteredUpdatableComponents.Add(Capsule);
				}
				for (USphereComponent_Edit* Sphere : CastedComp->CollisionRefSpheres) {
					PreviewScene->AddComponent(Sphere, Sphere->GetComponentTransform(), false);
					PreviewRegisteredUpdatableComponents.Add(Sphere);
				}
			}
		}

		TArray<UActorComponent*> JointCompArr;
		JointCompArr = EditorSpineRigActor->GetComponentsByClass(UPhysicsConstraintComponent::StaticClass());

		for (UActorComponent* Uncasted : JointCompArr) {
			if (UPhysicsConstraintComponent* CastedJointComp = Cast<UPhysicsConstraintComponent>(Uncasted)) {
				PreviewScene->AddComponent(CastedJointComp, CastedJointComp->GetComponentTransform(), false);
				PreviewRegisteredUpdatableComponents.Add(CastedJointComp);
			}
		}

	
	}
}


#undef LOCTEXT_NAMESPACE
