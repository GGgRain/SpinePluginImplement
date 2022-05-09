// Fill out your copyright notice in the Description page of Project Settings.


#include "SpineBaseCharacter.h"
#include "SpineAnimationHandlerComponent.h"
// Sets default values
ASpineBaseCharacter::ASpineBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(-1, -1, -1);

	GetCharacterMovement()->BrakingFrictionFactor = 0.5; //부드러운 제동을 위해서 추가한 값입니다. 만약 움직임이 이상하다 싶으면 제거를 고려하세요...
	GetCharacterMovement()->SetFixedBrakingDistance(200);





	SpineAnimComp = CreateDefaultSubobject<USpineAnimationHandlerComponent>(TEXT("AnimationHandler"));
	SpineAnimComp->SetupAttachment(GetRootComponent());
	SpineAnimComp->SetAbsolute(false, true, false);

	SpineAnimComp->GetSkeletonAnimationComp()->AnimationEvent.AddDynamic(this, &ASpineBaseCharacter::EventCallback);
}

// Called when the game starts or when spawned
void ASpineBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ASpineBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimation();

}

void ASpineBaseCharacter::UpdateAnimation() {
	//Put some state based animation control logic in here...

	if (CheckShoulUpdateAnimationManually()) {
		return;
	}

}


bool ASpineBaseCharacter::CheckShoulUpdateAnimationManually() {
	return bShouldUpdateAnimationManually;
}

void ASpineBaseCharacter::EventCallback(UTrackEntry* entry, FSpineEvent evt) {
	//Put some spine event control logic in here...

}

// Called to bind functionality to input
void ASpineBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASpineBaseCharacter::MoveRight(const float Val) {

	AddMovementInput(FVector(1.0f, 0.0f, 0.0f) , Val);

}

void ASpineBaseCharacter::MoveForward(const float Val) {

	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Val);

}