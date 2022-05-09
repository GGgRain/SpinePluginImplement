// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AutoFacialControlComponent.generated.h"


class USpineAnimationHandlerComponent;
class USpineAdvancedDriverComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPINEPLUGINIMPLEMENT_API UAutoFacialControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoFacialControlComponent();

public:

	UPROPERTY(VisibleAnywhere)
		USpineAnimationHandlerComponent* TargetComponent;

public:

	UPROPERTY(EditAnywhere)
		float BlinkFrequency = 3.5;

	UPROPERTY(EditAnywhere)
		float BlinkFrequencyModiRange = 3.0;

	UPROPERTY(EditAnywhere)
		FTimerHandle BlinkHandle;
public:

	UFUNCTION(BlueprintCallable)
		void Blink();

public:	

	UPROPERTY(EditAnywhere)
		FTimerHandle EyeMovementHandle;

	UPROPERTY(EditAnywhere)
		USceneComponent* ThisHeadBone;

	UPROPERTY(EditAnywhere)
		USpineAdvancedDriverComponent* EyeTargetBone;


	UPROPERTY(EditAnywhere)
		float EyeTrackingFrequency = 0.01;

	UPROPERTY(EditAnywhere)
		float EyeRemainningTime = 0;

	UPROPERTY(EditAnywhere)
		USceneComponent* SavedEyeTrackTarget;

public:

	UFUNCTION(BlueprintCallable)
		void StartEyeTracking(USceneComponent* TargetComp, float Duration);

	UFUNCTION(BlueprintCallable)
		void UpdateEyeTracking();

	UFUNCTION(BlueprintCallable)
		void FinishEyeTracking();

public:

	UFUNCTION(BlueprintCallable)
		void StartAnimation();

	UFUNCTION(BlueprintCallable)
		void StopAnimation();

		
};
