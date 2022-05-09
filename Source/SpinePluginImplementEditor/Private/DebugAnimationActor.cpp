// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugAnimationActor.h"

// Sets default values
ADebugAnimationActor::ADebugAnimationActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Handler = CreateDefaultSubobject<USpineAnimationHandlerComponent>(TEXT("Handler"));
	SetRootComponent(Handler);

	BoneRig = CreateDefaultSubobject<UBoneRigComponent>(TEXT("BoneRig"));
	BoneRig->SetupAttachment(Handler);

}
// Called when the game starts or when spawned
void ADebugAnimationActor::BeginPlay()
{
	Super::BeginPlay();


	Handler->SetAnimation(0, "animation", ESpineAnimationPlayType::PLAY_FORWARD_CONTINUOUSLY);
}

// Called every frame
void ADebugAnimationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

