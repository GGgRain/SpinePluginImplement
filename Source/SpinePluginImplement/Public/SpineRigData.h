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

//Reflection ��� ������ �� �ΰ��� ��ȭ��.
USTRUCT(Blueprintable)
struct FBoneHierarchyStruct {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray<USpineRigBoneDriverComponent*> HeightArr;

	FBoneHierarchyStruct() {}

};

//Reflection ��� ������ �� �ΰ��� ��ȭ��.
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
		FBodyInstance Ragdoll_CollisionPreset; //���׵� ��忡���� �ݸ��� ������ Ÿ���� �����մϴ�. �⺻������ BlockAll�� �����˴ϴ�.

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
	�ִϸ��̼��� ��Ʈ��� �����͸� ��Ÿ���ϴ�.
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
		FName BoneGroup = NAME_None; //���� �ȿ��� ���� ���� �׷��� ��Ÿ���ϴ�. (ex / �� , �ٸ�, ����, �Ӹ�...)


	UPROPERTY(EditAnywhere, Category = "Grouping")
		FName Ragdoll_BoneCollisionGroup = NAME_None; //���׵� ��忡�� ���� �ȿ��� ǥ���Ǵ� �������� �ݸ��� ä���� �ǹ��մϴ�. ä�ε��� ä�ε� �ȿ��� ��� ��������, �� ä�ε�� ��� �������� ���� �����Ҽ� �ֽ��ϴ�.

public:

	FSpineRigBoneData() {}

};

/**
 * ���׵����� ������Ʈ�� ���� �� ������, � ���� ǥ������ / �� ������ ������ / �����̼��� ����� �����ϴ� ���� ������Ʈ�Դϴ�.
 * ���׵� Ȱ��ȭ�� ����� ���� ������ / �ܽ�Ʈ����Ʈ / �ݸ����� ���� �����͵� �����ų �� �ֽ��ϴ�. ������ ����Ʈ�� ���� �¾����� �����մϴ�.
 * �� �����͸� �̿��ؼ� BoneRigComponent�� �̸� �����ص� �����ͷ� ������ ������ �� �ֽ��ϴ�.
 */

UCLASS(meta = (DisplayThumbnail = "true"))
class SPINEPLUGINIMPLEMENT_API USpineRigData : public UObject
{
	GENERATED_BODY()


	USpineRigData();

public:


	UPROPERTY(EditAnywhere, Category = "RigSetup")
		TMap<FName, FSpineRigBoneData> BoneRigDataList; //���� ���� �����Ϳ� ���� �����մϴ�.

	UPROPERTY(instanced, EditAnywhere, Category = "RigSetup")
		TMap<FName, UBodySetup*> BoneRigBodySetupList; //���� ������ �����Ϳ� ���� �����մϴ�.
	

	UPROPERTY(EditAnywhere, Category = "RigSetup")
		FName DefaultTransformUpdateRootBone; //��Ʈ ���� Ʈ�������� �����ϴµ� ���Ǵ� ���� �̸��Դϴ�. (��Ÿ�� ������Ʈ �������� ���� �����մϴ�)

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
		FConstraintMixStruct RagdollConstraintMixData; //���׵����� ����� �ͽ��������� �� �����Դϴ�.

	UPROPERTY(EditAnywhere, Category = "ConstraintMixes")
		FConstraintMixStruct DriveConstraintMixData; //����̺� ��忡�� ����� �ͽ��������� �� �����Դϴ�.

	UPROPERTY(EditAnywhere, Category = "ConstraintMixes")
		TMap<FName, FConstraintMixStruct> DriveModeConstraintMixDataPresets; //���׵����� ����� �ͽ��������� �� �����Դϴ�.


public:


	//Joint preset that be used in physical animation
	UPROPERTY(EditAnywhere, Category = "Physics")
		FConstraintProfileProperties PhysicsAnimationJointProfileInstance;


	//������� �ִϸ��̼ǿ��� ����� �������Դϴ�.
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
