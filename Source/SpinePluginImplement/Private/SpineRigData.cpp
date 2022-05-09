// Fill out your copyright notice in the Description page of Project Settings.


#include "SpineRigData.h"
#include "BoneRigComponent.h"
#include "SpineRigBoneDriverComponent.h"

USpineRigData::USpineRigData() {

	PhysicsAnimationJointProfileInstance.LinearLimit.XMotion = LCM_Free;
	PhysicsAnimationJointProfileInstance.LinearLimit.YMotion = LCM_Free;
	PhysicsAnimationJointProfileInstance.LinearLimit.ZMotion = LCM_Free;

	PhysicsAnimationJointProfileInstance.LinearLimit.Limit = 5;
	PhysicsAnimationJointProfileInstance.LinearLimit.bSoftConstraint = true;
	PhysicsAnimationJointProfileInstance.LinearLimit.Stiffness = 10;
	PhysicsAnimationJointProfileInstance.LinearLimit.Damping = 1;
	PhysicsAnimationJointProfileInstance.LinearLimit.Restitution = 0.5;

	PhysicsAnimationJointProfileInstance.ConeLimit.Swing1Motion = ACM_Free;
	PhysicsAnimationJointProfileInstance.ConeLimit.Swing2Motion = ACM_Free;
	PhysicsAnimationJointProfileInstance.TwistLimit.TwistMotion = ACM_Free;

	

	PhysicsAnimationJointProfileInstance.ConeLimit.Swing1LimitDegrees = 10;
	PhysicsAnimationJointProfileInstance.ConeLimit.Swing2LimitDegrees = 10;
	PhysicsAnimationJointProfileInstance.ConeLimit.bSoftConstraint = true;
	PhysicsAnimationJointProfileInstance.ConeLimit.Stiffness = 0;
	PhysicsAnimationJointProfileInstance.ConeLimit.Damping = 0;
	PhysicsAnimationJointProfileInstance.ConeLimit.Restitution = 0.1;

	PhysicsAnimationJointProfileInstance.TwistLimit.TwistLimitDegrees = 10;
	PhysicsAnimationJointProfileInstance.TwistLimit.bSoftConstraint = true;
	PhysicsAnimationJointProfileInstance.TwistLimit.Stiffness = 0;
	PhysicsAnimationJointProfileInstance.TwistLimit.Damping = 0;
	PhysicsAnimationJointProfileInstance.TwistLimit.Restitution = 0.1;

	

	PhysicsAnimationJointProfileInstance.LinearDrive.XDrive.bEnablePositionDrive = true;
	PhysicsAnimationJointProfileInstance.LinearDrive.XDrive.bEnableVelocityDrive = true;
	PhysicsAnimationJointProfileInstance.LinearDrive.YDrive.bEnablePositionDrive = true;
	PhysicsAnimationJointProfileInstance.LinearDrive.YDrive.bEnableVelocityDrive = true;
	PhysicsAnimationJointProfileInstance.LinearDrive.ZDrive.bEnablePositionDrive = true;
	PhysicsAnimationJointProfileInstance.LinearDrive.ZDrive.bEnableVelocityDrive = true;

	PhysicsAnimationJointProfileInstance.LinearDrive.XDrive.Stiffness = 0;
	PhysicsAnimationJointProfileInstance.LinearDrive.YDrive.Stiffness = 0;
	PhysicsAnimationJointProfileInstance.LinearDrive.ZDrive.Stiffness = 0;

	PhysicsAnimationJointProfileInstance.LinearDrive.XDrive.Damping = 0;
	PhysicsAnimationJointProfileInstance.LinearDrive.YDrive.Damping = 0;
	PhysicsAnimationJointProfileInstance.LinearDrive.ZDrive.Damping = 0;

	PhysicsAnimationJointProfileInstance.AngularDrive.SlerpDrive.bEnablePositionDrive = true;
	PhysicsAnimationJointProfileInstance.AngularDrive.SlerpDrive.bEnableVelocityDrive = true;
	PhysicsAnimationJointProfileInstance.AngularDrive.AngularDriveMode = EAngularDriveMode::SLERP;

	PhysicsAnimationJointProfileInstance.bEnableProjection = true;
	PhysicsAnimationJointProfileInstance.ProjectionAngularAlpha = 1;
	PhysicsAnimationJointProfileInstance.bParentDominates = true;
}


void USpineRigData::SaveBoneJointsFromComponent(UBoneRigComponent* Component) {
	if (Component != nullptr) {

		TArray<FName> Bones;
		Component->RigBoneDrivers.GetKeys(Bones);

		for (FName TestName : Bones) {
			if (USpineRigBoneDriverComponent* CastedComp = Component->RigBoneDrivers[TestName]) {
				FSpineRigBoneData RigBoneData = FSpineRigBoneData();

				UBodySetup* Setup = NewObject<UBodySetup>(this, NAME_None);
				BoneRigBodySetupList.Add(TestName, Setup);
				BoneRigDataList.Add(TestName, RigBoneData);
			}
		}
	}
}

void USpineRigData::FixupInstances() {
	RigCollisionPreset.CollisionPreset.FixupData(this);
	RigCollisionPreset.Ragdoll_CollisionPreset.FixupData(this);

	TArray<FName> Keys;
	RigCollisionPresetOverride.GetKeys(Keys);

	for (FName Key : Keys) {
		RigCollisionPresetOverride[Key].CollisionPreset.FixupData(this);
		RigCollisionPresetOverride[Key].Ragdoll_CollisionPreset.FixupData(this);
	}
}

void USpineRigData::Serialize(FArchive& Ar) {

	Super::Serialize(Ar);

	if (Ar.IsLoading() && this->IsTemplate()) {

		FixupInstances();

	}

}



#if WITH_EDITOR 


void USpineRigData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FixupInstances();

	if (OnPropertyChange.IsBound()) {
		OnPropertyChange.Broadcast();
	}
}
#endif