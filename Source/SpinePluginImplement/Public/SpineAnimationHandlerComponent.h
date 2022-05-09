// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpineSkeletonAnimationComponent.h"
#include "SpineSkeletonRendererComponent.h"
#include "SpineAnimationHandlerComponent.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnHandlerTeleportedMultiCast);
typedef FOnHandlerTeleportedMultiCast::FDelegate FOnHandlerTeleported;

//Forward declartion.

class UBoneRigComponent;
class USpineAdvancedFollowerComponent;
class USpineAdvancedDriverComponent;
class UIKConstraintBoneDriverComponent;


UENUM(BlueprintType)
enum class ESpineTrackIndex : uint8
{
	INDEX_BaseAnimation = 0 UMETA(DisplayName = "INDEX_BaseAnimation"),
	INDEX_AimmingAnimation = 1 UMETA(DisplayName = "INDEX_AimmingAnimation"),
	INDEX_CCAnimation = 2 UMETA(DisplayName = "INDEX_CCAnimation"),
	INDEX_AttackAnimation = 3 UMETA(DisplayName = "INDEX_AttackAnimation"),
	INDEX_IdlePlayAnimation = 5 UMETA(DisplayName = "INDEX_IdleAnimation"),
	INDEX_FacialBasic = 10 UMETA(DisplayName = "INDEX_FacialBasic"),
	INDEX_DialogueAnimation = 11 UMETA(DisplayName = "INDEX_FacialDetail"),
	INDEX_EyeAnimation = 12 UMETA(DisplayName = "INDEX_EyeAnimation"),
};

UENUM(BlueprintType)
enum class ESpineAnimationPlayType : uint8
{
	//��� �ִϸ��̼� ���� ��Ÿ���� ���������մϴ�. �̸� �̿��ؼ� �ϳ��� �ִϸ��̼����ε� �پ��� ǥ���� ���� �� �ֽ��ϴ�.
	//����) ����ġ�� �ø��� ������ �ִϸ��̼��� ����� �ʹٸ�, ����ġ�� ������ �ִϸ��̼��� PLAY_FORWARD_ONCE�� ������ ���¿��� �ִϸ��̼��� ���۽�Ű�� �ǰ�, ����ġ�� �ø��� �ִϸ��̼��� PLAY_BACKWARD_ONCE�� ������ ���¿��� �ִϸ��̼��� ���۽�Ű�� �˴ϴ�.

	//�ִϸ��̼��� �� �������� �ѹ��� ����մϴ�.
	PLAY_FORWARD_ONCE UMETA(DisplayName = "Play forward once"),
	//�ִϸ��̼��� �� �������� ����ؼ� ����մϴ�. 
	PLAY_FORWARD_CONTINUOUSLY UMETA(DisplayName = "Play forward continuously"),
	//�ִϸ��̼��� �� �������� �ѹ��� ����մϴ�.
	PLAY_BACKWARD_ONCE UMETA(DisplayName = "Play backward once"),
	//�ִϸ��̼��� �� �������� ����ؼ� ����մϴ�. 
	PLAY_BACKWARD_CONTINUOUSLY UMETA(DisplayName = "Play backward continuously"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAnimationStarted, const int, TrackIndex, const FString, NewAnimation, const ESpineAnimationPlayType, NewPlayType, const float, MixDuration);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAnimationEnded, const int, TrackIndex, const FString, NewAnimation, const ESpineAnimationPlayType, NewPlayType, const float, MixDuration);

USTRUCT(BlueprintType)
struct FSpineAnimationSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FString Animation;

	UPROPERTY()
		ESpineAnimationPlayType PlayType;


	FSpineAnimationSet() {}
	FSpineAnimationSet(FString PlayingAnimation, ESpineAnimationPlayType NewPlayType) {
		Animation = PlayingAnimation;
		PlayType = NewPlayType;
	}

	bool operator==(const FSpineAnimationSet& a) const
	{
		return Animation == a.Animation && PlayType == a.PlayType;
	}

	bool operator!=(const FSpineAnimationSet& a) const
	{
		return Animation != a.Animation || PlayType != a.PlayType;
	}

};

FORCEINLINE uint32 GetTypeHash(const FSpineAnimationSet& b)
{
	return FCrc::MemCrc_DEPRECATED(&b, sizeof(FSpineAnimationSet));
}


UCLASS(Blueprintable , meta = (BlueprintSpawnableComponent))
class SPINEPLUGINIMPLEMENT_API USpineAnimationHandlerComponent : public USpineSkeletonRendererComponent
{
	GENERATED_BODY()

public:

	USpineAnimationHandlerComponent(const FObjectInitializer& ObjectInitializer);

private:

	UPROPERTY(VisibleAnywhere)
		TMap<int, FSpineAnimationSet> PlayingAnimations; //TrackIndex , AnimSet;

public:

	UPROPERTY(VisibleAnywhere)
		class USpineSkeletonAnimationComponent* SkeletonAnimationComp;

// ���� �ִϸ��̼� �ڵ� ��� �ý��ۿ� �ʿ��� ����Դϴ�. 
// ������ ����:
// �� Map �����̳��� Key ������ �ڵ� ����� ���� �ִϸ��̼� ������, ��� ������ �ڵ� ����� ��ǥ �ִϸ��̼ǿ� ���� ������ �Է��Ͻø� �˴ϴ�.
// ex -> FSpineAnimationSet�� 
// 
//	Key �� PickUpStart, PLAY_FORWARD_ONCE,
//  Value �� PickUpEnd, PLAY_FORWARD_ONCE
// 
// �� �־�θ� PickUpStart �ִϸ��̼��� ���������� �ѹ� ����ϸ�
// PickUpStart �ִϸ��̼��� ������ ����Ǿ����� �ڵ����� PickUpEnd�� ���������� �ѹ� ����մϴ�.
// 
// SpineAnimationHandlerComponent.cpp �� 16��° �ٿ��� �ִϸ��̼��� ���� ����� �ÿ� ȣ��Ǵ� ��������Ʈ�� ��ϵ� �ݹ� �Լ� Internal_PlayNextAnimation()����
// ���� �ִϸ��̼��� ã�� �����Ű�� ������ Ȯ���ϽǼ� �ֽ��ϴ�!
//
// �ַ� ���ž��Ͻô� �κе��� SetAnimation() (�ִϸ��̼� ���) ,  SetEmptyAnimation() (������� �ִϸ��̼� ����) , Internal_PlayNextAnimation() (���� �ִϸ��̼� �ڵ� ���)
// Ȥ�� ���ذ� �ȵǽô� �κ��� �����ôٸ� �˷��ּ���! �� �׸��� �� ������Ʈ�� �ִϸ��̼� ���� �Ӹ� �ƴ϶� �ٸ� ��ɵ��� ��� ���յǾ� �ִ� ������Ʈ��, 
// �Ƹ� ���Ҵ��� �۾��Ͻô� �ڵ忡�� �����ϼŵ� �Ǵ°� ���� �� �ֽ��ϴ�.

	UPROPERTY(EditAnywhere)
		TMap<FSpineAnimationSet, FSpineAnimationSet> NextAnimationMap; 

public:

	UPROPERTY(EditAnywhere)
		TArray<UBoneRigComponent*> BoneRigArr; // For some occasions I didn't come up with, I left this as a array, not just a single pointer. preparing unforeseen consequences...

	UPROPERTY(EditAnywhere)
		TArray<USpineAdvancedFollowerComponent*> FollowerArr;

	UPROPERTY(EditAnywhere)
		TArray<USpineAdvancedDriverComponent*> DriverArr;

	UPROPERTY(EditAnywhere)
		TArray<UIKConstraintBoneDriverComponent*> IKDriverArr; 

public:

	void OnNotifyDriverBoneEndedUpdate(USpineAdvancedDriverComponent* Target);


	UPROPERTY(EditAnywhere)
		FVector RenderingOffset = FVector(0,0,0);

	UPROPERTY(EditAnywhere)
		bool bUpdatable = true;

public:

	/** Multicaster fired when this component teleports */
	FOnHandlerTeleportedMultiCast OnHandlerTeleported;

	FOnAnimationStarted AnimationStartedHandle;

	FOnAnimationEnded AnimationEndedHandle;


public:
	//Rendering Implement
	virtual void UpdateMesh(spine::Skeleton* Skeleton) override;

	UPROPERTY(EditAnywhere, Category = "Rendering")
		bool bUseAdvancedNormalCalculation = false;


	UPROPERTY(EditAnywhere, Category = "Rendering", meta = (EditCondition = "bUseAdvancedNormalCalculation"))
		bool bShouldFlipNormal = false;

public:

	//Movement Implement

	virtual bool MoveComponentImpl(const FVector& Delta, const FQuat& NewRotation, bool bSweep, FHitResult* OutHit = NULL, EMoveComponentFlags MoveFlags = MOVECOMP_NoFlags, ETeleportType Teleport = ETeleportType::None) override;

	/** Register/Unregister for teleport callback */
	FDelegateHandle RegisterOnTeleportDelegate(const FOnHandlerTeleported& Delegate);
	void UnregisterOnTeleportDelegate(const FDelegateHandle& DelegateHandle);

	UFUNCTION()
		const TMap<int, FSpineAnimationSet> GetCopiedPlayingAnimations() { return PlayingAnimations; }

public:

	//Animation Component Tick Sequence Event Delegate Callbacks.
	UFUNCTION()
		void BeforeApplyUpdateWorldTransform(class USpineSkeletonComponent* TargetComp);
	UFUNCTION()
		void BeforeUpdateWorldTransform(class USpineSkeletonComponent* TargetComp);
	UFUNCTION()
		void AfterUpdateWorldTransform(class USpineSkeletonComponent* TargetComp);


public:

	//Functions
	
	UFUNCTION(BlueprintCallable)
	const FSpineAnimationSet GetPlayingAnimation(const int TrackIndex) {
		return (PlayingAnimations.Contains(TrackIndex)) ? PlayingAnimations[TrackIndex] : FSpineAnimationSet();
	}

	//�ִϸ��̼��� ����մϴ�. �� ��ɾ�� ����Ǵ� �ִϸ��̼��� NextAnimationMap�� ��ϵǾ����� �ʴ��� ������ �ִϸ��̼ǿ� ��ϵǾ��ִٸ� ����˴ϴ�.
	UFUNCTION(BlueprintCallable)
	void SetAnimation(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration = -1) {
		if (bUpdatable) {
			if (SkeletonAnimationComp->HasAnimation(NewAnimation)) {
				if (PlayingAnimations.Contains(TrackIndex)) {

					if (PlayingAnimations[TrackIndex].Animation == NewAnimation && PlayingAnimations[TrackIndex].PlayType == NewPlayType) { //������ ���� �ִϸ��̼��� �����Ϸ��� �ҽ� : Abort

						return;

					}

					PlayingAnimations[TrackIndex].Animation = NewAnimation;
					PlayingAnimations[TrackIndex].PlayType = NewPlayType;
				}
				else {
					//���� �����Ǿ����� �ʾҴ� Ʈ���� �ִϸ��̼��� �����Ű�� ��Ȳ�̶�� �ڵ����� �� �ִϸ��̼ǿ��� �����ŵ�ϴ�.
					Internal_SetAnimationFromEmptyAnimation(TrackIndex, NewAnimation, NewPlayType);

					return;
				}


				UTrackEntry* AnimationTrackEntry = nullptr;

				switch (NewPlayType) {
				case ESpineAnimationPlayType::PLAY_FORWARD_ONCE: {
					AnimationTrackEntry = SkeletonAnimationComp->SetAnimation(TrackIndex, NewAnimation, false);
					SkeletonAnimationComp->SetTimeScale(1);
					break;
				}
				case ESpineAnimationPlayType::PLAY_FORWARD_CONTINUOUSLY: {
					AnimationTrackEntry = SkeletonAnimationComp->SetAnimation(TrackIndex, NewAnimation, true);
					SkeletonAnimationComp->SetTimeScale(1);
					break;
				}
				case ESpineAnimationPlayType::PLAY_BACKWARD_ONCE: {
					AnimationTrackEntry = SkeletonAnimationComp->SetAnimation(TrackIndex, NewAnimation, false);
					SkeletonAnimationComp->SetTimeScale(-1);
					break;
				}
				case ESpineAnimationPlayType::PLAY_BACKWARD_CONTINUOUSLY: {
					AnimationTrackEntry = SkeletonAnimationComp->SetAnimation(TrackIndex, NewAnimation, true);
					SkeletonAnimationComp->SetTimeScale(-1);
					break;
				}
				}

				if (MixDuration != -1) {
					AnimationTrackEntry->SetMixDuration(MixDuration);
				}
				else {
					AnimationTrackEntry->SetMixDuration(SkeletonAnimationComp->SkeletonData->DefaultMix);
				}

				if (AnimationStartedHandle.IsBound()) {
					AnimationStartedHandle.Broadcast(TrackIndex, NewAnimation, NewPlayType, MixDuration);
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Warning! %s doesn't have animation called %s"), *this->GetFName().ToString(), *NewAnimation);
			}
		}
	}

	//EmptyAnimation���� Ʈ���� ����Ѵ�.

	UFUNCTION(BlueprintCallable)
	void ClearAllAnimations(TArray<int> ExceptionTracks) {
		if (bUpdatable) {
			UTrackEntry* AnimationTrackEntry = nullptr;

			TArray<int> Keys;
			PlayingAnimations.GetKeys(Keys);

			for (const int TestKey : Keys) {
				if (!ExceptionTracks.Contains(TestKey)) {

					SkeletonAnimationComp->SetEmptyAnimation(TestKey, 0);

					FString Name = PlayingAnimations[TestKey].Animation;
					ESpineAnimationPlayType PlayType = PlayingAnimations[TestKey].PlayType;

					PlayingAnimations.Remove(TestKey);

					if (AnimationEndedHandle.IsBound()) {
						AnimationEndedHandle.Broadcast(TestKey, Name, PlayType, 0);
					}
				}
			}
		}
	}
	
	UFUNCTION(BlueprintCallable)
	void SetEmptyAnimation(const int TrackIndex, const float MixDuration = -1) {
		if (bUpdatable) {
			UTrackEntry* AnimationTrackEntry = nullptr;

			if (MixDuration != -1) {
				AnimationTrackEntry = SkeletonAnimationComp->SetEmptyAnimation(TrackIndex, MixDuration);
			}
			else {
				AnimationTrackEntry = SkeletonAnimationComp->SetEmptyAnimation(TrackIndex, SkeletonAnimationComp->SkeletonData->DefaultMix);
			}

			if (PlayingAnimations.Contains(TrackIndex)) {

				FString Name = PlayingAnimations[TrackIndex].Animation;
				ESpineAnimationPlayType PlayType = PlayingAnimations[TrackIndex].PlayType;

				PlayingAnimations.Remove(TrackIndex);

				if (AnimationEndedHandle.IsBound()) {
					AnimationEndedHandle.Broadcast(TrackIndex, Name, PlayType, MixDuration);
				}

			}
		}
	}

private: 

	UFUNCTION()
	void Internal_PlayNextAnimation(UTrackEntry* entry) {
		if (bUpdatable) {
			const int Index = entry->GetTrackIndex();

			if (SkeletonAnimationComp->HasAnimation(entry->getAnimationName())) {

				if (PlayingAnimations.Contains(Index)) {

					FSpineAnimationSet& EndedAnim = PlayingAnimations[entry->GetTrackIndex()];

					if (SkeletonAnimationComp->HasAnimation(EndedAnim.Animation)) {
						if (NextAnimationMap.Contains(EndedAnim)) {
							FSpineAnimationSet& AnimVal = NextAnimationMap[EndedAnim];

							SetAnimation(Index, AnimVal.Animation, AnimVal.PlayType);
						}
						else {
							if (entry->GetLoop() == false) {
								SetEmptyAnimation(Index);
							}
							else {
								//Notify EndAnimation Even if the animation is begin played with looping
								if (AnimationEndedHandle.IsBound()) {
									AnimationEndedHandle.Broadcast(Index, PlayingAnimations[Index].Animation, PlayingAnimations[Index].PlayType, -1);
								}
							}
						}
					}
				}
			}
		}
	}

	UFUNCTION()
	void Internal_SetAnimationFromEmptyAnimation(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration = -1) {
		if (bUpdatable) {
			if (SkeletonAnimationComp->HasAnimation(NewAnimation)) {
				if (PlayingAnimations.Contains(TrackIndex)) {
					PlayingAnimations[TrackIndex].Animation = NewAnimation;
					PlayingAnimations[TrackIndex].PlayType = NewPlayType;
				}
				else {
					PlayingAnimations.Add(TrackIndex, FSpineAnimationSet(NewAnimation, NewPlayType));
				}


				if (MixDuration != -1) {
					SkeletonAnimationComp->SetEmptyAnimation(TrackIndex, MixDuration);
				}
				else {
					SkeletonAnimationComp->SetEmptyAnimation(TrackIndex, SkeletonAnimationComp->SkeletonData->DefaultMix);
				}
				UTrackEntry* AnimationTrackEntry = nullptr;


				switch (NewPlayType) {
				case ESpineAnimationPlayType::PLAY_FORWARD_ONCE: {
					AnimationTrackEntry = SkeletonAnimationComp->AddAnimation(TrackIndex, NewAnimation, false, 0);
					SkeletonAnimationComp->SetTimeScale(1);
					break;
				}
				case ESpineAnimationPlayType::PLAY_FORWARD_CONTINUOUSLY: {
					AnimationTrackEntry = SkeletonAnimationComp->AddAnimation(TrackIndex, NewAnimation, true, 0);
					SkeletonAnimationComp->SetTimeScale(1);
					break;
				}
				case ESpineAnimationPlayType::PLAY_BACKWARD_ONCE: {
					AnimationTrackEntry = SkeletonAnimationComp->AddAnimation(TrackIndex, NewAnimation, false, 0);
					SkeletonAnimationComp->SetTimeScale(-1);
					break;
				}
				case ESpineAnimationPlayType::PLAY_BACKWARD_CONTINUOUSLY: {
					AnimationTrackEntry = SkeletonAnimationComp->AddAnimation(TrackIndex, NewAnimation, true, 0);
					SkeletonAnimationComp->SetTimeScale(-1);
					break;
				}
				}

				if (MixDuration != -1) {
					AnimationTrackEntry->SetMixDuration(MixDuration);
				}
				else {
					AnimationTrackEntry->SetMixDuration(SkeletonAnimationComp->SkeletonData->DefaultMix);
				}

				if (AnimationStartedHandle.IsBound()) {
					AnimationStartedHandle.Broadcast(TrackIndex, NewAnimation, NewPlayType, MixDuration);
				}

			}
		}
	}


public:

	UFUNCTION(BlueprintCallable)
		void FlipSkeleton(bool bIsForward);

	UFUNCTION(BlueprintCallable)
		void FlipSkeletonTowardVector(FVector Vec);

	UPROPERTY(EditAnywhere) // true�̸� ���׺����� �ﰢ������ ��� ���� �����̼��� �ø� ���� �����̼����� �����մϴ�. false�̸� �ø��Ǳ� ������ �����̼��� �����մϴ�.
		bool bShouldUseImmediateFlip = true;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE class USpineSkeletonAnimationComponent* GetSkeletonAnimationComp() {
		return SkeletonAnimationComp;
	}
};


