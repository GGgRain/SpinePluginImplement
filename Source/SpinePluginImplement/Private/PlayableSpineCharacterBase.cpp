// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableSpineCharacterBase.h"


APlayableSpineCharacterBase::APlayableSpineCharacterBase() {

	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void APlayableSpineCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpineBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpineBaseCharacter::MoveRight);

}

