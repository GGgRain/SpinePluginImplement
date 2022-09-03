// Fill out your copyright notice in the Description page of Project Settings.


#include "BoneRigComponent.h"

#if WITH_EDITOR 
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "DrawDebugHelpers.h"
#endif

#include "SceneManagement.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysXPublic.h"
#include "Physics/PhysScene_PhysX.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include <spine/Vector.h>


#include "SpineAnimationHandlerComponent.h"


const FConstraintProfileProperties UBoneRigComponent::PhysicalAnimationProfile = []()
{
	//Setup the default constraint profile for all joints created by physical animation system
	FConstraintProfileProperties RetProfile;
	RetProfile.LinearLimit.XMotion = LCM_Free;
	RetProfile.LinearLimit.YMotion = LCM_Free;
	RetProfile.LinearLimit.ZMotion = LCM_Free;

	RetProfile.ConeLimit.Swing1Motion = ACM_Free;
	RetProfile.ConeLimit.Swing2Motion = ACM_Free;
	RetProfile.TwistLimit.TwistMotion = ACM_Free;

	RetProfile.LinearDrive.XDrive.bEnablePositionDrive = true;
	RetProfile.LinearDrive.XDrive.bEnableVelocityDrive = true;
	RetProfile.LinearDrive.YDrive.bEnablePositionDrive = true;
	RetProfile.LinearDrive.YDrive.bEnableVelocityDrive = true;
	RetProfile.LinearDrive.ZDrive.bEnablePositionDrive = true;
	RetProfile.LinearDrive.ZDrive.bEnableVelocityDrive = true;

	RetProfile.AngularDrive.SlerpDrive.bEnablePositionDrive = true;
	RetProfile.AngularDrive.SlerpDrive.bEnableVelocityDrive = true;
	RetProfile.AngularDrive.AngularDriveMode = EAngularDriveMode::SLERP;

	RetProfile.bEnableProjection = true;
	RetProfile.bParentDominates = true;

	return RetProfile;
}();


// Sets default values for this component's properties
UBoneRigComponent::UBoneRigComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


	/*
	this->BodyInstance.bLockXTranslation = false;
	this->BodyInstance.bLockYTranslation = true;
	this->BodyInstance.bLockZTranslation = false;

	this->BodyInstance.bLockXRotation = true;
	this->BodyInstance.bLockYRotation = false;
	this->BodyInstance.bLockZRotation = true;

	*/

	this->BodyInstance.bLockXTranslation = false;
	this->BodyInstance.bLockYTranslation = true;
	this->BodyInstance.bLockZTranslation = false;

	this->BodyInstance.bLockXRotation = true;
	this->BodyInstance.bLockYRotation = false;
	this->BodyInstance.bLockZRotation = true;
	// ...
}

void UBoneRigComponent::BeginPlay() {

	Super::BeginPlay();

	if (!bInitializeInOutsideComponent) {
		InitializeRig();
	}
}

void UBoneRigComponent::EndPlay(EEndPlayReason::Type EndReason) {

	//Release
	StopPhysicalAnimationAdjust();

	RemoveJointConstraints();

	Super::EndPlay(EndReason);
}
void UBoneRigComponent::InitializeRig() {

	if (bInitialized) {
		return;
	}

	if (TargetComponent) {

		if (TargetComponent->SkeletonAnimationComp) {
			if (!TargetComponent->SkeletonAnimationComp->HasBegunPlay())
			{
				TargetComponent->SkeletonAnimationComp->BeginPlay();
			}
		}

		if (!TargetComponent->HasBegunPlay())
		{
			TargetComponent->BeginPlay();
		}
	}

	CreateCollisionSetupForBones();

	if (TargetComponent)
	{
		TargetComponent->TickComponent(0, ELevelTick::LEVELTICK_All, nullptr);
		OnTeleportDelegateHandle = TargetComponent->RegisterOnTeleportDelegate(FOnHandlerTeleported::CreateUObject(this, &UBoneRigComponent::OnTeleport));

		TargetComponent->AnimationStartedHandle.AddDynamic(this, &UBoneRigComponent::OnAnimationStarted);
		TargetComponent->AnimationEndedHandle.AddDynamic(this, &UBoneRigComponent::OnAnimationEnded);
		UpdatePhysicsEngine();
	}

	//DEBUG
	//BindAllBodyOnOverlapBeginEvent();


	bInitialized = true;

}

void UBoneRigComponent::DestroyComponent(bool bPromoteChildren) {

	SetUpdate_None();

	UnbindAllBodyOnOverlapBeginEvent();

	if (TargetComponent && OnTeleportDelegateHandle.IsValid())
	{
		TargetComponent->UnregisterOnTeleportDelegate(OnTeleportDelegateHandle);

		TargetComponent->AnimationStartedHandle.RemoveDynamic(this, &UBoneRigComponent::OnAnimationStarted);
		TargetComponent->AnimationEndedHandle.RemoveDynamic(this, &UBoneRigComponent::OnAnimationEnded);
	}

	if (TargetComponent->IsValidLowLevel()) {

		if (TargetComponent->BoneRigArr.Contains(this)) {
			TargetComponent->BoneRigArr.Remove(this);
		}
	}

	TargetComponent = nullptr;

	Super::DestroyComponent(bPromoteChildren);
}


void UBoneRigComponent::SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent) {
	if (InTargetComponent) {

		if (TargetComponent != nullptr) {

			if (TargetComponent->BoneRigArr.Contains(this)) {
				TargetComponent->BoneRigArr.Remove(this);
			}

			if (TargetComponent && OnTeleportDelegateHandle.IsValid())
			{
				TargetComponent->UnregisterOnTeleportDelegate(OnTeleportDelegateHandle);
			}

			TargetComponent->AnimationStartedHandle.RemoveDynamic(this, &UBoneRigComponent::OnAnimationStarted);
			TargetComponent->AnimationEndedHandle.RemoveDynamic(this, &UBoneRigComponent::OnAnimationEnded);

			TargetComponent = nullptr;
		}

		if (InTargetComponent) {

			if (!InTargetComponent->BoneRigArr.Contains(this)) {
				InTargetComponent->BoneRigArr.Add(this);
			}

			if (TargetComponent)
			{
				OnTeleportDelegateHandle = TargetComponent->RegisterOnTeleportDelegate(FOnHandlerTeleported::CreateUObject(this, &UBoneRigComponent::OnTeleport));
				TargetComponent->AnimationStartedHandle.AddDynamic(this, &UBoneRigComponent::OnAnimationStarted);
				TargetComponent->AnimationEndedHandle.AddDynamic(this, &UBoneRigComponent::OnAnimationEnded);
			}

			TargetComponent = InTargetComponent;
		}
	}
}



void UBoneRigComponent::InitializeState() {
	if (this) {

		EBoneRigState SavedCurrentState = CurrentState;

		CurrentState = EBoneRigState::STATE_NONE;

		switch (SavedCurrentState) {
		case EBoneRigState::STATE_FOLLWING: {
			SetUpdate_Following();
			break;
		}
		case EBoneRigState::STATE_DRIVING: {
			SetUpdate_Driving();
			break;
		}
		case EBoneRigState::STATE_RAGDOLL: {
			SetUpdate_RagdollUpdating();
			break;
		}

		}
	}
}


void UBoneRigComponent::CreateRig() {
	if (BaseData != nullptr) {

		if (TargetComponent) {

			if (AActor* Owner = this->GetOwner()) {

				TransformUpdateRootBone = BaseData->DefaultTransformUpdateRootBone;

				if (TargetComponent->GetSkeletonAnimationComp() != nullptr) {

					//본 재생성
					CreateBones();

					AttachAllChildBoneToClosestParentBone(true);

					//TransformUpdateRootBone = BaseData->DefaultTransformUpdateRootBone;

#if WITH_EDITOR //Set ArrowComponent's Color
					TArray<int> IndexKeys;
					RigBoneHierarchyMap.GetKeys(IndexKeys);

					for (const int& Index : IndexKeys) {

						for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
							if (Comp != nullptr) {
								Comp->ChangeBoxColorByHierarchyIndex(Index, RigBoneHierarchyMap.Num());
							}
						}
					}
#endif


					Owner->RerunConstructionScripts();

					Owner->ReregisterAllComponents();
				}
				else {
#if WITH_EDITOR 
					const FText notificationText = FText::FromString(FString("Its spine skeleton object was not valid! - Did you set a vaild data to the renderer component?"));
					FNotificationInfo info(notificationText);
					info.bFireAndForget = true;
					info.FadeInDuration = 0.5f;
					info.FadeOutDuration = 1.0f;
					info.ExpireDuration = 4.0f;

					FSlateNotificationManager::Get().AddNotification(info);

#endif
				}

			}
		}

	}

}

void UBoneRigComponent::RemoveRig() {

	SetUpdate_None();

	RemoveBones();

	RemoveJointConstraints(); //콘스트레인트들은 Rig의 바디에 기반해 작동하므로 Rig제거시에 같이 제거됩니다.

	RigBoneHierarchyMap.Empty();
}


void UBoneRigComponent::ApplySimulatePhysicsToBones(EPhysicsSimulationMode Mode) {
	TArray<FName> BoneDriverKeys;
	RigBoneDrivers.GetKeys(BoneDriverKeys);

	FAttachmentTransformRules Rule = FAttachmentTransformRules::KeepWorldTransform;
	Rule.bWeldSimulatedBodies = true;

	switch (Mode) {
	case EPhysicsSimulationMode::NONE: {

		for (const FName& Key : BoneDriverKeys) {
			if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

				if (this->BaseData->BoneRigBodySetupList.Contains(Key)) {
					if (UBodySetup* Setup = this->BaseData->BoneRigBodySetupList[Key]) {
						//Comp->AttachToComponent(this, Rule);
						if (Comp->BodyInstance.bSimulatePhysics != false) {
							Comp->BodyInstance.bSimulatePhysics = false;

							Comp->ReregisterComponent();
						}
					}
				}
			}

		}

		break;
	}
	case EPhysicsSimulationMode::ALL: {

		for (const FName& Key : BoneDriverKeys) {
			if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

				if (this->BaseData->BoneRigBodySetupList.Contains(Key)) {
					if (UBodySetup* Setup = this->BaseData->BoneRigBodySetupList[Key]) {

						//Comp->AttachToComponent(this, Rule);
						if (Comp->BodyInstance.bSimulatePhysics != true) {
							Comp->BodyInstance.bSimulatePhysics = true;

							Comp->ReregisterComponent();
						}
					}
				}

			}
		}

		break;
	}
	case EPhysicsSimulationMode::ONLY_SIMULATED: {

		for (const FName& Key : BoneDriverKeys) {
			if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

				if (this->BaseData->BoneRigBodySetupList.Contains(Key)) {
					if (UBodySetup* Setup = this->BaseData->BoneRigBodySetupList[Key]) {
						if (Setup->PhysicsType == EPhysicsType::PhysType_Simulated) {
							//RigBoneDrivers[TestName]->AttachToComponent(this, Rule);
							//Comp->AttachToComponent(this, Rule);
							if (Comp->BodyInstance.bSimulatePhysics != true) {
								Comp->BodyInstance.bSimulatePhysics = true;

								Comp->ReregisterComponent();
							}
						}
						else {
							//Comp->AttachToComponent(this, Rule);
							if (Comp->BodyInstance.bSimulatePhysics != false) {
								Comp->BodyInstance.bSimulatePhysics = false;

								Comp->ReregisterComponent();
							}
						}

					}
				}

			}
		}

		break;
	}
	}

}

void UBoneRigComponent::AttachAllChildBoneToClosestParentBone(bool bOnlyAttachNonSimulatableObject) {



	CalculateBoneHierarchyMap();


	TArray<FName> BoneNames;
	RigBoneDrivers.GetKeys(BoneNames);


	TMap<FName, FTransform> BoneTFs;

	for (const FName& Name : BoneNames) {
		BoneTFs.Add(Name, RigBoneDrivers[Name]->GetComponentTransform());
	}


	FAttachmentTransformRules AttachRule = FAttachmentTransformRules::KeepRelativeTransform;

	for (const FName& Name : BoneNames) {

		USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Name];

		if (bOnlyAttachNonSimulatableObject) {
			if (Comp->GetBodySetup()) {
				Comp->DetachFromParent(true);
				continue;
			}
		}

		if (spine::Bone* TestBone = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*Comp->BoneName))) {

			bool bShouldKeepSearch = true;

			spine::Bone* LastTestBone = TestBone;

			while (bShouldKeepSearch) {

				if (spine::Bone* TestParent = LastTestBone->getParent()) {
					const FName& LastTestParentBoneName = FName(FString(TestParent->getData().getName().buffer()));

					if (BoneNames.Contains(LastTestParentBoneName)) { // 부모 본이 컴포넌트로 구현되어있는 경우

						Comp->AttachToComponent(RigBoneDrivers[LastTestParentBoneName], AttachRule);

						Comp->SetWorldLocationAndRotation(BoneTFs[FName(Comp->BoneName)].GetLocation(), BoneTFs[FName(Comp->BoneName)].GetRotation());

						bShouldKeepSearch = false;
					}
					else { //  부모 본이 컴포넌트로 구현되어있지 않은 경우
						LastTestBone = TestParent;
					}

				}
				else {// 부모 본이 아예 스파인 파일 계층에 없는 경우 -> 이경우 아예 유효하지 않는 경우와 루트본인 경우의 두가지의 가능성이 있다.

					if (BoneNames.Contains(FName(Comp->BoneName))) { //현재 본이 컴포넌트로 구현되어있는 경우  -> 루트본인 경우

						Comp->AttachToComponent(this, AttachRule);

						Comp->SetWorldLocationAndRotation(BoneTFs[FName(Comp->BoneName)].GetLocation(), BoneTFs[FName(Comp->BoneName)].GetRotation());


					}

					//유효하지 않는 경우나 유효했던 경우 모두 더이상의 검색은 필요 없으므로

					bShouldKeepSearch = false;
				}

			}

		}

	}

}



//NOTE: The way this function find the target bone is quite different from function CalculateConstraintHierarchyMap. Check this for debug
void UBoneRigComponent::CalculateBoneHierarchyMap() {

	TArray<FName> BoneNames;
	RigBoneDrivers.GetKeys(BoneNames);

	RigBoneHierarchyMap.Empty();

	spine::Vector<spine::Bone*> IKJoinedBones;

	if (!TargetComponent->GetSkeletonAnimationComp()) {
		return;
	}

	if (!TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()) {
		return;
	}

	int size = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->getIkConstraints().size();

	for (size_t Index = 0; Index < size; ++Index) {
		int IKBonesArrSize = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->getIkConstraints()[Index]->getBones().size();

		for (size_t InterIndex = 0; InterIndex < IKBonesArrSize; ++InterIndex) {
			IKJoinedBones.add(TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->getIkConstraints()[Index]->getBones()[Index]);
		}

	}

	for (const FName& Name : BoneNames) {

		if (spine::Bone* TestBone = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*Name.ToString()))) {

			int HierarchyCount = 0;

			bool bShouldContinue = true;

			spine::Bone* LastTestBone = TestBone;

			while (bShouldContinue) {

				if (spine::Bone* TestParent = LastTestBone->getParent()) {
					const FName& LastTestParentBoneName = FName(FString(TestParent->getData().getName().buffer()));

					if (BoneNames.Contains(LastTestParentBoneName)) { // 부모 본이 컴포넌트로 구현되어있는 경우
						++HierarchyCount;
					}
					LastTestBone = TestParent;
				}
				else {// 부모 본이 아예 스파인 파일 계층에 없는 경우
					bShouldContinue = false;
				}
			}

			if (!RigBoneHierarchyMap.Contains(HierarchyCount)) {
				RigBoneHierarchyMap.Add(HierarchyCount, FBoneHierarchyStruct());
			}

			if (IKJoinedBones.contains(TestBone)) {
				RigBoneHierarchyMap[HierarchyCount].HeightArr.Insert(RigBoneDrivers[Name], 0);
			}
			else {
				RigBoneHierarchyMap[HierarchyCount].HeightArr.Add(RigBoneDrivers[Name]);
			}

		}
	}

	//Sort
	RigBoneHierarchyMap.KeySort([](int32 A, int32 B) {
		return A < B;
		});

}

//NOTE: The way this function find the target bone is quite different from function CalculateBoneHierarchyMap. Check this for debug
void UBoneRigComponent::CalculateConstraintHierarchyMap() {

	TArray<FName> BoneNames;
	RigBoneDrivers.GetKeys(BoneNames);

	TArray<FName> ConstraintNames;
	RigConstraints.GetKeys(ConstraintNames);

	RigConstraintHierarchyMap.Empty();

	for (const FName& Name : ConstraintNames) {

		if (UPhysicsConstraintComponent* Constraint = RigConstraints[Name]) {

			if (USpineRigBoneDriverComponent* TestChildBone = Cast<USpineRigBoneDriverComponent>(Constraint->OverrideComponent1)) {

				if (spine::BoneData* ConstraintTargetChildBone = TargetComponent->GetSkeletonAnimationComp()->SkeletonData->GetSkeletonData(TargetComponent->GetSkeletonAnimationComp()->Atlas->GetAtlas())->findBone(TCHAR_TO_UTF8(*TestChildBone->BoneName))) {

					int HierarchyCount = 0;

					bool bShouldContinue = true;

					spine::BoneData* LastTestBone = ConstraintTargetChildBone;

					while (bShouldContinue) {

						if (spine::BoneData* TestParent = LastTestBone->getParent()) {
							const FName& LastTestParentBoneName = FName(FString(TestParent->getName().buffer()));

							if (BoneNames.Contains(LastTestParentBoneName)) { // 부모 본이 컴포넌트로 구현되어있는 경우
								++HierarchyCount;
							}
							LastTestBone = TestParent;
						}
						else {// 부모 본이 아예 스파인 파일 계층에 없는 경우
							bShouldContinue = false;
						}
					}

					if (!RigConstraintHierarchyMap.Contains(HierarchyCount)) {
						RigConstraintHierarchyMap.Add(HierarchyCount, FConstraintHierarchyStruct());
					}

					RigConstraintHierarchyMap[HierarchyCount].HeightArr.Add(RigConstraints[Name]);
				}
			}
		}
	}

	//Sort
	RigConstraintHierarchyMap.KeySort([](int32 A, int32 B) {
		return A < B;
		});
}




void UBoneRigComponent::CreateBones() {
	if (BaseData != nullptr) {

		if (TargetComponent) {

			if (TargetComponent->GetSkeletonAnimationComp()) {

				if (TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()) {

					if (AActor* Owner = this->GetOwner()) {

						TArray<FName> BonesForRig;
						BaseData->BoneRigDataList.GetKeys(BonesForRig);

						TArray<FName> BonesExsiting;
						RigBoneDrivers.GetKeys(BonesExsiting);


						TArray<FName> BonesToCreate;
						TArray<FName> BonesToDelete;
						TArray<FName> BonesToUpdate;


						BonesToDelete = BonesExsiting;
						for (const FName& TestName : BonesForRig) {
							if (!BonesExsiting.Contains(TestName)) { //데이터상으론 있는데 아직 구현되지 않은 본인경우
								BonesToCreate.Add(TestName);
							}
							else {//데이터상으로 있고 구현도 된 경우
								BonesToUpdate.Add(TestName);
							}

							if (BonesToDelete.Contains(TestName)) { //데이터상에 있는본을 현존하는 본에서 제외 -> 데이터상에 없는 구현된 본
								BonesToDelete.Remove(TestName);
							}
						}

						for (const FName& Bone : BonesToDelete) {
							if (USpineRigBoneDriverComponent* BoneDriverComp = RigBoneDrivers[Bone]) {
								BoneDriverComp->DestroyComponent();
								RigBoneDrivers.Remove(Bone);
							}
						}

						for (const FName& Bone : BonesToCreate) {

							FString BoneNameString = Bone.ToString();

							if (spine::Bone* TargetBone = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneNameString))) {

								FName CompName = FName(BoneNameString + "_Rig");

								//FTransform Transform = TargetComponent->GetSkeletonAnimationComp()->GetBoneWorldTransform(TCHAR_TO_UTF8(*BoneNameString));

								if (USpineRigBoneDriverComponent* BoneDriverComp = NewObject<USpineRigBoneDriverComponent>(Owner, CompName)) {

									FTransform Transform = TargetComponent->GetSkeletonAnimationComp()->GetBoneWorldTransform(TCHAR_TO_UTF8(*BoneNameString));

									BoneDriverComp->SetWorldLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
									BoneDriverComp->BoneName = BoneNameString;

									//Setting for collision stabilization
									BoneDriverComp->BodyInstance.PositionSolverIterationCount = 32;
									BoneDriverComp->BodyInstance.VelocitySolverIterationCount = 4;

									BoneDriverComp->BodyInstance.bUseCCD = true;
									BoneDriverComp->BodyInstance.CustomSleepThresholdMultiplier = 3;
									BoneDriverComp->BodyInstance.StabilizationThresholdMultiplier = 3;

									Owner->AddInstanceComponent(BoneDriverComp);
									RigBoneDrivers.Add(Bone, BoneDriverComp);
								}

							}

						}

						TArray<FName> ArrForUpdate;
						ArrForUpdate.Append(BonesToCreate);
						ArrForUpdate.Append(BonesToUpdate);

						for (const FName& TestBoneName : ArrForUpdate) {

							if (RigBoneDrivers.Contains(TestBoneName)) {

								if (BaseData->BoneRigBodySetupList.Contains(TestBoneName)) {

									RigBoneDrivers[TestBoneName]->CreateBodySetupIfNeeded();

									RigBoneDrivers[TestBoneName]->GetBodySetup()->CopyBodyPropertiesFrom(BaseData->BoneRigBodySetupList[TestBoneName]);

									RigBoneDrivers[TestBoneName]->RebuildBodyCollisionWithBodyInstanceShapes();

									IStreamingManager::Get().NotifyPrimitiveUpdated(RigBoneDrivers[TestBoneName]);

								}


								if (spine::Bone* Bone = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*TestBoneName.ToString()))) {

									if (RigBoneDrivers.Contains(TestBoneName)) {
										RigBoneDrivers[TestBoneName]->TargetSkeleton = TargetComponent->GetSkeletonAnimationComp();
									}

									if (spine::Bone* ParentBone = Bone->getParent()) {
										/*
										if (spine::Bone* ParentBone = Bone->getParent()) {

											const FName& ParentName = FName(ParentBone->getData().getName().buffer());

											if (RigBoneDrivers.Contains(ParentName)) {
												RigBoneDrivers[TestBoneName]->ParentRigBone = RigBoneDrivers[ParentName];
											}
										}
										*/
									}
								}
							}
						}

						//Owner->ReregisterAllComponents();
					}
				}
				else {
#if WITH_EDITOR 
					const FText notificationText = FText::FromString(FString("Its spine skeleton object was not valid! - Did you set a vaild data to the renderer component?"));
					FNotificationInfo info(notificationText);
					info.bFireAndForget = true;
					info.FadeInDuration = 0.5f;
					info.FadeOutDuration = 1.0f;
					info.ExpireDuration = 4.0f;

					FSlateNotificationManager::Get().AddNotification(info);

#endif
				}
			}
			else {
#if WITH_EDITOR 
				const FText notificationText = FText::FromString(FString("CRITICAL ERROR : Its skeleton animation component was nullptr! - Please check the code of handler"));
				FNotificationInfo info(notificationText);
				info.bFireAndForget = true;
				info.FadeInDuration = 0.5f;
				info.FadeOutDuration = 1.0f;
				info.ExpireDuration = 4.0f;

				FSlateNotificationManager::Get().AddNotification(info);

#endif
			}
		}
	}

}

void UBoneRigComponent::RemoveBones() {

	TArray<FName> BoneNames;
	RigBoneDrivers.GetKeys(BoneNames);

	AActor* Owner = this->GetOwner();

	if (Owner != nullptr) {
		for (const FName& TestName : BoneNames) {
			if (USpineRigBoneDriverComponent* TestComponent = RigBoneDrivers[TestName]) {

				Owner->RemoveInstanceComponent(TestComponent);
				Owner->RemoveOwnedComponent(TestComponent);
				TestComponent->DetachFromParent();

				RigBoneDrivers.Remove(TestName);

				TestComponent->DestroyComponent();

			}
		}

		RigBoneDrivers.Empty();

		RigBodyInstances.Empty();

		Owner->ReregisterAllComponents();

	}
}



void UBoneRigComponent::CreateJointConstraints() {
	if (this) {
		if (BaseData != nullptr) {

			TArray<FName> JointNames;
			BaseData->JointList.GetKeys(JointNames);

			AActor* Owner = this->GetOwner();



			TArray<FName> TargetJointNames;

			if (CurrentState == EBoneRigState::STATE_FOLLWING) {
				for (const FName& TestJoint : JointNames) {
					if (BaseData->JointList[TestJoint].bShouldBeCreatedOnFollow) {
						TargetJointNames.Add(TestJoint);
					}
				}
			}
			else if (CurrentState == EBoneRigState::STATE_DRIVING) {
				for (const FName& TestJoint : JointNames) {
					if (BaseData->JointList[TestJoint].bShouldBeCreatedOnDrive) {
						TargetJointNames.Add(TestJoint);
					}
				}
			}
			else if (CurrentState == EBoneRigState::STATE_RAGDOLL) {
				for (const FName& TestJoint : JointNames) {
					if (BaseData->JointList[TestJoint].bShouldBeCreatedOnRagdoll) {
						TargetJointNames.Add(TestJoint);
					}
				}
			}

			if (Owner != nullptr) {

				TArray<UActorComponent*> ConstraintCompArr;
				ConstraintCompArr = Owner->GetComponentsByClass(UPhysicsConstraintComponent::StaticClass());

				TMap<FName, UPhysicsConstraintComponent*> ExsitingJointNameMap;
				TMap<FName, UPhysicsConstraintComponent*> CreatedJointNameMap;

				//구현되어있는 조인트콘스트레인트컴포넌트의 이름을 저장함.

				for (UActorComponent* TestComp : ConstraintCompArr) {
					if (UPhysicsConstraintComponent* CastedTestComp = Cast<UPhysicsConstraintComponent>(TestComp)) {
						ExsitingJointNameMap.Add(CastedTestComp->ConstraintInstance.JointName, CastedTestComp);
					}
				}

				FAttachmentTransformRules Rule = FAttachmentTransformRules::KeepWorldTransform;

				for (const FName& TestJointName : TargetJointNames) { //리그본데이터에 존재하는 모든 조인트 데이터에 대해...

					if (ExsitingJointNameMap.Contains(TestJointName)) { //이미 프리뷰 씬에 구현된 에디팅 컴포넌트인 경우 -> 데이터 업데이트

						//FVector Pos1 = ExsitingJointNameMap[TestJointName]->ConstraintInstance.Pos1;
						//FVector Pos2 = ExsitingJointNameMap[TestJointName]->ConstraintInstance.Pos2;

						ExsitingJointNameMap[TestJointName]->ConstraintInstance = BaseData->JointList[TestJointName].DefaultInstance;
						ExsitingJointNameMap[TestJointName]->UpdateConstraintFrames();

						//ExsitingJointNameMap[TestJointName]->ConstraintInstance.Pos1 = Pos1;
						//ExsitingJointNameMap[TestJointName]->ConstraintInstance.Pos2 = Pos2;


						ExsitingJointNameMap.Remove(TestJointName); //기존에 존재하던 오브젝트이면서 데이터에서도 확인이 되는 콘스트레인트를 리스트에서 제거 -> 이 리스트는 이제 불필요해진 컴포넌트를 저장하는 용도로 사용됨.
					}
					else { //프리뷰 씬에 존재하지 않는 조인트일경우 -> 새롭게 생성
						FConstraintInstance& Instance = BaseData->JointList[TestJointName].DefaultInstance;

						if (this->RigBoneDrivers.Contains(Instance.ConstraintBone1)) { // 부모 본이 정상적으로 존재할때 (스냅 타겟)

							if (UPhysicsConstraintComponent* Casted = NewObject<UPhysicsConstraintComponent>(Owner, TestJointName)) {

								CreatedJointNameMap.Add(TestJointName, Casted);

								if (CurrentState == EBoneRigState::STATE_FOLLWING) {

									Casted->RegisterComponentWithWorld(Owner->GetWorld());
									Casted->AttachToComponent(this->RigBoneDrivers[Instance.ConstraintBone1], Rule);


									Casted->SetRelativeLocation(FVector::ZeroVector);
									Casted->SetRelativeRotation(FRotator::ZeroRotator);

									Owner->AddInstanceComponent(Casted);
									Owner->AddOwnedComponent(Casted);


									//Flip 검사해서 처리하게 변경할것.

									Casted->SetRelativeLocation(Instance.Pos1);



									Casted->ConstraintInstance = Instance;



									Casted->ConstraintActor1 = Owner;
									Casted->ConstraintActor2 = Owner;

									RigConstraints.Add(TestJointName, Casted);

								}
								else if (CurrentState == EBoneRigState::STATE_DRIVING) {
									Casted->RegisterComponentWithWorld(Owner->GetWorld());
									Casted->AttachToComponent(this->RigBoneDrivers[Instance.ConstraintBone2], Rule);


									Casted->SetRelativeLocation(FVector::ZeroVector);
									Casted->SetRelativeRotation(FRotator::ZeroRotator);

									Owner->AddInstanceComponent(Casted);
									Owner->AddOwnedComponent(Casted);


									//Flip 검사해서 처리하게 변경할것.

									Casted->SetRelativeLocation(Instance.Pos2);



									Casted->ConstraintInstance = Instance;



									Casted->ConstraintActor1 = Owner;
									Casted->ConstraintActor2 = Owner;

									RigConstraints.Add(TestJointName, Casted);
								}
								else if (CurrentState == EBoneRigState::STATE_RAGDOLL) {

									Casted->RegisterComponentWithWorld(Owner->GetWorld());
									Casted->AttachToComponent(this->RigBoneDrivers[Instance.ConstraintBone1], Rule);


									Casted->SetRelativeLocation(FVector::ZeroVector);
									Casted->SetRelativeRotation(FRotator::ZeroRotator);

									Owner->AddInstanceComponent(Casted);
									Owner->AddOwnedComponent(Casted);


									//Flip 검사해서 처리하게 변경할것.

									Casted->SetRelativeLocation(Instance.Pos1);



									Casted->ConstraintInstance = Instance;



									Casted->ConstraintActor1 = Owner;
									Casted->ConstraintActor2 = Owner;

									RigConstraints.Add(TestJointName, Casted);

								}


							}
						}
					}
				}

				TArray<FName> RemoveTargetComponentsNames;
				ExsitingJointNameMap.GetKeys(RemoveTargetComponentsNames);

				for (const FName& TestJointName : RemoveTargetComponentsNames) { //리그본 데이터에는 존재하지 않는 조인트이지만, 에디터에는 존재할경우 -> 모종의 이유로 데이터상에서 제거됬으므로, 해당 오브젝트 삭제
					Owner->RemoveInstanceComponent(ExsitingJointNameMap[TestJointName]);
					Owner->RemoveOwnedComponent(ExsitingJointNameMap[TestJointName]);

					ExsitingJointNameMap[TestJointName]->DestroyComponent();

					RigConstraints.Remove(TestJointName);
				}

				TArray<FName> ConstraintComponentKeys;
				CreatedJointNameMap.GetKeys(ConstraintComponentKeys);

				for (const FName& Index : ConstraintComponentKeys) {

					if (UPhysicsConstraintComponent* Comp = RigConstraints[Index]) {
						const FName& JointName = Comp->ConstraintInstance.JointName;

						if (BaseData->JointList.Contains(JointName)) {

							if (RigBoneDrivers.Contains(Comp->ConstraintInstance.ConstraintBone1) && RigBoneDrivers.Contains(Comp->ConstraintInstance.ConstraintBone2)) {
								Comp->TermComponentConstraint();


								if (CurrentState == EBoneRigState::STATE_FOLLWING || CurrentState == EBoneRigState::STATE_DRIVING) {
									Comp->SetConstrainedComponents(RigBoneDrivers[Comp->ConstraintInstance.ConstraintBone2], NAME_None, RigBoneDrivers[Comp->ConstraintInstance.ConstraintBone1], NAME_None);

								}
								else if (CurrentState == EBoneRigState::STATE_RAGDOLL) {
									Comp->SetConstrainedComponents(RigBoneDrivers[Comp->ConstraintInstance.ConstraintBone1], NAME_None, RigBoneDrivers[Comp->ConstraintInstance.ConstraintBone2], NAME_None);

								}

							}


						}
					}
				}

			}


		}
	}
}

void UBoneRigComponent::RemoveJointConstraints() {

	TArray<FName> JointNames;
	RigConstraints.GetKeys(JointNames);

	AActor* Owner = this->GetOwner();

	if (Owner != nullptr) {
		for (const FName& TestName : JointNames) {
			if (UPhysicsConstraintComponent* TestComponent = RigConstraints[TestName]) {
				Owner->RemoveInstanceComponent(TestComponent);
				Owner->RemoveOwnedComponent(TestComponent);

				TestComponent->TermComponentConstraint();
				TestComponent->DetachFromParent();

				RigConstraints.Remove(TestName);

				TestComponent->DestroyComponent();

			}
		}

		RigConstraints.Empty();

		//Owner->ReregisterAllComponents(); -> 필요한가?

	}
}

void UBoneRigComponent::BindAllJointConstraints() {

	TArray<FName> JointNames;
	RigConstraints.GetKeys(JointNames);

	if (BaseData == nullptr) {
#if WITH_EDITOR 
		UE_LOG(LogTemp, Error, TEXT("Joint bind execution aborted '%s' : We don't have any vaild base data to restore constraint."), *this->GetName());
#endif
		return;
	}

	for (const FName& TestName : JointNames) {
		if (UPhysicsConstraintComponent* TestComponent = RigConstraints[TestName]) {

			const FConstraintInstance& InstanceRef = BaseData->JointList[TestName].DefaultInstance;

			TestComponent->ConstraintInstance = InstanceRef;

			TestComponent->ConstraintInstance.UpdateLinearLimit();
			TestComponent->ConstraintInstance.UpdateAngularLimit();

			//TestComponent->InitComponentConstraint();

			TestComponent->ConstraintInstance.DisableParentDominates();

		}
	}

}

void UBoneRigComponent::ReleaseAllJointConstraints() {
	TArray<FName> JointNames;
	RigConstraints.GetKeys(JointNames);

	if (BaseData == nullptr) {
#if WITH_EDITOR 
		UE_LOG(LogTemp, Warning, TEXT("Well, I will let you release those constraints, but Be sure that we don't have any vaild base data to restore constraint again."))
#endif
	}

	for (const FName& TestName : JointNames) {
		if (UPhysicsConstraintComponent* TestComponent = RigConstraints[TestName]) {

			TestComponent->ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0);
			TestComponent->ConstraintInstance.SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0);
			TestComponent->ConstraintInstance.SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0);

			TestComponent->ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Free, 0);
			TestComponent->ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 0);
			TestComponent->ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 0);

			TestComponent->UpdateConstraintFrames();

		}
	}


}

void UBoneRigComponent::CollectAllBoneTF(class USpineSkeletonComponent* TargetComp) {
	TArray<int> Keys;
	RigBoneHierarchyMap.GetKeys(Keys);

	for (const int& Index : Keys) {
		for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
			Comp->CollectBoneTranslateData(TargetComp);
			//DrawDebugBox(GetWorld(), Comp->LastBoneTranslateData.GetWorldTransformFromArchieve(TargetComponent).GetLocation(), FVector(1.2,4,1.2), FColor::Cyan, false, 0.01, 0, 1);

		}
	}
}

void UBoneRigComponent::UpdateOnlyIKBones(class USpineSkeletonComponent* TargetComp) {
	if (TargetComponent != nullptr) {
		//if (GetWorld()->IsGameWorld()) {

		if (TargetComponent->GetSkeletonAnimationComp() == TargetComp) {


			TArray<int> Keys;
			RigBoneHierarchyMap.GetKeys(Keys);

			switch (CurrentState) {
			case EBoneRigState::STATE_FOLLWING: {

				spine::Vector<spine::IkConstraint*> Constraints = TargetComp->GetSkeleton()->getIkConstraints();

				for (size_t i = 0; i < Constraints.size(); i++) {
					FName TargetBoneName = Constraints[i]->getTarget()->getData().getName().buffer();

					if (RigBoneDrivers.Contains(TargetBoneName)) {
						RigBoneDrivers[TargetBoneName]->UpdateFollowBoneWorldTransform(TargetComponent, InterpolationKey);
					}

					Constraints[i]->update();
				}
				break;
			}
			}

		}
	}
}

void UBoneRigComponent::UpdateSpineConstraints(class USpineSkeletonComponent* TargetComp) {
	spine::Vector<spine::IkConstraint*> IK = TargetComp->GetSkeleton()->getIkConstraints();

	for (size_t i = 0; i < IK.size(); i++) { //IkConstraint 에서 업데이트가 되는 본들을 애니메이션 기준으로 텔레포트 시킵니다.

		bool bShouldUpdateLocal = true;

		spine::Vector<spine::Bone*> Bones = IK[i]->getBones();

		for (size_t j = 0; j < Bones.size(); j++) {

			FName Name = FName(Bones[j]->getData().getName().buffer());

			if (RigBoneDrivers.Contains(Name)) {
				if (RigBoneDrivers[FName(Bones[j]->getData().getName().buffer())]->GetBodySetup()) {
					bShouldUpdateLocal = false;
				}
			}
		}

		if (bShouldUpdateLocal) {

			IK[i]->update();

			for (size_t j = 0; j < Bones.size(); j++) {

				FName Name = FName(Bones[j]->getData().getName().buffer());

				if (RigBoneDrivers.Contains(Name)) {

					RigBoneDrivers[Name]->SyncBoneToAnimation(TargetComponent);

					spine::Vector<spine::Bone*> Children = Bones[j]->getChildren();

					for (size_t k = 0; k < Children.size(); k++) {

						if (Children[k] == nullptr) {
							continue;
						}

						if (&Children[k]->getData() == nullptr) {
							continue;
						}

						if (Children[k]->getData().getName() == nullptr) {
							continue;
						}

						FName ChildName = FName(Children[k]->getData().getName().buffer());

						if (RigBoneDrivers.Contains(ChildName)) {

							Children[k]->update();

							RigBoneDrivers[ChildName]->SyncBoneToAnimation(TargetComponent);

						}

					}
				}
			}
		}
	}



	spine::Vector<spine::PathConstraint*> Path = TargetComp->GetSkeleton()->getPathConstraints();

	for (size_t i = 0; i < Path.size(); i++) { //IkConstraint 에서 업데이트가 되는 본들을 애니메이션 기준으로 텔레포트 시킵니다.

		bool bShouldUpdateLocal = true;

		spine::Vector<spine::Bone*> Bones = Path[i]->getBones();

		for (size_t j = 0; j < Bones.size(); j++) {

			FName Name = FName(Bones[j]->getData().getName().buffer());

			if (RigBoneDrivers.Contains(Name)) {
				if (RigBoneDrivers[FName(Bones[j]->getData().getName().buffer())]->GetBodySetup()) {
					bShouldUpdateLocal = false;
				}
			}
		}

		if (bShouldUpdateLocal) {

			Path[i]->update();

			for (size_t j = 0; j < Bones.size(); j++) {

				FName Name = FName(Bones[j]->getData().getName().buffer());

				if (RigBoneDrivers.Contains(Name)) {

					RigBoneDrivers[Name]->SyncBoneToAnimation(TargetComponent);

					spine::Vector<spine::Bone*> Children = Bones[j]->getChildren();

					for (size_t k = 0; k < Children.size(); k++) {

						if (Children[k] == nullptr) {
							continue;
						}

						if (&Children[k]->getData() == nullptr) {
							continue;
						}

						if (Children[k]->getData().getName() == nullptr) {
							continue;
						}

						FName ChildName = FName(Children[k]->getData().getName().buffer());

						if (RigBoneDrivers.Contains(ChildName)) {

							Children[k]->update();

							RigBoneDrivers[ChildName]->SyncBoneToAnimation(TargetComponent);

						}

					}
				}
			}
		}
	}

	spine::Vector<spine::TransformConstraint*> Transform = TargetComp->GetSkeleton()->getTransformConstraints();

	for (size_t i = 0; i < Path.size(); i++) { //IkConstraint 에서 업데이트가 되는 본들을 애니메이션 기준으로 텔레포트 시킵니다.

		bool bShouldUpdateLocal = true;

		spine::Vector<spine::Bone*> Bones = Transform[i]->getBones();

		for (size_t j = 0; j < Bones.size(); j++) {

			FName Name = FName(Bones[j]->getData().getName().buffer());

			if (RigBoneDrivers.Contains(Name)) {
				if (RigBoneDrivers[FName(Bones[j]->getData().getName().buffer())]->GetBodySetup()) {
					bShouldUpdateLocal = false;
				}
			}
		}

		if (bShouldUpdateLocal) {

			Transform[i]->update();

			for (size_t j = 0; j < Bones.size(); j++) {

				FName Name = FName(Bones[j]->getData().getName().buffer());

				if (RigBoneDrivers.Contains(Name)) {

					RigBoneDrivers[Name]->SyncBoneToAnimation(TargetComponent);

					spine::Vector<spine::Bone*> Children = Bones[j]->getChildren();

					for (size_t k = 0; k < Children.size(); k++) {

						if (Children[k] == nullptr) {
							continue;
						}

						if (&Children[k]->getData() == nullptr) {
							continue;
						}

						if (Children[k]->getData().getName() == nullptr) {
							continue;
						}

						FName ChildName = FName(Children[k]->getData().getName().buffer());

						if (RigBoneDrivers.Contains(ChildName)) {

							Children[k]->update();

							RigBoneDrivers[ChildName]->SyncBoneToAnimation(TargetComponent);

						}

					}
				}
			}
		}
	}
}


void UBoneRigComponent::ResetRootMotion() {

	AActor* Owner = GetOwner();

	if (CurrentState != EBoneRigState::STATE_FOLLWING) {
		return;
	}

	if (!Owner) {
		return;
	}

	if (!TargetComponent) {
		return;
	}

	if (spine::Bone* RootBone = this->TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->getRootBone()) {

		LastSavedRootBoneTs.PasteToBone(RootBone);
		RootBone->update();

	}
}

void UBoneRigComponent::UpdateRootMotion() {
	AActor* Owner = GetOwner();

	if (CurrentState != EBoneRigState::STATE_FOLLWING) {
		return;
	}

	if (!Owner) {
		return;
	}

	if (!TargetComponent) {
		return;
	}

	UActorComponent* Movement = Owner->GetComponentByClass(UCharacterMovementComponent::StaticClass());

	if (!Movement) {
		return;
	}

	if (UCharacterMovementComponent* CastedMovement = Cast<UCharacterMovementComponent>(Movement)) {

		if (spine::Bone* RootBone = this->TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->getRootBone()) {

			switch (RootMotionType) {

			case ESpineRootMotionType::NoRootMotionExtraction: {

				//UE_LOG(LogTemp, Log, TEXT("NoRootMotionExtraction"));

				//CastedMovement->RemoveRootMotionSource("ApplyRootMotionConstantForce");

				break;
			}
			case ESpineRootMotionType::IgnoreRootMotion: {

				RootBone->setToSetupPose();
				RootBone->update();

				//UE_LOG(LogTemp, Log, TEXT("IgnoreRootMotion"));

				break;
			}
			case ESpineRootMotionType::ApplyRootMotion: {

				//UE_LOG(LogTemp, Log, TEXT("ApplyRootMotion, RootBone X : %f, Y : %f"), RootBone->getWorldX(), RootBone->getWorldY());
				FTransform RootBoneTransform = this->TargetComponent->GetSkeletonAnimationComp()->GetBoneWorldTransform(FString(RootBone->getData().getName().buffer()));


				if (LastRootMotionTransform.Equals(FTransform::Identity)) { //Identity와 동일하다면 (저장된 데이터가 없다면)
					LastRootMotionTransform = this->TargetComponent->GetComponentTransform(); //현재 프레임의 루트본 로케이션을 저장합니다.
				}
				else {
					LastRootMotionTransform = LastSavedRootBoneTs.GetWorldTransformFromArchieve(TargetComponent); // 이전 프레임의 루트본 로케이션을 저장합니다.
				}


				//기존 액터 위치 + (이전 프레임에서 부터 루트본이 이동한 위치)
				Owner->SetActorLocation(Owner->GetActorLocation() + RootBoneTransform.GetLocation() - LastRootMotionTransform.GetLocation(), true, nullptr, ETeleportType::None);

				LastSavedRootBoneTs.CopyFromBone(RootBone);

				//DrawDebugBox(GetWorld(), RootBoneTransform.GetLocation(), FVector(12, 12, 12 ), FColor::Cyan);


				//DrawDebugBox(GetWorld(), Owner->GetActorLocation(), FVector(15, 15, 15), FColor::Red);

				RootBone->setToSetupPose();
				RootBone->update();
				RootBone->setAppliedValid(false);
				//RootBone->_appliedValid = false;

				break;
			}

			}



		}

	}


}


void UBoneRigComponent::UpdateBones(class USpineSkeletonComponent* TargetComp) {
	if (TargetComponent != nullptr) {
		//if (GetWorld()->IsGameWorld()) {

		if (TargetComponent->GetSkeletonAnimationComp() == TargetComp) {

			TArray<int> Keys;
			RigBoneHierarchyMap.GetKeys(Keys);

			InterpolationKey = FMath::Clamp<float>(InterpolationKey, 0.f, 1.f);

			switch (CurrentState) {
			case EBoneRigState::STATE_FOLLWING: {

				CalculateBoneBaseTransform();

				if (bPhysicsEngineNeedsUpdating)
				{
					StartPhysicalAnimation();
					OnTeleport();
				}

				for (const int& Index : Keys) { //모든 본들의 트랜스폼을 리그에 맞춥니다.
					for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
						Comp->UpdateFollowBoneWorldTransform(TargetComponent, InterpolationKey);
					}
				}

				UpdateTargetActors(ETeleportType::None);

				UpdateSpineConstraints(TargetComp);



				break;
			}
			case EBoneRigState::STATE_DRIVING: {

				CalculateBoneBaseTransform();

				for (const int& Index : Keys) {
					for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
						Comp->UpdateDriveBoneWorldTransform(TargetComponent, InterpolationKey);
					}
				}

				UpdateRootComponentBasedOnPolicy();

				UpdateSpineConstraints(TargetComp);

				break;
			}
			case EBoneRigState::STATE_RAGDOLL: {

				CalculateBoneBaseTransform();

				for (const int& Index : Keys) {
					for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
						Comp->UpdateDriveBoneWorldTransform(TargetComponent, InterpolationKey);
					}
				}

				UpdateRootComponentBasedOnPolicy();

				UpdateSpineConstraints(TargetComp);

				break;
			}
			}
		}
		//}
	}

}

void UBoneRigComponent::ResetAfterUpdateBones(class USpineSkeletonComponent* TargetComp) {
	//	if (GetWorld()->IsGameWorld()) {


	switch (CurrentState) {
	case EBoneRigState::STATE_FOLLWING: {

		ResetAllBonesTransform(TargetComp);

		break;
	}
	case EBoneRigState::STATE_DRIVING: {

		ResetAllBonesTransform(TargetComp);

		break;
	}
	case EBoneRigState::STATE_RAGDOLL: {

		ResetAllBonesTransform(TargetComp);

		break;
	}

	}
	//	}
}


void UBoneRigComponent::ResetAllBonesTransform(class USpineSkeletonComponent* TargetComp) {
	TArray<int> Keys;
	RigBoneHierarchyMap.GetKeys(Keys);

	for (const int& Index : Keys) {
		for (USpineRigBoneDriverComponent* Comp : RigBoneHierarchyMap[Index].HeightArr) {
			Comp->ResetBoneTranslateData(TargetComp);
		}
	}
}

void UBoneRigComponent::UpdateRootComponentBasedOnPolicy() {

	if (!BaseData) {
		return;
	}

	FName TargetBoneName = NAME_None;

	if (TransformUpdateRootBone.IsNone()) {
		TargetBoneName = BaseData->DefaultTransformUpdateRootBone;
	}
	else {
		TargetBoneName = TransformUpdateRootBone;
	}

	switch (RootComponentTransformUpdatePolicy) {

	case ERootComponentTransformUpdatePolicy::DO_NOTHING: {
		break;
	}
	case ERootComponentTransformUpdatePolicy::STRICTLY_FOLLOW_ROOTBONE: {

		AActor* Owner = this->GetOwner();

		if (Owner) {

			USceneComponent* RootComponent = Owner->GetRootComponent();
			if (RootComponent) {

				if (RigBoneDrivers.Contains(TargetBoneName)) {
					RootComponent->SetWorldLocation(RigBoneDrivers[TargetBoneName]->GetComponentLocation());

				}
			}
		}

		break;
	}
	case ERootComponentTransformUpdatePolicy::ONLY_TRANSLATE_FOLLOW_ROOTBONE: {

		AActor* Owner = this->GetOwner();

		if (Owner) {

			USceneComponent* RootComponent = Owner->GetRootComponent();
			if (RootComponent) {
				if (RigBoneDrivers.Contains(TargetBoneName)) {
					RootComponent->SetWorldLocation(RigBoneDrivers[TargetBoneName]->GetComponentLocation());
				}
			}
		}

		break;
	}
	case ERootComponentTransformUpdatePolicy::ONLY_TRANSLATE_FOLLOW_ROOTBONE_WITH_GROUNDCHECK: {

		AActor* Owner = this->GetOwner();

		if (Owner) {

			USceneComponent* RootComponent = Owner->GetRootComponent();
			if (RootComponent) {
				if (RigBoneDrivers.Contains(TargetBoneName)) {
					RootComponent->SetWorldLocation(RigBoneDrivers[TargetBoneName]->GetComponentLocation());
				}
			}
		}

		break;
	}

	}

}

//TODO : Make it more brief
void UBoneRigComponent::ToggleAbsoluteBasedOnState(bool bShouldSetAbsolute) {

	if (AActor* Owner = this->GetOwner()) {

		if (UActorComponent* RenderComp = Owner->GetComponentByClass(USpineSkeletonRendererComponent::StaticClass())) {

			if (USpineSkeletonRendererComponent* CastedRenderComp = Cast<USpineSkeletonRendererComponent>(RenderComp)) {

				if (bShouldSetAbsolute) {

					if (bIsAbsolute == false) {


						//FAttachmentTransformRules Rule = FAttachmentTransformRules::KeepWorldTransform;
						//this->AttachToComponent(Owner->GetRootComponent(), Rule);

						//FTransform WorldTransform = this->GetComponentTransform();
						//this->SetAbsolute(true, true, true);
						//this->SetWorldTransform(WorldTransform);

						FTransform WorldTransform = this->GetComponentTransform();
						this->SetAbsolute(true, true, true);
						this->SetWorldTransform(WorldTransform);

						bIsAbsolute = true;
					}
				}
				else {

					if (bIsAbsolute == true) {

						//FDetachmentTransformRules Rule = FDetachmentTransformRules::KeepWorldTransform;
						//this->DetachFromComponent(Rule);

						this->SetAbsolute(false, false, false);
						this->SetRelativeTransform(FTransform::Identity);

						bIsAbsolute = false;
					}
				}
			}
		}
	}
}

/*
void UBoneRigComponent::ToggleAbsoluteBasedOnStateAboutAllComponentThatHasPhysicsBody(bool bShouldSetAbsolute) { //TEST

	TArray<FName> BoneNames;
	RigBoneDrivers.GetKeys(BoneNames);

	AActor* Owner = this->GetOwner();

	if (Owner != nullptr) {

		for (FName TestName : BoneNames) {

			if (USpineRigBoneDriverComponent* TestComponent = RigBoneDrivers[TestName]) {

				if (RigBodyInstances.Contains(TestName)) {

					if (bShouldSetAbsolute) {
						FTransform WorldTransform = TestComponent->GetComponentTransform();
						TestComponent->SetAbsolute(true, true, true);
						TestComponent->SetWorldTransform(WorldTransform);

					}
					else {
						TestComponent->SetAbsolute(false, false, false);
					}
				}

			}
		}

	}

}

*/

void UBoneRigComponent::SetUpdate_Following() {
	//if (GetWorld()->IsGameWorld()) {

	if (!bCanSetUpdate) {
		return;
	}

	if (TargetComponent) {
		if (CurrentState != EBoneRigState::STATE_FOLLWING) {

			EBoneRigState SavedState = CurrentState;

			FinishUpdate(); // 기존 스테이트 초기화

			CurrentState = EBoneRigState::STATE_FOLLWING;

			UpdateCollisionProfileBasedOnUpdateMode();

			AttachAllChildBoneToClosestParentBone(true);

			ToggleAbsoluteBasedOnState(false);

			ToggleDOFConstraintOfAllBones(false);

			SetIKConstraints(true);

			if (this->BaseData) {
				if (AActor* Owner = GetOwner()) {


					//본 채널 업데이트
					ApplySimulatePhysicsToBones(EPhysicsSimulationMode::ALL);

					CreateJointConstraints();

					StartPhysicalAnimation();


				}

			}

			//AttachAllChildBoneToClosestParentBone();

			TargetComponent->GetSkeletonAnimationComp()->TickComponent(0, ELevelTick::LEVELTICK_All, &TargetComponent->GetSkeletonAnimationComp()->PrimaryComponentTick);
		}
	}
	/*
}
else {
	CurrentState = EBoneRigState::STATE_FOLLWING;


#if WITH_EDITOR
		const FText notificationText = FText::FromString(FString("We are in the editor right now! - the state 'Following' will be saved and applied when you start runtime"));
		FNotificationInfo info(notificationText);
		info.bFireAndForget = true;
		info.FadeInDuration = 0.5f;
		info.FadeOutDuration = 1.0f;
		info.ExpireDuration = 4.0f;

		FSlateNotificationManager::Get().AddNotification(info);
#endif
	}
	*/
}


void UBoneRigComponent::ToggleDOFConstraintOfAllBones(bool bShouldSetConstraint) {

	TArray<FName> BoneDriverKeys;
	RigBoneDrivers.GetKeys(BoneDriverKeys);


	if (bShouldSetConstraint) {
		for (const FName& Key : BoneDriverKeys) {
			if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {
				Comp->BodyInstance.bLockXTranslation = false;
				Comp->BodyInstance.bLockYTranslation = true;
				Comp->BodyInstance.bLockZTranslation = false;

				Comp->BodyInstance.bLockXRotation = true;
				Comp->BodyInstance.bLockYRotation = false;
				Comp->BodyInstance.bLockZRotation = true;

				
				Comp->BodyInstance.CreateDOFLock();
			}
		}
	}
	else {
		for (const FName& Key : BoneDriverKeys) {
			if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {
				Comp->BodyInstance.bLockXTranslation = false;
				Comp->BodyInstance.bLockYTranslation = false;
				Comp->BodyInstance.bLockZTranslation = false;

				Comp->BodyInstance.bLockXRotation = false;
				Comp->BodyInstance.bLockYRotation = false;
				Comp->BodyInstance.bLockZRotation = false;

				Comp->BodyInstance.CreateDOFLock();

			}
		}
	}


	//UpdateAllInstances();
}

void UBoneRigComponent::SetUpdate_Driving(bool bShouldSimulate, FName PresetName) {
	if (!bCanSetUpdate) {
		return;
	}

	if (TargetComponent) {
		if (CurrentState != EBoneRigState::STATE_DRIVING) {

			EBoneRigState SavedState = CurrentState;

			FinishUpdate(); // 기존 스테이트 초기화

			CurrentState = EBoneRigState::STATE_DRIVING;

			UpdateCollisionProfileBasedOnUpdateMode();

			ToggleAbsoluteBasedOnState(true);

			ToggleDOFConstraintOfAllBones(false);

			if (bShouldSimulate) {
				ApplySimulatePhysicsToBones(EPhysicsSimulationMode::ONLY_SIMULATED);
			}
			else {
				ApplySimulatePhysicsToBones(EPhysicsSimulationMode::NONE);
			}

			AttachAllChildBoneToClosestParentBone(false);

			SetIKConstraints(false, PresetName);

			if (this->BaseData) {

				if (AActor* Owner = GetOwner()) {

					if (TargetComponent) {

						RemoveJointConstraints();

						if (bShouldSimulate) {
							CreateJointConstraints();
						}

						CalculateBoneHierarchyMap();
					}
				}
			}

			TargetComponent->GetSkeletonAnimationComp()->TickComponent(0, ELevelTick::LEVELTICK_All, &TargetComponent->GetSkeletonAnimationComp()->PrimaryComponentTick);

		}
	}
}

void UBoneRigComponent::SetUpdate_RagdollUpdating() {
	if (!bCanSetUpdate) {
		return;
	}

	if (TargetComponent) {
		//if (GetWorld()->IsGameWorld()) {
		if (CurrentState != EBoneRigState::STATE_RAGDOLL) {

			EBoneRigState SavedState = CurrentState;

			FinishUpdate(); // 기존 스테이트 초기화

			CurrentState = EBoneRigState::STATE_RAGDOLL;

			UpdateCollisionProfileBasedOnUpdateMode();

			AttachAllChildBoneToClosestParentBone(true);

			ToggleAbsoluteBasedOnState(true);

			ToggleDOFConstraintOfAllBones(true);

			

			if (this->BaseData) {
				if (AActor* Owner = GetOwner()) {
					if (TargetComponent) {


						ApplySimulatePhysicsToBones(EPhysicsSimulationMode::ALL);

						CreateJointConstraints();

						CalculateConstraintHierarchyMap();

						SetIKConstraints(false);

					}

				}
			}

			TargetComponent->GetSkeletonAnimationComp()->TickComponent(0, ELevelTick::LEVELTICK_All, &TargetComponent->GetSkeletonAnimationComp()->PrimaryComponentTick);
		}

	}
}

void UBoneRigComponent::FinishUpdate() {
	if (TargetComponent) {

		ResetAfterUpdateBones(TargetComponent->GetSkeletonAnimationComp());

		EBoneRigState SavedState = CurrentState;

		switch (CurrentState) {

		case EBoneRigState::STATE_FOLLWING: {
			if (this->BaseData) {

				if (AActor* Owner = GetOwner()) {

					StopPhysicalAnimationAdjust();

				}
			}
			break;
		}
		case EBoneRigState::STATE_DRIVING: {


			StopPhysicalAnimationAdjust();


			break;
		}
		case EBoneRigState::STATE_RAGDOLL: {

			if (this->BaseData) {

				if (AActor* Owner = GetOwner()) {

					SetIKConstraints(true);

					ToggleDOFConstraintOfAllBones(false);
				}
			}

			break;
		}
		}

	}
}

void UBoneRigComponent::SetUpdate_None() {
	if (!bCanSetUpdate) {
		return;
	}

	FinishUpdate();

	CurrentState = EBoneRigState::STATE_NONE;

}



void UBoneRigComponent::UpdateCollisionProfileBasedOnUpdateMode() {

	switch (CurrentState) {

	case EBoneRigState::STATE_FOLLWING: {
		if (this->BaseData) {

			TArray<FName> BoneDriverKeys;
			RigBoneDrivers.GetKeys(BoneDriverKeys);

			for (const FName& Key : BoneDriverKeys) {
				if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

					const FName& TestName = FName(Comp->BoneName);

					FBodyInstance* Instance = nullptr;

					Instance = (this->BaseData->RigCollisionPresetOverride.Contains(TestName)) ? &this->BaseData->RigCollisionPresetOverride[TestName].CollisionPreset : &this->BaseData->RigCollisionPreset.CollisionPreset;

					if (Instance) {

						Instance->FixupData(this);

						Comp->SetCollisionEnabled(Instance->GetCollisionEnabled());
						Comp->SetCollisionObjectType(Instance->GetObjectType());
						Comp->SetCollisionResponseToChannels(Instance->GetResponseToChannels());

					}

					Comp->GetBodyInstance()->PhysicsBlendWeight = DefaultPhysicalWeight;
					Comp->GetBodyInstance()->FixupData(this);

				}
			}
		}

		break;
	}
	case EBoneRigState::STATE_DRIVING: {

		if (this->BaseData) {

			TArray<FName> BoneDriverKeys;
			RigBoneDrivers.GetKeys(BoneDriverKeys);

			for (const FName& Key : BoneDriverKeys) {
				if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

					const FName& TestName = FName(Comp->BoneName);

					FBodyInstance* Instance = nullptr;

					Instance = (this->BaseData->RigCollisionPresetOverride.Contains(TestName)) ? &this->BaseData->RigCollisionPresetOverride[TestName].CollisionPreset : &this->BaseData->RigCollisionPreset.CollisionPreset;

					if (Instance) {

						Instance->FixupData(this);

						Comp->SetCollisionEnabled(Instance->GetCollisionEnabled());
						Comp->SetCollisionObjectType(Instance->GetObjectType());
						Comp->SetCollisionResponseToChannels(Instance->GetResponseToChannels());

					}

					Comp->GetBodyInstance()->PhysicsBlendWeight = DefaultPhysicalWeight;
					Comp->GetBodyInstance()->FixupData(this);

				}
			}
		}
		break;
	}
	case EBoneRigState::STATE_RAGDOLL: {
		if (this->BaseData) {

			TArray<FName> BoneDriverKeys;
			RigBoneDrivers.GetKeys(BoneDriverKeys);

			for (const FName& Key : BoneDriverKeys) {
				if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

					const FName& TestName = FName(Comp->BoneName);

					FBodyInstance* Instance = nullptr;

					Instance = (this->BaseData->RigCollisionPresetOverride.Contains(TestName)) ? &this->BaseData->RigCollisionPresetOverride[TestName].Ragdoll_CollisionPreset : &this->BaseData->RigCollisionPreset.Ragdoll_CollisionPreset;

					if (Instance) {

						Instance->FixupData(this);

						Comp->SetCollisionEnabled(Instance->GetCollisionEnabled());
						Comp->SetCollisionObjectType(Instance->GetObjectType());
						Comp->SetCollisionResponseToChannels(Instance->GetResponseToChannels());

					}

					Comp->GetBodyInstance()->PhysicsBlendWeight = RagdollPhysicalWeight;
					Comp->GetBodyInstance()->FixupData(this);

				}
			}

		}

		break;
	}
	case EBoneRigState::STATE_NONE: {
		if (this->BaseData) {

			TArray<FName> BoneDriverKeys;
			RigBoneDrivers.GetKeys(BoneDriverKeys);

			for (const FName& Key : BoneDriverKeys) {
				if (USpineRigBoneDriverComponent* Comp = RigBoneDrivers[Key]) {

					const FName& TestName = FName(Comp->BoneName);

					FBodyInstance* Instance = nullptr;

					Instance = (this->BaseData->RigCollisionPresetOverride.Contains(TestName)) ? &this->BaseData->RigCollisionPresetOverride[TestName].CollisionPreset : &this->BaseData->RigCollisionPreset.CollisionPreset;

					if (Instance) {

						Instance->FixupData(this);

						Comp->SetCollisionEnabled(Instance->GetCollisionEnabled());
						Comp->SetCollisionObjectType(Instance->GetObjectType());
						Comp->SetCollisionResponseToChannels(Instance->GetResponseToChannels());

					}

					Comp->GetBodyInstance()->PhysicsBlendWeight = DefaultPhysicalWeight;
					Comp->GetBodyInstance()->FixupData(this);

				}
			}
		}
		break;
	}
	}
}

void UBoneRigComponent::CreateCollisionSetupForBones() {

	if (TargetComponent != nullptr) {
		TArray<FName> ArrForUpdate;
		RigBoneDrivers.GetKeys(ArrForUpdate);

		//FPhysicsActorHandle& Handle;

		for (const FName& TestBoneName : ArrForUpdate) {

			if (USpineRigBoneDriverComponent* TestComp = RigBoneDrivers[TestBoneName]) {


				if (BaseData->BoneRigBodySetupList.Contains(TestBoneName)) {

					TestComp->CreateBodySetupIfNeeded();

					TestComp->GetBodySetup()->CopyBodyPropertiesFrom(BaseData->BoneRigBodySetupList[TestBoneName]);

					TestComp->RebuildBodyCollisionWithBodyInstanceShapes();

					IStreamingManager::Get().NotifyPrimitiveUpdated(TestComp);

					TestComp->GetBodySetup()->BoneName = TestBoneName;

					RigBodyInstances.Add(TestBoneName, FSpineBodyInstanceWrapper(&TestComp->BodyInstance));
				}
			}
		}
	}
}


void UBoneRigComponent::SetIKConstraints(bool bShouldReset, FName PresetName) {

	//Spine의 IK 콘스트레인트를 조절합니다. 켜야한다면 리그의 디폴트값으로 복원합니다.
	if (bShouldReset) {

		spine::Vector<spine::IkConstraint*> Iks = GetTargetComponent()->GetSkeletonAnimationComp()->GetSkeleton()->getIkConstraints();

		for (size_t i = 0; i < Iks.size(); ++i) {

			spine::IkConstraint* TestIk = Iks[i];
			if (TestIk != nullptr) {
				TestIk->setMix(TestIk->getData().getMix());
				//FString String = FString(TestIk->getData().getName().buffer());
				//UE_LOG(LogTemp,Log,TEXT("%s, %f"), *String, TestIk->getData().getMix())
			}
		}

	}
	else {

		if (BaseData != nullptr) {

			if (TargetComponent != nullptr) {

				if (TargetComponent->IsValidLowLevelFast()) {

					switch (CurrentState) {

					case EBoneRigState::STATE_DRIVING: {

						if (BaseData->DriveModeConstraintMixDataPresets.Contains(PresetName)) {

							TArray<FName> Keys;
							BaseData->DriveModeConstraintMixDataPresets[PresetName].ConstraintMixes.GetKeys(Keys);

							for (const FName& TestName : Keys) {
								if (spine::IkConstraint* Constraint = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findIkConstraint(TCHAR_TO_UTF8(*TestName.ToString()))) {
									Constraint->setMix(BaseData->DriveModeConstraintMixDataPresets[PresetName].ConstraintMixes[TestName]);
								}
							}

						}
						else {

							TArray<FName> Keys;
							BaseData->DriveConstraintMixData.ConstraintMixes.GetKeys(Keys);

							for (const FName& TestName : Keys) {
								if (spine::IkConstraint* Constraint = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findIkConstraint(TCHAR_TO_UTF8(*TestName.ToString()))) {
									Constraint->setMix(BaseData->DriveConstraintMixData.ConstraintMixes[TestName]);
								}
							}

						}
						break;
					}
					case EBoneRigState::STATE_RAGDOLL: {

						TArray<FName> Keys;
						BaseData->RagdollConstraintMixData.ConstraintMixes.GetKeys(Keys);

						for (const FName& TestName : Keys) {
							if (spine::IkConstraint* Constraint = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findIkConstraint(TCHAR_TO_UTF8(*TestName.ToString()))) {
								Constraint->setMix(BaseData->RagdollConstraintMixData.ConstraintMixes[TestName]);
							}
						}

						break;
					}

					}
				}

			}
		}


	}

}


void UBoneRigComponent::FlipSkeleton(bool bShouldFlip) {
	if (TargetComponent) {
		TargetComponent->FlipSkeleton(bShouldFlip);
	}
}


void UBoneRigComponent::BindAllBodyOnOverlapBeginEvent() {

	if (TargetComponent != nullptr) {
		TArray<FName> ArrForUpdate;
		RigBoneDrivers.GetKeys(ArrForUpdate);

		for (const FName& TestBoneName : ArrForUpdate) {

			if (USpineRigBoneDriverComponent* TestComp = RigBoneDrivers[TestBoneName]) {

				if (BaseData->BoneRigBodySetupList.Contains(TestBoneName)) {
					TestComp->OnComponentBeginOverlap.AddDynamic(this, &UBoneRigComponent::OnBoneOverlapBegin);
				}
			}
		}
	}
}

void UBoneRigComponent::UnbindAllBodyOnOverlapBeginEvent() {
	if (TargetComponent != nullptr) {
		TArray<FName> ArrForUpdate;
		RigBoneDrivers.GetKeys(ArrForUpdate);

		for (const FName& TestBoneName : ArrForUpdate) {

			if (USpineRigBoneDriverComponent* TestComp = RigBoneDrivers[TestBoneName]) {

				if (BaseData->BoneRigBodySetupList.Contains(TestBoneName)) {
					TestComp->OnComponentBeginOverlap.RemoveDynamic(this, &UBoneRigComponent::OnBoneOverlapBegin);
				}
			}
		}
	}
}

void UBoneRigComponent::OnBoneOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor && OtherComp != nullptr) {
		this->OnComponentBeginOverlap.Broadcast(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}





bool UpdatePhysicalAnimationSettings(FName BodyName, const FPhysicalAnimationData& InPhysicalAnimationData, TArray<FPhysicalAnimationData>& DriveData, const USpineRigData& SpineRigData)
{

	if (BodyName != NAME_None)
	{
		//This code does a linear search in the insertion. This is by choice so that during tick we get nice tight cache for iteration. We could keep a map of sorts, but expected number of bodies is small
		FPhysicalAnimationData* UpdateAnimationData = DriveData.FindByPredicate([BodyName](const FPhysicalAnimationData& Elem) { return Elem.BodyName == BodyName; });
		if (UpdateAnimationData == nullptr)
		{
			UpdateAnimationData = &DriveData[DriveData.AddUninitialized()]; //->DriveData가 새롭게 Array에 들어가게되는 부분.
		}
		*UpdateAnimationData = InPhysicalAnimationData;
		UpdateAnimationData->BodyName = BodyName;

		return true;
	}

	return false;
}


void UBoneRigComponent::ApplyPhysicalAnimationSettingsBelow(FName BodyName, const FPhysicalAnimationData& PhysicalAnimationData, bool bIncludeSelf)
{
	USpineRigData* CurrentBaseData = BaseData;
	if (BaseData)
	{

		TArray<FPhysicalAnimationData>& NewDriveData = DriveData;
		bool bNeedsUpdating = false;
		this->ForEachBodyBelow(BodyName, bIncludeSelf, /*bSkipCustomType=*/false, [CurrentBaseData, &NewDriveData, PhysicalAnimationData, &bNeedsUpdating](const FBodyInstance* BI)
			{
				if (BI->GetBodySetup()) {
					const FName& IterBodyName = BI->GetBodySetup()->BoneName;


					bNeedsUpdating |= bool(UpdatePhysicalAnimationSettings(IterBodyName, PhysicalAnimationData, NewDriveData, *CurrentBaseData));

				}

			});

		if (bNeedsUpdating)
		{
			UpdatePhysicsEngine();
		}
	}
}


void UBoneRigComponent::ApplyRigDataPhysicalAnimationSettings(FName PresetName)
{


	USpineRigData* CurrentBaseData = BaseData;

	if (CurrentBaseData == nullptr) {
		return;
	}

	if (PresetName == NAME_None || !BaseData->PhysicsAnimationPreset.Contains(PresetName))
	{
		return;
	}

	DriveData.Empty();
	//This code does a linear search in the insertion. This is by choice so that during tick we get nice tight cache for iteration. We could keep a map of sorts, but expected number of bodies is small


	FSpinePhysicsAnimationPresetStruct& Preset = BaseData->PhysicsAnimationPreset[PresetName];


	TArray<FPhysicalAnimationData>& NewDriveData = DriveData;

	this->ForEachBodyBelow(NAME_None, true, /*bSkipCustomType=*/false, [CurrentBaseData, &NewDriveData, Preset](const FBodyInstance* BI)
		{
			if (BI->GetBodySetup()) {
				const FName IterBodyName = BI->GetBodySetup()->BoneName;

				FPhysicalAnimationData* UpdateAnimationData;

				UpdateAnimationData = &NewDriveData[NewDriveData.AddUninitialized()];

				if (Preset.PresetOverride.Contains(IterBodyName)) {

					const FPhysicalAnimationData& Data = Preset.PresetOverride[IterBodyName];

					UpdateAnimationData->bIsLocalSimulation = Data.bIsLocalSimulation;
					UpdateAnimationData->OrientationStrength = Data.OrientationStrength;
					UpdateAnimationData->AngularVelocityStrength = Data.AngularVelocityStrength;
					UpdateAnimationData->PositionStrength = Data.PositionStrength;
					UpdateAnimationData->VelocityStrength = Data.VelocityStrength;
					UpdateAnimationData->MaxLinearForce = Data.MaxLinearForce;
					UpdateAnimationData->MaxAngularForce = Data.MaxAngularForce;
				}
				else {
					const FPhysicalAnimationData& Data = Preset.GeneralPreset;

					UpdateAnimationData->bIsLocalSimulation = Data.bIsLocalSimulation;
					UpdateAnimationData->OrientationStrength = Data.OrientationStrength;
					UpdateAnimationData->AngularVelocityStrength = Data.AngularVelocityStrength;
					UpdateAnimationData->PositionStrength = Data.PositionStrength;
					UpdateAnimationData->VelocityStrength = Data.VelocityStrength;
					UpdateAnimationData->MaxLinearForce = Data.MaxLinearForce;
					UpdateAnimationData->MaxAngularForce = Data.MaxAngularForce;
				}


				UpdateAnimationData->BodyName = IterBodyName;

			}

		});


	//무조건 강제로 한번 업데이트 시킵니다.
	UpdatePhysicsEngine();

}



void UBoneRigComponent::UpdatePhysicsEngine()
{
	bPhysicsEngineNeedsUpdating = true;	//must defer until tick so that animation can finish
}



void UBoneRigComponent::OnTeleport()
{

	if (CurrentState == EBoneRigState::STATE_FOLLWING) {


		//TeleportAllBoneToAnimation(); //원래 애니메이션의 본 로케이션으로 텔레포트 시킵니다.

		UpdateTargetActors(ETeleportType::TeleportPhysics); // 피직스 애니메이션을 재설정합니다.

	}
	else {
		TeleportAllBoneToSavedTransform();
	}
}

void UBoneRigComponent::TeleportAllBoneToAnimation() {

	TArray<int> Keys;
	RigBoneHierarchyMap.GetKeys(Keys);

	//ReleaseAllJointConstraints();

	for (const int& Index : Keys) {
		for (USpineRigBoneDriverComponent* RigBone : RigBoneHierarchyMap[Index].HeightArr) {
			const FTransform& TF = TargetComponent->GetSkeletonAnimationComp()->GetBoneWorldTransform(RigBone->BoneName);

			RigBone->SetWorldLocationAndRotation(TF.GetLocation(), TF.GetRotation().Rotator(), false, nullptr, ETeleportType::ResetPhysics);
		}
	}

	for (const int& Index : Keys) {
		for (USpineRigBoneDriverComponent* RigBone : RigBoneHierarchyMap[Index].HeightArr) {
			RigBone->SetPhysicsLinearVelocity(FVector::ZeroVector);
			RigBone->SetPhysicsAngularVelocity(FVector::ZeroVector);
		}
	}

	//BindAllJointConstraints();

}

void UBoneRigComponent::TeleportAllBoneToSavedTransform() {
	TArray<int> Keys;
	RigBoneHierarchyMap.GetKeys(Keys);

	//ReleaseAllJointConstraints();

	for (const int& Index : Keys) {
		for (USpineRigBoneDriverComponent* RigBone : RigBoneHierarchyMap[Index].HeightArr) {
			const FTransform& TF = RigBone->LastBoneTranslateData.GetWorldTransformFromArchieve(TargetComponent);

			RigBone->SetWorldLocationAndRotation(TF.GetLocation(), TF.GetRotation().Rotator(), false, nullptr, ETeleportType::ResetPhysics);

		}
	}

	for (const int& Index : Keys) {
		for (USpineRigBoneDriverComponent* RigBone : RigBoneHierarchyMap[Index].HeightArr) {
			RigBone->SetPhysicsLinearVelocity(FVector::ZeroVector);
			RigBone->SetPhysicsAngularVelocity(FVector::ZeroVector);
		}
	}

	//BindAllJointConstraints();
}

void UBoneRigComponent::TeleportAllBoneToSkeletonDataTransform() {

	TArray<int> Keys;
	RigBoneHierarchyMap.GetKeys(Keys);


	TargetComponent->GetSkeletonAnimationComp()->SetBonesToSetupPose();

	TargetComponent->GetSkeletonAnimationComp()->UpdateWorldTransform();
	//TargetComponent->

	spine::Vector<spine::Bone*>& Bones = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->getBones();



	for (const int& Index : Keys) {

		for (USpineRigBoneDriverComponent* RigBone : RigBoneHierarchyMap[Index].HeightArr) {


			const FTransform& TF = TargetComponent->GetSkeletonAnimationComp()->GetBoneWorldTransform(RigBone->BoneName);

			RigBone->SetWorldLocationAndRotation(TF.GetLocation(), TF.GetRotation().Rotator(), false, nullptr, ETeleportType::ResetPhysics);

			//DrawDebugBox(GetWorld(), TF.GetLocation(), FVector(1,1,1),FColor::Green,false, 4 , 0 ,2 );
		}
	}

}


void UBoneRigComponent::AddAllChildrenToInstanceArray(spine::Bone* BoneRef, TArray<FName>& TargetArr, TArray<FName>& SearchArr) {
	if (BoneRef) {
		spine::Vector<spine::Bone*> Children = BoneRef->getChildren();

		size_t Capacity = Children.getCapacity();

		for (size_t Index = 0; Index < Capacity; Index++) {

			const FName& TestBoneName = FName(Children[Index]->getData().getName().buffer());

			if (SearchArr.Contains(TestBoneName)) {
				TargetArr.Add(TestBoneName);
			}

			AddAllChildrenToInstanceArray(Children[Index], TargetArr, SearchArr);
		}
	}
}


int32 UBoneRigComponent::ForEachBodyBelow(FName BoneName, bool bIncludeSelf, bool bSkipCustomType, TFunctionRef<void(FBodyInstance*)> Func)
{
	TArray<FName> Keys;
	RigBodyInstances.GetKeys(Keys);

	if (BoneName == NAME_None && bIncludeSelf && !bSkipCustomType)//모든 본을 전부 추가합니다.
	{
		//바디가 Default로 지정되어있을때만 물리기반 애니메이션을 호출합니다.
		int32 NumBodiesFound = 0;
		for (const FName& BodyName : Keys)
		{
			FBodyInstance* BI = RigBodyInstances[BodyName].Instance;


			if (UBodySetup* BodySetup = BI->GetBodySetup())
			{
				if (BodySetup->PhysicsType != EPhysicsType::PhysType_Default)
				{
					continue;
				}
			}


			++NumBodiesFound;
			Func(BI);
		}

		return NumBodiesFound;
	}
	else
	{
		if (!BaseData || !TargetComponent)
		{
			return 0;
		}

		TArray<FName> Instances;

		if (spine::Bone* BoneRef = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName.ToString()))) {
			if (bIncludeSelf) {
				if (RigBodyInstances.Contains(BoneName)) {
					Instances.Add(BoneName);
				}
			}

			AddAllChildrenToInstanceArray(BoneRef, Instances, Keys);
		}


		//바디가 Default로 지정되어있을때만 물리기반 애니메이션을 호출합니다.
		int32 NumBodiesFound = 0;
		for (const FName& BodyName : Instances)
		{
			FBodyInstance* BI = RigBodyInstances[BodyName].Instance;
			if (bSkipCustomType)
			{
				if (UBodySetup* BodySetup = BI->GetBodySetup())
				{
					if (BodySetup->PhysicsType != EPhysicsType::PhysType_Default)
					{
						continue;
					}
				}
			}

			++NumBodiesFound;
			Func(BI);
		}

		return NumBodiesFound;
	}

	return 0;
}


void UBoneRigComponent::SetPhysicalStrengthMultiplyer(float InStrengthMultiplyer)
{
	if (InStrengthMultiplyer >= 0.f)
	{
		PhysicalAnimationStrength = InStrengthMultiplyer;


		for (int32 DataIdx = 0; DataIdx < DriveData.Num(); ++DataIdx)
		{
			bool bNewConstraint = false;
			const FPhysicalAnimationData& PhysAnimData = DriveData[DataIdx];
			//added guard around crashing animation dereference
			if (DataIdx < RuntimeInstanceData.Num())
			{
				FSpinePhysicalAnimationInstanceData& InstanceData = RuntimeInstanceData[DataIdx];
				if (FConstraintInstance* ConstraintInstance = InstanceData.ConstraintInstance)
				{
					//Apply drive forces
					SetMotorStrength(*ConstraintInstance, PhysAnimData, PhysicalAnimationStrength);
				}
			}
		}
	}
}

bool UBoneRigComponent::CheckIsVaildAnimationComps() {
	if (TargetComponent != nullptr) {
		if (TargetComponent->GetSkeletonAnimationComp() != nullptr) {
			return true;
		}
	}

	return false;
}


FName UBoneRigComponent::GetParentBoneName(FName& TargetBoneName) {
	if (spine::Bone* Bone = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*TargetBoneName.ToString()))) {
		if (spine::Bone* ParentBone = Bone->getParent()) {
			return FName(ParentBone->getData().getName().buffer());
		}
	}

	return NAME_None;
}

FTransform UBoneRigComponent::ComputeTargetTM(const FPhysicalAnimationData& PhysAnimData, TMap<FName, FTransform>& BaseTF) {

	return (PhysAnimData.bIsLocalSimulation) ? ComputeLocalSpaceTargetTM(TargetComponent, BaseTF, PhysAnimData.BodyName) : ComputeWorldSpaceTargetTM(TargetComponent, BaseTF, PhysAnimData.BodyName);

}


FTransform UBoneRigComponent::ComputeWorldSpaceTargetTM(USpineAnimationHandlerComponent* Handler, TMap<FName, FTransform>& BaseTF, FName BoneName)
{
	if (BaseTF.Contains(BoneName)) {

		FTransform Tf = BaseTF[BoneName];

		bool bShouldFlip = (Handler->GetSkeletonAnimationComp()->GetScaleX() > 0) != (Handler->GetSkeletonAnimationComp()->GetScaleY() > 0);

		if (bShouldFlip) {
			Tf.SetRotation(Tf.GetRotation() * FRotator(0, 180, 0).Quaternion());
		}

		return Tf;
	}

	return FTransform::Identity;
}

FTransform UBoneRigComponent::ComputeLocalSpaceTargetTM(USpineAnimationHandlerComponent* Handler, TMap<FName, FTransform>& BaseTF, FName BoneName)
{
	if (!BaseTF.Contains(BoneName)) {
		return FTransform::Identity;
	}
	FTransform AccumulatedDelta = BaseTF[BoneName]; //BaseTF는 애니메이션 상의 월드스페이스 본 로케이션입니다.

	FName& CurBoneName = BoneName;
	while ((CurBoneName = GetParentBoneName(CurBoneName)) != NAME_None)
	{

		if (CurBoneName == BoneName)	//some kind of loop so just stop TODO: warn?
		{
			break;
		}

		if (BaseTF.Contains(CurBoneName))
		{
			const FTransform NewWorldTM = BaseTF[BoneName].GetRelativeTransform(BaseTF[CurBoneName]) * RigBoneDrivers[CurBoneName]->GetComponentTransform();

			return NewWorldTM;
		}

	}

	return FTransform::Identity;


	/*

	const FReferenceSkeleton& RefSkeleton = SkeletalMeshComponent.SkeletalMesh->RefSkeleton;
	FTransform AccumulatedDelta = LocalTransforms[BoneIndex];
	int32 CurBoneIdx = BoneIndex;
	while ((CurBoneIdx = RefSkeleton.GetParentIndex(CurBoneIdx)) != INDEX_NONE)
	{
		FName BoneName = RefSkeleton.GetBoneName(CurBoneIdx);
		int32 BodyIndex = PhysAsset.FindBodyIndex(BoneName);

		if (CurBoneIdx == BoneIndex)	//some kind of loop so just stop TODO: warn?
		{
			break;
		}

		if (SkeletalMeshComponent.Bodies.IsValidIndex(BodyIndex))
		{
			if (BodyIndex < SkeletalMeshComponent.Bodies.Num())
			{
				FBodyInstance* ParentBody = SkeletalMeshComponent.Bodies[BodyIndex];
				const FTransform NewWorldTM = AccumulatedDelta * ParentBody->GetUnrealWorldTransform_AssumesLocked();
				return NewWorldTM;
			}
			else
			{
				// Bodies array has changed on us?
				break;
			}
		}

		AccumulatedDelta = AccumulatedDelta * LocalTransforms[CurBoneIdx];
	}

	return FTransform::Identity;
	*/
}

void UBoneRigComponent::StartPhysicalAnimation() {

	if (!BaseData) {
		UE_LOG(LogTemp, Warning, TEXT("BoneRigComponent '%s' tried to start physical animation, but it didn't have a basedata for initialization"), *this->GetName());
		return;
	}

	if (TargetComponent)
	{

		//TODO: This is hacky and assumes constraints can only be added and not removed. True for now, but bad in general!
		const int32 NumData = DriveData.Num();
		const int32 NumInstances = RuntimeInstanceData.Num();

		RuntimeInstanceData.AddZeroed(NumData - NumInstances);



		TMap<FName, FTransform> BaseTF = GetBoneBaseTransform();


		for (int32 DataIdx = 0; DataIdx < DriveData.Num(); ++DataIdx)
		{
			bool bNewConstraint = false;
			const FPhysicalAnimationData& PhysAnimData = DriveData[DataIdx];
			FSpinePhysicalAnimationInstanceData& InstanceData = RuntimeInstanceData[DataIdx];
			FConstraintInstance*& ConstraintInstance = InstanceData.ConstraintInstance;
			if (ConstraintInstance == nullptr)
			{
				bNewConstraint = true;
				ConstraintInstance = new FConstraintInstance();
				ConstraintInstance->ProfileInstance = BaseData->PhysicsAnimationJointProfileInstance;
			}

			//TMap<FName,FTransform> LocalTransforms = GetBoneSpaceTransforms();

			//Apply drive forces
			SetMotorStrength(*ConstraintInstance, PhysAnimData, PhysicalAnimationStrength);

			if (bNewConstraint)
			{

				if (FBodyInstance* ChildBody = (PhysAnimData.BodyName == NAME_None ? nullptr : RigBodyInstances[PhysAnimData.BodyName].Instance))
				{


					if (PxRigidActor* PRigidActor = FPhysicsInterface_PhysX::GetPxRigidActor_AssumesLocked(ChildBody->ActorHandle))
					{
						ConstraintInstance->SetRefFrame(EConstraintFrame::Frame1, FTransform::Identity);
						ConstraintInstance->SetRefFrame(EConstraintFrame::Frame2, FTransform::Identity);

						const FTransform TargetTM = ComputeTargetTM(PhysAnimData, BaseTF);

						// Create kinematic actor we are going to create joint with. This will be moved around with calls to SetLocation/SetRotation.
						PxScene* PScene = PRigidActor->getScene();
						PxRigidDynamic* KineActor = PScene->getPhysics().createRigidDynamic(U2PTransform(TargetTM));
						KineActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
						KineActor->setMass(1.0f);
						KineActor->setMassSpaceInertiaTensor(PxVec3(1.0f, 1.0f, 1.0f));

						// No bodyinstance
						KineActor->userData = NULL;

						// Add to Scene
						PScene->addActor(*KineActor);

						// Save reference to the kinematic actor.
						InstanceData.TargetActor = KineActor;

						FPhysicsActorHandle TargetRef;
						TargetRef.SyncActor = InstanceData.TargetActor;

						ConstraintInstance->InitConstraint_AssumesLocked(ChildBody->ActorHandle, TargetRef, 1.f);
					}

				}
			}
		}
	}

	bPhysicsEngineNeedsUpdating = false;
}

void UBoneRigComponent::SetMotorStrength(FConstraintInstance& ConstraintInstance, const FPhysicalAnimationData& PhysAnimData, float StrengthMultiplyer) {
	ConstraintInstance.SetAngularDriveParams(PhysAnimData.OrientationStrength * StrengthMultiplyer, PhysAnimData.AngularVelocityStrength * StrengthMultiplyer, PhysAnimData.MaxAngularForce * StrengthMultiplyer);
	if (PhysAnimData.bIsLocalSimulation)	//linear only works for world space simulation
	{
		ConstraintInstance.SetLinearDriveParams(0.f, 0.f, 0.f);
	}
	else
	{
		ConstraintInstance.SetLinearDriveParams(PhysAnimData.PositionStrength * StrengthMultiplyer, PhysAnimData.VelocityStrength * StrengthMultiplyer, PhysAnimData.MaxLinearForce * StrengthMultiplyer);
	}
}

void UBoneRigComponent::StopPhysicalAnimationAdjust() {

	for (FSpinePhysicalAnimationInstanceData& Instance : RuntimeInstanceData)
	{
		if (Instance.ConstraintInstance)
		{
			Instance.ConstraintInstance->TermConstraint();
			delete Instance.ConstraintInstance;
			Instance.ConstraintInstance = nullptr;
		}

		if (Instance.TargetActor)
		{
#if PHYSICS_INTERFACE_PHYSX
			PxScene* PScene = Instance.TargetActor->getScene();
			if (PScene)
			{
				SCOPED_SCENE_WRITE_LOCK(PScene);
				PScene->removeActor(*Instance.TargetActor);
			}
			Instance.TargetActor->release();
			Instance.TargetActor = nullptr;
#else
			FChaosScene* PhysScene = FChaosEngineInterface::GetCurrentScene(Instance.TargetActor);
			if (ensure(PhysScene))
			{
				FPhysInterface_Chaos::ReleaseActor(Instance.TargetActor, PhysScene);
			}
			Instance.TargetActor = nullptr;
#endif
		}
	}

	RuntimeInstanceData.Reset();

}

void UBoneRigComponent::CalculateBoneBaseTransform() {
	LastBoneBaseTransforms.Empty();

	TArray<FName> Keys;

	RigBoneDrivers.GetKeys(Keys);

	for (FName& TestName : Keys) {
		LastBoneBaseTransforms.Add(TestName, RigBoneDrivers[TestName]->LastBoneTranslateData.GetWorldTransformFromArchieve(TargetComponent));

		//DrawDebugBox(GetWorld(), LastBoneBaseTransforms[TestName].GetLocation(), FVector(1,6,1),FColor::Green,false, 0.01 , 0 ,2 );

	}
}

const TMap<FName, FTransform>& UBoneRigComponent::GetBoneBaseTransform() {
	return LastBoneBaseTransforms;
}


void UBoneRigComponent::UpdateTargetActors(ETeleportType TeleportType)
{

	if (TargetComponent)
	{


		TMap<FName, FTransform> BaseTF = GetBoneBaseTransform();

		for (int32 DataIdx = 0; DataIdx < DriveData.Num(); ++DataIdx)
		{
			const FPhysicalAnimationData& PhysAnimData = DriveData[DataIdx];

			if (RuntimeInstanceData.IsValidIndex(DataIdx)) {
				FSpinePhysicalAnimationInstanceData& InstanceData = RuntimeInstanceData[DataIdx];

				if (PxRigidDynamic* TargetActor = InstanceData.TargetActor)
				{

					if (PhysAnimData.BodyName != NAME_None)	//It's possible the skeletal mesh has changed out from under us. In that case we should probably reset, but at the very least don't do work on non-existent bones
					{
						const FTransform& TargetTM = ComputeTargetTM(PhysAnimData, BaseTF);

						//DrawDebugBox(GetWorld(), TargetTM.GetLocation(), FVector(1,1,1),FColor::Green,false, 0.01 , 0 ,2 );


						FTransform TF = TargetTM;


						//DrawDebugBox(GetWorld(), P2UTransform(TargetActor->getGlobalPose()).GetLocation(), FVector(1.2,1.2,1.2), FColor::Cyan, false, 0.01, 0, 1);

						TargetActor->setKinematicTarget(U2PTransform(TF));	//TODO: this doesn't work with sub-stepping!

						if (TeleportType == ETeleportType::TeleportPhysics)
						{
							TargetActor->setGlobalPose(U2PTransform(TF));	//Note that we still set the kinematic target because physx doesn't clear this


							TargetActor->clearForce(physx::PxForceMode::eIMPULSE);
							TargetActor->clearTorque(physx::PxForceMode::eIMPULSE);
						}

					}
				}
			}
		}
	}
}



void UBoneRigComponent::Serialize(FArchive& Ar) {


	if (Ar.IsSaving()) {

		if (GetWorld()) {

			if (GetWorld()->IsGameWorld()) {

				if (Ar.IsSaveGame()) {

					SavedBoneTransform.Empty();

					TArray<FName> Keys;

					RigBoneDrivers.GetKeys(Keys);

					//Ensure that we have all bone data stored.
					for (FName& TestName : Keys) {
						SavedBoneTransform.Add(TestName, RigBoneDrivers[TestName]->GetComponentTransform());
					}
				}
			}
		}
	}


	Super::Serialize(Ar);

	if (!GetWorld()) {
		return;
	}

	if (!GetWorld()->IsGameWorld()) {
		return;
	}

	if (!Ar.IsSaveGame()) {
		return;
	}


	if (Ar.IsLoading()) {

		TArray<FName> Keys;

		RigBoneDrivers.GetKeys(Keys);

		//Ensure that we have all bone data stored.
		for (FName& TestName : Keys) {
			if (!SavedBoneTransform.Contains(TestName)) {
				return;
			}
		}


		InitializeState();

		for (FName& TestName : Keys) {
			if (SavedBoneTransform.Contains(TestName)) {
				//DrawDebugBox(GetWorld(), SavedBoneTransform[TestName].GetLocation(), SavedBoneTransform[TestName].GetScale3D(), FColor::Green, false, 5, 0, 1);
				RigBoneDrivers[TestName]->SetWorldTransform(SavedBoneTransform[TestName]);
			}
		}

	}

}


void UBoneRigComponent::CollectRootMotionType() {
	if (!BaseData) {
		return;
	}


	if (!TargetComponent) {
		return;
	}

	ESpineRootMotionType SavedType = RootMotionType;

	ESpineRootMotionType FinalType = ESpineRootMotionType(0);

	//priority : ApplyRootMotion -> IgnoreRootMotion ->NoRootMotionExtraction

	const TMap<int, FSpineAnimationSet>& Tracks = TargetComponent->GetCopiedPlayingAnimations();

	TArray<int> TrackIndexsForIter;
	Tracks.GetKeys(TrackIndexsForIter);


	for (const int& Index : TrackIndexsForIter) {

		if (BaseData->RootMotionDatas.Contains(FName(Tracks[Index].Animation))) {
			const ESpineRootMotionType& TestType = BaseData->RootMotionDatas[FName(Tracks[Index].Animation)].Type;
			FinalType = (FinalType > TestType) ? FinalType : TestType;
		}

	}

	if (SavedType == ESpineRootMotionType::ApplyRootMotion && SavedType != FinalType) {
		LastRootMotionTransform = FTransform::Identity;
	}

	if (FinalType == ESpineRootMotionType::NoRootMotionExtraction) {
		LastRootMotionTransform = FTransform::Identity;
	}

	RootMotionType = FinalType;
}

void UBoneRigComponent::OnAnimationStarted(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration) {
	CollectRootMotionType();
}


void UBoneRigComponent::OnAnimationEnded(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration) {

	CollectRootMotionType();

}