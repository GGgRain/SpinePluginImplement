// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "SpineRigBoneDriverComponent.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

#include <spine/IkConstraint.h>
#include <spine/Vector.h>

#include "SpineRigData.h"

#include "SpineAnimationHandlerComponent.h"

#include "BoneRigComponent.generated.h"

class USpineAnimationHandlerComponent;

UENUM(BlueprintType)
enum class EBoneRigState : uint8
{
	STATE_NONE UMETA(DisplayName = "STATE_NONE"), // Exists , but calculate nothing.
	STATE_FOLLWING UMETA(DisplayName = "STATE_FOLLWING"), // All Bone Replicated Drive Components will follow current animation's bone location, But It can be affected by physicsSystem.
	STATE_DRIVING UMETA(DisplayName = "STATE_DRIVING"), // All Bone Replicated Drive Components will drive current animation's bone location
	STATE_RAGDOLL UMETA(DisplayName = "STATE_RAGDOLL"), // It Will Calculate the physics updates for bones.
};

UENUM(BlueprintType)
enum class ERootComponentTransformUpdatePolicy : uint8
{
	DO_NOTHING UMETA(DisplayName = "DO_NOTHING"), // Calculate nothing.
	STRICTLY_FOLLOW_ROOTBONE UMETA(DisplayName = "STRICTLY_FOLLOW_ROOTBONE"), // Root component's entire transform will be updated as same as the root bone's transform.
	ONLY_TRANSLATE_FOLLOW_ROOTBONE UMETA(DisplayName = "ONLY_TRANSLATE_FOLLOW_ROOTBONE"), // Root component's Translate on transform will be updated as same as the root bone's transform's one.
	ONLY_TRANSLATE_FOLLOW_ROOTBONE_WITH_GROUNDCHECK UMETA(DisplayName = "ONLY_TRANSLATE_FOLLOW_ROOTBONE_WITH_GROUNDCHECK"), // Identical with the ONLY_TRANSLATE_FOLLOW_ROOTBONE, but it includes cast-based adjust code to make it doesn't go through a ground and stay in proper height.
};

UENUM(BlueprintType)
enum class EPhysicsSimulationMode : uint8
{
	NONE UMETA(DisplayName = "NONE"), 
	ALL UMETA(DisplayName = "ALL"), 
	ONLY_SIMULATED UMETA(DisplayName = "ONLY_SIMULATED"), 
};



USTRUCT()
struct FSpineBodyInstanceWrapper
{
	GENERATED_BODY()
	struct FBodyInstance* Instance;
	FSpineBodyInstanceWrapper() : Instance(nullptr) {}
	FSpineBodyInstanceWrapper(struct FBodyInstance* InInstance) : Instance(InInstance) {}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPINEPLUGINIMPLEMENT_API UBoneRigComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	struct FSpinePhysicalAnimationInstanceData
	{
		struct FConstraintInstance* ConstraintInstance;

		physx::PxRigidDynamic* TargetActor;
	};

public:

	// Sets default values for this component's properties
	UBoneRigComponent();

	UFUNCTION(BlueprintCallable)
		TMap<FName, USpineRigBoneDriverComponent*> GetRigBoneDrivers() { return RigBoneDrivers; }


private:

	UPROPERTY(VisibleAnywhere)
		USpineAnimationHandlerComponent* TargetComponent = nullptr;

public:

	UPROPERTY(EditAnywhere)
		TMap<FName, USpineRigBoneDriverComponent*> RigBoneDrivers;

	UPROPERTY(EditAnywhere)
		TMap<FName, UPhysicsConstraintComponent*> RigConstraints;

	UPROPERTY(EditAnywhere)
		TMap<FName, FSpineBodyInstanceWrapper> RigBodyInstances;

	UPROPERTY(EditAnywhere)
		USpineRigData* BaseData; // Data about collision bodysetups and constraints for each bone.

	UPROPERTY(EditAnywhere)
		ERootComponentTransformUpdatePolicy RootComponentTransformUpdatePolicy;

	UPROPERTY(EditAnywhere)
		FName TransformUpdateRootBone; //레그돌 모드에서 루트 본의 트랜스폼을 조정하는데 사용되는 본의 이름입니다. 

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Interpolation", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float InterpolationKey = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Interpolation", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
		float PhysicalAnimationStrength = 1.f;



private:

	UPROPERTY(VisibleAnywhere, SaveGame)
		EBoneRigState CurrentState = EBoneRigState::STATE_NONE;

public:

	UFUNCTION()
		FORCEINLINE EBoneRigState GetCurrentState() { return CurrentState; }

public:

	UFUNCTION()
		void UpdateRootMotion();

	UFUNCTION()
		void ResetRootMotion();

	UFUNCTION()
		void CollectRootMotionType();

	UPROPERTY(EditAnywhere, Category = "RootMotion")
		ESpineRootMotionType RootMotionType = ESpineRootMotionType(0);

	UPROPERTY(VisibleAnywhere, Category = "RootMotion")
		FBoneTranslate LastSavedRootBoneTs;

	UPROPERTY(VisibleAnywhere, Category = "RootMotion")
		FTransform LastRootMotionTransform = FTransform::Identity;

public:

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndReason) override;

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

public:

	void InitializeRig();

	UPROPERTY(Transient)
		bool bInitializeInOutsideComponent = false;

	UPROPERTY(Transient)
		bool bInitialized = false;

	void InitializeState();

public:

	UFUNCTION(BlueprintCallable)
		void SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent);

	UFUNCTION(BlueprintCallable)
		USpineAnimationHandlerComponent* GetTargetComponent() { return TargetComponent; }

public:


	UFUNCTION(BlueprintCallable)
		void CreateRig();

	UFUNCTION(BlueprintCallable)
		void RemoveRig();

public:

	UFUNCTION()
		void CreateBones();

	UFUNCTION()
		void RemoveBones();


	UFUNCTION()
		void CreateJointConstraints();

	UFUNCTION()
		void RemoveJointConstraints();

	UFUNCTION()
		void BindAllJointConstraints();

	UFUNCTION()
		void ReleaseAllJointConstraints();
		

public:

	UFUNCTION()
		void UpdateBones(class USpineSkeletonComponent* TargetComp);

	UFUNCTION()
		void UpdateSpineConstraints(class USpineSkeletonComponent* TargetComp);

	UFUNCTION()
		void CollectAllBoneTF(class USpineSkeletonComponent* TargetComp);

	UFUNCTION()
		void ResetAfterUpdateBones(class USpineSkeletonComponent* TargetComp);

	UFUNCTION()
		void ResetAllBonesTransform(class USpineSkeletonComponent* TargetComp);

	UFUNCTION()
		void UpdateOnlyIKBones(class USpineSkeletonComponent* TargetComp);

public:

	
	UFUNCTION(BlueprintCallable)
		void SetUpdate_Following();

	//머리카락이나 옷 소매같은 조인트들과 본의 업데이트 때문에 추가적인 파라미터가 존재합니다.
	//bShouldUpdateKinematic이 true이면 Simulate으로 설정된 본은 물리 업데이트를 하고, Drive 모드에서 생성되게 설정된 조인트들이 생성됩니다.
	//프리셋 네임은 해당 업데이트에서 사용할 IK 세팅을 의미합니다.
	UFUNCTION(BlueprintCallable)
		void SetUpdate_Driving(bool bShouldSimulate = true, FName IKPresetName = NAME_None); 

	UFUNCTION(BlueprintCallable)
		void SetUpdate_RagdollUpdating();

	UFUNCTION(BlueprintCallable)
		void SetUpdate_None();

	UFUNCTION()
		void FinishUpdate();

	void UpdateCollisionProfileBasedOnUpdateMode();

	UFUNCTION(BlueprintCallable)
		void ApplySimulatePhysicsToBones(EPhysicsSimulationMode Mode);


private:

	UPROPERTY(EditAnywhere)
		bool bCanSetUpdate = true;

public:

	UFUNCTION(BlueprintCallable)
		void SetCanSetUpdate(bool bNew) { bCanSetUpdate = bNew; }

	UFUNCTION(BlueprintCallable)
		bool GetCanSetUpdate() { return bCanSetUpdate; }


public:
	
	//Spine의 콘스트레인트 데이터를 프리셋을 이용해 조작합니다. bShouldReset이 false이면 리그의 디폴트 세팅으로 콘스트레인트를 복원하고, true이면 주어진 프리셋 이름을 기반으로 콘스트레인트를 변경합니다. 
	//기본적으로 follow mode거나 ragdoll mode일 경우 프리셋 지정은 아무런 동작도 하지 않고, drive 모드일때만 작동합니다.
	//Drive 모드일때 프리셋 이름이 지정되지 않으면 기본 프리셋을 사용하며, 프리셋이 지정되면 프리셋 리스트에서 이름을 검색합니다.
	UFUNCTION(BlueprintCallable)
		void SetIKConstraints(bool bShouldReset, FName PresetName = NAME_None);
	

public:

	UFUNCTION(BlueprintCallable)
		void FlipSkeleton(bool bShouldFlip);



public:

	UFUNCTION()
		void CreateCollisionSetupForBones();

public:

	//Bind all components that has bodysetup and its own shape to invoke this component's OnOverlapBegin event on beginning overlaping.
	UFUNCTION()
		void BindAllBodyOnOverlapBeginEvent(); 

	UFUNCTION()
		void UnbindAllBodyOnOverlapBeginEvent();

	UFUNCTION()
		void OnBoneOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:

	//A container that has Bone's Hierarchy level. This is essential at updating bone properly.
	UPROPERTY(EditAnywhere)
		TMap<int, FBoneHierarchyStruct> RigBoneHierarchyMap;

	//A container that has Constraint's Hierarchy level. This is essential at updating constraint and bone properly.
	UPROPERTY(EditAnywhere)
		TMap<int, FConstraintHierarchyStruct> RigConstraintHierarchyMap;

	void CalculateBoneHierarchyMap();
	
	void CalculateConstraintHierarchyMap();


	void AttachAllChildBoneToClosestParentBone(bool bOnlyAttachNonSimulatableObject);

	void UpdateRootComponentBasedOnPolicy();

	UPROPERTY()
		bool bIsAbsolute = false;

	void ToggleAbsoluteBasedOnState(bool bShouldSetAbsolute);

	void ToggleDOFConstraintOfAllBones(bool bShouldSetConstraint);

public:

	/** Iterates over all bodies below and executes Func. Returns number of bodies found */
	int32 ForEachBodyBelow(FName BoneName, bool bIncludeSelf, bool bSkipCustomType, TFunctionRef<void(FBodyInstance*)> Func);

	void OnTeleport();

	void TeleportAllBoneToAnimation();

	void TeleportAllBoneToSavedTransform();

	void TeleportAllBoneToSkeletonDataTransform();

public:

	
	void StartPhysicalAnimation();

	void StopPhysicalAnimationAdjust();

	void SetMotorStrength(FConstraintInstance& ConstraintInstance, const FPhysicalAnimationData& PhysAnimData, float StrengthMultiplyer);

	void UpdateTargetActors(ETeleportType TeleportType);

	static const FConstraintProfileProperties PhysicalAnimationProfile;

	/** Applies the physical animation settings to the body given and all bodies below.*/
	UFUNCTION(BlueprintCallable, Category = PhysicalAnimation, meta = (UnsafeDuringActorConstruction))
		void ApplyPhysicalAnimationSettingsBelow(FName BodyName, const FPhysicalAnimationData& PhysicalAnimationData, bool bIncludeSelf = true);

	/** Applies the physical animation settings by preset name. */
	UFUNCTION(BlueprintCallable, Category = PhysicalAnimation, meta = (UnsafeDuringActorConstruction))
		void ApplyRigDataPhysicalAnimationSettings(FName PresetName);

	void SetPhysicalStrengthMultiplyer(float InStrengthMultiplyer);

private: 

	//Physical Animation Related

	/** constraints used to apply the drive data */


	TArray<FSpinePhysicalAnimationInstanceData> RuntimeInstanceData;

	TArray<FPhysicalAnimationData> DriveData;

	FDelegateHandle OnTeleportDelegateHandle;

	void UpdatePhysicsEngine();

	bool bPhysicsEngineNeedsUpdating;

	FTransform ComputeTargetTM(const FPhysicalAnimationData& PhysAnimData, TMap<FName, FTransform>& BaseTF);

	FTransform ComputeWorldSpaceTargetTM(USpineAnimationHandlerComponent* Handler, TMap<FName, FTransform>& BaseTF, FName BoneName);

	FTransform ComputeLocalSpaceTargetTM(USpineAnimationHandlerComponent* Handler, TMap<FName, FTransform>& BaseTF, FName BoneName);

	void AddAllChildrenToInstanceArray(spine::Bone* BoneRef, TArray<FName>& TargetArr, TArray<FName>& SearchArr);

	bool CheckIsVaildAnimationComps();

	FName GetParentBoneName(FName& TargetBoneName);

public:
	
	UPROPERTY(EditAnywhere, Category = Physics)
		float DefaultPhysicalWeight = 15.f;

	UPROPERTY(EditAnywhere, Category = Physics)
		float RagdollPhysicalWeight = 3.f;

private:

	UPROPERTY()
		TMap<FName, FTransform> LastBoneBaseTransforms;

public:

	void CalculateBoneBaseTransform();

	const TMap<FName, FTransform>& GetBoneBaseTransform();

	//A container that has Bone's Hierarchy level. This is essential at updating bone properly.


public:

	//Serialization

	virtual void Serialize(FArchive& Ar) override;

	UPROPERTY(SaveGame)
		TMap<FName, FTransform> SavedBoneTransform;

public:

	UFUNCTION()
		void OnAnimationStarted(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration);

	UFUNCTION()
		void OnAnimationEnded(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration);

public:

#if WITH_EDITORONLY_DATA

	//에디터상에서 부모 핸들러와의 연결을 위해서 사용하는 타겟 지정 변수입니다.
	UPROPERTY(EditAnywhere, Category = "Setting")
		FName TargetComponentName = NAME_None;
#endif

};
