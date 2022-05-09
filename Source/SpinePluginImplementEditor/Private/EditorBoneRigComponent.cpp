// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorBoneRigComponent.h"
#include "SpineRigDataEditor/EditorRigBoneDriverComponent.h"
#include "SpineRigDataEditor/SpineRigDataEditorToolkit.h"

#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "SpineRigDataEditor/EditingShapeComponent/EditorPhysicsConstraintComponent.h"

#include "SpineAnimationHandlerComponent.h"

void UEditorBoneRigComponent::GenerateDefaultRigData() {
	if (BaseData != nullptr) {

		if (GetTargetComponent()) {

			BaseData->BoneRigDataList.Empty();
			BaseData->JointList.Empty();

			spine::Vector<spine::Bone*> Bones = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getBones();
			spine::Vector<spine::IkConstraint*> Iks = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getIkConstraints();
			spine::Vector<spine::Slot*> Slots = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getSlots();
			//�����Ǿ��ִ� �� ������Ʈ�� �����͵��� ���� �����մϴ�.

			UpdateBoneData();

			for (size_t i = 0; i < Slots.size(); ++i) {

				spine::Slot* TestSlot = Slots[i];
				if (TestSlot != nullptr) {
					spine::Bone ParentBone = TestSlot->getBone();

					FName ParentBoneName = ParentBone.getData().getName().buffer();

					if (BaseData->BoneRigDataList.Contains(ParentBoneName)) {

						spine::Attachment* TestAttachment = TestSlot->getAttachment();

						if (TestAttachment != nullptr) {

							spine::String AttachmentName = TestAttachment->getName();

							if (TestAttachment->getRTTI().getClassName() == spine::RegionAttachment::rtti.getClassName()) {

								spine::RegionAttachment* CastedAttachment = (spine::RegionAttachment*)(TestAttachment);

								UBodySetup* Setup = NewObject<UBodySetup>(BaseData, NAME_None);
								BaseData->BoneRigBodySetupList.Add(ParentBoneName, Setup);


								if (UBodySetup* BodySetup = BaseData->BoneRigBodySetupList[ParentBoneName]) {

									FKBoxElem CollisionBox;
									{
										CollisionBox.Center = FVector(
											CastedAttachment->getY(),
											0,
											CastedAttachment->getX()
										);

										CollisionBox.X = CastedAttachment->getRegionOriginalHeight() * CastedAttachment->getScaleY() / 2;
										CollisionBox.Y = 3;
										CollisionBox.Z = CastedAttachment->getRegionOriginalWidth() * CastedAttachment->getScaleX() / 2;

										CollisionBox.Rotation = FRotator(CastedAttachment->getRotation(), 0, 0);
									}

									BodySetup->AggGeom.BoxElems.Add(CollisionBox);
								}
							}

							else if (TestAttachment->getRTTI().getClassName() == spine::MeshAttachment::rtti.getClassName()) {

								spine::MeshAttachment* CastedAttachment = (spine::MeshAttachment*)(TestAttachment);

								//�޽� ��� ���׷��̼��� �̿ϼ��Դϴ�.
							
								FVector2D Location;
								

								spine::Vector<size_t> MeshBones = CastedAttachment->getBones();
								spine::Vector<float> vertices = CastedAttachment->getVertices();
								
								for (int v = 0, b = 0, n = vertices.size(); v < n;) {
									int boneCount = MeshBones[b++];
									for (int ii = 0; ii < boneCount; ii++, v += 3) {
										spine::Bone* Meshbone = Bones[boneCount];
										
										Location.X += vertices[v];
										Location.Y += vertices[v + 1];
									}
								}

								if (int Size = vertices.size()) {
									Location.X /= Size;
									Location.Y /= Size;
								}

								FName MeshWeightBoneName = TestSlot->getData().getName().buffer();

								if (BaseData->BoneRigDataList.Contains(ParentBoneName)) {

									UBodySetup* Setup = NewObject<UBodySetup>(BaseData, NAME_None);
									BaseData->BoneRigBodySetupList.Add(ParentBoneName, Setup);

									if (UBodySetup* BodySetup = BaseData->BoneRigBodySetupList[ParentBoneName]) {

										FKBoxElem CollisionBox;
										{
											CollisionBox.Center = FVector(
												Location.Y,
												0,
												Location.X
											);

											CollisionBox.X = CastedAttachment->getRegionOriginalHeight();
											CollisionBox.Y = 3;
											CollisionBox.Z = CastedAttachment->getRegionOriginalWidth();

											CollisionBox.Rotation = FRotator(0, 0, 0);
										}

										BodySetup->AggGeom.BoxElems.Add(CollisionBox);
									}
								}

								/*
								int count = 0;
								size_t stride = 2;
								count =  (count >> 1) * stride;

								spine::Vector<float>* deformArray = &TestSlot->getDeform();
								spine::Vector<float>* vertices = &CastedAttachment->getVertices();
								spine::Vector<size_t>& bones = CastedAttachment->getBones();
								

								int v = 0, skip = 0;
								for (size_t i = 0; i < 0; i += 2) {
									int n = bones[v];
									v += n + 1;
									skip += n;
								}

								
								if (deformArray->size() == 0) {
									for (size_t w = 0, b = skip * 3; w < count; w += stride) {
										float wx = 0, wy = 0;
										int n = bones[v++];
										n += v;
										for (; v < n; v++, b += 3) {
											spine::Bone* boneP = Bones[bones[v]];
											spine::Bone& bone = *boneP;
											float vx = (*vertices)[b];
											float vy = (*vertices)[b + 1];
											float weight = (*vertices)[b + 2];

											FName MeshWeightBoneName = boneP->getData().getName().buffer();

											
											//wx += (vx * bone._a + vy * bone._b + bone._worldX) * weight;
											//wy += (vx * bone._c + vy * bone._d + bone._worldY) * weight;
										}
									}
								}
								else {
									for (size_t w = 0, b = skip * 3, f = skip << 1; w < count; w += stride) {
										float wx = 0, wy = 0;
										int n = bones[v++];
										n += v;
										for (; v < n; v++, b += 3, f += 2) {
											spine::Bone* boneP = Bones[bones[v]];
											spine::Bone& bone = *boneP;
											float vx = (*vertices)[b] + (*deformArray)[f];
											float vy = (*vertices)[b + 1] + (*deformArray)[f + 1];
											float weight = (*vertices)[b + 2];

											FName MeshWeightBoneName = boneP->getData().getName().buffer();

											if (BaseData->BoneRigDataList.Contains(MeshWeightBoneName)) {

												FSpineRigBoneData& SavedBoneData = BaseData->BoneRigDataList[MeshWeightBoneName];

												FKBoxElem CollisionBox;
												{
													CollisionBox.Center = FVector(
														0,
														0,
														0
													);

													CollisionBox.X = 0.1;
													CollisionBox.Y = 0.1;
													CollisionBox.Z = 0.1;

													CollisionBox.Rotation = FRotator(0, 0, 0);
												}

												SavedBoneData.BoxElems.Add(CollisionBox);
											}
											//wx += (vx * bone._a + vy * bone._b + bone._worldX) * weight;
											//wy += (vx * bone._c + vy * bone._d + bone._worldY) * weight;
										}
										//worldVertices[w] = wx;
										//worldVertices[w + 1] = wy;
									}
								}
								*/

							
					
							}
						}
					}
				
				}
			}

			UpdateConstraintMixesData();

		}
	}



}



void UEditorBoneRigComponent::UpdateBoneData() {

	if (BaseData != nullptr) {

		if (GetTargetComponent()) {

			BaseData->BoneRigDataList.Empty();

			spine::Vector<spine::Bone*> Bones = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getBones();

			for (size_t i = 0; i < Bones.size(); ++i) {

				spine::Bone* TestBone = Bones[i];
				if (TestBone != nullptr) {
					FName BoneName = TestBone->getData().getName().buffer();
					FSpineRigBoneData Data;
					BaseData->BoneRigDataList.Add(BoneName, Data);
				}
			}
		}
	}
}



void UEditorBoneRigComponent::UpdateConstraintMixesData() {

	if (BaseData != nullptr) {

		if (GetTargetComponent()) {

			spine::Vector<spine::IkConstraint*> Iks = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getIkConstraints();
			FConstraintMixStruct Data;

			for (size_t i = 0; i < Iks.size(); ++i) {

				spine::IkConstraint* TestIk = Iks[i];
				if (TestIk != nullptr) {
					FName BoneName = TestIk->getData().getName().buffer();
					Data.ConstraintMixes.Add(BoneName, TestIk->getMix());
				}
			}

			BaseData->RagdollConstraintMixData = Data;
			BaseData->DriveConstraintMixData = Data;
		}
	}
}


void UEditorBoneRigComponent::UpdateAnimationRootMotionData() {
	if (BaseData != nullptr) {

		BaseData->RootMotionDatas.Empty();

		if (GetTargetComponent()) {

			spine::Vector<spine::Animation*> Animations = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getData()->getAnimations();

			for (size_t i = 0; i < Animations.size(); ++i) {

				FSpineRootMotionData Data;

				spine::Animation* TestAnimation = Animations[i];

				if (TestAnimation != nullptr) {
					FName Name = TestAnimation->getName().buffer();
					BaseData->RootMotionDatas.Add(Name, Data);
				}
			}
		}
	}
}


void UEditorBoneRigComponent::RecreateRigWithoutFlushingAllComp(USpineAnimationHandlerComponent* Handler) {
	if (BaseData != nullptr) {

		if (Handler && Handler != GetTargetComponent()) {

			if (Handler->GetSkeletonAnimationComp()->GetSkeleton()) {

				if (AActor* Owner = this->GetOwner()) {

					SetTargetComponent(Handler);

					TArray<FName> BonesForRig;
					BaseData->BoneRigDataList.GetKeys(BonesForRig);

					TArray<FName> BonesExsiting;
					RigBoneDrivers.GetKeys(BonesExsiting);


					TArray<FName> BonesToCreate;
					TArray<FName> BonesToDelete;
					TArray<FName> BonesToUpdate;


					BonesToDelete = BonesExsiting;
					for (FName TestName : BonesForRig) {
						if (!BonesExsiting.Contains(TestName)) { //�����ͻ����� �ִµ� ���� �������� ���� ���ΰ��
							BonesToCreate.Add(TestName);
						}
						else {//�����ͻ����� �ְ� ������ �� ���
							BonesToUpdate.Add(TestName);
						}

						if (BonesToDelete.Contains(TestName)) { //�����ͻ� �ִº��� �����ϴ� ������ ���� -> �����ͻ� ���� ������ ��
							BonesToDelete.Remove(TestName);
						}
					}

					for (FName Bone : BonesToDelete) {
						if (USpineRigBoneDriverComponent* BoneDriverComp = RigBoneDrivers[Bone]) {
							BoneDriverComp->DestroyComponent();
							RigBoneDrivers.Remove(Bone);
						}
					}

					for (FName Bone : BonesToCreate) {

						FString BoneNameString = Bone.ToString();

						if (spine::Bone* TargetBone = Handler->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneNameString))) {

							FName CompName = FName(BoneNameString + "_Rig");

							if (UEditorRigBoneDriverComponent* BoneDriverComp = NewObject<UEditorRigBoneDriverComponent>(this, CompName)) {

								FTransform Transform = Handler->GetSkeletonAnimationComp()->GetBoneWorldTransform(TCHAR_TO_UTF8(*BoneNameString));
								FAttachmentTransformRules Rule = FAttachmentTransformRules::KeepWorldTransform;

								Transform.SetScale3D(FVector(1, 1, 1));
								BoneDriverComp->SetWorldTransform(Transform);
								BoneDriverComp->BoneName = BoneNameString;

								BoneDriverComp->RegisterComponentWithWorld(Owner->GetWorld());
								BoneDriverComp->AttachToComponent(this, Rule);
								Owner->AddInstanceComponent(BoneDriverComp);

								BoneDriverComp->CreateBodySetupIfNeeded();

								RigBoneDrivers.Add(Bone, BoneDriverComp);
							}
						}

					}

					TArray<FName> ArrForUpdate;
					ArrForUpdate.Append(BonesToCreate);
					ArrForUpdate.Append(BonesToUpdate);

					for (FName TestBoneName : ArrForUpdate) {

						if (RigBoneDrivers.Contains(TestBoneName)) {

							Cast<UEditorRigBoneDriverComponent>(RigBoneDrivers[TestBoneName])->BaseDataToModify = BaseData;

							UBodySetup* BoneDriverComponentBodySetup = RigBoneDrivers[TestBoneName]->GetBodySetup();

							if (UBodySetup* From = (BaseData->BoneRigBodySetupList.Contains(TestBoneName)) ? BaseData->BoneRigBodySetupList[TestBoneName] : nullptr) {
								BoneDriverComponentBodySetup->CopyBodyPropertiesFrom(From);
							}

						}
					}


					AttachAllChildBoneToClosestParentBone(false);

					CalculateBoneHierarchyMap();


#if WITH_EDITOR //Set ArrowComponent's Color
					TArray<int> IndexKeys;
					RigBoneHierarchyMap.GetKeys(IndexKeys);

					for (int Index : IndexKeys) {

						for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
							if (Comp != nullptr) {
								Comp->ChangeBoxColorByHierarchyIndex(Index, RigBoneHierarchyMap.Num());
							}
						}
					}
#endif

				}
			}
		}
	}
				
}

void UEditorBoneRigComponent::RefreshAllJointConstraintComponents() {
	if (this) {
		if (BaseData != nullptr) {

			TArray<FName> JointNames;
			BaseData->JointList.GetKeys(JointNames);

			AActor* Owner = this->GetOwner();

			if (Owner != nullptr) {

				TArray<UActorComponent*> ConstraintCompArr;
				ConstraintCompArr = Owner->GetComponentsByClass(UPhysicsConstraintComponent::StaticClass());

				TMap<FName, UPhysicsConstraintComponent*> ExsitingJointNameMap;

				//�����Ǿ��ִ� ����Ʈ�ܽ�Ʈ����Ʈ������Ʈ�� �̸��� ������. 
				for (UActorComponent* TestComp : ConstraintCompArr) {
					if (UPhysicsConstraintComponent* CastedTestComp = Cast<UPhysicsConstraintComponent>(TestComp)) {
						ExsitingJointNameMap.Add(CastedTestComp->ConstraintInstance.JointName , CastedTestComp);
					}
				}

				
				for (FName TestJointName : JointNames) { //���׺������Ϳ� �����ϴ� ��� ����Ʈ �����Ϳ� ����...
					
					if (ExsitingJointNameMap.Contains(TestJointName)) { //�̹� ������ ���� ������ ������ ������Ʈ�� ��� -> ������ ������Ʈ
						FConstraintInstance& Instance = BaseData->JointList[TestJointName].DefaultInstance;

						ExsitingJointNameMap[TestJointName]->ConstraintInstance = Instance;
						ExsitingJointNameMap[TestJointName]->SetRelativeLocation(Instance.Pos1);
						ExsitingJointNameMap[TestJointName]->UpdateConstraintFrames();
						Instance.Pos2 = ExsitingJointNameMap[TestJointName]->ConstraintInstance.Pos2;

						ExsitingJointNameMap.Remove(TestJointName); //������ �����ϴ� ������Ʈ�̸鼭 �����Ϳ����� Ȯ���� �Ǵ� �ܽ�Ʈ����Ʈ�� ����Ʈ���� ���� -> �� ����Ʈ�� ���� ���ʿ����� ������Ʈ�� �����ϴ� �뵵�� ����.
					}
					else { //������ ���� �������� �ʴ� ����Ʈ�ϰ�� -> ���Ӱ� ����
						FConstraintInstance& Instance = BaseData->JointList[TestJointName].DefaultInstance;

						if (this->RigBoneDrivers.Contains(Instance.ConstraintBone1)) { // �ڽ� ���� ���������� �����Ҷ� (���� Ÿ��)

							if (UEditorPhysicsConstraintComponent* Casted = NewObject<UEditorPhysicsConstraintComponent>(Owner)) {

								Casted->RegisterComponentWithWorld(Owner->GetWorld());
								Casted->AttachToComponent(this->RigBoneDrivers[Instance.ConstraintBone1], FAttachmentTransformRules::KeepRelativeTransform);
								Owner->AddInstanceComponent(Casted);
								Owner->AddOwnedComponent(Casted);

								Casted->SetRelativeLocation(Instance.Pos1);

								Casted->ConstraintInstance = Instance;

								Casted->ConstraintActor1 = Owner;
								Casted->ConstraintActor2 = Owner;
								Casted->ComponentName1.ComponentName = FName(Instance.ConstraintBone1.ToString() + FString("_Rig"));
								Casted->ComponentName2.ComponentName = FName(Instance.ConstraintBone2.ToString() + FString("_Rig"));

								Casted->ParentBoneComponent = Cast<UEditorRigBoneDriverComponent>(this->RigBoneDrivers[Instance.ConstraintBone1]);

								Casted->RegisterComponent();

							}
						}
						else { // �ڽ� ���� ���������� �������� ������(���� Ÿ��)
							const FText notificationText = FText::FromString(FString("Failed to create constraint " + TestJointName.ToString() + " : Bone " + Instance.ConstraintBone1.ToString() + " was Invaild!"));
							FNotificationInfo info(notificationText);
							info.bFireAndForget = true;
							info.FadeInDuration = 0.5f;
							info.FadeOutDuration = 1.0f;
							info.ExpireDuration = 4.0f;

							FSlateNotificationManager::Get().AddNotification(info);
						}
					}
				}

				TArray<FName> RemoveTargetComponentsNames; 
				ExsitingJointNameMap.GetKeys(RemoveTargetComponentsNames);

				for (FName TestJointName : RemoveTargetComponentsNames) { //���׺� �����Ϳ��� �������� �ʴ� ����Ʈ������, �����Ϳ��� �����Ұ�� -> ������ ������ �����ͻ󿡼� ���ŉ����Ƿ�, �ش� ������Ʈ ����
					Owner->RemoveInstanceComponent(ExsitingJointNameMap[TestJointName]);
					Owner->RemoveOwnedComponent(ExsitingJointNameMap[TestJointName]);

					ExsitingJointNameMap[TestJointName]->DestroyComponent();
				}
				
			}

		
		}
	}
}

void UEditorBoneRigComponent::RefreshCollisionComponents() {

	TArray<FName> BonesForRig;
	RigBoneDrivers.GetKeys(BonesForRig);

	for (FName Bone : BonesForRig) {

		if (RigBoneDrivers.Contains(Bone)) {
			Cast<UEditorRigBoneDriverComponent>(RigBoneDrivers[Bone])->RefreshRepresentingComponentsByData();
		}
	}

}

void UEditorBoneRigComponent::RequestForRefresh() {
	if (ParentToolkit.IsValid()) {
		ParentToolkit.Pin()->RefreshEditorData();
	}
}


