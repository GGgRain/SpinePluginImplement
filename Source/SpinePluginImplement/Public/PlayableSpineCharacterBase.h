// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpineBaseCharacter.h"
#include "PlayableSpineCharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class SPINEPLUGINIMPLEMENT_API APlayableSpineCharacterBase : public ASpineBaseCharacter
{
	GENERATED_BODY()

public:

	APlayableSpineCharacterBase();

public:

	//DefaultSubobject


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;


public:

	//Input

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:

	//INLINE

	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
};
