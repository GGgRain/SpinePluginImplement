// Fill out your copyright notice in the Description page of Project Settings.

#include "AdvancedComponents/SpineAdvancedFollowerComponent.h"
#include "SpineAnimationHandlerComponent.h"

USpineAdvancedFollowerComponent::USpineAdvancedFollowerComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bTickInEditor = false;
	bAutoActivate = true;
}

void USpineAdvancedFollowerComponent::DestroyComponent(bool bPromoteChildren) {

	if (TargetComponent) {

		if (TargetComponent->IsValidLowLevel()) {

			if (TargetComponent->FollowerArr.Contains(this)) {
				TargetComponent->FollowerArr.Remove(this);
			}

			TargetComponent = nullptr;
		}

	}

	Super::DestroyComponent(bPromoteChildren);
}

void USpineAdvancedFollowerComponent::AdjustLocation(class USpineSkeletonComponent* TargetComp) {
	if (bShouldUpdate) {

		if (TargetComp) {

			FTransform transform = TargetComp->GetBoneWorldTransform(BoneName);

			if (UsePosition) SetWorldLocation(transform.GetLocation());
			if (UseRotation) SetWorldRotation(transform.GetRotation());
			if (UseScale) SetWorldScale3D(transform.GetScale3D());

		}
	}
}

void USpineAdvancedFollowerComponent::SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent) {
	if (InTargetComponent) {

		if (TargetComponent != nullptr) {

			if (TargetComponent->FollowerArr.Contains(this)) {
				TargetComponent->FollowerArr.Remove(this);
			}

			TargetComponent = nullptr;
		}

		if (InTargetComponent) {

			if (!InTargetComponent->FollowerArr.Contains(this)) {
				InTargetComponent->FollowerArr.Add(this);
			}

			TargetComponent = InTargetComponent;
		}
	}
}