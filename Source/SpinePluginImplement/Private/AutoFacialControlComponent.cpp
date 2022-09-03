// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoFacialControlComponent.h"
#include "AdvancedComponents/SpineAdvancedDriverComponent.h"
#include "SpineAnimationHandlerComponent.h"


// Sets default values for this component's properties
UAutoFacialControlComponent::UAutoFacialControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UAutoFacialControlComponent::StartAnimation() {
	if (UWorld* World = GetWorld()) {
		World->GetTimerManager().SetTimer(BlinkHandle, this, &UAutoFacialControlComponent::Blink, BlinkFrequency + (FMath::FRand() - 0.5) * 2 * BlinkFrequencyModiRange, false);
	}
}

void UAutoFacialControlComponent::StopAnimation() {
	if (UWorld* World = GetWorld()) {
		World->GetTimerManager().ClearTimer(BlinkHandle);
	}
}


void UAutoFacialControlComponent::StartEyeTracking(USceneComponent* TargetComp, float Duration) {

	UWorld* World = GetWorld();

	if (!World) {
		return;
	}

	if (!TargetComponent) {
		return;
	}

	if (!TargetComp) {
		return;
	}

	if (!EyeTargetBone) {
		return;
	}

	SavedEyeTrackTarget = TargetComp;

	if (EyeTargetBone) {
		EyeTargetBone->StartUpdateBone();
	}

	EyeRemainningTime = Duration;

	World->GetTimerManager().SetTimer(EyeMovementHandle, this, &UAutoFacialControlComponent::UpdateEyeTracking, EyeTrackingFrequency, true);


}


void UAutoFacialControlComponent::UpdateEyeTracking() {

	UWorld* World = GetWorld();

	if (!World) {
		return;
	}
	
	EyeRemainningTime -= World->GetTimerManager().GetTimerElapsed(EyeMovementHandle);

	if (SavedEyeTrackTarget && EyeTargetBone) {
		EyeTargetBone->SetWorldLocation(SavedEyeTrackTarget->GetComponentLocation());
	}

	if (EyeRemainningTime <= 0) {
		FinishEyeTracking();
	}
}

void UAutoFacialControlComponent::FinishEyeTracking() {
	UWorld* World = GetWorld();

	if (!World) {
		return;
	}

	World->GetTimerManager().ClearTimer(EyeMovementHandle);

	if (EyeTargetBone) {
		EyeTargetBone->StopUpdateBone();
	}

	if (TargetComponent) {
		spine::Bone * Bone = TargetComponent->GetSkeletonAnimationComp()->GetSkeleton()->findBone(TCHAR_TO_UTF8(*EyeTargetBone->BoneName));

		if (Bone) {
			Bone->setX(Bone->getData().getX());
			Bone->setY(Bone->getData().getY());

			Bone->update();
		}
	}

	EyeRemainningTime = 0;
	SavedEyeTrackTarget = 0;

}

void UAutoFacialControlComponent::Blink() {

	TargetComponent->SetAnimation(int(ESpineTrackIndex::INDEX_FacialBasic), "FacialExpression/Basic/Blink",ESpineAnimationPlayType::PLAY_FORWARD_ONCE);


	if (UWorld* World = GetWorld()) {
		World->GetTimerManager().SetTimer(BlinkHandle, this, &UAutoFacialControlComponent::Blink, BlinkFrequency + (FMath::FRand() - 0.5) * 2 * BlinkFrequencyModiRange, false);
	}
}