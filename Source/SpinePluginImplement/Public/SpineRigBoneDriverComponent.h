// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "IKConstraintBoneDriverComponent.h"
#include "SpineRigBoneDriverComponent.generated.h"

/**
 * 
 */
class USpineSkeletonComponent;
class UBoneRigComponent;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class SPINEPLUGINIMPLEMENT_API USpineRigBoneDriverComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:

	USpineRigBoneDriverComponent();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString BoneName;

	UPROPERTY()
		FBoneTranslate LastBoneTranslateData;

	UPROPERTY(EditAnywhere, transient, duplicatetransient)
		class UBodySetup* BoneBodySetup;



public:

	//Properties for rig data
	UPROPERTY()
		class USpineSkeletonComponent* TargetSkeleton = nullptr;


public:

	//Transform Update for Rig

	void UpdateDriveBoneWorldTransform(class USpineAnimationHandlerComponent* TargetComp , float MixValue);

	void UpdateFollowBoneWorldTransform(class USpineAnimationHandlerComponent* TargetComp, float PhysicalWeight);

	void SyncBoneToAnimation(class USpineAnimationHandlerComponent* TargetComp);

	void SyncAnimationToBone(class USpineAnimationHandlerComponent* TargetComp);

	void UpdateFollowBoneWorldTransformFinalApply(class USpineAnimationHandlerComponent* TargetComp, float PhysicalWeight);

	void CollectBoneTranslateData(class USpineSkeletonComponent* TargetComp);

	void ResetBoneTranslateData(class USpineSkeletonComponent* TargetComp);


public:

	//Primitive Overrides
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual class UBodySetup* GetBodySetup() override;

	//virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	//FCollisionShape GetCollisionShape(float Inflation) const override;

	//Primitive Overrides End

public:

	void CreateBodySetupIfNeeded();

	void RebuildBodyCollisionWithBodyInstanceShapes();

	void SetupPhysicActorHandle(FPhysicsActorHandle& Handle);

	UPROPERTY()
		FColor BoneColor;


#if WITH_EDITOR 

	virtual bool ShouldRecreateProxyOnUpdateTransform() const override {
		return true;
	}

	void ChangeBoxColorByHierarchyIndex(const int Index, const int MaxIndex); //sry for bad name

	const FColor GetColorForBoneHierarchy(const int index, const int MaxIndex);

#endif

	
	virtual void AddImpulse(FVector Impulse, FName BoneName = NAME_None, bool bVelChange = false) override;


	virtual void AddAngularImpulseInRadians(FVector Impulse, FName BoneName = NAME_None, bool bVelChange = false) override;


	virtual void AddImpulseAtLocation(FVector Impulse, FVector Location, FName BoneName = NAME_None) override;


	virtual void AddRadialImpulse(FVector Origin, float Radius, float Strength, enum ERadialImpulseFalloff Falloff, bool bVelChange = false) override;


	virtual void AddForce(FVector Force, FName BoneName = NAME_None, bool bAccelChange = false) override;


	virtual void AddForceAtLocation(FVector Force, FVector Location, FName BoneName = NAME_None) override;


	virtual void AddForceAtLocationLocal(FVector Force, FVector Location, FName BoneName = NAME_None) override;


	virtual void AddRadialForce(FVector Origin, float Radius, float Strength, enum ERadialImpulseFalloff Falloff, bool bAccelChange = false) override;


	virtual void AddTorqueInRadians(FVector Torque, FName BoneName = NAME_None, bool bAccelChange = false) override;
	

};
