// Fill out your copyright notice in the Description page of Project Settings.


#include "SpineRigDataEditor/EditorSpineRigActor.h"

// Sets default values
AEditorSpineRigActor::AEditorSpineRigActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HandlerComponent = CreateDefaultSubobject<USpineAnimationHandlerComponent>(TEXT("HandlerComp"));

	EditorBoneRigComponent = CreateDefaultSubobject<UEditorBoneRigComponent>(TEXT("RigComp"));

	SetRootComponent(HandlerComponent);

	EditorBoneRigComponent->SetupAttachment(HandlerComponent);

	/*

	HandlerComponent->PrimaryComponentTick.bCanEverTick = true;

	HandlerComponent->bTickInEditor = true;

	BoneRigComponent->PrimaryComponentTick.bCanEverTick = true;

	BoneRigComponent->bTickInEditor = true;
	*/
}

// Called when the game starts or when spawned
void AEditorSpineRigActor::BeginPlay()
{
	Super::BeginPlay();
	
}


bool AEditorSpineRigActor::ShouldTickIfViewportsOnly() const {
	return true;
}

