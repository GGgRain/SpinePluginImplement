// Fill out your copyright notice in the Description page of Project Settings.


#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "SpineRigDataEditor/EditingShapeComponent/EditorPhysicsConstraintComponent.h"
#include "SpinePluginImplementSetting.h"
#include "Internationalization/Text.h"

#include "EditorBoneRigComponent.h"

void UEditorRigBoneDriverComponent::RefreshBoxes() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			if (Data->BoneRigDataList.Contains(FName(BoneName))) {
				FSpineRigBoneData& BoneData = Data->BoneRigDataList[FName(BoneName)];


				if (UBodySetup* Setup = (Data->BoneRigBodySetupList.Contains(FName(BoneName))) ? Data->BoneRigBodySetupList[FName(BoneName)] : nullptr) {
				AActor* Owner = this->GetOwner();

				FAttachmentTransformRules Rule = FAttachmentTransformRules::KeepWorldTransform;

				if (Owner != nullptr) {

					Owner->Modify();

					FTransform ThisWorldTransform = this->GetComponentTransform();
					ThisWorldTransform.SetScale3D(FVector(0.5, 0.5, 0.5));

					int DataNum = Setup->AggGeom.BoxElems.Num();

					int CompNum = CollisionRefBoxs.Num();

					for (int i = CompNum; i > DataNum; --i) { // 현존하는 컴포넌트가 더 많을때

						if (UBoxComponent_Edit* TestComp = CollisionRefBoxs[i - 1]) {
							Owner->RemoveInstanceComponent(TestComp);
							Owner->RemoveOwnedComponent(TestComp);

							TestComp->DestroyComponent();

							CollisionRefBoxs.RemoveAt(i - 1);
						}
					}

					for (int i = DataNum; i > CompNum; --i) { // 현존하는 컴포넌트가 더 적을때

						if (UBoxComponent_Edit* Casted = NewObject<UBoxComponent_Edit>(Owner)) {

							Casted->Rename(*FName(this->BoneName + Casted->GetName()).ToString());
							Casted->SetWorldTransform(ThisWorldTransform);

							Casted->RegisterComponentWithWorld(Owner->GetWorld());
							Casted->AttachToComponent(this, Rule);
							Owner->AddInstanceComponent(Casted);
							Owner->AddOwnedComponent(Casted);

							Casted->RegisterComponent();

							CollisionRefBoxs.Add(Casted);


						}
					}

					for (int Index = 0; Index < DataNum; ++Index) {
						if (UBoxComponent_Edit* ShapeComp = CollisionRefBoxs[Index]) {
							if (UBodySetup* Body = ShapeComp->GetBodySetup()) {
								ShapeComp->ShapeColor = this->BoneColor;

								FKBoxElem& Box = Setup->AggGeom.BoxElems[Index];

								Body->AggGeom.BoxElems.Empty();
								Body->AggGeom.BoxElems.Add(Box);

								ShapeComp->UpdateBodySetup();

								FTransform Trans;
								Trans = Box.GetTransform();
								Trans.SetScale3D(FVector(0.5, 0.5, 0.5));

								ShapeComp->SetBoxExtent(FVector(Box.X, Box.Y, Box.Z));

								ShapeComp->SetRelativeTransform(Trans);

								ShapeComp->ParentBoneComponent = this;

								ShapeComp->MarkRenderStateDirty();
							}
						}
					}
					}
				}
			}
		}
	}
}

void UEditorRigBoneDriverComponent::RefreshCapsules() {

}


void UEditorRigBoneDriverComponent::RefreshSpheres() {

}

void UEditorRigBoneDriverComponent::RefreshRepresentingComponentsByData() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			RefreshBoxes();
			RefreshCapsules();
			RefreshSpheres();
		}
	}
}

void UEditorRigBoneDriverComponent::RefreshRepresentingConstraintComponentsByData() {
	if (this) {
		TArray<USceneComponent*> Arr;
		this->GetParentComponents(Arr);

		for (USceneComponent* Comp : Arr) {
			if (UEditorBoneRigComponent* RigComp = Cast<UEditorBoneRigComponent>(Comp)) {
				RigComp->RefreshAllJointConstraintComponents();

				RigComp->RequestForRefresh();
				break;
			}
		}
	}
}


void UEditorRigBoneDriverComponent::DeleteRepresentingComponent(USceneComponent* Comp) {
	AActor* Owner = this->GetOwner();

	if (Owner != nullptr) {

		bool bFound = false;

		if (CollisionRefBoxs.Contains(Comp)) {
			CollisionRefBoxs.Remove((UBoxComponent_Edit*)Comp);
			bFound = true;
		}
		else if (CollisionRefCapsules.Contains(Comp)) {
			CollisionRefCapsules.Remove((UCapsuleComponent_Edit*)Comp);
			bFound = true;
		}
		else if (CollisionRefSpheres.Contains(Comp)) {
			CollisionRefSpheres.Remove((USphereComponent_Edit*)Comp);
			bFound = true;
		}

		if (bFound) {
			Owner->RemoveInstanceComponent(Comp);
			Owner->RemoveOwnedComponent(Comp);

			Comp->DestroyComponent();
		}
	}
}

void UEditorRigBoneDriverComponent::CollectCollisionByRepresentingComponents() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			if (Data->BoneRigDataList.Contains(FName(BoneName))) {
				FSpineRigBoneData& BoneData = Data->BoneRigDataList[FName(BoneName)];
				
				if (UBodySetup* Setup = (Data->BoneRigBodySetupList.Contains(FName(BoneName))) ? Data->BoneRigBodySetupList[FName(BoneName)] : nullptr) {
					Setup->AggGeom.EmptyElements();

					for (UBoxComponent_Edit* Box : CollisionRefBoxs) {
						TArray<FKBoxElem> Arr = Box->GetBodySetup()->AggGeom.BoxElems;
						FVector Location = Box->GetRelativeLocation();
						FVector Scale = Box->GetRelativeScale3D();
						FRotator Rotation = Box->GetRelativeRotation();

						for (FKBoxElem& Test : Arr) {
							Test.Center = Location;

							Test.Rotation = Rotation;

							Test.X = FMath::Abs(Box->GetUnscaledBoxExtent().X);
							Test.Y = FMath::Abs(Box->GetUnscaledBoxExtent().Y);
							Test.Z = FMath::Abs(Box->GetUnscaledBoxExtent().Z);

							Test.X = (FMath::IsNearlyZero(Test.X)) ? 0.1 : Test.X;
							Test.Y = (FMath::IsNearlyZero(Test.Y)) ? 0.1 : Test.Y;
							Test.Z = (FMath::IsNearlyZero(Test.Z)) ? 0.1 : Test.Z;
						}
						Setup->AggGeom.BoxElems.Append(Arr);
					}
					for (UCapsuleComponent_Edit* Capsule : CollisionRefCapsules) {
						Setup->AggGeom.TaperedCapsuleElems.Append(Capsule->GetBodySetup()->AggGeom.TaperedCapsuleElems);
					}
					for (USphereComponent_Edit* Sphere : CollisionRefSpheres) {
						Setup->AggGeom.SphereElems.Append(Sphere->GetBodySetup()->AggGeom.SphereElems);
					}
				}
			}
		}
	}
}


void UEditorRigBoneDriverComponent::CollectConstraintDataByComponent() {
	if (this) {

		if (BaseDataToModify != nullptr) {

			TArray<FName> JointConstraintDataKeys;
			BaseDataToModify->JointList.GetKeys(JointConstraintDataKeys);

			TArray<USceneComponent*> Arr;
			Arr = this->GetAttachChildren();

			for (USceneComponent* TestComponent : Arr) {

				if (UEditorPhysicsConstraintComponent* Component = Cast<UEditorPhysicsConstraintComponent>(TestComponent)) {

					FName TestJointName = Component->ConstraintInstance.JointName;
					FName TestJointBone1 = Component->ConstraintInstance.ConstraintBone1;
					FName TestJointBone2 = Component->ConstraintInstance.ConstraintBone2;

					for (FName TestKey : JointConstraintDataKeys) {
						FJointConstraintData& DataToModify = BaseDataToModify->JointList[TestKey];

						if (
							DataToModify.DefaultInstance.JointName == TestJointName &&
							DataToModify.DefaultInstance.ConstraintBone1 == TestJointBone1 &&
							DataToModify.DefaultInstance.ConstraintBone2 == TestJointBone2
							)
						{ //수정하려는 조인트를 찾았을때.
							DataToModify.DefaultInstance = Component->ConstraintInstance;
							break;
						}
					}
				}
			
			}
		}
	}
}


FPrimitiveSceneProxy* UEditorRigBoneDriverComponent::CreateSceneProxy()
{
	/** Represents a UBoxComponent_Edit to the scene manager. */
	class FBoneBaseSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FBoneBaseSceneProxy(const USpineRigBoneDriverComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(false)
			, BoxExtents(InComponent->GetComponentScale() * FVector(1,5,1))
			, BoxColor(InComponent->BoneColor)
			, LineThickness(0.5)
		{
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_BoxSceneProxy_GetDynamicMeshElements);

			const FMatrix& LocalToWorld = GetLocalToWorld();

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];

					const FLinearColor DrawColor = GetViewSelectionColor(BoxColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					DrawOrientedWireBox(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis(EAxis::X), LocalToWorld.GetScaledAxis(EAxis::Y), LocalToWorld.GetScaledAxis(EAxis::Z), BoxExtents, DrawColor, SDPG_World, LineThickness);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const uint32	bDrawOnlyIfSelected : 1;
		const FVector	BoxExtents;
		const FColor	BoxColor;
		const float LineThickness;
	};

	return new FBoneBaseSceneProxy(this);
}


void UEditorRigBoneDriverComponent::Callback_ChildPropertyChange() {
	if (bUpdatable) {
		bUpdatable = false;

		CollectCollisionByRepresentingComponents();
		RefreshRepresentingComponentsByData();

		CollectConstraintDataByComponent();
		RefreshRepresentingConstraintComponentsByData();
		

		bUpdatable = true;
	}
}

void UEditorRigBoneDriverComponent::AddNewBoxElemToCurrentBoneData() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			if (Data->BoneRigDataList.Contains(FName(BoneName))) {

				if (!Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
					UBodySetup* Setup = NewObject<UBodySetup>(Data, NAME_None);
					Data->BoneRigBodySetupList.Add(FName(BoneName), Setup);

					const FText notificationText = FText::FromString("Automatically generated a bodysetup for the bone : " + BoneName);
					FNotificationInfo info(notificationText);
					info.bFireAndForget = true;
					info.FadeInDuration = 0.5f;
					info.FadeOutDuration = 1.0f;
					info.ExpireDuration = 4.0f;

					FSlateNotificationManager::Get().AddNotification(info);
				}

				if (Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
					if (UBodySetup* BodySetup = (Data->BoneRigBodySetupList.Contains(FName(BoneName))) ? Data->BoneRigBodySetupList[FName(BoneName)] : nullptr) {

						BodySetup->AggGeom.BoxElems.Add(FKBoxElem());

						const FText notificationText = NSLOCTEXT("Notifications", "SucceedNotify", "Box added!");
						FNotificationInfo info(notificationText);
						info.bFireAndForget = true;
						info.FadeInDuration = 0.5f;
						info.FadeOutDuration = 1.0f;
						info.ExpireDuration = 4.0f;

						FSlateNotificationManager::Get().AddNotification(info);

						RequestForRefresh();
					}
					else {
						const FText notificationText = NSLOCTEXT("Notifications", "FailedNotify", "There is error in storing bodysetup! check the code in EditorRigBoneDriveerComponent.cpp");

						FNotificationInfo info(notificationText);
						info.bFireAndForget = true;
						info.FadeInDuration = 0.5f;
						info.FadeOutDuration = 1.0f;
						info.ExpireDuration = 4.0f;

						FSlateNotificationManager::Get().AddNotification(info);
					}

					if (Data->PreviewSkeletonData && Data->PreviewAtlas) {


						spine::Vector<spine::IkConstraintData*> IkArr = Data->PreviewSkeletonData->GetSkeletonData(Data->PreviewAtlas->GetAtlas())->getIkConstraints();

						size_t ArrSize = IkArr.size();
						for (size_t Index = 0; Index < ArrSize; ++Index) {

							if (FString(IkArr[Index]->getTarget()->getName().buffer()) == BoneName) {

								if (!Data->RigCollisionPresetOverride.Contains(FName(BoneName))) {

									if (const USpinePluginImplementSetting* EditorSetting = USpinePluginImplementSetting::Get()) {

										FText Text = NSLOCTEXT("Notifications", "OverrideAssignSucceedNotify", "This bone is Ik TargetBone; Automatically assigned to the RigCollisionPresetOverride Map with Preset : {0} !");
										FText notificationText = FText::Format(Text, FText::FromString(EditorSetting->DefaultBoneRigIKBoneOverrideCollisionProfileName.ToString()));


										FNotificationInfo info(notificationText);
										info.bFireAndForget = true;
										info.FadeInDuration = 0.5f;
										info.FadeOutDuration = 1.0f;
										info.ExpireDuration = 4.0f;

										FSlateNotificationManager::Get().AddNotification(info);

										FBodyInstance Inst;
										Inst.SetCollisionProfileName(EditorSetting->DefaultBoneRigIKBoneOverrideCollisionProfileName);

										FSpineBoneBodyPresetStruct PresetStruct;
										PresetStruct.CollisionPreset = Inst;
										PresetStruct.Ragdoll_CollisionPreset = Inst;

										Data->RigCollisionPresetOverride.Add(FName(BoneName), PresetStruct);

									}
								}
								else {
									const FText notificationText = NSLOCTEXT("Notifications", "OverrideAssignFailedNotify", "This bone is Ik TargetBone; But There is already a preset has the same name");

									FNotificationInfo info(notificationText);
									info.bFireAndForget = true;
									info.FadeInDuration = 0.5f;
									info.FadeOutDuration = 1.0f;
									info.ExpireDuration = 4.0f;

									FSlateNotificationManager::Get().AddNotification(info);
								}
							}
						}

					}
				}
			}
		}
	}
}

void UEditorRigBoneDriverComponent::AddNewSphereElemToCurrentBoneData() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			if (Data->BoneRigDataList.Contains(FName(BoneName))) {

				if (!Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
					UBodySetup* Setup = NewObject<UBodySetup>(Data, NAME_None);
					Data->BoneRigBodySetupList.Add(FName(BoneName), Setup);

					const FText notificationText = FText::FromString("Automatically generated a bodysetup for the bone : " + BoneName);
					FNotificationInfo info(notificationText);
					info.bFireAndForget = true;
					info.FadeInDuration = 0.5f;
					info.FadeOutDuration = 1.0f;
					info.ExpireDuration = 4.0f;

					FSlateNotificationManager::Get().AddNotification(info);
				}

				if (Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
					if (UBodySetup* BodySetup = (Data->BoneRigBodySetupList.Contains(FName(BoneName))) ? Data->BoneRigBodySetupList[FName(BoneName)] : nullptr) {

						BodySetup->AggGeom.SphereElems.Add(FKSphereElem());

						const FText notificationText = NSLOCTEXT("Notifications", "SucceedNotify", "Sphere added!");
						FNotificationInfo info(notificationText);
						info.bFireAndForget = true;
						info.FadeInDuration = 0.5f;
						info.FadeOutDuration = 1.0f;
						info.ExpireDuration = 4.0f;

						FSlateNotificationManager::Get().AddNotification(info);

						RequestForRefresh();
					}
					else {
						const FText notificationText = NSLOCTEXT("Notifications", "FailedNotify", "There is error in storing bodysetup! check the code in EditorRigBoneDriveerComponent.cpp");

						FNotificationInfo info(notificationText);
						info.bFireAndForget = true;
						info.FadeInDuration = 0.5f;
						info.FadeOutDuration = 1.0f;
						info.ExpireDuration = 4.0f;

						FSlateNotificationManager::Get().AddNotification(info);
					}

					if (Data->PreviewSkeletonData && Data->PreviewAtlas) {


						spine::Vector<spine::IkConstraintData*> IkArr = Data->PreviewSkeletonData->GetSkeletonData(Data->PreviewAtlas->GetAtlas())->getIkConstraints();

						size_t ArrSize = IkArr.size();
						for (size_t Index = 0; Index < ArrSize; ++Index) {

							if (FString(IkArr[Index]->getTarget()->getName().buffer()) == BoneName) {

								if (!Data->RigCollisionPresetOverride.Contains(FName(BoneName))) {

									if (const USpinePluginImplementSetting* EditorSetting = USpinePluginImplementSetting::Get()) {

										FText Text = NSLOCTEXT("Notifications", "OverrideAssignSucceedNotify", "This bone is Ik TargetBone; Automatically assigned to the RigCollisionPresetOverride Map with Preset : {0} !");
										FText notificationText = FText::Format(Text, FText::FromString(EditorSetting->DefaultBoneRigIKBoneOverrideCollisionProfileName.ToString()));

										FNotificationInfo info(notificationText);
										info.bFireAndForget = true;
										info.FadeInDuration = 0.5f;
										info.FadeOutDuration = 1.0f;
										info.ExpireDuration = 4.0f;

										FSlateNotificationManager::Get().AddNotification(info);

										FBodyInstance Inst;
										Inst.SetCollisionProfileName(EditorSetting->DefaultBoneRigIKBoneOverrideCollisionProfileName);

										FSpineBoneBodyPresetStruct PresetStruct;
										PresetStruct.CollisionPreset = Inst;
										PresetStruct.Ragdoll_CollisionPreset = Inst;

										Data->RigCollisionPresetOverride.Add(FName(BoneName), PresetStruct);

									}
								}
								else {
									const FText notificationText = NSLOCTEXT("Notifications", "OverrideAssignFailedNotify", "This bone is Ik TargetBone; But There is already a preset has the same name");

									FNotificationInfo info(notificationText);
									info.bFireAndForget = true;
									info.FadeInDuration = 0.5f;
									info.FadeOutDuration = 1.0f;
									info.ExpireDuration = 4.0f;

									FSlateNotificationManager::Get().AddNotification(info);
								}
							}
						}

					}
				}
			}
		}
	}
}

void UEditorRigBoneDriverComponent::AddNewCapsuleElemToCurrentBoneData() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			if (Data->BoneRigDataList.Contains(FName(BoneName))) {

				if (!Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
					UBodySetup* Setup = NewObject<UBodySetup>(Data, NAME_None);
					Data->BoneRigBodySetupList.Add(FName(BoneName), Setup);

					const FText notificationText = FText::FromString("Automatically generated a bodysetup for the bone : " + BoneName);
					FNotificationInfo info(notificationText);
					info.bFireAndForget = true;
					info.FadeInDuration = 0.5f;
					info.FadeOutDuration = 1.0f;
					info.ExpireDuration = 4.0f;

					FSlateNotificationManager::Get().AddNotification(info);
				}

				if (Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
					if (UBodySetup* BodySetup = (Data->BoneRigBodySetupList.Contains(FName(BoneName))) ? Data->BoneRigBodySetupList[FName(BoneName)] : nullptr) {

						BodySetup->AggGeom.TaperedCapsuleElems.Add(FKTaperedCapsuleElem());

							const FText notificationText = NSLOCTEXT("Notifications", "SucceedNotify", "Capsule added!");
							FNotificationInfo info(notificationText);
							info.bFireAndForget = true;
							info.FadeInDuration = 0.5f;
							info.FadeOutDuration = 1.0f;
							info.ExpireDuration = 4.0f;

							FSlateNotificationManager::Get().AddNotification(info);

						RequestForRefresh();
					}
					else {
						const FText notificationText = NSLOCTEXT("Notifications", "FailedNotify", "There is error in storing bodysetup! check the code in EditorRigBoneDriveerComponent.cpp");

						FNotificationInfo info(notificationText);
						info.bFireAndForget = true;
						info.FadeInDuration = 0.5f;
						info.FadeOutDuration = 1.0f;
						info.ExpireDuration = 4.0f;

						FSlateNotificationManager::Get().AddNotification(info);
					}

					if (Data->PreviewSkeletonData && Data->PreviewAtlas) {


						spine::Vector<spine::IkConstraintData*> IkArr = Data->PreviewSkeletonData->GetSkeletonData(Data->PreviewAtlas->GetAtlas())->getIkConstraints();

						size_t ArrSize = IkArr.size();
						for (size_t Index = 0; Index < ArrSize; ++Index) {

							if (FString(IkArr[Index]->getTarget()->getName().buffer()) == BoneName) {

								if (!Data->RigCollisionPresetOverride.Contains(FName(BoneName))) {

									if (const USpinePluginImplementSetting* EditorSetting = USpinePluginImplementSetting::Get()) {

										FText Text = NSLOCTEXT("Notifications", "OverrideAssignSucceedNotify", "This bone is Ik TargetBone; Automatically assigned to the RigCollisionPresetOverride Map with Preset : {0} !");
										FText notificationText = FText::Format(Text, FText::FromString(EditorSetting->DefaultBoneRigIKBoneOverrideCollisionProfileName.ToString()));

										FNotificationInfo info(notificationText);
										info.bFireAndForget = true;
										info.FadeInDuration = 0.5f;
										info.FadeOutDuration = 1.0f;
										info.ExpireDuration = 4.0f;

										FSlateNotificationManager::Get().AddNotification(info);

										FBodyInstance Inst;
										Inst.SetCollisionProfileName(EditorSetting->DefaultBoneRigIKBoneOverrideCollisionProfileName);

										FSpineBoneBodyPresetStruct PresetStruct;
										PresetStruct.CollisionPreset = Inst;
										PresetStruct.Ragdoll_CollisionPreset = Inst;

										Data->RigCollisionPresetOverride.Add(FName(BoneName), PresetStruct);

									}
								}
								else {
									const FText notificationText = NSLOCTEXT("Notifications", "OverrideAssignFailedNotify", "This bone is Ik TargetBone; But There is already a preset has the same name");

									FNotificationInfo info(notificationText);
									info.bFireAndForget = true;
									info.FadeInDuration = 0.5f;
									info.FadeOutDuration = 1.0f;
									info.ExpireDuration = 4.0f;

									FSlateNotificationManager::Get().AddNotification(info);
								}
							}
						}

					}
				}


			}
		}
	}
}

void UEditorRigBoneDriverComponent::AddNewConstraintBetweenParentBone() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			FName BoneFName = FName(BoneName);
			if (Data->BoneRigDataList.Contains(BoneFName)) {
				FSpineRigBoneData& BoneData = Data->BoneRigDataList[BoneFName];

				if (Data->PreviewSkeletonData && Data->PreviewAtlas) {
					spine::BoneData* CurrentBone = Data->PreviewSkeletonData->GetSkeletonData(Data->PreviewAtlas->GetAtlas())->findBone(TCHAR_TO_UTF8(*BoneName));

					if (CurrentBone->getParent()) {

						if (Data->BoneRigDataList.Contains(FName(CurrentBone->getParent()->getName().buffer()))) {


							FName TestName = FName(BoneName + "Joint");

							if (!Data->JointList.Contains(TestName)) {

								FJointConstraintData NewJointData;
								NewJointData.DefaultInstance.JointName = FName(BoneName + "Joint");
								NewJointData.DefaultInstance.ConstraintBone1 = BoneFName;
								NewJointData.DefaultInstance.ConstraintBone2 = FName(CurrentBone->getParent()->getName().buffer());

								NewJointData.DefaultInstance.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Locked);
								NewJointData.DefaultInstance.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Limited);
								NewJointData.DefaultInstance.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Locked);

								NewJointData.DefaultInstance.SetDisableCollision(true);

								Data->JointList.Add(TestName, NewJointData);

								const FText notificationText = NSLOCTEXT("Notifications", "SucceedNotify", "Constraint added!");
								FNotificationInfo info(notificationText);
								info.bFireAndForget = true;
								info.FadeInDuration = 0.5f;
								info.FadeOutDuration = 1.0f;
								info.ExpireDuration = 4.0f;

								FSlateNotificationManager::Get().AddNotification(info);
							}
							else {

								const FText notificationText = NSLOCTEXT("Notifications", "FailedNotify", "Already there is same joint on the list!");
								FNotificationInfo info(notificationText);
								info.bFireAndForget = true;
								info.FadeInDuration = 0.5f;
								info.FadeOutDuration = 1.0f;
								info.ExpireDuration = 4.0f;

								FSlateNotificationManager::Get().AddNotification(info);
							}
						}
						else {
							const FText notificationText = NSLOCTEXT("Notifications", "ParentBoneNullNotify", "Parent bone is not in the Rig bone list!");
							FNotificationInfo info(notificationText);
							info.bFireAndForget = true;
							info.FadeInDuration = 0.5f;
							info.FadeOutDuration = 1.0f;
							info.ExpireDuration = 4.0f;

							FSlateNotificationManager::Get().AddNotification(info);
						}
					}
					else {

						const FText notificationText = NSLOCTEXT("Notifications", "ParentBoneNullNotify", "Parent bone in spine data was nullptr! - Is this root bone?");
						FNotificationInfo info(notificationText);
						info.bFireAndForget = true;
						info.FadeInDuration = 0.5f;
						info.FadeOutDuration = 1.0f;
						info.ExpireDuration = 4.0f;

						FSlateNotificationManager::Get().AddNotification(info);

					}

					RequestForRefresh();
				}
			}

		}
	}
}


void UEditorRigBoneDriverComponent::FlushAllShapes() {
	if (USpineRigData* Data = BaseDataToModify) {
		if (!BoneName.IsEmpty()) {
			if (Data->BoneRigDataList.Contains(FName(BoneName))) {

				if (!Data->BoneRigBodySetupList.Contains(FName(BoneName))) {


					const FText notificationText = FText::FromString("No bodysetup (of course also the shapes) for bone : " + BoneName + " yet! So we will not flush it...");
					FNotificationInfo info(notificationText);
					info.bFireAndForget = true;
					info.FadeInDuration = 0.5f;
					info.FadeOutDuration = 1.0f;
					info.ExpireDuration = 4.0f;

					FSlateNotificationManager::Get().AddNotification(info);
				}
				else {

					if (Data->BoneRigBodySetupList.Contains(FName(BoneName))) {
						if (UBodySetup* BodySetup = (Data->BoneRigBodySetupList.Contains(FName(BoneName))) ? Data->BoneRigBodySetupList[FName(BoneName)] : nullptr) {

							BodySetup->AggGeom.EmptyElements();

							const FText notificationText = NSLOCTEXT("Notifications", "SucceedNotify", "Flushed!");

							FNotificationInfo info(notificationText);
							info.bFireAndForget = true;
							info.FadeInDuration = 0.5f;
							info.FadeOutDuration = 1.0f;
							info.ExpireDuration = 4.0f;

							FSlateNotificationManager::Get().AddNotification(info);

							RequestForRefresh();
						}
						else {
							const FText notificationText = NSLOCTEXT("Notifications", "FailedNotify", "There is error in storing bodysetup! check the code in EditorRigBoneDriveerComponent.cpp");

							FNotificationInfo info(notificationText);
							info.bFireAndForget = true;
							info.FadeInDuration = 0.5f;
							info.FadeOutDuration = 1.0f;
							info.ExpireDuration = 4.0f;

							FSlateNotificationManager::Get().AddNotification(info);
						}
					}
				}
			}
		}
	}
}


void UEditorRigBoneDriverComponent::RequestForRefresh() {
	if (AActor* Owner = GetOwner()) {
		UActorComponent* Uncasted = Owner->GetComponentByClass(UEditorBoneRigComponent::StaticClass());
		Cast<UEditorBoneRigComponent>(Uncasted)->RequestForRefresh();
	}
}