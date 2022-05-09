// Fill out your copyright notice in the Description page of Project Settings.


#include "IKConstraintBoneDriverComponent.h"
#include "SpineAnimationHandlerComponent.h"

#include "DrawDebugHelpers.h"
//USpineSkeletonComponent* skeleton = static_cast<USpineSkeletonComponent*>(Target->GetComponentByClass(USpineSkeletonComponent::StaticClass()));


UIKConstraintBoneDriverComponent::UIKConstraintBoneDriverComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bTickInEditor = false;
	bAutoActivate = true;

}


void UIKConstraintBoneDriverComponent::DestroyComponent(bool bPromoteChildren) {

	if (TargetComponent) {

		if (TargetComponent->IsValidLowLevel()) {

			if (TargetComponent->IKDriverArr.Contains(this)) {
				TargetComponent->IKDriverArr.Remove(this);
			}

			TargetComponent = nullptr;
		}

	}

	Super::DestroyComponent(bPromoteChildren);
}

void UIKConstraintBoneDriverComponent::SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent) {
	if (InTargetComponent) {

		if (TargetComponent != nullptr) {

			if (TargetComponent->IKDriverArr.Contains(this)) {
				TargetComponent->IKDriverArr.Remove(this);
			}

			TargetComponent = nullptr;
		}

		if (InTargetComponent) {

			if (!InTargetComponent->IKDriverArr.Contains(this)) {
				InTargetComponent->IKDriverArr.Add(this);
			}

			TargetComponent = InTargetComponent;
		}
	}
}

/*
spine::Vector<spine::TrackEntry*> UIKConstraintBoneDriverComponent::DoesAnimationStateHasIKBoneTimeline() {

	spine::Vector<spine::TrackEntry*> InvolvingTracks;
	if (TargetComponent != nullptr) {

		//�˻翡 ����� IK���� �ε����Դϴ�.
		const int IKBoneIndex = TargetComponent->Get->GetSkeleton()->findBoneIndex(TCHAR_TO_UTF8(*IKBoneName));

		spine::Vector<spine::TrackEntry*> Tracks = lastBoundAnimationComponent->GetAnimationState()->getTracks();

		for (size_t i = 0, n = Tracks.size(); i < n; ++i) { //�ִϸ��̼� ���̾�(0,1,2,...)
			spine::TrackEntry* currentP = Tracks[i];
			if (currentP == NULL || currentP->getDelay() > 0) { // �ܼ� ������̶� �̷��� ó����.
				continue;
			}

			spine::TrackEntry& current = *currentP;

			size_t timelineCount = current.getAnimation()->getTimelines().size();
			spine::Vector<spine::Timeline*>& timelines = current.getAnimation()->getTimelines();

			for (size_t ii = 0; ii < timelineCount; ++ii) {
				spine::Timeline* timeline = timelines[ii];
				if (timeline->getRTTI().isExactly(spine::TranslateTimeline::rtti)) {
					const int BoneIndex = timeline->getPropertyId() - ((int)spine::TimelineType_Translate << 24);
					if (IKBoneIndex == BoneIndex) {
						//dynamic_cast<spine::TranslateTimeline>(timeline).
						InvolvingTracks.add(currentP);
						break;
					}
				}
			}
		
		}
	}

	return InvolvingTracks; //���� �츮�� IK������Ʈ�� �ؾ��ϴ� ���� ������Ʈ�� �����Ǿ��ִٸ� True�� �����մϴ�. (���� Ÿ�Ӷ����� �����Ѵٸ� ���� �����̼��� ������Ʈ�Ǵ°��̹Ƿ� �׳� ���� �����̼��� �״�� �̿��ϸ� �ǰ�, Ÿ�Ӷ����� �������� �ʴ´ٸ� �����ص� ���� �����̼��� �����ͼ� ���� �ȴ�.
}
*/

const bool UIKConstraintBoneDriverComponent::IsTrackMixTimeOver(spine::TrackEntry* CheckEntry) {
	return CheckEntry->getMixTime() > CheckEntry->getMixDuration();
}

void UIKConstraintBoneDriverComponent::AdjustTargetIKBone(USpineSkeletonComponent* skeleton) {

	if (bShouldUpdate) {

		if (skeleton) {

			FTransform BoneTransform;

			if (spine::Bone* IKBone = skeleton->GetSkeleton()->findBone(TCHAR_TO_UTF8(*IKBoneName))) {

				IKBone->update();

				FTransform baseTransform;
				AActor* owner = GetOwner();
				if (owner) {
					USpineSkeletonRendererComponent* rendererComponent = static_cast<USpineSkeletonRendererComponent*>(owner->GetComponentByClass(USpineSkeletonRendererComponent::StaticClass()));
					if (rendererComponent) baseTransform = rendererComponent->GetComponentTransform();
					else baseTransform = owner->GetActorTransform();
				}

				FVector position(IKBone->getWorldX(), 0, IKBone->getWorldY());
				FMatrix localTransform;
				localTransform.SetIdentity();
				localTransform.SetAxis(2, FVector(IKBone->getA(), 0, IKBone->getC()));
				localTransform.SetAxis(0, FVector(IKBone->getB(), 0, IKBone->getD()));
				localTransform.SetOrigin(FVector(IKBone->getWorldX(), 0, IKBone->getWorldY()));
				localTransform = localTransform * baseTransform.ToMatrixWithScale();

				BoneTransform.SetFromMatrix(localTransform);

				LastBoneData.CopyFromBone(IKBone);

				TraceDist(skeleton, BoneTransform);


				if (!FMath::IsNearlyZero(OffLength)) {
					FVector UpVec = GetOwner()->GetActorUpVector();
					skeleton->SetBoneWorldPosition(IKBoneName, BoneTransform.GetLocation() + OffLength * UpVec);
					skeleton->UpdateWorldTransform();
				}
			}

		}

	}

}

void UIKConstraintBoneDriverComponent::ResetTargetIKBone(USpineSkeletonComponent* skeleton) {
	if (bShouldUpdate) {
		if (skeleton) {
			if (spine::Bone* IKBone = skeleton->GetSkeleton()->findBone(TCHAR_TO_UTF8(*IKBoneName))) {
				LastBoneData.PasteToBone(IKBone);
			}
		}
	}
}


void UIKConstraintBoneDriverComponent::TraceDist(USpineSkeletonComponent* skeleton, FTransform& TransformBeforeUpdate) {

	if (AActor* Owner = GetOwner()) {
		UWorld* CurrentWorld = GetWorld();

		FHitResult HitResult;

		if (!CurrentWorld) {
			return;
		}

		if (!CurrentWorld->IsGameWorld()) {
			return;
		}


		FVector UpVec = Owner->GetActorUpVector();
		bool bIsHit = false;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Owner);

		FVector LastLoc = TransformBeforeUpdate.GetLocation();

		bIsHit = CurrentWorld->LineTraceSingleByProfile(HitResult, LastLoc + UpVec * (CastDistance + CastOffset), LastLoc + UpVec * CastOffset, TraceProfile, Params);

		if (bIsHit) {
			float TempVec = (HitResult.Location.Z - LastLoc.Z) + GroundOffset;
			OffLength = FMath::Clamp(TempVec, 0.f, CastDistance);
		}
		else {
			OffLength = FMath::Clamp(OffLength - AdjustSpeed, 0.f, CastDistance);
		}

#if WITH_EDITORONLY_DATA 
		if (bShouldDisplayDebugCastingDraw) {
			DrawDebugLine(CurrentWorld, LastLoc + UpVec * (CastDistance + CastOffset), LastLoc + UpVec * CastOffset, FColor::Red, false, 0.01, 0, 1);

			if (bIsHit) {
				DrawDebugBox(CurrentWorld, HitResult.Location, FVector(1,1,1), FColor::Green, false, 0.01, 0, 1);
			}
		}
#endif 

	}
}


