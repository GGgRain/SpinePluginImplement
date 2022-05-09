// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpineBaseCharacter.generated.h"

class USpineAnimationHandlerComponent;

UCLASS()
class SPINEPLUGINIMPLEMENT_API ASpineBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpineBaseCharacter();

	UPROPERTY(EditAnywhere ,Category = "AnimationHandler")
		class USpineAnimationHandlerComponent* SpineAnimComp;

public:

	UPROPERTY(EditAnywhere, Interp, Category = "AnimationHandler")
		bool bShouldUpdateAnimationManually = false;

	UFUNCTION(BlueprintCallable)
		void MoveRight(const float Val);

	UFUNCTION(BlueprintCallable)
		void MoveForward(const float Val);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		USpineAnimationHandlerComponent* GetSpineAnimComp() { return SpineAnimComp; }

	UFUNCTION(BlueprintCallable)
		bool CheckShoulUpdateAnimationManually();

public:

	UFUNCTION()
		virtual void UpdateAnimation();

	UFUNCTION(BlueprintCallable)
		virtual void EventCallback(UTrackEntry* entry, FSpineEvent evt);

};

