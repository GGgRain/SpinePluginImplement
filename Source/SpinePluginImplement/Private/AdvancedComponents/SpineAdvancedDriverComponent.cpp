// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedComponents/SpineAdvancedDriverComponent.h"
#include "SpineAnimationHandlerComponent.h"

USpineAdvancedDriverComponent::USpineAdvancedDriverComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bTickInEditor = false;
	bAutoActivate = true;
}

void USpineAdvancedDriverComponent::DestroyComponent(bool bPromoteChildren) {

	if (TargetComponent) {

		if (TargetComponent->IsValidLowLevel()) {

			if (TargetComponent->DriverArr.Contains(this)) {
				TargetComponent->DriverArr.Remove(this);
			}

			TargetComponent = nullptr;
		}

	}

	Super::DestroyComponent(bPromoteChildren);
}


void USpineAdvancedDriverComponent::StartUpdateBone() {

	if (!TargetComponent) {
		return;
	}

	if (!bShouldUpdate) {
		bShouldUpdate = true;
	}
}

void USpineAdvancedDriverComponent::UpdateBone(class USpineSkeletonComponent* TargetComp) {

	if (bShouldUpdate) {

		if (TargetComp) {

			if (spine::Bone* Bone = TargetComp->GetSkeleton()->findBone(TCHAR_TO_UTF8(*BoneName))) {

				
				if (UseFixedRotation) {
					Bone->setToSetupPose();
				}

				TargetComp->SetBoneWorldPosition(BoneName, GetComponentLocation());


				//DrawDebugBox(GetWorld(), GetComponentLocation(), FVector(5, 3, 5), FColor::Orange, false, 0.001, 0, 3);


				Bone->update();
			}
		}
	}
}

void USpineAdvancedDriverComponent::StopUpdateBone() {

	if (!TargetComponent) {
		return;
	}

	if (bShouldUpdate) {
		bShouldUpdate = false;

		TargetComponent->OnNotifyDriverBoneEndedUpdate(this);
	}
}

void USpineAdvancedDriverComponent::SetTargetComponent(USpineAnimationHandlerComponent* InTargetComponent) {

	if (InTargetComponent) {

		if (TargetComponent != nullptr) {

			if (TargetComponent->DriverArr.Contains(this)) {
				TargetComponent->DriverArr.Remove(this);
			}

			TargetComponent = nullptr;
		}

		if (InTargetComponent) {

			if (!InTargetComponent->DriverArr.Contains(this)) {
				InTargetComponent->DriverArr.Add(this);
			}

			TargetComponent = InTargetComponent;
		}
	}
}

