// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "SpineSkeletonDataAsset.h"
#include "SpineAtlasAsset.h"

#include "PhysicsEngine/BodySetup.h"

#include "SpineRigData.generated.h"

DECLARE_MULTICAST_DELEGATE(FDelegate_OnPropertyChange);

class USpineRigBoneDriverComponent;

USTRUCT(Blueprintable)
struct FConstraintMixStruct {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap<FName,float> ConstraintMixes;

	FConstraintMixStruct() {}

};

USTRUCT(Blueprintable)
struct FJointConstraintData {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Joint)
		FConstraintInstance DefaultInstance;

public:

	UPROPERTY(EditAnywhere, Category = Joint)
		bool bShouldBeCreatedOnFollow = true;

	UPROPERTY(EditAnywhere, Category = Joint)
		bool bShouldBeCreatedOnDrive = false;

	UPROPERTY(EditAnywhere, Category = Joint)
		bool bShouldBeCreatedOnRagdoll = true;


	FJointConstraintData() {}

};

FORCEINLINE uint32 GetTypeHash(const FJointConstraintData& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FJointConstraintData));
	return Hash;
}

//Reflection 비용 때문에 걍 두개로 분화함.
USTRUCT(Blueprintable)
struct FBoneHierarchyStruct {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray<USpineRigBoneDriverComponent*> HeightArr;

	FBoneHierarchyStruct() {}

};

//Reflection 비용 때문에 걍 두개로 분화함.
USTRUCT(Blueprintable)
struct FConstraintHierarchyStruct {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray<UPhysicsConstraintComponent*> HeightArr;

	FConstraintHierarchyStruct() {}

};


USTRUCT(Blueprintable)
struct FSpineBoneBodyPresetStruct {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Collsion")
		FBodyInstance CollisionPreset;

	UPROPERTY(EditAnywhere, Category = "Collsion")
		FBodyInstance Ragdoll_CollisionPreset; //레그돌 모드에서의 콜리전 프로필 타입을 설정합니다. 기본적으로 BlockAll로 지정됩니다.

	FSpineBoneBodyPresetStruct() {}


};


USTRUCT(Blueprintable)
struct FSpinePhysicsAnimationPresetStruct {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Data")
		FPhysicalAnimationData GeneralPreset;

	UPROPERTY(EditAnywhere, Category = "Data")
		TMap<FName, FPhysicalAnimationData> PresetOverride;

	FSpinePhysicsAnimationPresetStruct() {
	
		GeneralPreset.bIsLocalSimulation = false;
		GeneralPreset.OrientationStrength = 3000;
		GeneralPreset.AngularVelocityStrength = 150;
		GeneralPreset.PositionStrength = 1000;
		GeneralPreset.VelocityStrength = 25;
		GeneralPreset.MaxAngularForce = 0;
		GeneralPreset.MaxLinearForce = 1000000.0;

	}


};





/**
	애니메이션의 루트모션 데이터를 나타냅니다.
 */

UENUM(BlueprintType)
enum class ESpineRootMotionType : uint8
{
	NoRootMotionExtraction UMETA(DisplayName = "NoRootMotionExtraction"), // It does nothing.
	IgnoreRootMotion UMETA(DisplayName = "IgnoreRootMotion"), // It fixes Root bone's Location to center
	ApplyRootMotion UMETA(DisplayName = "ApplyRootMotion"), // It will adjust movement capsule to fit with root bone location.
};


USTRUCT()
struct FSpineRootMotionData {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Type")
		ESpineRootMotionType Type = ESpineRootMotionType::NoRootMotionExtraction;

public:

	FSpineRootMotionData() {}

};

USTRUCT()
struct FSpineRigBoneData {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Grouping")
		FName BoneGroup = NAME_None; //리그 안에서 본이 속할 그룹을 나타냅니다. (ex / 팔 , 다리, 몸통, 머리...)


	UPROPERTY(EditAnywhere, Category = "Grouping")
		FName Ragdoll_BoneCollisionGroup = NAME_None; //레그돌 모드에서 리그 안에서 표현되는 관절들의 콜리전 채널을 의미합니다. 채널들이 채널들 안에서 어떻게 반응할지, 각 채널들과 어떻게 반응할지 등을 설정할수 있습니다.

public:

	FSpineRigBoneData() {}

};

/**
 * 리그데이터 오브젝트는 본의 각 관절중, 어떤 것을 표현할지 / 각 관절의 스케일 / 로케이션은 어떤지를 저장하는 에셋 오브젝트입니다.
 * 레그돌 활성화시 사용할 본의 데이터 / 콘스트레인트 / 콜리전에 관한 데이터도 저장시킬 수 있습니다. 본간의 조인트에 대한 셋업또한 가능합니다.
 * 이 데이터를 이용해서 BoneRigComponent에 미리 저장해둔 데이터로 관절을 생성할 수 있습니다.
 */

UCLASS(meta = (DisplayThumbnail = "true"))
class SPINEPLUGINIMPLEMENT_API USpineRigData : public UObject
{
	GENERATED_BODY()


	USpineRigData();

public:


	UPROPERTY(EditAnywhere, Category = "RigSetup")
		TMap<FName, FSpineRigBoneData> BoneRigDataList; //본의 실제 데이터에 관해 저장합니다.

	UPROPERTY(instanced, EditAnywhere, Category = "RigSetup")
		TMap<FName, UBodySetup*> BoneRigBodySetupList; //본의 피직스 데이터에 관해 저장합니다.
	

	UPROPERTY(EditAnywhere, Category = "RigSetup")
		FName DefaultTransformUpdateRootBone; //루트 본의 트랜스폼을 조정하는데 사용되는 본의 이름입니다. (런타임 컴포넌트 내에서도 수정 가능합니다)

#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere, Category = "RigSetup")
		USpineAtlasAsset* PreviewAtlas;

	UPROPERTY(EditAnywhere, Category = "RigSetup")
		USpineSkeletonDataAsset* PreviewSkeletonData;

#endif

	void FixupInstances();

#if WITH_EDITOR 
	FDelegate_OnPropertyChange OnPropertyChange;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

	virtual void Serialize(FArchive& Ar) override;

public:

	UPROPERTY(EditAnywhere, Category = "Joints")
		TMap<FName, FJointConstraintData> JointList;

public:


	UPROPERTY(EditAnywhere, Category = "Collsion")
		FSpineBoneBodyPresetStruct RigCollisionPreset;

	UPROPERTY(EditAnywhere, Category = "Collsion")
		TMap<FName, FSpineBoneBodyPresetStruct> RigCollisionPresetOverride;


public:


	UPROPERTY(EditAnywhere, Category = "ConstraintMixes")
		FConstraintMixStruct RagdollConstraintMixData; //레그돌에서 사용할 믹스데이터의 총 집합입니다.

	UPROPERTY(EditAnywhere, Category = "ConstraintMixes")
		FConstraintMixStruct DriveConstraintMixData; //드라이브 모드에서 사용할 믹스데이터의 총 집합입니다.

	UPROPERTY(EditAnywhere, Category = "ConstraintMixes")
		TMap<FName, FConstraintMixStruct> DriveModeConstraintMixDataPresets; //레그돌에서 사용할 믹스데이터의 총 집합입니다.


public:


	//Joint preset that be used in physical animation
	UPROPERTY(EditAnywhere, Category = "Physics")
		FConstraintProfileProperties PhysicsAnimationJointProfileInstance;


	//물리기반 애니메이션에서 사용할 프리셋입니다.
	UPROPERTY(EditAnywhere, Category = "Physics")
		TMap<FName, FSpinePhysicsAnimationPresetStruct> PhysicsAnimationPreset;


public:

	//Animation Name , RootMotionPreset
	UPROPERTY(EditAnywhere, Category = "RootMotion", meta = (TitleProperty = "Type"))
		TMap<FName, FSpineRootMotionData> RootMotionDatas;


public:

	UFUNCTION()
		void SaveBoneJointsFromComponent(UBoneRigComponent* Component);
	
};
