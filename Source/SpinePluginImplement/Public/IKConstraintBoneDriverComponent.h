// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpineSkeletonRendererComponent.h"
#include "SpineSkeletonAnimationComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "IKConstraintBoneDriverComponent.generated.h"

class USpineAnimationHandlerComponent;

USTRUCT()
struct FBoneTranslate {

	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		bool bShouldApply = false;

	UPROPERTY()
		float _x;
	UPROPERTY()
		float _y;
	UPROPERTY()
		float _rotation;
	UPROPERTY()
		float _scaleX;
	UPROPERTY()
		float _scaleY;
	UPROPERTY()
		float _shearX;
	UPROPERTY()
		float _shearY;
	UPROPERTY()
		float _ax;
	UPROPERTY()
		float _ay;
	UPROPERTY()
		float _arotation;
	UPROPERTY()
		float _ascaleX;
	UPROPERTY()
		float _ascaleY;
	UPROPERTY()
		float _ashearX;
	UPROPERTY()
		float _ashearY;
	UPROPERTY()
		float _a;
	UPROPERTY()
		float _b;
	UPROPERTY()
		float _worldX;
	UPROPERTY()
		float _c;
	UPROPERTY()
		float _d;
	UPROPERTY()
		float _worldY;


	void CopyFromBone(spine::Bone* Bone) {
		if (!bShouldApply) {
			_x = Bone->getX();
			_y = Bone->getY();
			_rotation = Bone->getRotation();
			_scaleX = Bone->getScaleX();
			_scaleY = Bone->getScaleY();
			_shearX = Bone->getShearX();
			_shearY = Bone->getShearY();
			_ax = Bone->getAX();
			_ay = Bone->getAY();
			_arotation = Bone->getAppliedRotation();
			_ascaleX = Bone->getAScaleX();
			_ascaleY = Bone->getAScaleY();
			_ashearX = Bone->getAShearX();
			_ashearY = Bone->getAShearY();
			_a = Bone->getA();
			_b = Bone->getB();
			_worldX = Bone->getWorldX();
			_c = Bone->getC();
			_d = Bone->getD();
			_worldY = Bone->getWorldY();

			bShouldApply = true;
		}
	}

	void PasteToBone(spine::Bone* Bone) {
		if (bShouldApply) {
			Bone->setX(_x);
			Bone->setY(_y);
			Bone->setRotation(_rotation);
			Bone->setScaleX(_scaleX);
			Bone->setScaleY(_scaleY);
			Bone->setShearX(_shearX);
			Bone->setShearY(_shearY);
			Bone->setAX(_ax);
			Bone->setAY(_ay);
			Bone->setAppliedRotation(_arotation);
			Bone->setAScaleX(_ascaleX);
			Bone->setAScaleY(_ascaleY);
			Bone->setAShearX(_ashearX);
			Bone->setAShearY(_ashearY);
			Bone->setA(_a);
			Bone->setB(_b);
			Bone->setWorldX(_worldX);
			Bone->setC(_c);
			Bone->setD(_d);
			Bone->setWorldY(_worldY);

			bShouldApply = false;
		}
	}

	FTransform GetWorldTransformFromArchieve(USpineSkeletonRendererComponent* Renderer) {

		FTransform baseTransform = Renderer->GetComponentTransform();

		FMatrix localTransform;
		localTransform.SetIdentity();
		localTransform.SetAxis(2, FVector(_a, 0, _c));
		localTransform.SetAxis(0, FVector(_b, 0, _d));
		localTransform.SetOrigin(FVector(_worldX, 0, _worldY));
		localTransform = localTransform * baseTransform.ToMatrixWithScale();

		FTransform result;
		result.SetFromMatrix(localTransform);
		return result;
	}
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPINEPLUGINIMPLEMENT_API UIKConstraintBoneDriverComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UIKConstraintBoneDriverComponent();


public:

	// (multiplied with component's scales) 
	//X : Facing direction / Y : Backward direction trace distance muitlplier. * It will be displayed in the game world as arrowcomponent's length.
	UPROPERTY(EditAnywhere, Category = "IKSetting")
		float CastDistance = 5.f;
	UPROPERTY(EditAnywhere, Category = "IKSetting")
		float CastOffset = 0;
	UPROPERTY(EditAnywhere, Category = "IKSetting")
		float GroundOffset = 0;
	UPROPERTY(EditAnywhere, Category = "IKSetting")
		float AdjustSpeed = 5;
	UPROPERTY(EditAnywhere, Category = "IKSetting")
		FName TraceProfile = "TerrainAdjustIKPreset";

	UPROPERTY(Transient)
		float OffLength;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString IKBoneName;

	//Uses just this component when set to true. Updates owning actor otherwise.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UsePosition = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool UseScale = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShouldUpdate = true;

#if WITH_EDITORONLY_DATA 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShouldDisplayDebugCastingDraw = true;

#endif
private:

	UPROPERTY(VisibleAnywhere)
		USpineAnimationHandlerComponent* TargetComponent = nullptr;

	UPROPERTY()
		FBoneTranslate LastBoneData;

public:

	UFUNCTION()
		void AdjustTargetIKBone(USpineSkeletonComponent* skeleton);

	UFUNCTION(BlueprintCallable)
		void ResetTargetIKBone(USpineSkeletonComponent* skeleton);

public:

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

public:

	UFUNCTION(BlueprintCallable)
		void SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent);

	UFUNCTION(BlueprintCallable)
		USpineAnimationHandlerComponent* GetTargetComponent() { return TargetComponent; }

public:

	//spine::Vector<spine::TrackEntry*> DoesAnimationStateHasIKBoneTimeline();

	const bool IsTrackMixTimeOver(spine::TrackEntry* CheckEntry);

	//업데이트가 되지 않은 기존 트랜스폼을 주면 업데이트해야하는 위치로 변환해서 리턴합니다.
	void TraceDist(USpineSkeletonComponent* skeleton, FTransform& TransformBeforeUpdate);


public:

#if WITH_EDITORONLY_DATA

	//에디터상에서 부모 핸들러와의 연결을 위해서 사용하는 타겟 지정 변수입니다.
	UPROPERTY(EditAnywhere, Category = "Setting")
		FName TargetComponentName = NAME_None;
#endif

};
