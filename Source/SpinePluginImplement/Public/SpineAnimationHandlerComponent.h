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
	//모든 애니메이션 노드는 런타임중 수정가능합니다. 이를 이용해서 하나의 애니메이션으로도 다양한 표현을 만들 수 있습니다.
	//예시) 스위치를 올리고 내리는 애니메이션을 만들고 싶다면, 스위치를 내리는 애니메이션은 PLAY_FORWARD_ONCE로 지정된 상태에서 애니메이션을 시작시키면 되고, 스위치를 올리는 애니메이션은 PLAY_BACKWARD_ONCE로 지정된 상태에서 애니메이션을 시작시키면 됩니다.

	//애니메이션을 순 방향으로 한번만 재생합니다.
	PLAY_FORWARD_ONCE UMETA(DisplayName = "Play forward once"),
	//애니메이션을 순 방향으로 계속해서 재생합니다. 
	PLAY_FORWARD_CONTINUOUSLY UMETA(DisplayName = "Play forward continuously"),
	//애니메이션을 역 방향으로 한번만 재생합니다.
	PLAY_BACKWARD_ONCE UMETA(DisplayName = "Play backward once"),
	//애니메이션을 역 방향으로 계속해서 재생합니다. 
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

// 다음 애니메이션 자동 재생 시스템에 필요한 어레이입니다. 
// 간단한 설명:
// 이 Map 컨테이너의 Key 값에는 자동 재생의 시작 애니메이션 세팅을, 밸류 값에는 자동 재생할 목표 애니메이션에 대한 세팅을 입력하시면 됩니다.
// ex -> FSpineAnimationSet에 
// 
//	Key 를 PickUpStart, PLAY_FORWARD_ONCE,
//  Value 를 PickUpEnd, PLAY_FORWARD_ONCE
// 
// 로 넣어두면 PickUpStart 애니메이션을 순방향으로 한번 재생하면
// PickUpStart 애니메이션이 완전히 종료되었을때 자동으로 PickUpEnd을 순방향으로 한번 재생합니다.
// 
// SpineAnimationHandlerComponent.cpp 의 16번째 줄에서 애니메이션이 완전 종료될 시에 호출되는 델리게이트에 등록된 콜백 함수 Internal_PlayNextAnimation()에서
// 다음 애니메이션을 찾고 실행시키는 구조를 확인하실수 있습니다!
//
// 주로 보셔야하시는 부분들은 SetAnimation() (애니메이션 재생) ,  SetEmptyAnimation() (재생중인 애니메이션 제거) , Internal_PlayNextAnimation() (다음 애니메이션 자동 재생)
// 혹시 이해가 안되시는 부분이 있으시다면 알려주세요! 아 그리고 이 컴포넌트는 애니메이션 제어 뿐만 아니라 다른 기능들이 몇가지 통합되어 있는 컴포넌트라서, 
// 아마 염소님이 작업하시는 코드에선 무시하셔도 되는게 있을 수 있습니다.

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

	//애니메이션을 재생합니다. 이 명령어로 재생되는 애니메이션은 NextAnimationMap에 등록되어있지 않더라도 스파인 애니메이션에 등록되어있다면 재생됩니다.
	UFUNCTION(BlueprintCallable)
	void SetAnimation(const int TrackIndex, const FString NewAnimation, const ESpineAnimationPlayType NewPlayType, const float MixDuration = -1) {
		if (bUpdatable) {
			if (SkeletonAnimationComp->HasAnimation(NewAnimation)) {
				if (PlayingAnimations.Contains(TrackIndex)) {

					if (PlayingAnimations[TrackIndex].Animation == NewAnimation && PlayingAnimations[TrackIndex].PlayType == NewPlayType) { //완전히 같은 애니메이션을 지정하려고 할시 : Abort

						return;

					}

					PlayingAnimations[TrackIndex].Animation = NewAnimation;
					PlayingAnimations[TrackIndex].PlayType = NewPlayType;
				}
				else {
					//만약 지정되어있지 않았던 트랙에 애니메이션을 실행시키는 상황이라면 자동으로 빈 애니메이션에서 실행시킵니다.
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

	//EmptyAnimation또한 트랙을 사용한다.

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

	UPROPERTY(EditAnywhere) // true이면 리그본에서 즉각적으로 모든 본의 로케이션을 플립 후의 로케이션으로 지정합니다. false이면 플립되기 이전의 로케이션을 유지합니다.
		bool bShouldUseImmediateFlip = true;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE class USpineSkeletonAnimationComponent* GetSkeletonAnimationComp() {
		return SkeletonAnimationComp;
	}
};


